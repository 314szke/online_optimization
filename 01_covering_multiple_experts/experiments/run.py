import subprocess

with open('result.csv', 'w') as result_f:
    result_f.write('K;L;objective;type;color\n')
    for K in range(4, 10):
        for L in range(2, 10):
            N = L*K + 1
            M = L* (K-1)
            print(f'K[{K}]\tL[{L}]\tN[{N}]\tM[{M}]')
            subprocess.call(f'./OCME ce_k_{K}_l_{L} > output.txt', shell=True)

            with open('output.txt', 'r') as f:
                lines = f.readlines()
                mwu = float(lines[3].split('=')[1])
                cra = float(lines[5].split('=')[1])
                result_f.write(f'{K};{L};{mwu};MWU;#1f77b4\n')
                result_f.write(f'{K};{L};{cra};CRA;#ff7f0e\n')
