#include "experts.h"

#include <cmath>
#include <regex>
#include <sstream>


static const std::regex SOLUTION_PATTERN("^\\s*(?:[0-9]+(?:\\.[0-9]+)?\\s+)*[0-9]+(?:\\.[0-9]+)?\\s*$");
static const std::regex VALUE_PATTERN("([0-9]+(?:\\.[0-9]+)?)");


Experts::Experts(const OfflineModel& model, const Config& config, const std::string& expert_file) :
    off_model(model),
    epsilon(config.epsilon),
    b(model.getBound(0)),
    A(model.getCoefficient(0)),
    nb_experts(0)
{
    f_in.open(expert_file);
    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << expert_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    solutions.resize(off_model.getNbConstraints());
    tight_solutions.resize(off_model.getNbConstraints());

    nb_experts = readInteger();
    for (uint32_t t = 0; t < off_model.getNbConstraints(); t++) {
        solutions[t].resize(nb_experts);
        tight_solutions[t].resize(nb_experts);

        for (uint32_t k = 0; k < nb_experts; k++) {
            solutions[t][k].resize(off_model.getNbVariables());
            tight_solutions[t][k].resize(off_model.getNbVariables());

            for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
                solutions[t][k][i] = 0.0;
                tight_solutions[t][k][i] = 0.0;
            }
        }
    }

    zero_solution.resize(nb_experts);
    for (uint32_t k = 0; k < nb_experts; k++) {
        zero_solution[k].resize(off_model.getNbVariables());
        for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
            zero_solution[k][i] = 0.0;
        }
    }

    std::smatch match;
    std::stringstream ss;
    bool invalid = false;
    for (uint32_t t = 0; t < off_model.getNbConstraints(); t++) {
        for (uint32_t k = 0; k < nb_experts; k++) {
            readLine();
            if (std::regex_match(line, match, SOLUTION_PATTERN)) {
                for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
                    if (regex_search(line, match, VALUE_PATTERN)) {
                        ss << match[1].str();
                        ss >> solutions[t][k][i];
                        ss.clear();
                        line = match.suffix();
                    } else {
                        invalid = true;
                    }
                }
            } else {
                invalid = true;
            }

            if (invalid) {
                std::stringstream message;
                message << "ERROR: suggestion for constraint " << t << " and expert " << k << " has invalid format!" << std::endl;
                throw std::runtime_error(message.str());
            }
        }
    }

    f_in.close();

    avg_solutions.resize(off_model.getNbConstraints());
    for (uint32_t t = 0; t < off_model.getNbConstraints(); t++) {
        avg_solutions[t].resize(off_model.getNbVariables());
        for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
            double value = 0.0;
            for (uint32_t k = 0; k < nb_experts; k++) {
                value += solutions[t][k][i];
            }
            avg_solutions[t][i] = (value / static_cast<double>( nb_experts));
        }
    }

    int32_t t = findInvalidSolution(solutions);
    if (t != -1) {
        std::stringstream message;
        message << "ERROR: Solutions do not satisfy constraint " << t << " !" << std::endl;
        throw std::runtime_error(message.str());
    }

    tightenSolutions();

    t = findInvalidSolution(solutions);
    if (t != -1) {
        std::stringstream message;
        message << "ERROR: Tightening the solutions made " << t << " infeasible!" << std::endl;
        throw std::runtime_error(message.str());
    }

    t = findInvalidSolution(tight_solutions);
    if (t != -1) {
        std::stringstream message;
        message << "ERROR: Tight solutions do not satisfy constraint " << t << " !" << std::endl;
        throw std::runtime_error(message.str());
    }

    for (uint32_t t = 0; t < off_model.getNbConstraints(); t++) {
        for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
            for (uint32_t k = 0; k < nb_experts; k++) {
                if ((solutions[t][k][i] < tight_solutions[t][k][i]) && std::abs(solutions[t][k][i] - tight_solutions[t][k][i]) > epsilon) {
                    std::stringstream message;
                    message << "ERROR: Solution (" << t << ", " << k << ", " << i << ") is smaller than the tight one!" << std::endl;
                    throw std::runtime_error(message.str());
                }
            }
        }
    }

    // Used to display the competitive ratio of each expert
    objective_values.resize(nb_experts, 0.0);
    for (uint32_t k = 0; k < nb_experts; k++) {
        objective_values[k] = off_model.getObjectiveValue(solutions[(off_model.getNbConstraints() - 1)][k]);
    }
}

uint32_t Experts::getNbExperts() const
{
    return nb_experts;
}

const DoubleMat_t& Experts::getSolutions(const int64_t t) const
{
    if ((t < 0) || (t >= static_cast<int64_t>(solutions.size()))) {
        return zero_solution;
    }
    return solutions[t];
}

const DoubleMat_t& Experts::getTightSolutions(const int64_t t) const
{
    if ((t < 0) || (t >= static_cast<int64_t>(tight_solutions.size()))) {
        std::stringstream message;
        message << "ERROR: tight solutions for  " << t << " do not exist!" << std::endl;
        throw std::runtime_error(message.str());
    }
    return tight_solutions[t];
}

const DoubleVec_t& Experts::getAverageSolutions(const int64_t t) const
{
    if ((t < 0) || (t >= static_cast<int64_t>(avg_solutions.size()))) {
        return avg_solutions[0];
    }
    return avg_solutions[t];
}

const DoubleVec_t& Experts::getObjectiveValues() const
{
    return objective_values;
}

void Experts::tightenSolutions()
{
    double constr_value = 0.0;

    // First constraint
    for (uint32_t k = 0; k < nb_experts; k++) {
        constr_value = getConstraintValue(k, 0, 0);
        if (constraintIsNotTight(constr_value, b[0])) {
            downscaleSolution(constr_value, k, 0);
        }
        constr_value = getConstraintValue(k, 0, 0);
        if (constraintIsNotTight(constr_value, b[0])) {
            std::stringstream message;
            message << "ERROR: The constraint should be tight for expert " << k << " !" << std::endl;
            throw std::runtime_error(message.str());
        } else {
            setTightSolution(k, 0);
        }
    }

    // All other constraints
    for (uint32_t t = 1; t < off_model.getNbConstraints(); t++) {
        for (uint32_t k = 0; k < nb_experts; k++) {

            // Check if previous value satisfies the new constraint
            constr_value = getConstraintValue(k, t, t-1);
            if (constraintIsNotTight(constr_value, b[t])) {

                // Check if new expert solution is tight
                constr_value = getConstraintValue(k, t, t);
                verifySolutionFeasibility(constr_value, b[t], k, t);
                if (constraintIsNotTight(constr_value, b[t])) {
                    downscaleSolutionMultipleTimes(k, t);
                }

                // Security check: the solution should still satisfy the constraint
                constr_value = getConstraintValue(k, t, t);
                if (constr_value < b[t]) {
                    throw std::runtime_error("ERROR: solution scaled down too much!\n");
                }

                // If the satisfaction became tight after scaling, set the variable
                if (! constraintIsNotTight(constr_value, b[t])) {
                    setTightSolution(k, t);
                } else {
                    setTightSolutionInTheInterval(k, t);
                }
            } else {
                setTightSolution(k, t);
            }
        }
    }
}

double Experts::getConstraintValue(uint32_t k, uint32_t t1, uint32_t t2)
{
    double constr_value = 0.0;
    for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
        constr_value += A[t1][i] * solutions[t2][k][i];
    }
    return constr_value;
}

bool Experts::constraintIsNotTight(double value, double limit)
{
    return (value < limit) || std::abs(value - limit) > epsilon;
}

void Experts::downscaleSolution(double value, uint32_t k, uint32_t t)
{
    double scale = b[t] / (value - 0.0000000000001);
    for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
        solutions[t][k][i] = scale * solutions[t][k][i];
    }
}

void Experts::setTightSolution(uint32_t k, uint32_t t)
{
    for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
        tight_solutions[t][k][i] = solutions[t][k][i];
    }
}

void Experts::verifySolutionFeasibility(double value, double limit, uint32_t k, uint32_t t)
{
    if (value < (limit - 0.0001)) {
        std::stringstream message;
        message << "ERROR: expert " << k << " gave infeasible suggestion for constraint " << t << " !" << std::endl;
        throw std::runtime_error(message.str());
    }
}

void Experts::downscaleSolutionMultipleTimes(uint32_t k, uint32_t t)
{
    double scale = 0.0;
    double scaled_value = 0.0;
    double constr_value = getConstraintValue(k, t, t);
    double previous_constr_value = 0.0;

    do {
        scale = b[t] / constr_value;
        if (scale < 1) {
            break;
        }

        for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
            scaled_value = scale * solutions[t][k][i];
            if (scaled_value >= solutions[(t - 1)][k][i]) {
                solutions[t][k][i] = scaled_value;
            } else {
                solutions[t][k][i] = solutions[(t - 1)][k][i];
            }
        }

        previous_constr_value = constr_value;
        constr_value = getConstraintValue(k, t, t);
    } while (std::abs(previous_constr_value - constr_value) > epsilon);
}

void Experts::setTightSolutionInTheInterval(uint32_t k, uint32_t t)
{
    double var_limit = 0.0;
    BoolVec_t marker(off_model.getNbVariables(), false);

    for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
        if (A[t][i] == 0) {
            var_limit = solutions[t][k][i];
        } else {
            var_limit = tight_solutions[(t - 1)][k][i] * (A[(t - 1)][i] / A[t][i]);
        }

        if (solutions[t][k][i] <= var_limit) {
            tight_solutions[t][k][i] = solutions[t][k][i];
        } else {
            tight_solutions[t][k][i] = var_limit;
            marker[i] = true;
        }
    }

    double constr_value = 0.0;
    for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
        constr_value += A[t][i] * tight_solutions[t][k][i];
    }

    double scale = 0.0;
    double scaled_value = 0.0;
    while (constraintIsNotTight(constr_value, b[t])) {
        scale = b[t] / constr_value;
        if (scale < 1.0) {
            break;
        }

        constr_value = 0.0;
        for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
            if (tight_solutions[t][k][i] == 0) {
                scaled_value = scale * 0.000000001;
            } else {
                scaled_value = scale * tight_solutions[t][k][i];
            }
            if (marker[i] && scaled_value <= solutions[t][k][i]) {
                tight_solutions[t][k][i] = scaled_value;
            } else if (marker[i]) {
                tight_solutions[t][k][i] = solutions[t][k][i];
            }
            constr_value += A[t][i] * tight_solutions[t][k][i];
        }
    }
}

int32_t Experts::findInvalidSolution(const DoubleMatVec_t& sol_vec)
{
    for (uint32_t t = 0; t < off_model.getNbConstraints(); t++) {
        for (uint32_t k = 0; k < nb_experts; k++) {
            double value = 0.0;
            for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
                    value += A[t][i] * sol_vec[t][k][i];
                }
            if (value < 1.0) {
                return t;
            }
        }
    }
    return -1;
}
