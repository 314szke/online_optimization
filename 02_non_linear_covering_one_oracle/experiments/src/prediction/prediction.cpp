#include "prediction.h"

#include <algorithm>
#include <limits>
#include <numeric>

#include "model/model.h"


Prediction::Prediction(const Config& config, Model& model, const DoubleMat_t& offline_paths) :
    _model(model),
    predictions(config.nb_oracles),
    objective_values(config.nb_oracles, 0.0),
    random_engine(rd())
{
    Model::RequestVec_t requests_copy = _model.requests;
    double obj_value = 0.0;

    for (uint32_t oracle_idx = 0; oracle_idx < config.nb_oracles; oracle_idx) {
        predictions[oracle_idx].resize(_model.graph.nb_edges, 0);

        while (oracleIsNotUnique(obj_value)) {
            if (std::next_permutation(requests_copy.begin(), requests_copy.end())) {
                createPredictions(requests_copy);
            } else {
                throw std::runtime_error("ERROR: number of oracles surpasses number of request permutations");
            }
        }
    }


}

bool Prediction::oracleIsNotUnique(double obj_value)
{
    if (obj_value == 0.0) {
        return false; // there is no prediction yet (first while loop call)
    }

    return std::find(objective_values.begin(), objective_values.end(), obj_value) != objective_values.end();
}

void Prediction::createPredictions(const Model::RequestVec_t& requests_copy)
{
    for (uint32_t r = 0; r < _model.nb_requests; r++) {

    }
}
