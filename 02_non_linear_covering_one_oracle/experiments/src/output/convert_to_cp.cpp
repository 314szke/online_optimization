#include "convert_to_cp.h"

#include <fstream>


ConvertToCP::ConvertToCP(const std::string& data_file) :
    model(data_file)
{}

void ConvertToCP::convert(const std::string& output_file)
{
    std::ofstream f_out(output_file);

    f_out << model.graph.nb_edges << std::endl;
    // Number of constraints = connect y with x + flow preservation for each vertex
    f_out << (model.graph.nb_edges + (model.graph.nb_vertices * model.nb_requests)) << std::endl;
    f_out << model.nb_requests << std::endl;

    // Objective
    f_out << "min ";
    for (uint32_t idx = 0; idx < model.graph.nb_edges; idx++) {
        Edge e = model.graph.A[model.graph.ID[idx].i][model.graph.ID[idx].j];
        f_out << "+" << e.coefficient << " x" << idx << "^" << e.exponent << " ";
    }
    f_out << std::endl;

    // Constraint connecting y with x
    for (uint32_t e = 0; e < model.graph.nb_edges; e++) {
        f_out << "+1 x" << e << " ";
        for (uint32_t r = 0; r < model.nb_requests; r++) {
            f_out << "-1 x" << getID(e, r) << " "; // x^r_e
        }
        f_out << ">= 0" << std::endl;
    }

    // Flow preservation constraints
    for (uint32_t r = 0; r < model.nb_requests; r++) {
        for (uint32_t i = 0; i < model.graph.nb_vertices; i++) {
            for (uint32_t j = 0; j < model.graph.nb_vertices; j++) {
                if (i != j) {
                    if (model.graph.A[i][j].id != -1) {
                        f_out << "+1 x" << getID(model.graph.A[i][j].id, r) << " ";
                    }
                    if (model.graph.A[j][i].id != -1) {
                        f_out << "-1 x" << getID(model.graph.A[j][i].id, r) << " ";
                    }
                }
            }

            if (i == model.requests[r].source) {
                f_out << ">= 1" << std::endl;
            } else if (i == model.requests[r].target) {
                f_out << ">= -1" << std::endl;
            } else {
                f_out << ">= 0" << std::endl;
            }
        }
    }

    f_out.close();
}

uint32_t ConvertToCP::getID(uint32_t e, uint32_t r) const
{
    return (model.graph.nb_edges) + (e + (r * model.graph.nb_edges));
}
