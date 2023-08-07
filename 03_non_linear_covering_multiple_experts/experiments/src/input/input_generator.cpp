#include "input/input_generator.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <random>

#include "model/offline_model.h"
#include "model/online_model.h"
#include "model/solution.h"
#include "offline/offline_solver.h"
#include "types/local_types.h"


InputGenerator::InputGenerator(const ArgumentParser& arg_parser) :
    _arg_parser(arg_parser),
    nb_machines(0),
    nb_jobs(0),
    min_processing_time(0),
    max_processing_time(0),
    nb_experts(0),
    nb_perfect_expert(0),
    nb_online_expert(0),
    nb_adversary_expert(0),
    nb_random_expert(0)
{
    f_in.open(_arg_parser.generator_file);

    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.generator_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    readParameter(nb_machines);
    readParameter(nb_jobs);
    readParameter(min_processing_time);
    readParameter(max_processing_time);
    readParameter(nb_perfect_expert);
    readParameter(nb_online_expert);
    readParameter(nb_adversary_expert);
    readParameter(nb_random_expert);

    nb_experts = nb_perfect_expert + nb_online_expert + nb_adversary_expert + nb_random_expert;

    f_in.close();
}

void InputGenerator::generate()
{
    generateModel();
    generateExperts();
    generateConfigFile();
}

void InputGenerator::generateModel()
{
    std::ofstream f_out(_arg_parser.data_file);

    if (! f_out.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.data_file << " could not be created!" << std::endl;
        throw std::runtime_error(message.str());
    }

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> proc_time_dist(min_processing_time, max_processing_time);

    double random_double;

    // Problem parameters
    f_out << nb_machines << std::endl;
    f_out << nb_jobs << std::endl;

    // Generate the objective function
    f_out << "min " ;
    for (uint32_t j = 0; j < nb_jobs; j++) {
        for (uint32_t i = 0; i < nb_machines; i++) {
            random_double = proc_time_dist(engine);
            f_out << random_double << " x" << i << "_" << j << " + ";
        }
    }
    uint32_t position = f_out.tellp();
    f_out.seekp(position - 2);
    f_out << std::endl;

    // Generate the constraints
    for (uint32_t j = 0; j < nb_jobs; j++) {
        for (uint32_t i = 0; i < (nb_machines - 1); i++) {
            f_out << "1 x" << i << "_" << j << " + ";
        }
        f_out << "1 x" << (nb_machines - 1)  << "_" << j << " >= 1" << std::endl;
    }

    f_out.close();
}

void InputGenerator::generateExperts()
{
    std::ofstream f_out(_arg_parser.expert_file);

    if (! f_out.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.expert_file << " could not be created!" << std::endl;
        throw std::runtime_error(message.str());
    }

    f_out << std::fixed << std::setprecision(15);
    f_out << nb_experts << std::endl;

    OfflineModel offline_model(_arg_parser.data_file);
    OfflineSolver offline_solver(offline_model, 0);
    DoubleVec_t optimal_solution = offline_solver.solve();
    Solution::RoundSolutionIfNeeded(offline_model, optimal_solution, offline_model.getNbConstraints());

    OnlineModel online_model(offline_model);

    DoubleMat_t perfect_solution(nb_perfect_expert);
    for (uint32_t k = 0; k < nb_perfect_expert; k++) {
        perfect_solution[k].resize(offline_model.getNbVariables(), 0.0);
    }

    DoubleMat_t min_solution(nb_online_expert);
    for (uint32_t k = 0; k < nb_online_expert; k++) {
        min_solution[k].resize(offline_model.getNbVariables(), 0.0);
    }

    DoubleMat_t max_solution(nb_adversary_expert);
    for (uint32_t k = 0; k < nb_adversary_expert; k++) {
        max_solution[k].resize(offline_model.getNbVariables(), 0.0);
    }

    DoubleMat_t random_solution(nb_random_expert);
    for (uint32_t k = 0; k < nb_random_expert; k++) {
        random_solution[k].resize(offline_model.getNbVariables(), 0.0);
    }

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<uint32_t> var_dist(0, (offline_model.getNbMachines() - 1));

    for (uint32_t j = 0; j < offline_model.getNbConstraints(); j++) {
        // Perfect experts
        for (uint32_t k = 0; k < nb_perfect_expert; k++) {
            for (uint32_t i = (j * offline_model.getNbMachines()); i < ((j+1) * offline_model.getNbMachines()); i++) {
                perfect_solution[k][i] = optimal_solution[i];
            }
            for (uint32_t i = 0; i < (offline_model.getNbVariables() - 1); i++) {
                f_out << perfect_solution[k][i] << " ";
            }
            f_out << perfect_solution[k][(offline_model.getNbVariables() - 1)] << std::endl;
        }

        online_model.revealNextConstraint();
        OfflineSolver online_solver(online_model, 0);
        DoubleVec_t solution = online_solver.solve();
        Solution::RoundSolutionIfNeeded(offline_model, solution, (j+1));

        // Online experts
        for (uint32_t k = 0; k < nb_online_expert; k++) {
            for (uint32_t i = (j * offline_model.getNbMachines()); i < ((j+1) * offline_model.getNbMachines()); i++) {
                min_solution[k][i] = solution[i];
            }
            for (uint32_t i = 0; i < (offline_model.getNbVariables() - 1); i++) {
                f_out << min_solution[k][i] << " ";
            }
            f_out << min_solution[k][(offline_model.getNbVariables() - 1)] << std::endl;
        }

        // Adversary experts
        const DoubleVec_t& c = offline_model.getCost();
        uint32_t max_idx = (j * offline_model.getNbMachines());
        for (uint32_t i = (j * offline_model.getNbMachines()); i < ((j+1) * offline_model.getNbMachines()); i++) {
            if (c[max_idx] < c[i]) {
                max_idx = i;
            }
        }

        for (uint32_t k = 0; k < nb_adversary_expert; k++) {
            max_solution[k][max_idx] = 1;
            for (uint32_t i = 0; i < (offline_model.getNbVariables() - 1); i++) {
                f_out << max_solution[k][i] << " ";
            }
            f_out << max_solution[k][(offline_model.getNbVariables() - 1)] << std::endl;
        }

        // Random experts
        uint32_t random_uint;
        uint32_t var_idx;

        for (uint32_t k = 0; k < nb_random_expert; k++) {
            random_uint = var_dist(engine);
            var_idx = (j * offline_model.getNbMachines()) + random_uint;
            random_solution[k][var_idx] = 1;

            for (uint32_t i = 0; i < (offline_model.getNbVariables() - 1); i++) {
                f_out << random_solution[k][i] << " ";
            }
            f_out << random_solution[k][(offline_model.getNbVariables() - 1)] << std::endl;
        }
    }

    f_out.close();
}

void InputGenerator::generateConfigFile()
{
    std::ofstream f_out(_arg_parser.config_file);

    if (! f_out.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << _arg_parser.config_file << " could not be created!" << std::endl;
        throw std::runtime_error(message.str());
    }

    f_out << "time_horizon = 20                                # number of iterations in the Frank-Wolfe algorithm" << std::endl;
    f_out << "max_search_iter = 15                             # the maximum number of eta search steps in the Frank-Wolfe algorithm" << std::endl;
    f_out << "max_distance = 0.000001                          # maximum distance between x and v before terminating the Frank-Wolfe algorithm" << std::endl;
    f_out << "gurobi_verbosity = 0                             # if set to 1 gurobi will print a lot of messages" << std::endl;

    f_out.close();
}
