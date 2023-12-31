#include "prediction.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>

#include "model/model.h"


Prediction::Prediction(const Config& config, Model& model, const CP_Model::SolutionVec_t& offline_paths) :
    _model(model),
    _solution(offline_paths),
    random_engine(config.random_seed)
{
    // Generate oracles
    for (uint32_t oracle_idx = 0; oracle_idx < config.nb_generated_oracles; oracle_idx++) {
        Oracle oracle;
        oracle.dimensions.resize(_model.nb_requests);
        oracle.predictions.resize(_model.nb_requests);

        int attempts = config.nb_oracle_search;
        while (oracleIsNotUnique(oracle) && attempts >= 0) {
            createPredictions(oracle);
            attempts--;
        }

        if (! oracleIsNotUnique(oracle)) {
            oracles.push_back(oracle);
        }
    }
/*
    // Keep only the best oracles
    while (oracles.size() > config.nb_oracles) {
        double max_objective = 0.0;
        int64_t max_idx = -1;
        for (uint32_t idx = 0; idx < oracles.size(); idx++) {
            if (oracles[idx].objective_value > max_objective) {
                max_objective = oracles[idx].objective_value;
                max_idx = idx;
            }
        }
        oracles.erase(oracles.begin() + max_idx);
    }
*/

    // Keep best, worst, middle oracles
    double min_objective = std::numeric_limits<double>::infinity();
    int64_t min_idx = -1;
    for (uint32_t idx = 0; idx < oracles.size(); idx++) {
        if (oracles[idx].objective_value < min_objective) {
            min_objective = oracles[idx].objective_value;
            min_idx = idx;
        }
    }

    double max_objective = 0.0;
    int64_t max_idx = -1;
    for (uint32_t idx = 0; idx < oracles.size(); idx++) {
        if (oracles[idx].objective_value > max_objective) {
            max_objective = oracles[idx].objective_value;
            max_idx = idx;
        }
    }

    double min_distance = std::numeric_limits<double>::infinity();
    double middle_value = min_objective + ((max_objective - min_objective) / 2.0);
    int64_t middle_idx = -1;
    double distance;
    for (uint32_t idx = 0; idx < oracles.size(); idx++) {
        distance = std::abs(oracles[idx].objective_value - middle_value);
        if (distance < min_distance) {
            min_distance = distance;
            middle_idx = idx;
        }
    }

    OracleVec_t final_oracles;
    final_oracles.push_back(oracles[min_idx]);
    final_oracles.push_back(oracles[middle_idx]);
    final_oracles.push_back(oracles[max_idx]);
    oracles = final_oracles;

}

uint32_t Prediction::getNbOracles() const
{
    return oracles.size();
}

Oracle* Prediction::getOracle(uint32_t oracle_idx)
{
    return &(oracles[oracle_idx]);
}

void Prediction::printFormattedSolution(uint32_t oracle_idx) const
{
    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        Solution formatted_solution(_model, oracles[oracle_idx].predictions[r], _model.requests[r].source, _model.requests[r].target);

        std::cout << "\tRequest " << (r+1) << " (v" << _model.requests[r].source << " -> v" << _model.requests[r].target << "):" << std::endl;
        for (uint32_t p = 0; p < formatted_solution.vertices.size(); p++) {
            std::cout << "\t\tPath " << (p+1) << " with ratio (" << formatted_solution.ratios[p] << "): [ v";
            for (uint32_t idx = 0; idx < (formatted_solution.vertices[p].size() - 1); idx++) {
                std::cout << formatted_solution.vertices[p][idx] << " v";
            }
            std::cout << formatted_solution.vertices[p].back() << " ]" << std::endl;
        }
    }
    std::cout << std::flush;
}

bool Prediction::oracleIsNotUnique(const Oracle& oracle)
{
    if (oracle.objective_value == 0.0) {
        return true; // there is no prediction yet (first while loop call)
    }

    for (uint32_t idx = 0; idx < oracles.size(); idx++) {
        if (oracles[idx].objective_value == oracle.objective_value) {
            return true;
        }
    }

    return false;
}

void Prediction::createPredictions(Oracle& oracle)
{
    uint32_t p_idx;
    uint32_t edge;

    // Round the paths with the probability of the traffic passing through them
    for (uint32_t r_idx = 0; r_idx < _model.nb_requests; r_idx++) {
        oracle.predictions[r_idx].clear();
        oracle.predictions[r_idx].resize(_model.graph.nb_edges, 0.0);

        std::discrete_distribution<uint32_t> ratio_distribution(_solution[r_idx].ratios.begin(), _solution[r_idx].ratios.end());
        p_idx = ratio_distribution(random_engine);

        for (uint32_t e_idx = 0; e_idx < _solution[r_idx].paths[p_idx].size(); e_idx++) {
            edge = _solution[r_idx].paths[p_idx][e_idx];
            oracle.predictions[r_idx][edge] = 1.0;
        }
    }


    double cost = 0.0;
    double amount = 0.0;
    uint32_t i,j;

    // Calculate the objective value
    for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
        amount = 0.0;
        for (uint32_t r = 0; r < _model.nb_requests; r++) {
            amount += oracle.predictions[r][e];
        }
        i = _model.graph.ID[e].i;
        j = _model.graph.ID[e].j;
        cost += _model.graph.A[i][j].getCost(amount);
    }
    oracle.objective_value = cost;


    // Calculate the dimensions
    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        oracle.dimensions[r] = 0.0;
        for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
            if (oracle.predictions[r][e] == 1.0) {
                oracle.dimensions[r]++;
            }
        }
    }
}
