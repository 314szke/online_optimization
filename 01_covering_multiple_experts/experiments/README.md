# Experiments with our online algorithm with multiple experts

## Requirements

    Gurobi version at least 10
    C++17

## Build

Run the following command:

    make

## Experiment name list

    instance_1
    instance_2
    instance_3
    instance_4
    counter_example
    worst_case

## Execution

Run the following command with the experiment name (here we used instance_1) you wish to run:

    ./OCME data/instance_1.lp config/instance_1.conf experts/instance_1.pred

## Result

The program prints out the objective values of the optimal offline solution (OPT Offline), the multiplicative weight update algorithm (MWA Online), our algorithm (Our Algo) and the average of the experts (Avg of experts). For the first three, the solutions are displayed as well.
