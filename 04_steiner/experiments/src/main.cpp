#include "input/argument_parser.h"
#include "input/input_generator.h"
#include "instance/instance.h"
#include "solver/predictor.h"


int main(int argc, char** argv)
{
    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    arg_parser.parse();

    Instance instance;
    InputGenerator generator(arg_parser.config_file);
    generator.generate(instance);

    Predictor predictor(instance.graph, instance.scenarios);
    instance.print();

    return 0;
}
