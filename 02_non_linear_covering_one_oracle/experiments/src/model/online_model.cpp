#include "online_model.h"

#include <iostream>


OnlineModel::OnlineModel(Model& model) :
    BaseModel(model,
        model.graph.nb_edges, // number of variables
        model.graph.nb_vertices), // number of constraints (one per vertex to preserve the flow)
    _model(model),
    time(0)
{}

void OnlineModel::next()
{
    if (time >= _model.nb_requests) {
        std::cout << "ERROR: OnlineModel::next() was called more than the number of requests!" << std::endl;
        return;
    }

    for (uint32_t c_idx = 0; c_idx < nb_constraints; c_idx++) {
        bounds[c_idx] = 0.0;
        for (uint32_t v_idx = 0; v_idx < nb_variables; v_idx++) {
            coefficients[c_idx][v_idx] = 0.0;
        }
    }

    uint32_t constr_idx = 0.0;

    // Flow preservation constraints
    for (uint32_t i = 0; i < _model.graph.nb_vertices; i++) {
        if (i == _model.requests[time].source) {
            bounds[constr_idx] = 1.0;
        } else if (i == _model.requests[time].target) {
            bounds[constr_idx] = -1.0;
        } else {
            bounds[constr_idx] = 0.0;
        }

        for (uint32_t j = 0; j < _model.graph.nb_vertices; j++) {
            if (i != j) {
                if (_model.graph.A[i][j].id != -1) {
                    coefficients[constr_idx][_model.graph.A[i][j].id] = 1.0;
                }
                if (_model.graph.A[j][i].id != -1) {
                    coefficients[constr_idx][_model.graph.A[j][i].id] = -1.0;
                }
            }
        }

        constr_idx++;
    }

    // Save previous solution
    previous_solution = solution;
    time++;
}

void OnlineModel::setCurrentSolution(const DoubleVec_t& x)
{
    current_solution = x;
    for (uint32_t i = 0 ; i < nb_variables; i++) {
        solution[i] = current_solution[i] + previous_solution[i];
    }
}
