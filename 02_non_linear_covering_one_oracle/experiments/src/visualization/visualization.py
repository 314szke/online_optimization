import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

result_file = "result.txt"

plt.rc('font', size=16)
plt.rc('axes', titlesize=16)
plt.rc('axes', labelsize=16)
plt.rc('xtick', labelsize=16)
plt.rc('ytick', labelsize=16)
plt.rc('legend', fontsize=16)

data = pd.read_csv(result_file, sep=';', index_col=0)

figure, axis = plt.subplots(1, 1, figsize=(7, 7))

sns.lineplot(
    data=data,
    x='Eta',
    y='CompRatio',
    hue='Algo',
    linewidth=2,
    palette='colorblind',
    style='Algo',
    markers=True,
    legend='full')

axis.set_xlabel(r'$\eta$')
axis.set_ylabel('OPT(I) / ALGO(I)')
axis.legend(loc='lower right')

figure.tight_layout()
plt.show()
