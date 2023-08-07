#pragma once

#include "base/base_model.h"
#include "model/offline_model.h"


class OnlineModel : public BaseModel {
public:
    OnlineModel(const OfflineModel& model);
    void revealNextConstraint();
private:
    const OfflineModel& offline_model;
    int64_t time;
};
