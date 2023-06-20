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

    bounds = DoubleVec_t(nb_constraints, 0.0);
    coefficients.resize(nb_constraints);
    for (uint32_t idx = 0; idx < nb_constraints; idx++) {
        coefficients[idx].resize(nb_variables, 0.0);
    }

    uint32_t constr_idx = 0.0;

    // Flow preservation constraints on non-source and non-target vertices
    for (uint32_t i = 0; i < _model.graph.nb_vertices; i++) {
        if (i != _model.requests[time].source && i != _model.requests[time].target) {
            for (uint32_t j = 0; j < _model.graph.nb_vertices; j++) {
                if (j != _model.requests[time].source && j != _model.requests[time].target) {

                    if (_model.graph.A[i][j]) {
                        coefficients[constr_idx][_model.graph.A[i][j]->id] = 1.0;
                    }
                    if (_model.graph.A[j][i]) {
                        coefficients[constr_idx][_model.graph.A[j][i]->id] = -1.0;
                    }
                }
            }
            constr_idx++;
        }
    }

    // Source constraints
    bounds[constr_idx] = 1.0;
    for (uint32_t j = 0; j < _model.graph.nb_vertices; j++) {
        if (j != _model.requests[time].source) {

            uint32_t i = _model.requests[time].source;
            if (_model.graph.A[i][j]) {
                coefficients[constr_idx][_model.graph.A[i][j]->id] = 1.0;
            }
        }
    }
    constr_idx++;

    // Target constraints
    bounds[constr_idx] = 1.0;
    for (uint32_t i = 0; i < _model.graph.nb_vertices; i++) {
        if (i != _model.requests[time].target) {

            uint32_t j = _model.requests[time].target;
            if (_model.graph.A[i][j]) {
                coefficients[constr_idx][_model.graph.A[i][j]->id] = 1.0;
            }
        }
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
