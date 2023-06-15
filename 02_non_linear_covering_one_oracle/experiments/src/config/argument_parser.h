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
    std::string generator_file;
private:
    void displayUsage();
    void checkIfFileExists(std::string& filename);

    int _argc;
    char** _argv;
    bool mode_generation;
};
