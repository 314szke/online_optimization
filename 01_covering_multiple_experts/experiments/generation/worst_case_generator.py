model_file = 'new_model.lp'
expert_file = 'new_model.pred'

N = 10
M = 10
K = 10

with open(model_file, 'w') as f:
    f.write(f'{N}\n')
    f.write(f'{M}\n')
    f.write('min ')
    for i in range(N-1):
        f.write(f'1 x{i} + ')
    f.write(f'1 x{N-1}\n')

    for i in range(N):
        for j in range(i, N-1):
            f.write(f'1 x{j} + ')
        f.write(f'1 x{N-1} >= 1\n')

with open(expert_file, 'w') as f:
    f.write(f'{K}\n')
    for i in range(N):
        for k in range(K-1):
            for j in range(N-1):
                f.write(f'1 ')
            f.write(f'1 \n')
        for j in range(N-1):
            f.write(f'0 ')
        f.write(f'1 \n')

            #if i < k:
            #    for j in range():
            #        f.write(f'0 ')
            #else:
            #    for j in range(i):
            #        f.write(f'0 ')
            #for j in range(i, N-1):
            #    f.write(f'1 ')

