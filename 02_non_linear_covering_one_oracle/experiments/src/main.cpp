#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

#include "config/argument_parser.h"
#include "config/config.h"
#include "input/input_generator.h"
#include "model/model.h"
#include "model/cp_model.h"
#include "model/request.hpp"
#include "offline/frank_wolfe.h"
#include "online/greedy_solver.h"
#include "online/solver.h"
#include "prediction/oracle.hpp"
#include "prediction/prediction.h"


int main(int argc, char** argv)
{
    std::cout << std::fixed;
    std::cout.precision(2);

    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    arg_parser.parse();

    if (arg_parser.modeIsGeneration()) {
        InputGenerator input_generator(arg_parser);
        input_generator.generate();
        return 0;
    }

    Config config(arg_parser.config_file);
    Model model(arg_parser.data_file);
    CP_Model cp_model(model);
    config.dimension = cp_model.max_dimension;


    if (model.nb_requests == 0) {
        return 0;
    }


    // Calculate the online solution with a greedy algorithm
    GreedySolver greedy_solver(config, model);
    const DoubleVec_t& greedy_solution = greedy_solver.solve();
    cp_model.setCurrentSolution(greedy_solution);
    double greedy_objective_value = cp_model.getObjectiveValue();
    std::cout << ">>> GREEDY = " << greedy_objective_value << std::endl << std::flush;
    cp_model.getFormattedSolution();
    cp_model.printFormattedSolution();


    // Calculate the optimal offline fractional solution
    FrankWolfe fw_algo(config, cp_model);
    const DoubleVec_t& offline_solution = fw_algo.solve(greedy_solution);
    cp_model.setCurrentSolution(offline_solution);
    double offline_objective_value = cp_model.getObjectiveValue();
    std::cout << ">>> OFFLINE OPTIMAL FRACTIONAL = " << offline_objective_value << std::endl << std::flush;
    const CP_Model::SolutionVec_t& formatted_solution = cp_model.getFormattedSolution();
    cp_model.printFormattedSolution();


    // Create predictions
    Prediction oracles(config, model, formatted_solution);
    for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
        const Oracle& oracle = oracles.getOracle(oracle_idx);
        std::cout << ">>> ORACLE (" << (oracle_idx + 1) << ") = " << oracle.objective_value << std::endl << std::flush;
        oracles.printFormattedSolution(oracle_idx);
    }


    // Calculate the online solution with different eta and oracle
    std::ofstream out("result.txt");
    out << "Eta;CompRatio;Algo" << std::endl;
    double greedy_comp_ratio = (offline_objective_value / greedy_objective_value);

    double best_eta = -1.0;
    int best_oracle_idx = -1;
    double best_objective_value = std::numeric_limits<double>::infinity();

    double eta = 1.0;
    Solver solver(config, model);

    while (eta > 0) {
        config.update(eta);

        for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
            const Oracle& oracle = oracles.getOracle(oracle_idx);
            double prediction_comp_ratio = (offline_objective_value / oracle.objective_value);

            std::cout << "ONLINE (eta  = " << eta << "; oracle = " << (oracle_idx + 1) << ") is running" << std::endl << std::flush;
            const DoubleVec_t& online_solution = solver.solve(oracle);
            cp_model.setCurrentSolution(online_solution);
            double online_objective_value = cp_model.getObjectiveValue();
            double online_comp_ratio = (offline_objective_value / online_objective_value);
            std::cout << ">>> ONLINE (eta  = " << eta << "; oracle = " << (oracle_idx + 1) << ") = " << online_objective_value << std::endl << std::flush;

            // Save best scenario
            if (online_objective_value < best_objective_value) {
                best_objective_value = online_objective_value;
                best_eta = eta;
                best_oracle_idx = oracle_idx;
            }

            // Save results to a file
            if (oracle_idx == 0) { // only print once
                out << eta << ";" << greedy_comp_ratio << ";greedy" << std::endl;
            }
            out << eta << ";" << prediction_comp_ratio << ";oracle " << (oracle_idx + 1) << std::endl;
            out << eta << ";" << online_comp_ratio << ";algo(oracle " << (oracle_idx + 1) << ")" << std::endl << std::flush;
        }

        eta = eta - config.eta_step;
    }
    out.close();

    // Print best online scenario
    const Oracle& oracle = oracles.getOracle(best_oracle_idx);
    config.update(best_eta);

    const DoubleVec_t& online_solution = solver.solve(oracle);
    cp_model.setCurrentSolution(online_solution);
    double online_objective_value = cp_model.getObjectiveValue();

    std::cout << ">>> BEST ONLINE (eta  = " << best_eta << "; oracle = " << (best_oracle_idx + 1) << ") = " << online_objective_value << std::endl;
    cp_model.getFormattedSolution();
    cp_model.printFormattedSolution();

    return 0;
}
