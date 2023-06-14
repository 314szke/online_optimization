#include <iostream>
#include <fstream>
#include <sstream>

#include "config/argument_parser.h"
#include "config/config.h"
#include "input/input_generator.h"
#include "model/model.h"
#include "model/request.hpp"
#include "offline/frank_wolfe.h"
#include "online/greedy_solver.h"
#include "online/solver.h"
#include "path_finding/floyd_warshall.h"
#include "prediction/prediction.h"
#include "visualization/print.hpp"


int main(int argc, char** argv)
{
    std::cout << std::fixed;
    std::cout.precision(2);

    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    arg_parser.parse();

    if (! arg_parser.generator_file.empty()) {
        InputGenerator input_generator(arg_parser.generator_file);
        input_generator.generate();
        return 0;
    }

    Config config;
    config.parse(arg_parser.config_file);

    Model model;
    model.parse(arg_parser.data_file);

    const Model::RequestVec_t& requests = model.getRequests();
    std::cout << "Model parsed, number of requests = " << requests.size() << std::endl << std::flush;

    if (requests.empty()) {
        return 0;
    }

    // Calculate the offline fractional solution
    FrankWolfe fw_algo(config, model, requests);
    DoubleVec_t extra_cost(model.getNbEdges(), 0.0);
    DoubleMat_t& offline_solution = fw_algo.solve(extra_cost);
    double offline_objective_value = model.getObjectiveValue(offline_solution);
    //print_solution("offline_solution", offline_objective_value, offline_solution);
    std::cout << "offline_solution = " << offline_objective_value << std::endl << std::flush;

    // Calculate the online solution with a greedy algorithm
    GreedySolver greedy_solver(model);
    DoubleMat_t& greedy_solution = greedy_solver.solve(requests[0]);

    for (uint32_t idx = 1; idx < requests.size(); idx++) {
        greedy_solution = greedy_solver.solve(requests[idx]);
    }

    double greedy_objective_value = model.getObjectiveValue(greedy_solution);
    double greedy_comp_ratio = (offline_objective_value / greedy_objective_value);
    //print_solution("greedy_solution", greedy_objective_value, greedy_solution);
    std::cout << "greedy_solution = " << greedy_objective_value << std::endl << std::flush;

    // Create predictions
    Prediction pred(model, offline_solution);
    const DoubleMat_t& integral_solution = pred.createPredictionWithError(0.0);
    double integral_objective_value = model.getObjectiveValue(integral_solution);
    std::cout << "integral_solution = " << integral_objective_value << std::endl << std::flush;

    // Needed to construct the coefficients for the algorithm
    FloydWarshall floyd_warshall(model);
    config.dimension = floyd_warshall.getMaxDimension();
    std::cout << std::flush;

    // Calculate the online solution with different error and eta
    DoubleMat_t eta_values;
    DoubleMat_t comp_ratios;
    DoubleVec_t pred_comp_ratios;
    DoubleVec_t errors = {0.0, 0.1, 0.2};

    for (uint32_t k = 0; k < errors.size(); k++) {
        eta_values.push_back(DoubleVec_t());
        comp_ratios.push_back(DoubleVec_t());

        const DoubleMat_t& pred_solution = pred.createPredictionWithError(errors[k]);
        double pred_error = pred.getErrorRate();
        double pred_objective_value = model.getObjectiveValue(pred_solution);
        errors[k] = pred_error;
        pred_comp_ratios.push_back(integral_objective_value / pred_objective_value);

        std::cout << "prediction_error = " << pred_error << std::endl;
        //print_solution("prediction_solution", pred_objective_value, pred_solution);
        std::cout << "prediction_solution = " << pred_objective_value << std::endl << std::flush;

        double eta = 1.0;
        while (eta > 0) {
            config.eta = eta;
            config.calculateLambda();
            config.calculateMu();
            eta_values[k].push_back(eta);

            std::cout << "eta = " << eta << std::endl;
            //std::cout << "lambda = " << config.lambda << std::endl;
            //std::cout << "mu = " << config.mu << std::endl;

            Solver solver(config, model);
            DoubleMat_t& online_solution = solver.solve(pred.predict(requests[0]), greedy_solution[requests[0].id], requests[0]);

            for (uint32_t idx = 1; idx < requests.size(); idx++) {
                online_solution = solver.solve(pred.predict(requests[idx]), greedy_solution[requests[idx].id], requests[idx]);
            }

            double online_objective_value = model.getObjectiveValue(online_solution);
            comp_ratios[k].push_back(integral_objective_value / online_objective_value);
            //print_solution("online_solution", online_objective_value, online_solution);
            std::cout << "online_solution = " << online_objective_value << std::endl << std::flush;

            eta = eta - config.eta_step;
            std::cout << std::flush;
        }
    }

    std::ofstream out("result.txt");
    out << "Eta;CompRatio;Algo" << std::endl;
    for (uint32_t k = 0; k < errors.size(); k++) {
        for (uint32_t idx = 0; idx < eta_values[k].size(); idx++) {
            out << eta_values[k][idx] << ";" << comp_ratios[k][idx] << ";algo(error=" << errors[k] << ")" << std::endl;
            out << eta_values[k][idx] << ";" << greedy_comp_ratio << ";greedy" << std::endl;
            out << eta_values[k][idx] << ";" << pred_comp_ratios[k] << ";pred(error=" << errors[k] << ")" << std::endl;
        }
    }
    out.close();

    return 0;
}
