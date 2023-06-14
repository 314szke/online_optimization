#include "solver.h"

#include "visualization/print.hpp"


Solver::Solver(Config& config, Model& model):
    eps(config.epsilon),
    d(config.dimension),
    L(config.lambda),
    eta(config.eta),
    R(config.random_iteration),
    c(0),
    _model(model),
    cbfs(model),
    random_engine(rd()),
    uni_dist(0.0, 1.0)
{
    x.resize(_model.getNbRequests());
    B.resize(_model.getNbRequests());
    marker.resize(_model.getNbRequests());
    temp.resize(_model.getNbRequests());

    for (uint32_t r = 0; r < _model.getNbRequests(); r++) {
        x[r].resize(_model.getNbEdges());
        B[r].resize(_model.getNbEdges());
        marker[r].resize(_model.getNbEdges());
        temp[r].resize(_model.getNbEdges());

        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            x[r][e] = 0.0;
            B[r][e] = 0.0;
            marker[r][e] = false;
            temp[r][e] = 0.0;
        }
    }
}

DoubleMat_t& Solver::solve(const BoolVec_t& pred_x, const DoubleVec_t& greedy, const Request& request)
{
    BoolVec_t edges(_model.getNbEdges(), false);
    uint32_t pred_size = 0;
    for (uint32_t idx = 0; idx < _model.getNbEdges(); idx++) {
        if (pred_x[idx]) {
            edges[idx] = true;
            pred_size++;
        }
        if (greedy[idx] > 0.0) {
            edges[idx] = true;
        }
    }

    double delta_e_F;
    double lambda_delta_e_F;

    while (! constraintIsSatisfied(request.source, request.target)) {
        for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
            if (! marker[c][e] && edges[e]) {
                delta_e_F = getDeltaF(e);
                lambda_delta_e_F = (1.0 / L) * delta_e_F;
                if (B[c][e] < lambda_delta_e_F) {
                    B[c][e] = lambda_delta_e_F;
                }

                x[c][e] += x[c][e] / (L * B[c][e]) + eta / (L * B[c][e] * d);
                if (pred_x[e]) {
                    x[c][e] += (1 - eta) / (delta_e_F * pred_size);
                }

                if (x[c][e] >= (1.0 - eps)) {
                    marker[c][e] = true;
                    x[c][e] = 1.0;
                }
            }
        }
    }


    UIntVec_t path = cbfs.getPath(request.source, request.target, marker[c]);
    x[c] = DoubleVec_t(_model.getNbEdges(), 0.0);
    for (uint32_t e = 0; e < path.size(); e++) {
        x[c][path[e]] = 1.0;
    }


    for (uint32_t e = 0; e < _model.getNbEdges(); e++) {
        temp[c][e] = x[c][e];
    }

    c++;
    return x;
}

bool Solver::constraintIsSatisfied(const uint32_t s, const uint32_t t)
{
    UIntVec_t path = cbfs.getPath(s, t, marker[c]);
    return (! path.empty());
}

double Solver::getDeltaF(const uint32_t e)
{
    double value = 0.0;
    double random_number;
    BoolVec_t random_marker(_model.getNbEdges());

    for (uint32_t r = 0; r < R; r++) {
        for (uint32_t idx = 0; idx < _model.getNbEdges(); idx++) {
            random_marker[idx] = false;
            temp[c][idx] = 0.0;
            if (idx != e) {
                random_number = uni_dist(random_engine);
                if (random_number < x[c][idx]) {
                    random_marker[idx] = true;
                    temp[c][idx] = 1.0;
                }
            }
        }
        value -= _model.getObjectiveValue(temp);
        temp[c][e] = 1.0;
        value += _model.getObjectiveValue(temp);
    }

    value = value / R;
    return value;
}
