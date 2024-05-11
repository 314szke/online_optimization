#pragma once

#include <string>

#include "base/base_model.h"
#include "base/parsed_object.h"


class OfflineModel : private ParsedObject, public BaseModel {
public:
    OfflineModel(const std::string& data_file, bool is_convex_mode);
};
