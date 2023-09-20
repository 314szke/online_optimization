#include <iostream>
#include <string>

#include "config/argument_parser.h"
#include "input/grid_generator.h"
#include "input/random_generator.h"
#include "online/execution_manager.h"


int main(int argc, char** argv)
{
    std::cout << std::fixed;
    std::cout.precision(2);

    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    arg_parser.parse();

    if (arg_parser.modeIsGeneration()) {
        if (arg_parser.generator_file.find("random") != std::string::npos) {
            RandomGenerator random_generator(arg_parser);
            random_generator.generate();
        } else {
            GridGenerator grid_generator(arg_parser);
            grid_generator.generate();
        }
        return 0;
    }

    ExecutionManager manager(arg_parser);
    manager.run();

    return 0;
}
