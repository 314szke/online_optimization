#include "algorithm/mcp.h"
#include "input/argument_parser.h"
#include "input/input_generator.h"
#include "instance/instance.h"
#include "visualization/print.hpp"


int main(int argc, char** argv)
{
    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    arg_parser.parse();

    Instance instance;
    InputGenerator generator(arg_parser.config_file);
    generator.generate(instance);
    instance.print();
    Path p1 = MCWP(instance.graph, instance.terminals[0]);
    instance.graph.setWeight(3,4,0.7);
    instance.graph.setWeight(4,2,0.7);
    instance.graph.setWeight(2,0,0.7);
    Path p2 = MCWP(instance.graph, instance.terminals[0]);

    print_vector("P1", p1.vertices);
    print_vector("P2", p2.vertices);
    return 0;
}
