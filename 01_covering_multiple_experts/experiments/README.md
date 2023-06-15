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

Navigate to the **experiments/** folder!

Run the following command with the experiment name you wish to run:

    ./OCME <test_name>

For example:

    ./OCME instance_1

If you want to generate a new test instance, create a generator file in the generator folder and then run the same command as before to create the necessary files.
## Result

The program prints out the objective values of the optimal offline solution (OPT Offline), the multiplicative weight update algorithm (MWA Online), our algorithm (Our Algo) and the average of the experts (Avg of experts). For the first three, the solutions are displayed as well.
