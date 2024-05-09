#include "argument_parser.h"

#include <iostream>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>


ArgumentParser::ArgumentParser(int argc, char** argv) :
    _argc(argc),
    _argv(argv),
    mode_generation(false),
    general_config("config/general.conf")
{}

void ArgumentParser::parse()
{
    if (_argc != 3) {
        displayUsage();
    }

    test_name = std::string(_argv[1]);
    data_file = "data/" + test_name + ".lp";
    config_file = "config/" + test_name + ".conf";
    expert_file = "experts/" + test_name + ".pred";
    generator_file = "generation/" + test_name + ".gen";

    try {
        checkIfFileExists(data_file);
        checkIfConfigFileExists(config_file);
        checkIfFileExists(expert_file);
        checkOption(std::string(_argv[2]));
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
    message << std::endl << std::endl << "Usage: ./OCME <test_name> --linear" << std::endl;
    message << std::endl << std::endl << "or:    ./OCME <convex_test_name> --convex" << std::endl;
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

void ArgumentParser::checkIfConfigFileExists(std::string& filename)
{
    std::filesystem::path file_path(filename);
    if (! std::filesystem::exists(file_path)) {
        std::filesystem::path general_path(general_config);
        if (! std::filesystem::exists(general_path)) {
            throw std::runtime_error("file does not exist");
        } else {
            std::cout << "WARNING: No test specific config file found, using the general configuration file!" << std::endl;
            config_file = general_config;
        }
    }
}

void ArgumentParser::checkOption(std::string option)
{
    if (option == std::string("--linear")) {
        is_convex = false;
    } else if (option == std::string("--convex")) {
        is_convex = true;
    } else {
        throw std::runtime_error("invalid option");
    }
}
