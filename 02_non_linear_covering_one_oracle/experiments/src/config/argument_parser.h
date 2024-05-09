#pragma once

#include <string>


class ArgumentParser {
public:
    ArgumentParser(int argc, char** argv);
    void parse();
    bool modeIsGeneration() const;
    bool modeIsConversion() const;

    std::string test_name;
    std::string data_file;
    std::string config_file;
    std::string generator_file;
    std::string output_file;
    std::string expert_file;
private:
    void displayUsage();
    void checkIfFileExists(std::string& filename);
    void checkConversionMode();

    int _argc;
    char** _argv;
    bool mode_generation;
    bool mode_conversion;
};
