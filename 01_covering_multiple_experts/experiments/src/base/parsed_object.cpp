#include "parsed_object.h"

#include <iostream>
#include <regex>
#include <sstream>


ParsedObject::ParsedObject() :
    line_counter(0)
{}

void ParsedObject::readLine()
{
    line.resize(0);
    std::getline(f_in, line);
    line_counter++;

    if (line.empty()) {
        std::stringstream message;
        message << "ERROR: line " << line_counter << " should not be empty!" << std::endl;
        throw std::runtime_error(message.str());
    }
}

uint32_t ParsedObject::readInteger()
{
    readLine();

    const std::regex integer_pattern("\\s*([0-9]+)\\s*[\\S\\s]*");

    std::smatch matches;
    if (std::regex_match(line, matches, integer_pattern)) {
        std::ssub_match match = matches[1];
        return stoi(match.str());
    }

    std::stringstream message;
    message << "ERROR: the number on line " << line_counter << " has invalid format!" << std::endl;
    throw std::runtime_error(message.str());
}
