model_file = 'new_model.lp'
expert_file = 'new_model.pred'

K = 10
L = 4
N = L*K + 1
M = L* (K-1)

with open(model_file, 'w') as f:
    f.write(f'{N}\n')
    f.write(f'{M}\n')
    f.write('min ')
    for i in range(N-1):
        f.write(f'1 x{i} + ')
    f.write(f'1 x{N-1}\n')

    for l in range(L):
        for k in range(K-1):
            start = l*K + k
            stop = (l+1) * K
            for i in range(start, stop):
                f.write(f'1 x{i} + ')
            f.write(f'1 x{N-1} >= 1\n')

with open(expert_file, 'w') as f:
    f.write(f'{K}\n')
    for l1 in range(L):
        for c in range(K-1):
            for k in range(K):
                for l2 in range(l1):
                    start = l2*K
                    stop = l2*K + k
                    for i in range(start, stop):
                        f.write(f'0 ')
                    start = l2*K + k
                    stop = l2*K + K-1
                    for i in range(start, stop):
                        f.write(f'1 ')
                    if (k == (K-1)):
                        f.write(f'1 ')
                    else:
                        f.write(f'0 ')

                start = l1*K
                stop = l1*K + k
                for i in range(start, stop):
                    f.write(f'0 ')
                f.write(f'1 ')
                start = l1*K + k +1
                stop = l1*K + c + 1
                for i in range(start, stop):
                    f.write(f'1 ')
                if (k > c) :
                    start = l1*K + k +1
                else:
                    start = l1*K + c +1
                stop = l1*K + K
                for i in range(start, stop):
                    f.write(f'0 ')

                for _ in range(((l1+1)*K), N):
                    f.write(f'0 ')
                f.write('\n')
