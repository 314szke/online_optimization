#pragma once

#include <string>


class ArgumentParser {
public:
    ArgumentParser(int argc, char** argv);
    void parse();
    bool modeIsGeneration() const;

    std::string test_name;
    std::string data_file;
    std::string config_file;
    std::string expert_file;
    std::string generator_file;
    bool is_convex;
private:
    void displayUsage();
    void checkIfFileExists(std::string& filename);
    void checkIfConfigFileExists(std::string& filename);
    void checkOption(std::string option);

    int _argc;
    char** _argv;
    bool mode_generation;
    std::string general_config;
};
