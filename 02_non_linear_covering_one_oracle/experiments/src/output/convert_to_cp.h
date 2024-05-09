#pragma once

#include <string>

#include "model/model.h"


class ConvertToCP {
public:
    ConvertToCP(const std::string& data_file);
    void convert(const std::string& output_file);
private:
    uint32_t getID(uint32_t e, uint32_t r) const;
    Model model;
};
