#include "input/argument_parser.h"
#include "input/config.h"
#include "input/input_generator.h"

#include "instance/instance.h"

#include "prediction/edge_predictor.h"
#include "prediction/vertex_predictor.h"

#include "solver/greedy_solver.h"
#include "solver/offline_solver.h"
#include "solver/weighted_greedy_solver.h"
#include "solver/Xu_Moseley_algorithm.h"

#include "statistics/data_exporter.h"


int main(int argc, char** argv)
{
    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    arg_parser.parse();
    Config config(arg_parser.config_file);

    Instance instance;
    InputGenerator generator(config);
    generator.generate(instance);

    OfflineSolver offline(instance.graph, instance.terminals);
    GreedySolver greedy(instance.graph);

    VertexPredictor vertex_predictor(instance.graph, instance.scenarios);
    XuMoseleyAlgorithm xm_algorithm(instance.graph, vertex_predictor);

    EdgePredictor edge_predictor(instance.graph, instance.scenarios, config.edge_weight_limit);
    WeightedGreedySolver solver(instance.graph);

    offline.print(config.verbosity);

    while (instance.isRunning()) {
        uint32_t t = instance.getNextTerminal();
        edge_predictor.updateWeights(t);

        greedy.connectTerminal(t);
        xm_algorithm.connectTerminal(t);
        solver.connectTerminal(t);

        if (config.verbosity > 1) {
            instance.graph.print(config.verbosity);
        }
    }

    greedy.print(config.verbosity);
    xm_algorithm.print(config.verbosity);
    solver.print(config.verbosity);


    DataExporter exporter(
        arg_parser.test_name,
        instance,
        offline.getSolution(),
        xm_algorithm.getSolution(),
        greedy.getSolution(),
        solver.getSolution());

    exporter.exportToFile();
    return 0;
}
