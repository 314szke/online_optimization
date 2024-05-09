import subprocess


def runWorstCaseExample():
    with open('result.csv', 'w') as result_f:
        result_f.write('N;K;objective;type;color\n')
        for N in range(10, 60, 10):
            for K in range(10, 60, 10):
                print(f'N[{N}]\tK[{K}]')
                subprocess.call(f'./OCME wc_n_{N}_k_{K} --linear > output.txt', shell=True)

                with open('output.txt', 'r') as f:
                    lines = f.readlines()
                    mwu = float(lines[3].split('=')[1])
                    cra = float(lines[5].split('=')[1])
                    result_f.write(f'{N};{K};{mwu};MWU;#1f77b4\n')
                    result_f.write(f'{N};{K};{cra};CRA;#ff7f0e\n')


def runCounterExample():
    with open('result.csv', 'w') as result_f:
        result_f.write('K;L;objective;type;color\n')
        for K in range(4, 10):
            for L in range(2, 10):
                N = L*K + 1
                M = L* (K-1)
                print(f'K[{K}]\tL[{L}]\tN[{N}]\tM[{M}]')
                subprocess.call(f'./OCME ce_k_{K}_l_{L} --linear > output.txt', shell=True)

                with open('output.txt', 'r') as f:
                    lines = f.readlines()
                    mwu = float(lines[3].split('=')[1])
                    cra = float(lines[5].split('=')[1])
                    result_f.write(f'{K};{L};{mwu};MWU;#1f77b4\n')
                    result_f.write(f'{K};{L};{cra};CRA;#ff7f0e\n')


### main ###
runWorstCaseExample()
