#include <iostream>
#include <fstream>
#include <sstream>

#include "config/argument_parser.h"
#include "config/config.h"
#include "input/input_generator.h"
#include "model/model.h"
#include "model/cp_model.h"
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

    if (arg_parser.modeIsGeneration()) {
        InputGenerator input_generator(arg_parser);
        input_generator.generate();
        return 0;
    }

    Config config(arg_parser.config_file);
    Model model(arg_parser.data_file);
    CP_Model cp_model(model);
    config.dimension = cp_model.max_dimension;


    if (model.nb_requests == 0) {
        return 0;
    }


    // Calculate the online solution with a greedy algorithm
    GreedySolver greedy_solver(config, model);
    const DoubleVec_t& greedy_solution = greedy_solver.solve();
    cp_model.setCurrentSolution(greedy_solution);
    double greedy_objective_value = cp_model.getObjectiveValue();
    std::cout << "GREEDY = " << greedy_objective_value << std::endl << std::flush;
    cp_model.getFormattedSolution();
    cp_model.printFormattedSolution();


    // Calculate the optimal offline fractional solution
    FrankWolfe fw_algo(config, cp_model);
    const DoubleVec_t& offline_solution = fw_algo.solve(greedy_solution);
    cp_model.setCurrentSolution(offline_solution);
    double offline_objective_value = cp_model.getObjectiveValue();
    std::cout << "OFFLINE OPTIMAL = " << offline_objective_value << std::endl << std::flush;
    cp_model.getFormattedSolution();
    cp_model.printFormattedSolution();


    // Create predictions
    const CP_Model::SolutionVec_t& formatted_solution = cp_model.getFormattedSolution();
    Prediction oracles(config, model, formatted_solution);


    // Calculate the online solution with different eta and oracle
    for (uint32_t oracle_idx = 0; oracle_idx < config.nb_oracles; oracle_idx++) {
        const UIntMat_t& predictions = oracles.getPredictionsOfOracle(oracle_idx);
        double prediction_objective_value = oracles.getObjectiveValueOfOracle(oracle_idx);
        std::cout << "ORACLE (" << (oracle_idx + 1) << ") = " << prediction_objective_value << std::endl << std::flush;
        oracles.printFormattedSolution(oracle_idx);
    }


/*
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
            config.update(eta);
            eta_values[k].push_back(eta);

            std::cout << "eta = " << eta << std::endl;
            //std::cout << "lambda = " << config.lambda << std::endl;
            //std::cout << "mu = " << config.mu << std::endl;

            Solver solver(config, model);
            DoubleMat_t& online_solution = solver.solve(pred.predict(model.requests[0]), greedy_solution[model.requests[0].id], model.requests[0]);

            for (uint32_t idx = 1; idx < model.requests.size(); idx++) {
                online_solution = solver.solve(pred.predict(model.requests[idx]), greedy_solution[model.requests[idx].id], model.requests[idx]);
            }

            double online_objective_value = model.getObjectiveValue(online_solution);
            comp_ratios[k].push_back(integral_objective_value / online_objective_value);
            //print_solution("online_solution", online_objective_value, online_solution);
            std::cout << "online_solution = " << online_objective_value << std::endl << std::flush;

            eta = eta - config.eta_step;
            std::cout << std::flush;
        }
    }

    double greedy_comp_ratio = (offline_objective_value / greedy_objective_value);

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
*/
    return 0;
}
