#pragma once

#include "base/base_model.h"
#include "model/offline_model.h"


class OnlineModel : public BaseModel {
public:
    OnlineModel(const OfflineModel& model);
    void revealNextConstraints();
    bool isSatisfiedBy(const DoubleVec_t& x);
private:
    const OfflineModel& _offline_model;
};
