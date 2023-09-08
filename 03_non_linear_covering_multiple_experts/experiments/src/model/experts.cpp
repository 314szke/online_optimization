#include "experts.h"

#include <cmath>
#include <regex>
#include <sstream>


static const std::regex SOLUTION_PATTERN("^\\s*(?:[0-9]+(?:\\.[0-9]+)?\\s+)*[0-9]+(?:\\.[0-9]+)?\\s*$");
static const std::regex VALUE_PATTERN("([0-9]+(?:\\.[0-9]+)?)");


Experts::Experts(const OfflineModel& model, const std::string& expert_file) :
    off_model(model),
    nb_experts(0)
{
    f_in.open(expert_file);
    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << expert_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    nb_experts = readInteger();

    // Initialize solutions
    solutions.resize(off_model.getNbConstraints());
    for (uint32_t t = 0; t < off_model.getNbConstraints(); t++) {
        solutions[t].resize(nb_experts);
        for (uint32_t k = 0; k < nb_experts; k++) {
            solutions[t][k].resize(off_model.getNbVariables());
            for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
                solutions[t][k][i] = 0.0;
            }
        }
    }

    // For time = -1
    zero_solution.resize(nb_experts);
    for (uint32_t k = 0; k < nb_experts; k++) {
        zero_solution[k].resize(off_model.getNbVariables());
        for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
            zero_solution[k][i] = 0.0;
        }
    }

    // Read expert solutions from the expert file
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

    // Calculate the average of the solutions
    avg_solutions.resize(off_model.getNbConstraints());
    for (uint32_t t = 0; t < off_model.getNbConstraints(); t++) {
        avg_solutions[t].resize(off_model.getNbVariables());
        for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
            double value = 0.0;
            for (uint32_t k = 0; k < nb_experts; k++) {
                value += solutions[t][k][i];
            }
            avg_solutions[t][i] = (value / static_cast<double>(nb_experts));
        }
    }

    int32_t t = findInvalidSolution();
    if (t != -1) {
        std::stringstream message;
        message << "ERROR: Solutions do not satisfy constraint " << t << " !" << std::endl;
        throw std::runtime_error(message.str());
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

int32_t Experts::findInvalidSolution()
{
    const DoubleMat_t& A = off_model.getCoefficient();

    for (uint32_t t = 0; t < off_model.getNbConstraints(); t++) {
        for (uint32_t k = 0; k < nb_experts; k++) {
            double value = 0.0;
            for (uint32_t i = 0; i < off_model.getNbVariables(); i++) {
                    value += A[t][i] * solutions[t][k][i];
                }
            if (value < 1.0) {
                return t;
            }
        }
    }
    return -1;
}
