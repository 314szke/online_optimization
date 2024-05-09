# Parameters
input_file = 'experts/connected_1_converted.pred'
output_file = 'experts/connected_1_converted_fixed.pred'
E = 120
R = 20
P = 6
N = 2520


# Data structure to sum up edge costs
variables = []
for p in range(P):
    variables.append([])
    for n in range(N):
        variables[p].append(0.0)


# Reformat the expert file
with open(input_file, 'r') as f_in:
    with open(output_file, 'w') as f_out:
        lines = f_in.readlines()
        f_out.write(lines[0]) # number of experts
        lines = lines[1:]

        for r in range(R):
            for p in range(P):
                l_idx = (r * P) + p
                values = lines[l_idx].split(' ')
                for e in range(E):
                    e_idx = ((r + 1) * E) + e
                    variables[p][e] += float(values[e])
                    variables[p][e_idx] = float(values[e])

            for p in range(P):
                for n in range(N):
                    f_out.write(f'{variables[p][n]} ')
                f_out.write('\n')
