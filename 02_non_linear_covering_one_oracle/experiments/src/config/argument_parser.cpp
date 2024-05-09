#include "argument_parser.h"

#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>


ArgumentParser::ArgumentParser(int argc, char** argv) :
    _argc(argc),
    _argv(argv),
    mode_generation(false),
    mode_conversion(false)
{}

void ArgumentParser::parse()
{
    if (_argc > 3) {
        displayUsage();
    }

    test_name = std::string(_argv[1]);
    data_file = "data/" + test_name + ".lp";
    config_file = "config/" + test_name + ".conf";
    generator_file = "generation/" + test_name + ".gen";
    output_file = test_name + "_converted.lp";
    expert_file = test_name + "_converted.pred";

    try {
        checkIfFileExists(data_file);
        checkIfFileExists(config_file);
    } catch(...) {
        mode_generation = true;
        try {
            checkIfFileExists(generator_file);
        } catch(...) {
            displayUsage();
        }
    }

    checkConversionMode();
}

bool ArgumentParser::modeIsGeneration() const
{
    return mode_generation;
}

bool ArgumentParser::modeIsConversion() const
{
    return mode_conversion;
}

void ArgumentParser::displayUsage()
{
    std::stringstream message;
    message << std::endl << std::endl << "Usage: ./OCF <test_name>" << std::endl;
    message << std::endl << std::endl << "or:    ./OCF <test_name> --convert" << std::endl;
    message << "1) If the test name exists in the data/ and config/ folders, the test is executed." << std::endl;
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

void ArgumentParser::checkConversionMode()
{
    if (_argc != 3) {
        return;
    }

    if (std::string(_argv[2]) == std::string("--convert")) {
        mode_conversion = true;
    }
}
