#include "input/argument_parser.h"
#include "input/input_generator.h"
#include "instance/instance.h"


int main(int argc, char** argv)
{
    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    arg_parser.parse();

    Instance instance;
    InputGenerator generator(arg_parser.config_file);
    generator.generate(instance);
    instance.graph.print();

    return 0;
}
