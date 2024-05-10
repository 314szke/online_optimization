#include "offline_model.h"

#include <math.h>
#include <sstream>
#include <regex>


static const std::regex OBJ_FUNC_PATTERN("\\s*min\\s+(?:\\+?\\s*[0-9\\.]+\\s+x[0-9]+(?:\\s*\\^\\s*[0-9]+)?\\s+)+(?:\\+?\\s*[0-9\\.]+\\s+x[0-9]+(?:\\s*\\^\\s*[0-9]+)?\\s*)(?:\\s*\\r?\\n?)?");
static const std::regex CONSTRAINT_PATTERN("\\s*(?:(?:\\+|-)?\\s*[0-9\\.]+\\s+x[0-9]+)(?:\\s+(?:\\+|-)\\s*[0-9\\.]+\\s+x[0-9]+)+\\s+>=\\s+(?:\\+|-)?\\s*([0-9\\.]+)(?:\\s*\\r?\\n?)?");
static const std::regex SIMPLE_VARIABLE_PATTERN("([0-9\\.]+)\\s*x([0-9]+)");
static const std::regex VARIABLE_PATTERN("(\\+|-)\\s*([0-9\\.]+)\\s*x([0-9]+)\\s*\\^\\s*([0-9]+)");


OfflineModel::OfflineModel(const std::string& data_file, bool is_convex_mode)
{
    f_in.open(data_file);
    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << data_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    is_convex = is_convex_mode;

    nb_variables = readInteger();
    nb_objective_variables = nb_variables;
    nb_constraints = readInteger();
    nb_revealed_constraints = nb_constraints;
    nb_batches = nb_constraints;

    if (is_convex) {
        batch_size = readInteger();
        nb_batches = (nb_constraints - nb_initial_constraints) / batch_size;
        nb_initial_constraints = nb_objective_variables;
        nb_variables = (nb_batches + 1) * nb_objective_variables;
    }

    c.resize(nb_variables, 0.0);
    e.resize(nb_variables, 1.0);
    b.resize(1);
    A.resize(1);
    b[0].resize(nb_constraints, 0.0);
    A[0].resize(nb_constraints);

    for (uint32_t j = 0; j < nb_constraints; j++) {
        A[0][j].resize(nb_variables, 0.0);
    }

    // Read the objective value
    readLine();

    std::smatch match;
    if (std::regex_match(line, match, OBJ_FUNC_PATTERN)) {
        std::stringstream ss;
        std::string sign;
        double x_cost;
        uint32_t x_exponent;
        uint32_t x_id;
        for (uint32_t i = 0; i < nb_variables; i++) {
            if (is_convex) {
                if (regex_search(line, match, VARIABLE_PATTERN)) {
                    ss << match[1].str();
                    ss >> sign;
                    ss.clear();
                    ss << match[2].str();
                    ss >> x_cost;
                    ss.clear();
                    ss << match[3].str();
                    ss >> x_id;
                    ss.clear();
                    ss << match[4].str();
                    ss >> x_exponent;
                    ss.clear();

                    if (x_id < 0 || x_id >= nb_variables) {
                        throw std::runtime_error("ERROR: An objective function variable has invalid id!\n");
                    }

                    if (sign == std::string("+")) {
                        c[x_id] = x_cost;
                    } else {
                        c[x_id] = x_cost * -1;
                    }
                    e[x_id] = x_exponent;
                    line = match.suffix();
                } else {
                    throw std::runtime_error("ERROR: The objective function has invalid format!\n");
                }
            } else {
                if (regex_search(line, match, SIMPLE_VARIABLE_PATTERN)) {
                    ss << match[1].str();
                    ss >> x_cost;
                    ss.clear();
                    ss << match[2].str();
                    ss >> x_id;
                    ss.clear();

                    if (x_id < 0 || x_id >= nb_variables) {
                        throw std::runtime_error("ERROR: An objective function variable has invalid id!\n");
                    }

                    c[x_id] = x_cost;
                    line = match.suffix();
                } else {
                    throw std::runtime_error("ERROR: The objective function has invalid format!\n");
                }
            }
        }
    } else {
        throw std::runtime_error("ERROR: The objective function has invalid format!\n");
    }

    // Read the constraints
    for (uint32_t j = 0; j < nb_constraints; j++) {
        readLine();
        if (std::regex_match(line, match, CONSTRAINT_PATTERN)) {
            std::stringstream ss;
            ss << match[1].str();
            ss >> b[0][j];
            ss.clear();

            std::string sign;
            double x_coefficient;
            uint32_t x_id;
            if (is_convex) {
                while (regex_search(line, match, VARIABLE_PATTERN)) {
                    ss << match[1].str();
                    ss >> sign;
                    ss.clear();
                    ss << match[2].str();
                    ss >> x_coefficient;
                    ss.clear();
                    ss << match[3].str();
                    ss >> x_id;
                    ss.clear();

                    if (x_id < 0 || x_id >= nb_variables) {
                        std::stringstream message;
                        message << "ERROR: a constraint variable on line " << line_counter << " has invalid id!" << std::endl;
                        throw std::runtime_error(message.str());
                    }

                    if (sign == std::string("+")) {
                        A[0][j][x_id] = x_coefficient;
                    } else {
                        A[0][j][x_id] = x_coefficient * -1;
                    }
                    line = match.suffix();
                }
            } else {
                while (regex_search(line, match, SIMPLE_VARIABLE_PATTERN)) {
                    ss << match[1].str();
                    ss >> x_coefficient;
                    ss.clear();
                    ss << match[2].str();
                    ss >> x_id;
                    ss.clear();

                    if (x_id < 0 || x_id >= nb_variables) {
                        std::stringstream message;
                        message << "ERROR: a constraint variable on line " << line_counter << " has invalid id!" << std::endl;
                        throw std::runtime_error(message.str());
                    }

                    A[0][j][x_id] = x_coefficient;
                    line = match.suffix();
                }
            }
        } else {
            std::stringstream message;
            message << "ERROR: constraint on line " << line_counter << " has invalid format!" << std::endl;
            throw std::runtime_error(message.str());
        }
    }

    f_in.close();
}

double OfflineModel::getObjectiveValue(const DoubleVec_t& x) const
{
    double value = 0.0;
    for (uint32_t i = 0; i < nb_variables; i++) {
        if (is_convex) {
            value += c[i] * std::pow(x[i], e[i]);
        } else {
            value += c[i] * x[i];
        }
    }
    return value;
}
