#pragma once

#include "base_model.h"
#include "model.h"
#include "solution.h"
#include "types/local_types.h"


class CP_Model : public BaseModel {
public:
    typedef std::vector<Solution> SolutionVec_t;

    CP_Model(Model& model);

    void setCurrentSolution(const DoubleVec_t& x);
    const SolutionVec_t& getFormattedSolution();
    void printFormattedSolution() const;

    uint32_t max_dimension;
private:
    uint32_t getID(uint32_t e, uint32_t r) const;
    Model& _model;
    SolutionVec_t formatted_solution;
};
