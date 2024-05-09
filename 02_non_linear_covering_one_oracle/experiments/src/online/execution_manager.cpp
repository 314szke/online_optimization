#include "execution_manager.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <pthread.h>

#include "offline/frank_wolfe.h"
#include "online/greedy_solver.h"
#include "online/solver.h"


ExecutionManager::ExecutionManager(const ArgumentParser& arg_parser) :
    config(arg_parser.config_file),
    model(arg_parser.data_file),
    cp_model(model),
    greedy_objective(0.0),
    offline_objective(0.0),
    save_to_file(arg_parser.modeIsConversion()),
    expert_file(arg_parser.expert_file)
{
    config.dimension = cp_model.max_dimension;
}

void ExecutionManager::run()
{
    if (model.nb_requests == 0) {
        return;
    }

    GreedySolver greedy_solver(config, model);
    const DoubleVec_t& greedy_solution = greedy_solver.solve();
    greedy_objective = evaluateSolution("GREEDY", greedy_solution);


    FrankWolfe fw_algo(config, cp_model);
    const DoubleVec_t& offline_solution = fw_algo.solve(greedy_solution);
    offline_objective = evaluateSolution("OFFLINE", offline_solution);


    const CP_Model::SolutionVec_t& formatted_solution = cp_model.getLastFormattedSolution();
    Prediction oracles(config, model, formatted_solution);
    for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
        Oracle* oracle = oracles.getOracle(oracle_idx);
        std::cout << std::endl << ">>> ORACLE (" << (oracle_idx + 1) << ") = " << oracle->objective_value << std::endl << std::flush;
        oracles.printFormattedSolution(oracle_idx);
    }


    RandomStore random_store(config.random_seed, config.random_store_size);

    struct ExecutionData_t data[oracles.getNbOracles()];
    for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
        data[oracle_idx].solution = DoubleVec_t(cp_model.getNbVariables(), 0.0);
        data[oracle_idx].config = &config;
        data[oracle_idx].random_store = &random_store;
        data[oracle_idx].model = &model;
        data[oracle_idx].oracle = oracles.getOracle(oracle_idx);
    }

    solveForAllEta(oracles, data);

    saveResultsToFile(greedy_solution, offline_solution, oracles, data);
}

double ExecutionManager::evaluateSolution(std::string name, const DoubleVec_t& solution)
{
    cp_model.setCurrentSolution(solution);
    double objective_value = cp_model.getObjectiveValue();
    std::cout << ">>> " << name << " = " << objective_value << std::endl << std::flush;
    cp_model.getFormattedSolution();
    cp_model.printFormattedSolution();
    std::cout << std::endl << std::flush;
    return objective_value;
}

double ExecutionManager::evaluateSolverSolution(const DoubleVec_t& solution, double eta, uint32_t oracle_idx, bool printSolution)
{
    cp_model.setCurrentSolution(solution);
    double objective_value = cp_model.getObjectiveValue();
    if (printSolution) {
        std::cout << std::endl << ">>> BEST ONLINE (eta  = " << eta << "; oracle = " << (oracle_idx + 1) << ") = " << objective_value << std::endl << std::flush;
        cp_model.getFormattedSolution();
        cp_model.printFormattedSolution();
    } else {
        std::cout << "\tONLINE (eta  = " << eta << "; oracle = " << (oracle_idx + 1) << ") = " << objective_value << std::endl << std::flush;
    }
    return objective_value;
}

void ExecutionManager::solveForAllEta(Prediction& oracles, struct ExecutionData_t* data) {
    pthread_t threads[oracles.getNbOracles()];
    void* status;

    best_eta = -1.0;
    best_oracle_idx = -1;
    double best_objective = std::numeric_limits<double>::infinity();

    std::ofstream out("result.txt");
    out << "Eta;CompRatio;Algo;Oracle" << std::endl;
    double greedy_comp_ratio = (offline_objective / greedy_objective);

    std::cout << std::endl << "ONLINE run the solver with different eta and oracles..." << std::endl << std::flush;

    double eta = 1.0;
    while (eta > 0) {
        config.update(eta);

        if (config.use_threads) {
            for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
                int return_code = pthread_create(&threads[oracle_idx], NULL, threadSolve, (void *)&(data[oracle_idx]));

                if (return_code) {
                    std::cout << "ERROR: thread creation failed!" << std::endl << std::flush;
                }
            }
            for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
                pthread_join(threads[oracle_idx], &status);
            }
        } else {
            std::cout << std::endl << "\tONLINE (eta = " << eta << ") is running" << std::endl << std::flush;
            for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
                std::cout << "\t\tSolving [" << (oracle_idx + 1) << "/" << oracles.getNbOracles() << "]" << std::endl << std::flush;
                solveWithOracle(&(data[oracle_idx]));
            }
        }

        for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
            double prediction_comp_ratio = (offline_objective / oracles.getOracle(oracle_idx)->objective_value);
            double online_objective = evaluateSolverSolution(data[oracle_idx].solution, eta, oracle_idx, false);
            double online_comp_ratio = (offline_objective / online_objective);

            // Save best scenario
            if (online_objective < best_objective) {
                best_objective = online_objective;
                best_eta = eta;
                best_oracle_idx = oracle_idx;
            }

            // Save results to a file
            out << eta << ";" << greedy_comp_ratio << ";greedy;" << (oracle_idx + 1) << std::endl;
            out << eta << ";" << prediction_comp_ratio << ";oracle " << (oracle_idx + 1) << ";" << (oracle_idx + 1) << std::endl;
            out << eta << ";" << online_comp_ratio << ";algo(oracle " << (oracle_idx + 1) << ")" << ";" << (oracle_idx + 1) << std::endl << std::flush;
        }

        eta = eta - config.eta_step;
    }
    out.close();

    config.update(best_eta);
    solveWithOracle(&(data[best_oracle_idx]));
    evaluateSolverSolution(data[best_oracle_idx].solution, best_eta, best_oracle_idx, true);
}

void* ExecutionManager::threadSolve(void *thread_args)
{
    struct ExecutionData_t *data;
    data = (struct ExecutionData_t *) thread_args;

    solveWithOracle(data);

    pthread_exit(NULL);
}

void ExecutionManager::solveWithOracle(struct ExecutionData_t *data)
{
    Solver solver(*(data->config), *(data->random_store), *(data->model));
    const DoubleVec_t& solution = solver.solve(*(data->oracle));
    for (uint32_t idx = 0; idx < solution.size(); idx++) {
        data->solution[idx] = solution[idx];
    }
}

void ExecutionManager::saveResultsToFile(const DoubleVec_t& greedy_solution, const DoubleVec_t& offline_solution, Prediction& oracles, struct ExecutionData_t* data) const
{
    if (!save_to_file) {
        return;
    }

    // Saves the routes per request. A line does not include all variables.
    std::ofstream f_out(expert_file);
    f_out << std::fixed << std::setprecision(2);

    f_out << 3 + oracles.getNbOracles() << std::endl; // greedy + offline + algo + oracles

    uint32_t segment_pointer = model.graph.nb_edges;
    for (uint32_t r = 0; r < model.nb_requests; r++) {
        // Greedy
        for (uint32_t idx = segment_pointer; idx < (segment_pointer + model.graph.nb_edges); idx++) {
            f_out << greedy_solution[idx] << " ";
        }
        f_out << std::endl;

        // Offline
        for (uint32_t idx = segment_pointer; idx < (segment_pointer + model.graph.nb_edges); idx++) {
            f_out << offline_solution[idx] << " ";
        }
        f_out << std::endl;

        // Algo
        for (uint32_t idx = segment_pointer; idx < (segment_pointer + model.graph.nb_edges); idx++) {
            f_out << data[best_oracle_idx].solution[idx] << " ";
        }
        f_out << std::endl;

        // Oracles
        for (uint32_t oracle_idx = 0; oracle_idx < oracles.getNbOracles(); oracle_idx++) {
            Oracle* oracle = oracles.getOracle(oracle_idx);
            for (uint32_t idx = 0; idx < model.graph.nb_edges; idx++) {
                f_out << oracle->predictions[r][idx] << " ";
            }
            f_out << std::endl;
        }

        segment_pointer += model.graph.nb_edges;
    }

    f_out.close();
}
