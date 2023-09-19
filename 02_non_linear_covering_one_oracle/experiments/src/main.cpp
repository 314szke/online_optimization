#include <fstream>
#include <iostream>
#include <limits>
#include <pthread.h>
#include <sstream>
#include <string>

#include "config/argument_parser.h"
#include "config/config.h"
#include "input/grid_generator.h"
#include "input/random_generator.h"
#include "model/model.h"
#include "model/cp_model.h"
#include "model/request.hpp"
#include "offline/frank_wolfe.h"
#include "online/greedy_solver.h"
#include "online/solver.h"
#include "online/random_store.h"
#include "prediction/oracle.hpp"
#include "prediction/prediction.h"


struct ThreadData_t {
    double* online_solution;
    Config* config;
    RandomStore* random_store;
    Model* model;
    Oracle* oracle;
};

void* solve(void *thread_args)
{
    struct ThreadData_t *local_data;
    local_data = (struct ThreadData_t *) thread_args;

    Solver solver(*(local_data->config), *(local_data->random_store), *(local_data->model));
    const DoubleVec_t& solution = solver.solve(*(local_data->oracle));
    for (uint32_t idx = 0; idx < solution.size(); idx++) {
        local_data->online_solution[idx] = solution[idx];
    }

    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    std::cout << std::fixed;
    std::cout.precision(2);

    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    arg_parser.parse();

    if (arg_parser.modeIsGeneration()) {
        if (arg_parser.generator_file.find("random") != std::string::npos) {
            RandomGenerator random_generator(arg_parser);
            random_generator.generate();
        } else {
            GridGenerator grid_generator(arg_parser);
            grid_generator.generate();
        }
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
    std::cout << std::endl << ">>> GREEDY = " << greedy_objective_value << std::endl << std::flush;
    cp_model.getFormattedSolution();
    cp_model.printFormattedSolution();


    // Calculate the optimal offline fractional solution
    FrankWolfe fw_algo(config, cp_model);
    const DoubleVec_t& offline_solution = fw_algo.solve(greedy_solution);
    cp_model.setCurrentSolution(offline_solution);
    double offline_objective_value = cp_model.getObjectiveValue();
    std::cout << std::endl << ">>> OFFLINE OPTIMAL FRACTIONAL = " << offline_objective_value << std::endl << std::flush;
    const CP_Model::SolutionVec_t& formatted_solution = cp_model.getFormattedSolution();
    cp_model.printFormattedSolution();


    // Create predictions
    Prediction oracles(config, model, formatted_solution);
    for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
        Oracle* oracle = oracles.getOracle(oracle_idx);
        std::cout << std::endl << ">>> ORACLE (" << (oracle_idx + 1) << ") = " << oracle->objective_value << std::endl << std::flush;
        oracles.printFormattedSolution(oracle_idx);
    }
    std::cout << std::endl << std::flush;


    // Calculate the online solution with different eta and oracle
    std::ofstream out("result.txt");
    out << "Eta;CompRatio;Algo" << std::endl;
    double greedy_comp_ratio = (offline_objective_value / greedy_objective_value);

    double best_eta = -1.0;
    int best_oracle_idx = -1;
    double best_objective_value = std::numeric_limits<double>::infinity();

    RandomStore random_store(config.random_seed, config.random_store_size);

    // Setup threads
    pthread_t threads[oracles.getNbOracles()];
    struct ThreadData_t thread_data[oracles.getNbOracles()];
    void* status;
    for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
        thread_data[oracle_idx].online_solution = new double[cp_model.getNbVariables()];
        thread_data[oracle_idx].config = &config;
        thread_data[oracle_idx].random_store = &random_store;
        thread_data[oracle_idx].model = &model;
        thread_data[oracle_idx].oracle = oracles.getOracle(oracle_idx);
    }

    double eta = 1.0;
    while (eta > 0) {
        config.update(eta);

        for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
            int return_code = pthread_create(&threads[oracle_idx], NULL, solve, (void *)&thread_data[oracle_idx]);

            if (return_code) {
                std::cout << "ERROR: thread creation failed!" << std::endl;
            }
        }

        for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
            pthread_join(threads[oracle_idx], &status);
        }

        for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
            Oracle* oracle = oracles.getOracle(oracle_idx);
            double prediction_comp_ratio = (offline_objective_value / oracle->objective_value);

            DoubleVec_t local_solution(cp_model.getNbVariables(), 0.0);
            for (uint32_t idx = 0; idx < cp_model.getNbVariables(); idx++) {
                local_solution[idx] = thread_data[oracle_idx].online_solution[idx];
            }

            cp_model.setCurrentSolution(local_solution);
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

    for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
        delete[] thread_data[oracle_idx].online_solution;
    }

    // Print best online scenario
    Oracle* oracle = oracles.getOracle(best_oracle_idx);
    config.update(best_eta);

    Solver solver(config, random_store, model);
    const DoubleVec_t& online_solution = solver.solve(*oracle);
    cp_model.setCurrentSolution(online_solution);
    double online_objective_value = cp_model.getObjectiveValue();

    std::cout << std::endl << ">>> BEST ONLINE (eta  = " << best_eta << "; oracle = " << (best_oracle_idx + 1) << ") = " << online_objective_value << std::endl;
    cp_model.getFormattedSolution();
    cp_model.printFormattedSolution();

    return 0;
}
