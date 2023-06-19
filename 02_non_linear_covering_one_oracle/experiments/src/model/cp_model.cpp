#include "cp_model.h"


CP_Model::CP_Model(Model& model) :
    _model(model),
    nb_variables(model.graph.nb_edges + (model.graph.nb_edges * model.nb_requests)),
    nb_constraints(_model.graph.nb_edges + 3 * _model.nb_requests),
    costs(nb_variables, 0.0),
    coefficients(nb_constraints)
{
    // Constraint connecting y with x
    for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
        bounds.push_back(0);
        coefficients[e].resize(nb_variables, 0.0);
        coefficients[e][e] = 1.0; // y_e
        for (uint32_t r = 0; r < _model.nb_requests; r++) {
            coefficients[e][getID(e, r)] = 1.0; // x^r_e
        }
    }

    // Flow preservation constraints
    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        bounds.push_back(0);
    }

    // Source constraints
    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        bounds.push_back(1);
    }

    // Target constraints
    for (uint32_t r = 0; r < _model.nb_requests; r++) {
        bounds.push_back(1);
    }
}

double CP_Model::getObjectiveValue(const DoubleVec_t& x)
{
    double amount = 0.0;
    double cost = 0.0;
    uint32_t i,j;

    for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
        amount = 0.0;
        for (uint32_t r = 0; r < nb_requests; r++) {
            amount += x[getID(e, r)];
        }
        i = _model.graph.ID[e].i;
        j = _model.graph.ID[e].j;
        cost += _model.graph.A[i][j]->getCost(amount);
    }

    return cost;
}

uint32_t CP_Model::getNbVariables() const
{
    return nb_variables;
}

const DoubleVec_t& CP_Model::getCostVector(const DoubleVec_t& x)
{
    double amount = 0.0;
    uint32_t i,j;

    for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
        amount = 0.0;
        for (uint32_t r = 0; r < nb_requests; r++) {
            amount += x[getID(e, r)];
        }
        i = _model.graph.ID[e].i;
        j = _model.graph.ID[e].j;
        costs[e] = _model.graph.A[i][j]->getDerivative(amount);
    }
    return costs;
}

const DoubleVec_t& getBounds()
{

}

const DoubleMat_t& getCoefficients()
{

}

uint32_t CP_Model::getID(uint32_t e, uint32_t r) const
{
    return (_model.graph.nb_edges - 1) + (e + (r * _model.graph.nb_edges));
}