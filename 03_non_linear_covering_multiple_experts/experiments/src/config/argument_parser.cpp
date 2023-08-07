#include "argument_parser.h"

#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>


ArgumentParser::ArgumentParser(int argc, char** argv) :
    _argc(argc),
    _argv(argv),
    mode_generation(false)
{}

void ArgumentParser::parse()
{
    if (_argc != 2) {
        displayUsage();
    }

    test_name = std::string(_argv[1]);
    data_file = "data/" + test_name + ".lp";
    config_file = "config/" + test_name + ".conf";
    expert_file = "experts/" + test_name + ".pred";
    generator_file = "generation/" + test_name + ".gen";

    try {
        checkIfFileExists(data_file);
        checkIfFileExists(config_file);
        checkIfFileExists(expert_file);
    } catch(...) {
        mode_generation = true;
        try {
            checkIfFileExists(generator_file);
        } catch(...) {
            displayUsage();
        }
    }
}

bool ArgumentParser::modeIsGeneration() const
{
    return mode_generation;
}

void ArgumentParser::displayUsage()
{
    std::stringstream message;
    message << std::endl << std::endl << "Usage: ./COCME <test_name>" << std::endl;
    message << "1) If the test name exists in the data/ config/ and experts/ folders, the test is executed." << std::endl;
    message << "2) If 1) fails, if the test name exists in generation/ folder, the test is generated." << std::endl;
    message << "3) Otherwise ERROR!" << std::endl;
    throw std::runtime_error(message.str());
}

void ArgumentParser::checkIfFileExists(std::string& filename)
{
    std::filesystem::path file_path(filename);
    if (! std::filesystem::exists(file_path)) {
        throw std::runtime_error("file does not exist");
    }
}
