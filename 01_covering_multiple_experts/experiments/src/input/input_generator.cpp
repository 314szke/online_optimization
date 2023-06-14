#include "input/input_generator.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <random>

#include "model/offline_model.h"
#include "model/online_model.h"
#include "model/solution.h"
#include "offline/lp_solver.h"
#include "types/local_types.h"


static const std::string MODEL_FILENAME = "new_input.lp";
static const std::string EXPERT_FILENAME = "new_experts.pred";


InputGenerator::InputGenerator(const std::string& config_file) :
    nb_variables(0),
    nb_constraints(0),
    minimization(1),
    covering(1),
    min_cost(0.0),
    max_cost(0.0),
    min_coefficient(0.0),
    max_coefficient(0.0),
    min_nb_zero_coefficient(0),
    max_nb_zero_coefficient(0),
    min_bound(0.0),
    max_bound(0.0),
    nb_experts(0),
    nb_perfect_expert(0),
    nb_online_expert(0),
    nb_adversary_expert(0),
    nb_random_expert(0),
    min_online_error(0.0),
    max_online_error(0.0),
    min_random_increase(0.0),
    max_random_increase(0.0)
{
    f_in.open(config_file);

    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << config_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    readParameter(nb_variables);
    readParameter(nb_constraints);
    readParameter(minimization);
    readParameter(covering);
    readParameter(min_cost);
    readParameter(max_cost);
    readParameter(min_coefficient);
    readParameter(max_coefficient);
    readParameter(min_nb_zero_coefficient);
    readParameter(max_nb_zero_coefficient);
    readParameter(min_bound);
    readParameter(max_bound);
    readParameter(nb_perfect_expert);
    readParameter(nb_online_expert);
    readParameter(nb_adversary_expert);
    readParameter(nb_random_expert);
    readParameter(min_online_error);
    readParameter(max_online_error);
    readParameter(min_random_increase);
    readParameter(max_random_increase);

    nb_experts = nb_perfect_expert + nb_online_expert + nb_adversary_expert + nb_random_expert;
    f_in.close();
}

void InputGenerator::generate()
{
    generateModel();
    generateExperts();
}

void InputGenerator::generateModel()
{
    std::ofstream f_out(MODEL_FILENAME);

    if (! f_out.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << MODEL_FILENAME << " could not be created!" << std::endl;
        throw std::runtime_error(message.str());
    }

    f_out << nb_variables << std::endl;
    f_out << nb_constraints << std::endl;

    if (minimization > 0) {
        f_out << "min " ;
    } else {
        f_out << "max " ;
    }

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> cost_dist(min_cost, max_cost);
    std::uniform_real_distribution<double> coefficient_dist(min_coefficient, max_coefficient);
    std::uniform_real_distribution<double> bound_dist(min_bound, max_bound);
    std::uniform_int_distribution<uint32_t> zero_var_dist(min_nb_zero_coefficient, max_nb_zero_coefficient);

    double random_double;
    uint32_t random_uint;

    UIntVec_t indices(nb_variables);
    for (uint32_t i = 0; i < nb_variables; i++) {
        indices[i] = i;
    }

    // Generate the objective function
    for (uint32_t i = 0; i < (nb_variables - 1); i++) {
        random_double = cost_dist(engine);
        f_out << random_double << " x" << i << " + ";
    }
    random_double = cost_dist(engine);
    f_out << random_double << " x" << (nb_variables - 1) << std::endl;

    // Generate the constraints
    for (uint32_t j = 0; j < nb_constraints; j++) {
        random_uint = zero_var_dist(engine);
        std::shuffle(indices.begin(), indices.end(), engine);

        for (uint32_t i = random_uint; i < (nb_variables - 1); i++) {
            random_double = coefficient_dist(engine);
            f_out << random_double << " x" << indices[i] << " + ";
        }
        random_double = coefficient_dist(engine);
        f_out << random_double << " x" << indices[(nb_variables - 1)];

        if (covering > 0) {
            f_out << " >= ";
        } else {
            f_out << " <= " ;
        }
        random_double = bound_dist(engine);
        f_out << random_double << std::endl;
    }

    f_out.close();
}

void InputGenerator::generateExperts()
{
    std::ofstream f_out(EXPERT_FILENAME);

    if (! f_out.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << EXPERT_FILENAME << " could not be created!" << std::endl;
        throw std::runtime_error(message.str());
    }

    f_out << std::fixed << std::setprecision(15);
    f_out << nb_experts << std::endl;

    OfflineModel off_model(MODEL_FILENAME);
    LP_Solver off_solver(off_model, 0);
    DoubleVec_t optimal_solution = off_solver.solve();
    Solution::RoundSolutionIfNeeded(off_model, optimal_solution, off_model.getNbConstraints());

    OnlineModel on_model(off_model);
    LP_Solver on_solver(on_model, 0);

    DoubleMat_t min_solution(nb_online_expert);
    for (uint32_t k = 0; k < nb_online_expert; k++) {
        min_solution[k].resize(off_model.getNbVariables(), 0.0);
    }

    double max_x_value = max_bound / min_coefficient;

    DoubleMat_t random_solution(nb_random_expert);
    for (uint32_t k = 0; k < nb_random_expert; k++) {
        random_solution[k].resize(off_model.getNbVariables(), 0.0);
    }

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> online_dist(min_online_error, max_online_error);
    std::uniform_real_distribution<double> random_dist(min_random_increase, max_random_increase);
    std::uniform_int_distribution<uint32_t> var_dist(0, (nb_variables - 1));

    double random_double;
    uint32_t random_uint;
    double constr_value;

    for (uint32_t j = 1; j < (nb_constraints + 1); j++) {
        // Perfect experts
        for (uint32_t k = 0; k < nb_perfect_expert; k++) {
            for (uint32_t i = 0; i < (nb_variables - 1); i++) {
                f_out << optimal_solution[i] << " ";
            }
            f_out << optimal_solution[(nb_variables - 1)] << std::endl;
        }

        on_model.revealNextConstraint();
        on_solver.addNewConstraints(j);
        DoubleVec_t solution = on_solver.solve();
        Solution::RoundSolutionIfNeeded(off_model, solution, j);

        // Imperfect experts
        for (uint32_t k = 0; k < nb_online_expert; k++) {
            for (uint32_t i = 0; i < (nb_variables - 1); i++) {
                random_double = 1.0 + online_dist(engine);
                if (min_solution[k][i] < solution[i]) {
                    min_solution[k][i] = (solution[i] * random_double);
                }
                f_out << min_solution[k][i] << " ";
            }
            random_double = 1.0 + online_dist(engine);
            if (min_solution[k][(nb_variables - 1)] < solution[(nb_variables - 1)]) {
                min_solution[k][(nb_variables - 1)] = (solution[(nb_variables - 1)] * random_double);
            }
            f_out << min_solution[k][(nb_variables - 1)] << std::endl;
        }

        // Adversary experts
        for (uint32_t k = 0; k < nb_adversary_expert; k++) {
            for (uint32_t i = 0; i < (nb_variables - 1); i++) {
                f_out << max_x_value << " ";
            }
            f_out << max_x_value << std::endl;
        }

        // Random experts
        for (uint32_t k = 0; k < nb_random_expert; k++) {
            const DoubleMat_t & A = off_model.getCoefficient(0);
            const DoubleVec_t & b = off_model.getBound(0);

            constr_value = 0.0;
            for (uint32_t i = 0; i < nb_variables; i++) {
                constr_value += A[(j-1)][i] * random_solution[k][i];
            }
            while (constr_value < b[(j-1)]) {
                random_uint = var_dist(engine);
                random_double = random_dist(engine);
                random_solution[k][random_uint] += random_double;

                constr_value = 0.0;
                for (uint32_t i = 0; i < nb_variables; i++) {
                    constr_value += A[(j-1)][i] * random_solution[k][i];
                }
            }

            for (uint32_t i = 0; i < (nb_variables - 1); i++) {
                f_out << random_solution[k][i] << " ";
            }
            f_out << random_solution[k][(nb_variables - 1)] << std::endl;
        }
    }

    f_out.close();
}
