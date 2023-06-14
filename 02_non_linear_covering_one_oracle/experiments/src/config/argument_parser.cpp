#include "argument_parser.h"

#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>

#define MIN_NB_ARGS 2
#define MAX_NB_ARGS 3


ArgumentParser::ArgumentParser(int argc, char** argv) :
    _argc(argc),
    _argv(argv)
{}

void ArgumentParser::parse()
{
    if (_argc < MIN_NB_ARGS || _argc > MAX_NB_ARGS) {
        std::stringstream message;
        message << std::endl << "Usage1: ./OCF <data_file>.lp <config_file>.conf" << std::endl;
        message << "Usage2: ./OCF <generator_file>.gen" << std::endl;
        message << "ERROR: Inappropriate amount of arguments!" << std::endl;
        throw std::runtime_error(message.str());
    }

    if (_argc == MIN_NB_ARGS) {
        data_file = std::string(_argv[1]);
        std::string format = ".lp";
        if (! std::equal(data_file.rbegin(), data_file.rend(), format.rbegin())) {
            data_file = std::string();
            generator_file = std::string(_argv[1]);
            verify(generator_file, ".gen");
        } else {
            data_file = std::string(_argv[1]);
            verify(data_file, ".lp");
        }
    } else {
        data_file = std::string(_argv[1]);
        config_file = std::string(_argv[2]);

        verify(data_file, ".lp");
        verify(config_file, ".conf");
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
