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

    EdgePredictor edge_predictor(instance.graph, instance.scenarios, config.lambda);
    WeightedGreedySolver solver(instance.graph, edge_predictor);

    offline.print();

    while (instance.isRunning()) {
        uint32_t t = instance.getNextTerminal();
        edge_predictor.updateWeights(t);

        greedy.connectTerminal(t);
        xm_algorithm.connectTerminal(t);
        solver.connectTerminal(t);

        instance.graph.print();
    }

    greedy.print();
    xm_algorithm.print();
    solver.print();

    return 0;
}
