#pragma once

#include<fstream>
#include<string>

#include "model.h"


class ModelParser {
public:
    ModelParser(std::string& file_name);
    void parse(Model* model);
private:
    void readLine();
    uint32_t readInteger();

    std::ifstream f_in;
    std::string line;
    uint32_t line_counter;
};
