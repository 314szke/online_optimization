#pragma once

#include "base/parsed_object.h"
#include "config/config.h"
#include "offline_model.h"
#include "types/local_types.h"


class Experts : private ParsedObject {
public:
    Experts(const OfflineModel& model, const std::string& expert_file);

    uint32_t getNbExperts() const;
    const DoubleMat_t& getSolutions(const int64_t t) const;
    const DoubleVec_t& getAverageSolutions(const int64_t t) const;
    const DoubleVec_t& getObjectiveValues() const;
private:
    int32_t findInvalidSolution();

    const OfflineModel& off_model;

    uint32_t nb_experts;
    DoubleVec_t objective_values;
    DoubleMat_t avg_solutions;
    DoubleMatVec_t solutions;

    // For time = 0
    DoubleMat_t zero_solution;
};
