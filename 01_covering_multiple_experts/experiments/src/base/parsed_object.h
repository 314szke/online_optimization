#pragma once

#include <fstream>
#include <string>


class ParsedObject {
protected:
    ParsedObject();

    void readLine();
    uint32_t readInteger();

    std::ifstream f_in;
    std::string line;
    uint32_t line_counter;
};
