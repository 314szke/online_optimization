#include "online_model.h"


OnlineModel::OnlineModel(const OfflineModel& model) :
    offline_model(model),
    time(-1)
{
    nb_machines = offline_model.getNbMachines();
    nb_jobs = offline_model.getNbJobs();
    nb_variables = offline_model.getNbVariables();

    nb_constraints = 0;
    c = offline_model.getCost();
    b.resize(1);
    A.resize(1);
}

void OnlineModel::revealNextConstraint()
{
    time++;
    nb_constraints++;

    const DoubleVec_t& b_lp = offline_model.getBound();
    const DoubleMat_t& A_lp = offline_model.getCoefficient();

    b[0].push_back(b_lp[time]);
    A[0].push_back(DoubleVec_t(nb_variables, 0.0));
    for (uint32_t i = 0; i < nb_variables; i++) {
        A[0][time][i] = A_lp[time][i];
    }
}
