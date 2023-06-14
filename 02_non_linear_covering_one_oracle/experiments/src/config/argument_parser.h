#pragma once

#include <string>


class ArgumentParser {
public:
    ArgumentParser(int argc, char** argv);
    void parse();

    std::string data_file;
    std::string config_file;
    std::string generator_file;
private:
    void verify(std::string& filename, std::string file_format);

    int _argc;
    char** _argv;
};
