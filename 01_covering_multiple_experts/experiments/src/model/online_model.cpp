#include "online_model.h"


OnlineModel::OnlineModel(const OfflineModel& model) :
    _offline_model(model),
    time(0)
{
    is_minimization = _offline_model.is_minimization;
    is_convex = _offline_model.is_convex;
    nb_variables = _offline_model.nb_variables;
    nb_constraints = _offline_model.nb_constraints;
    batch_size = _offline_model.batch_size;

    c = _offline_model.c;
    e = _offline_model.e;

    // Constraints at time = 0
    b.resize(1);
    A.resize(1);
    if (is_convex) {

    }
}

void OnlineModel::revealNextConstraints()
{
    time++;
    b.resize((time + 1));
    A.resize((time + 1));
    online_step++;

    const DoubleVec_t& b_lp = _offline_model.getBound(0);
    const DoubleMat_t& A_lp = _offline_model.getCoefficient(0);

    b[time].resize(1);
    b[time][0] = b_lp[(time - 1)];

    A[time].resize(1);
    A[time][0].resize(nb_variables);
    for (uint32_t i = 0; i < nb_variables; i++) {
        A[time][0][i] = A_lp[(time - 1)][i];
    }
}
