#pragma once

#include <string>

#include "config/argument_parser.h"
#include "config/config.h"
#include "model/model.h"
#include "model/cp_model.h"
#include "online/random_store.h"
#include "prediction/oracle.hpp"
#include "prediction/prediction.h"
#include "types/local_types.h"


struct ExecutionData_t {
    DoubleVec_t solution;
    Config* config;
    RandomStore* random_store;
    Model* model;
    Oracle* oracle;
};


class ExecutionManager {
public:
    ExecutionManager(const ArgumentParser& arg_parser);
    void run();

private:
    double evaluateSolution(std::string name, const DoubleVec_t& solution);
    double evaluateSolverSolution(const DoubleVec_t& solution, double eta, uint32_t oracle_idx, bool printSolution);
    void solveForAllEta(Prediction& oracles, struct ExecutionData_t* data);
    static void* threadSolve(void *thread_args);
    static void solveWithOracle(struct ExecutionData_t *data);

    Config config;
    Model model;
    CP_Model cp_model;

    double greedy_objective;
    double offline_objective;
};
