#include "input/argument_parser.h"
#include "input/input_generator.h"
#include "instance/instance.h"
#include "solver/greedy_solver.h"
#include "solver/offline_solver.h"
#include "solver/predictor.h"
#include "solver/weighted_greedy_solver.h"


int main(int argc, char** argv)
{
    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    arg_parser.parse();

    Instance instance;
    InputGenerator generator(arg_parser.config_file);
    generator.generate(instance);

    OfflineSolver offline(instance.graph, instance.terminals);
    GreedySolver greedy(instance.graph);

    Predictor predictor(instance.graph, instance.scenarios);
    WeightedGreedySolver solver(instance.graph);

    //instance.print();
    while (instance.isRunning()) {
        uint32_t t = instance.getNextTerminal();
        greedy.connectTerminal(t);
        solver.connectTerminal(t);
    }

    offline.print();
    greedy.print();
    solver.print();

    return 0;
}
