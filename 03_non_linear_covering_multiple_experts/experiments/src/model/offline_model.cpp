#include "offline_model.h"

#include <sstream>
#include <regex>


static const std::regex OBJ_FUNC_PATTERN("\\s*(min|max)\\s+(?:[0-9\\.]+\\s*x[0-9]+_[0-9]+\\s+[+\\-]\\s*)*\\s+(?:[0-9\\.]+\\s*x[0-9]+_[0-9]+\\s*)+\\s*[\\S\\s]*");
static const std::regex CONSTRAINT_PATTERN("\\s*(?:[0-9\\.]+\\s*x[0-9]+_[0-9]+\\s+[+\\-]\\s*)*\\s*(?:[0-9\\.]+\\s*x[0-9]+_[0-9]+\\s*)+\\s+>=\\s+([0-9\\.]+)\\s*[\\S\\s]*");
static const std::regex VARIABLE_PATTERN("([0-9\\.]+)\\s*x([0-9]+)_([0-9]+)");


OfflineModel::OfflineModel(const std::string& data_file)
{
    f_in.open(data_file);
    if (! f_in.is_open()) {
        std::stringstream message;
        message << "ERROR: File " << data_file << " could not be open!" << std::endl;
        throw std::runtime_error(message.str());
    }

    nb_machines = readInteger();
    nb_jobs = readInteger();

    nb_variables = nb_machines * nb_jobs;
    nb_constraints = nb_jobs;

    c.resize(nb_variables, 0.0);
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
        double x_cost;
        uint32_t x_i;
        uint32_t x_j;

        for (uint32_t i = 0; i < nb_variables; i++) {
            if (regex_search(line, match, VARIABLE_PATTERN)) {
                ss << match[1].str();
                ss >> x_cost;
                ss.clear();
                ss << match[2].str();
                ss >> x_i;
                ss.clear();
                ss << match[3].str();
                ss >> x_j;
                ss.clear();

                if (x_i < 0 || x_i >= nb_machines || x_j < 0 || x_j >= nb_jobs) {
                    throw std::runtime_error("ERROR: An objective function variable has invalid id!\n");
                }

                c[getId(x_i, x_j)] = x_cost;
                line = match.suffix();
            } else {
                throw std::runtime_error("ERROR: The objective function has invalid format!\n");
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

            double x_cost;
            uint32_t x_i;
            uint32_t x_j;
            while (regex_search(line, match, VARIABLE_PATTERN)) {
                ss << match[1].str();
                ss >> x_cost;
                ss.clear();
                ss << match[2].str();
                ss >> x_i;
                ss.clear();
                ss << match[3].str();
                ss >> x_j;
                ss.clear();

                if (x_i < 0 || x_i >= nb_machines || x_j < 0 || x_j >= nb_jobs) {
                    std::stringstream message;
                    message << "ERROR: a constraint variable on line " << line_counter << " has invalid id!" << std::endl;
                    throw std::runtime_error(message.str());
                }

                A[0][j][getId(x_i, x_j)] = x_cost;
                line = match.suffix();
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
    DoubleVec_t load(nb_machines, 0.0);

    uint32_t idx;
    for (uint32_t i = 0; i < nb_machines; i++) {
        for (uint32_t j = 0; j < nb_jobs; j++) {
            idx = getId(i,j);
            load[i] += c[idx] * x[idx];
        }
    }

    return *max_element(load.begin(), load.end());
}
