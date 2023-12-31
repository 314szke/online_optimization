#pragma once

#include "config/config.h"
#include "model/convex_model.h"
#include "model/experts.h"
#include "model/offline_model.h"
#include "online/lp_solver.h"
#include "online/frank_wolfe.h"
#include "types/local_types.h"


// Convex Regularization
class CR_Algorithm {
public:
    CR_Algorithm(const OfflineModel& model, const Config& config, const Experts& experts);
    const DoubleVec_t& solve();
    double getObjectiveValue() const;
private:
    void verifySolution(const uint32_t t);
    void verifyWeights(const DoubleVec_t& w);
    void roundWeightsIfNeeded(DoubleVec_t& w);
    bool sumOfWeightsAreLessThanOne(const DoubleVec_t& w, uint32_t i);

    const OfflineModel& offline_model;
    const Experts& _experts;

    ConvexModel convex_model;
    FrankWolfe frank_wolfe;

    double online_objective;
    DoubleVec_t online_solution;
    DoubleVec_t previous_solution;
};
