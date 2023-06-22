#include "cp_model.h"

#include <iostream>


CP_Model::CP_Model(Model& model) :
    BaseModel(model, // + receives number of variables and number of constraints
        (model.graph.nb_edges + (model.graph.nb_edges * model.nb_requests)), // y_e and x^t_e
        (model.graph.nb_edges // connect y with x
        + ((model.graph.nb_vertices - 2) * model.nb_requests) // flow preservation for each vertex (except source and target) for each request
        + (2 * model.nb_requests) // flow for source and target
        )),
    max_dimension(0.0),
    _model(model)
{
    uint32_t constr_idx = 0.0;

    // Constraint connecting y with x
    for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
        bounds[constr_idx] = 0.0;
        coefficients[constr_idx][e] = 1.0; // y_e
        for (uint32_t r = 0; r < _model.nb_requests; r++) {
            coefficients[constr_idx][getID(e, r)] = -1.0; // x^r_e
        }
        constr_idx++;
    }

    // Flow preservation constraints on non-source and non-target vertices
    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        for (uint32_t i = 0; i < _model.graph.nb_vertices; i++) {
            if (i != _model.requests[r].source && i != _model.requests[r].target) {
                bounds[constr_idx] = 0.0;

                for (uint32_t j = 0; j < _model.graph.nb_vertices; j++) {
                    if (_model.graph.A[i][j]) {
                        coefficients[constr_idx][getID(_model.graph.A[i][j]->id, r)] = 1.0;
                    }
                    if (_model.graph.A[j][i]) {
                        coefficients[constr_idx][getID(_model.graph.A[j][i]->id, r)] = -1.0;
                    }
                }
                constr_idx++;
            }
        }
    }

    // Source constraints
    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        bounds[constr_idx] = 1.0;

        for (uint32_t j = 0; j < _model.graph.nb_vertices; j++) {
            if (j != _model.requests[r].source) {

                uint32_t i = _model.requests[r].source;
                if (_model.graph.A[i][j]) {
                    coefficients[constr_idx][getID(_model.graph.A[i][j]->id, r)] = 1.0;
                }
            }
        }
        constr_idx++;
    }

    // Target constraints
    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        bounds[constr_idx] = 1.0;

        for (uint32_t i = 0; i < _model.graph.nb_vertices; i++) {
            if (i != _model.requests[r].target) {

                uint32_t j = _model.requests[r].target;
                if (_model.graph.A[i][j]) {
                    coefficients[constr_idx][getID(_model.graph.A[i][j]->id, r)] = 1.0;
                }
            }
        }
        constr_idx++;
    }

    // Calculate the maximum number of non-zero coefficients in the original (flow-type) constraints
    uint32_t coeff_counter = 0.0;
    for (uint32_t c_idx = _model.graph.nb_edges; c_idx < nb_constraints; c_idx++) {
        coeff_counter = 0.0;
        for (uint32_t v_idx = 0.0; v_idx < nb_variables; v_idx++) {
            if (coefficients[c_idx][v_idx] != 0) {
                coeff_counter++;
            }
        }

        if (coeff_counter > max_dimension) {
            max_dimension = coeff_counter;
        }
    }
}

void CP_Model::setCurrentSolution(const DoubleVec_t& x)
{
    solution = x;
}

const CP_Model::SolutionVec_t& CP_Model::getFormattedSolution()
{
    formatted_solution.clear();
    DoubleVec_t edges(_model.graph.nb_edges, 0.0);

    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
            edges[e] = solution[getID(e, r)];
        }
        formatted_solution.push_back(Solution(_model, edges, _model.requests[r].source, _model.requests[r].target));
    }

    return formatted_solution;
}

void CP_Model::printFormattedSolution() const
{
    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        std::cout << "Request " << (r+1) << " (v" << _model.requests[r].source << " -> v" << _model.requests[r].target << "):" << std::endl;
        for (uint32_t p = 0; p < formatted_solution[r].paths.size(); p++) {
            std::cout << "\tPath " << (p+1) << " with ratio (" << formatted_solution[r].ratios[p] << "): [e";
            for (uint32_t idx = 0; idx < (formatted_solution[r].paths[p].size() - 1); idx++) {
                std::cout << formatted_solution[r].paths[p][idx] << ", e";
            }
            std::cout << formatted_solution[r].paths[p].back() << "]" << std::endl;
        }
    }
    std::cout << std::flush;
}

uint32_t CP_Model::getID(uint32_t e, uint32_t r) const
{
    return (_model.graph.nb_edges) + (e + (r * _model.graph.nb_edges));
}
