#pragma once

#include "base/parsed_object.h"
#include "config/config.h"
#include "offline_model.h"
#include "types/local_types.h"


class Experts : private ParsedObject {
public:
    Experts(const OfflineModel& model, const Config& config, const std::string& expert_file);

    uint32_t getNbExperts() const;
    const DoubleMat_t& getSolutions(const int64_t t) const;
    const DoubleMat_t& getTightSolutions(const int64_t t) const;
    const DoubleVec_t& getAverageSolutions(const int64_t t) const;
    const DoubleVec_t& getObjectiveValues() const;
private:
    void tightenSolutions();
    double getConstraintValue(uint32_t k, uint32_t t1, uint32_t t2);
    bool constraintIsNotTight(double value, double limit);
    void downscaleSolution(double value, uint32_t k, uint32_t t);
    void setTightSolution(uint32_t k, uint32_t t);
    void verifySolutionFeasibility(double value, double limit, uint32_t k, uint32_t t);
    void downscaleSolutionMultipleTimes(uint32_t k, uint32_t t);
    void setTightSolutionInTheInterval(uint32_t k, uint32_t t);

    int32_t findInvalidSolution(const DoubleMatVec_t& sol_vec);

    const OfflineModel& off_model;
    double epsilon;
    const DoubleVec_t& b;
    const DoubleMat_t& A;

    uint32_t nb_experts;
    DoubleVec_t objective_values;
    DoubleMat_t avg_solutions;
    DoubleMatVec_t solutions;
    DoubleMatVec_t tight_solutions;

    // For time = 0
    DoubleMat_t zero_solution;
};
