#include "prediction.h"

#include <algorithm>
#include <limits>
#include <numeric>

#include "visualization/print.hpp"


Prediction::Prediction(Model& model, const DoubleMat_t& off_solution) :
    _model(model),
    cbfs(model),
    pred_error(0.0),
    random_engine(rd())
{
    integral_solution.resize(_model.getNbRequests());
    prediction.resize(_model.getNbRequests());
    solution.resize(_model.getNbRequests());
    available_edges.resize(_model.getNbRequests());

    for (uint32_t r = 0; r < _model.getNbRequests(); r++) {
        integral_solution[r].resize(_model.getNbEdges());
        prediction[r].resize(_model.getNbEdges());
        solution[r].resize(_model.getNbEdges());
        available_edges[r].resize(_model.getNbEdges());

        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            integral_solution[r][e] = 0.0;
            prediction[r][e] = false;
            solution[r][e] = 0.0;
            available_edges[r][e] = true;
        }
    }

    createIntegralSolution(off_solution);
}

double Prediction::getErrorRate() const
{
    return pred_error;
}

const BoolVec_t& Prediction::predict(const Request& request) const
{
    return prediction[request.id];
}

void Prediction::createIntegralSolution(const DoubleMat_t& offline_solution)
{
    // Select the edges of the fractional solution
    BoolMat_t edges;
    for (uint32_t r = 0; r < _model.getNbRequests(); r++) {
        edges.push_back(BoolVec_t(_model.getNbEdges(), false));
        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            if (offline_solution[r][e] > 0.0) {
                edges[r][e] = true;
            }
        }
    }

    // Find all possible paths with the selected edges
    UIntMatVec_t all_paths;
    for (uint32_t r = 0; r < _model.getNbRequests(); r++) {
        all_paths.push_back(UIntMat_t());
    }

    const Model::RequestVec_t& requests = _model.getRequests();
    for (const Request& request : requests) {
        UIntVec_t path;
        while (true) {
            path = cbfs.getPath(request.source, request.target, edges[request.id]);
            if (path.empty()) {
                break;
            }
            all_paths[request.id].push_back(path);
            edges[request.id][path[0]] = false;
        }
    }

    uint64_t num_possibilities = 1;
    for (uint32_t r = 0; r < _model.getNbRequests(); r++) {
        num_possibilities = num_possibilities * all_paths[r].size();
    }
    std::cout << "Possibilities = " << num_possibilities << std::endl;

    // Find best path combination across the requests
    double best_cost = std::numeric_limits<double>::infinity();
    double current_cost;
    UIntVec_t best_path_indices;
    UIntVec_t path_indices(_model.getNbRequests(), 0);

    uint64_t counter = 0;
    while (true) {
        if ((counter % 10000) == 0) {
            std::cout << "Process: " << counter << std::endl << std::flush;
        }
        // Reset the solution
        for (uint32_t r = 0; r < _model.getNbRequests(); r++) {
            for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
                solution[r][e] = 0.0;
            }
            for (uint32_t e = 0; e < all_paths[r][path_indices[r]].size(); e++) {
                solution[r][all_paths[r][path_indices[r]][e]] = 1.0;
            }
        }

        // Check solution quality
        current_cost = _model.getObjectiveValue(solution);
        if (current_cost < best_cost) {
            best_path_indices = path_indices;
            best_cost = current_cost;
        }

        // Update the indices to the next path combination
        bool carry = true;
        int64_t idx = (_model.getNbRequests() - 1);
        while (carry && idx >= 0) {
            carry = false;
            path_indices[idx]++;
            if (path_indices[idx] >= all_paths[idx].size()) {
                path_indices[idx] = 0;
                carry = true;
            }
            idx--;
        }
        if (carry) {
            break;
        }
        counter++;
    }

    // Save integral solution
    for (uint32_t r = 0; r < _model.getNbRequests(); r++) {
        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            integral_solution[r][e] = 0.0;
            solution[r][e] = 0.0;
            prediction[r][e] = false;
        }
        for (uint32_t e = 0; e < all_paths[r][best_path_indices[r]].size(); e++) {
            integral_solution[r][all_paths[r][best_path_indices[r]][e]] = 1.0;
            solution[r][all_paths[r][best_path_indices[r]][e]] = 1.0;
            prediction[r][all_paths[r][best_path_indices[r]][e]] = true;
        }
    }
}

const DoubleMat_t& Prediction::createPredictionWithError(const double target_error_rate)
{
    if (target_error_rate == 0.0) {
        return integral_solution;
    }

    initializeToIntegralSolution();

    double min_cost = _model.getObjectiveValue(integral_solution);
    double current_cost;
    double new_error;
    double additional_error;

    UIntVec_t indices(_model.getNbRequests());
    for (uint32_t idx = 0; idx < _model.getNbRequests(); idx++) {
        indices[idx] = idx;
    }

    const Model::RequestVec_t& requests = _model.getRequests();
    while (pred_error < target_error_rate) {
        additional_error = 0.0;
        std::shuffle(indices.begin(), indices.end(), random_engine);

        for (uint32_t idx = 0; idx < _model.getNbRequests(); idx++) {
            const Request& request = requests[indices[idx]];
            introduceErrors(request);
            current_cost = _model.getObjectiveValue(solution);
            new_error = (1 - (min_cost / current_cost));

            if (new_error < 0) {
                print_solution("new_pred", current_cost, solution);
                throw std::runtime_error("ERROR: the integral solution is not minimal!");
            }

            additional_error += (new_error - pred_error);
            pred_error = new_error;
            if (pred_error >= target_error_rate) {
                return solution;
            }
        }
        if (additional_error == 0.0) {
            return solution; // not possible to introduce more error
        }
    }

    return solution;
}

void Prediction::initializeToIntegralSolution()
{
    pred_error = 0.0;
    for (uint32_t r = 0; r < _model.getNbRequests(); r++) {
        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            prediction[r][e] = static_cast<bool>(integral_solution[r][e]);
            solution[r][e] = integral_solution[r][e];
            available_edges[r][e] = true;
        }
    }
}

void Prediction::introduceErrors(const Request& request)
{
    BoolVec_t marker = available_edges[request.id];
    UIntVec_t path = cbfs.getPath(request.source, request.target, prediction[request.id]);

    std::shuffle(path.begin(), path.end(), random_engine);

    for (uint32_t idx = 0; idx < path.size(); idx++) {
        marker[path[idx]] = false;
        UIntVec_t new_path = cbfs.getPath(request.source, request.target, marker);

        if (new_path.empty()) {
            marker[path[idx]] = true;
        } else {
            // Mark the blocked edge as unavailable for future perturbations
            available_edges[request.id][path[idx]] = false;

            // Update the prediction
            prediction[request.id] = BoolVec_t(_model.getNbEdges(), false);
            solution[request.id] = DoubleVec_t(_model.getNbEdges(), 0.0);
            for (uint32_t v = 0; v < new_path.size(); v++) {
                prediction[request.id][new_path[v]] = true;
                solution[request.id][new_path[v]] = 1.0;
            }
            return;
        }
    }
}
