#include "data_exporter.h"

#include <fstream>


DataExporter::DataExporter(
    const std::string& test_name,
    const Instance& instance,
    const std::vector<uint32_t>& offline_solution,
    const std::vector<uint32_t>& xm_solution,
    const std::vector<uint32_t>& greedy_solution,
    const std::vector<uint32_t>& solution) :
    _test_name(test_name),
    _instance(instance),
    _offline_solution(offline_solution),
    _xm_solution(xm_solution),
    _greedy_solution(greedy_solution),
    _solution(solution)
{}

void DataExporter::exportToFile()
{
    std::string file_name = _test_name + "_scenarios.csv";
    std::ofstream scenario_file(file_name, std::ofstream::out | std::ofstream::trunc);
    scenario_file << "ID;";
    for (uint32_t v = 0; v < _instance.graph.getNbVertices(); v++) {
        scenario_file << "V" << (v+1) << ";";
    }
    for (uint32_t e = 0; e < (_instance.graph.getNbEdges() - 1); e++) {
        scenario_file << "E" << (e+1) << ";";
    }
    scenario_file << "E" << _instance.graph.getNbEdges() << std::endl;

    for (uint32_t idx = 0; idx < _instance.scenarios.size(); idx++) {
        scenario_file << (idx+1) << ";";

        for (uint32_t v = 0; v < _instance.graph.getNbVertices(); v++) {
            if (_instance.scenarios[idx].containsVertex(v)) {
                scenario_file << 1 << ";";
            } else {
                scenario_file << 0 << ";";
            }
        }

        for (uint32_t e = 0; e < (_instance.graph.getNbEdges() - 1); e++) {
            if (_instance.scenarios[idx].containsEdge(e)) {
                scenario_file << 1 << ";";
            } else {
                scenario_file << 0 << ";";
            }
        }
        if (_instance.scenarios[idx].containsEdge((_instance.graph.getNbEdges() - 1))) {
            scenario_file << 1 << std::endl;
        } else {
            scenario_file << 0 << std::endl;
        }
    }

    scenario_file.close();
}
