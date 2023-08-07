#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "config/argument_parser.h"
#include "config/config.h"
#include "input/input_generator.h"
#include "model/convex_model.h"
#include "model/experts.h"
#include "model/offline_model.h"
#include "model/solution.h"
#include "offline/offline_solver.h"
#include "online/cr_algorithm.h"
#include "visualization/print.hpp"


int main(int argc, char** argv)
{
    // Load the model and the configuration
    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    arg_parser.parse();


    // Generate model
    if (arg_parser.modeIsGeneration()) {
        InputGenerator input_generator(arg_parser);
        input_generator.generate();
        return 0;
    }


    // Set up the model
    Config config(arg_parser.config_file);
    OfflineModel offline_model(arg_parser.data_file);


    // Optimal Offline Solution
    OfflineSolver offline_solver(offline_model, config.gurobi_verbosity);
    DoubleVec_t optimal_solution = offline_solver.solve();
    Solution::RoundSolutionIfNeeded(offline_model, optimal_solution, offline_model.getNbConstraints());
    const double optimal_objective = offline_solver.getObjectiveValue();
    print_solution("OPT Offline", optimal_objective, optimal_solution);


    // Online Solution with convex regularization 1st layer
    Experts experts(offline_model, arg_parser.expert_file);
    CR_Algorithm cr_algorithm(offline_model, config, experts);
    DoubleVec_t online_solution = cr_algorithm.solve();
    Solution::RoundSolutionIfNeeded(offline_model, online_solution, offline_model.getNbConstraints());
    const double online_objective = offline_model.getObjectiveValue(online_solution);
    print_solution("CR Algo", online_objective, online_solution);


    // Save result
    const DoubleVec_t& expert_objective_values = experts.getObjectiveValues();
    double sum_experts = 0.0;
    for (uint32_t k = 0; k < experts.getNbExperts(); k++) {
        std::cout << "Expert " << (k+1) << " = " << expert_objective_values[k] << std::endl;
        sum_experts += expert_objective_values[k];
    }
    std::cout << "Avg of experts = " << sum_experts / (double) experts.getNbExperts() << std::endl;

    return 0;
}
