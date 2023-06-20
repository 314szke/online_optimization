#pragma once

#include "base_model.h"
#include "model.h"
#include "types/local_types.h"


class OnlineModel : public BaseModel {
public:
    OnlineModel(Model& model);

    void next();
    void setCurrentSolution(const DoubleVec_t& x);
private:
    Model& _model;
    uint32_t time;
};
