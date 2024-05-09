#include "dummy_expert.h"

#include "model/online_model.h"
#include "model/solution.h"
#include "offline/lp_solver.h"


DummyExpert::DummyExpert(const OfflineModel& off_model) :
    init_value(0.0001),
    dummy_solution(off_model.getNbConstraints() + 1)
{
    OnlineModel on_model(off_model);
    LP_Solver on_solver(on_model, 0);
    dummy_solution[0] = DoubleVec_t(off_model.getNbVariables(), init_value);

    for (uint32_t j = 1; j < (off_model.getNbConstraints() + 1); j++) {
        dummy_solution[j] = dummy_solution[j-1];

        on_model.revealNextConstraints();
        on_solver.addNewConstraints(j);
        DoubleVec_t solution = on_solver.solve();
        Solution::RoundSolutionIfNeeded(off_model, solution, j);

        for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
            if (dummy_solution[j][i] < solution[i]) {
                dummy_solution[j][i] = solution[i];
            }
        }
    }
}

const DoubleVec_t& DummyExpert::getSolution(const uint32_t time) const
{
    if (time >= dummy_solution.size()) {
        return dummy_solution[0];
    }
    return dummy_solution[time];
}
