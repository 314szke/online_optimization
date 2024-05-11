#include "online_model.h"


OnlineModel::OnlineModel(const OfflineModel& model) :
    _offline_model(model)
{
    is_convex = _offline_model.isConvex();
    is_online = true;

    nb_variables = _offline_model.getNbVariables();
    nb_objective_variables = _offline_model.getNbObjectiveVariables();

    nb_constraints = _offline_model.getNbConstraints();
    nb_initial_constraints = _offline_model.getNbInitialConstraints();

    nb_batches = _offline_model.getNbConstraintBatches();
    batch_size = _offline_model.getConstraintBatchSize();

    c = _offline_model.getCost();
    dc.resize(nb_variables, 0.0);
    e = _offline_model.getCostExponent();

    // Initial constraints
    b.resize(1);
    A.resize(1);

    b[0].resize(nb_initial_constraints, 0.0);
    A[0].resize(nb_initial_constraints);

    const DoubleVec_t& b_lp = _offline_model.getBound(0);
    const DoubleMat_t& A_lp = _offline_model.getCoefficient(0);

    for (uint32_t j = 0; j < nb_initial_constraints; j++) {
        b[0][j] = b_lp[j];
        A[0][j].resize(nb_variables, 0.0);
        for (uint32_t i = 0; i < nb_variables; i++) {
            A[0][j][i] = A_lp[j][i];
        }
    }
    nb_revealed_constraints = nb_initial_constraints;
}

void OnlineModel::revealNextConstraints()
{
    time++;
    b.resize(getNbSteps());
    A.resize(getNbSteps());

    b[time].resize(batch_size, 0.0);
    A[time].resize(batch_size);

    const DoubleVec_t& b_lp = _offline_model.getBound(0);
    const DoubleMat_t& A_lp = _offline_model.getCoefficient(0);

    for (uint32_t j = 0; j < batch_size; j++) {
        b[time][j] = b_lp[(nb_revealed_constraints + j)];
        A[time][j].resize(nb_variables, 0.0);
        for (uint32_t i = 0; i < nb_variables; i++) {
            A[time][j][i] = A_lp[(nb_revealed_constraints + j)][i];
        }
    }
    nb_revealed_constraints += batch_size;
}

bool OnlineModel::isSatisfiedBy(const DoubleVec_t& x)
{
    const DoubleVec_t& b_lp = _offline_model.getBound(0);
    const DoubleMat_t& A_lp = _offline_model.getCoefficient(0);

    double value;
    for (uint32_t j = 0; j < nb_revealed_constraints; j++) {
        value = 0.0;
        for (uint32_t i = 0; i < _offline_model.getNbVariables(); i++) {
            value += A_lp[j][i] * x[i];
        }
        if (value < b_lp[j]) {
            if ((b_lp[j] - value) < 0.000001) {
                return true;
            }
            return false;
        }
    }
    return true;
}
