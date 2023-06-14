#include "argument_parser.h"

#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>

#define GEN_NB_ARGS 2
#define RUN_NB_ARGS 4


ArgumentParser::ArgumentParser(int argc, char** argv) :
    _argc(argc),
    _argv(argv)
{}

void ArgumentParser::parse()
{
    if (!(_argc == GEN_NB_ARGS || _argc == RUN_NB_ARGS)) {
        std::stringstream message;
        message << std::endl << "Usage1: ./OCF <data_file>.lp <config_file>.conf <expert_file>.pred" << std::endl;
        message << "Usage2: ./OCF <generator_file>.gen" << std::endl;
        message << "ERROR: Inappropriate amount of arguments!" << std::endl;
        throw std::runtime_error(message.str());
    }

    if (_argc == GEN_NB_ARGS) {
        generator_file = std::string(_argv[1]);
        verify(generator_file, ".gen");
    } else {
        data_file = std::string(_argv[1]);
        config_file = std::string(_argv[2]);
        expert_file = std::string(_argv[3]);

        verify(data_file, ".lp");
        verify(config_file, ".conf");
        verify(expert_file, ".pred");
    }
}

void ArgumentParser::verify(std::string& filename, std::string file_format)
{
    if (file_format.size() > filename.size()) {
        std::stringstream message;
        message << "ERROR: Invalid filename: " << filename << std::endl;
        throw std::runtime_error(message.str());
    } else if (! std::equal(file_format.rbegin(), file_format.rend(), filename.rbegin())) {
        std::stringstream message;
        message << "ERROR: File: " << filename << " has invalid format!" << std::endl;
        throw std::runtime_error(message.str());
    }

    std::filesystem::path file_path(filename);
    if (! std::filesystem::exists(file_path)) {
        std::stringstream message;
        message << "ERROR: File: " << filename << " does not exist!" << std::endl;
        throw std::runtime_error(message.str());
    }
}
