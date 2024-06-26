#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "config/argument_parser.h"
#include "config/config.h"
#include "input/input_generator.h"
#include "model/experts.h"
#include "model/internal_model.h"
#include "model/offline_model.h"
#include "model/solution.h"
#include "offline/lp_solver.h"
#include "online/cr_algorithm.h"
#include "online/frank_wolfe.h"
#include "online/std_algorithm.h"
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
    OfflineModel offline_model(arg_parser.data_file, arg_parser.is_convex);
    DummyExpert dummy_expert(offline_model);
    Experts experts(offline_model, dummy_expert, config, arg_parser.expert_file);


    // Optimal Offline Solution
    DoubleVec_t optimal_solution;
    LP_Solver lp_solver(offline_model, config.gurobi_verbosity);
    if (arg_parser.is_convex) {
        FrankWolfe frank_wolfe(config, offline_model, lp_solver);
        optimal_solution = frank_wolfe.solve(dummy_expert.getFinalSolution());
    } else {
        optimal_solution = lp_solver.solve(offline_model.getCost());
    }
    Solution::RoundSolutionIfNeeded(offline_model, optimal_solution, offline_model.getNbConstraints());
    const double optimal_objective = offline_model.getObjectiveValue(optimal_solution);
    print_solution("OPT Offline", optimal_objective, optimal_solution);


    // Online solution with Multiplicative Weight Update
    DoubleVec_t std_solution;
    DoubleMat_t std_solutions;
    if (arg_parser.is_convex) {
        std_solutions = experts.getExpert(0);
        std_solution = std_solutions.back();
    } else {
        STD_Algorithm std_algorithm(offline_model);
        std_solution = std_algorithm.solve();
        std_solutions = std_algorithm.getSubSolutions();
    }
    Solution::RoundSolutionIfNeeded(offline_model, std_solution, offline_model.getNbConstraints());
    const double std_objective = offline_model.getObjectiveValue(std_solution);
    print_solution("MWU Online", std_objective, std_solution);


    // Online Solution with convex regularization 1st layer
    CR_Algorithm cr_algorithm(offline_model, config, experts);
    DoubleVec_t online_solution = cr_algorithm.solve();
    Solution::RoundSolutionIfNeeded(offline_model, online_solution, offline_model.getNbConstraints());


    // Save results to do the second layer of the algo
    const DoubleMat_t cr_solutions = cr_algorithm.getSubSolutions();
    std::string tmp_file_name = "tmp.txt";
    std::ofstream tmp_save_file(tmp_file_name, std::ofstream::out | std::ofstream::trunc);
    tmp_save_file << "2" << std::endl;
    tmp_save_file << std::fixed << std::setprecision(15);
    for (uint32_t j = 0; j < offline_model.getNbConstraints(); j++) {
        for (uint32_t i = 0; i < offline_model.getNbVariables(); i++) {
            tmp_save_file << cr_solutions[j][i] << " ";
        }
        tmp_save_file << std::endl;
        for (uint32_t i = 0; i < offline_model.getNbVariables(); i++) {
            tmp_save_file << std_solutions[j][i] << " ";
        }
        tmp_save_file << std::endl;
    }
    tmp_save_file.close();


    // Second layer online solution with convex regularization
    Experts second_experts(offline_model, dummy_expert, config, tmp_file_name);
    CR_Algorithm second_cr_algorithm(offline_model, config, second_experts);
    DoubleVec_t second_online_solution = second_cr_algorithm.solve();
    Solution::RoundSolutionIfNeeded(offline_model, second_online_solution, offline_model.getNbConstraints());
    const double second_online_objective = second_cr_algorithm.getObjectiveValue();
    print_solution("Our Algo", second_online_objective, second_online_solution);


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
