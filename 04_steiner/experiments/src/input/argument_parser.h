#pragma once

#include <string>


class ArgumentParser {
public:
    ArgumentParser(int argc, char** argv);
    void parse();

    std::string test_name;
    std::string config_file;
private:
    void displayUsage();
    void checkIfFileExists(std::string& filename);

    int _argc;
    char** _argv;
};
