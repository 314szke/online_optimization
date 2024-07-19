#include "argument_parser.h"

#include <iostream>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>


ArgumentParser::ArgumentParser(int argc, char** argv) :
    _argc(argc),
    _argv(argv)
{}

void ArgumentParser::parse()
{
    if (_argc != 2) {
        displayUsage();
    }

    test_name = std::string(_argv[1]);
    config_file = "config/" + test_name + ".conf";

    try {
        checkIfFileExists(config_file);
    } catch(...) {
        displayUsage();
    }
}

void ArgumentParser::displayUsage()
{
    std::stringstream message;
    message << std::endl << std::endl << "Usage: ./STEINER <test_name>" << std::endl;
    message << "1) If the test name exists in the config/ folder, the test is executed." << std::endl;
    message << "2) Otherwise ERROR!" << std::endl;
    throw std::runtime_error(message.str());
}

void ArgumentParser::checkIfFileExists(std::string& filename)
{
    std::filesystem::path file_path(filename);
    if (! std::filesystem::exists(file_path)) {
        throw std::runtime_error("file does not exist");
    }
}
