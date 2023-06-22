#include "prediction.h"

#include <algorithm>
#include <iostream>

#include "model/model.h"


Prediction::Prediction(const Config& config, Model& model, const CP_Model::SolutionVec_t& offline_paths) :
    _model(model),
    _solution(offline_paths),
    predictions(config.nb_oracles),
    objective_values(config.nb_oracles, 0.0),
    random_engine(config.random_seed)
{
    for (uint32_t oracle_idx = 0; oracle_idx < config.nb_oracles; oracle_idx++) {
        predictions[oracle_idx].resize(_model.nb_requests);

        uint32_t attempts = config.nb_oracle_search;
        while (oracleIsNotUnique(oracle_idx) && attempts >= 0) {
            createPredictions(oracle_idx);
            attempts--;
        }
    }
}

const UIntMat_t& Prediction::getPredictionsOfOracle(uint32_t oracle_idx) const
{
    return predictions[oracle_idx];
}

double Prediction::getObjectiveValueOfOracle(uint32_t oracle_idx) const
{
    return objective_values[oracle_idx];
}

void Prediction::printFormattedSolution(uint32_t oracle_idx) const
{
    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        std::cout << "Request " << (r+1) << " (v" << _model.requests[r].source << " -> v" << _model.requests[r].target << "):" << std::endl;
        std::cout << "\tPath:";
        for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
            if (predictions[oracle_idx][r][e] != 0) {
                std::cout << " e" << e;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::flush;
}

bool Prediction::oracleIsNotUnique(uint32_t oracle_idx)
{
    if (objective_values[oracle_idx] == 0.0) {
        return true; // there is no prediction yet (first while loop call)
    }

    for (uint32_t idx = 0; idx < predictions.size(); idx++) {
        if ((idx != oracle_idx) && (objective_values[idx] == objective_values[oracle_idx])) {
            return true;
        }
    }

    return false;
}

void Prediction::createPredictions(uint32_t oracle_idx)
{
    uint32_t p_idx;
    uint32_t edge;

    // Round the paths with the probability of the traffic passing through them
    for (uint32_t r_idx = 0; r_idx < _model.nb_requests; r_idx++) {
        predictions[oracle_idx][r_idx].clear();
        predictions[oracle_idx][r_idx].resize(_model.graph.nb_edges, 0);

        std::discrete_distribution<uint32_t> ratio_distribution(_solution[r_idx].ratios.begin(), _solution[r_idx].ratios.end());
        p_idx = ratio_distribution(random_engine);

        for (uint32_t e_idx = 0; e_idx < _solution[r_idx].paths[p_idx].size(); e_idx++) {
            edge = _solution[r_idx].paths[p_idx][e_idx];
            predictions[oracle_idx][r_idx][edge] = 1;
        }
    }

    double cost = 0.0;
    double amount = 0.0;
    uint32_t i,j;

    // Calculate the objective value
    for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
        amount = 0.0;
        for (uint32_t r = 0; r < _model.nb_requests; r++) {
            amount += predictions[oracle_idx][r][e];
        }
        i = _model.graph.ID[e].i;
        j = _model.graph.ID[e].j;
        cost += _model.graph.A[i][j]->getCost(amount);
    }
    objective_values[oracle_idx] = cost;
}
