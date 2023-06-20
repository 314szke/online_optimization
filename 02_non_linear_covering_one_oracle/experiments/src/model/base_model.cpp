#include "base_model.h"


BaseModel::BaseModel(Model& model, uint32_t n, uint32_t m) :
    _model(model),
    nb_variables(n),
    nb_constraints(m),
    solution(nb_variables, 0.0),
    current_solution(nb_variables, 0.0),
    previous_solution(nb_variables, 0.0),
    costs(nb_variables, 0.0),
    bounds(nb_constraints, 0.0),
    coefficients(nb_constraints, DoubleVec_t(nb_variables, 0.0))
{}

double BaseModel::getObjectiveValue()
{
    double cost = 0.0;
    uint32_t i,j;

    for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
        i = _model.graph.ID[e].i;
        j = _model.graph.ID[e].j;
        cost += _model.graph.A[i][j]->getCost(solution[e]);
    }

    return cost;
}

uint32_t BaseModel::getNbVariables() const
{
    return nb_variables;
}

uint32_t BaseModel::getNbConstraints() const
{
    return nb_constraints;
}

DoubleVec_t& BaseModel::getCostVector()
{
    uint32_t i,j;

    for (uint32_t e = 0; e < _model.graph.nb_edges; e++) {
        i = _model.graph.ID[e].i;
        j = _model.graph.ID[e].j;
        costs[e] = _model.graph.A[i][j]->getDerivative(solution[e]);
    }

    return costs;
}

DoubleVec_t& BaseModel::getBounds()
{
    return bounds;
}

DoubleMat_t& BaseModel::getCoefficients()
{
    return coefficients;
}

