import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

result_file = "worst_case_results2.csv"
data = pd.read_csv(result_file, sep=';')

plt.figure()
axes = plt.axes(projection="3d")

axes.scatter3D(data['N'], data['K'], data['objective'], c=list(data['color']))

axes.set_xlabel("variables")
axes.set_ylabel("experts")
axes.set_zlabel("objective value")

axes.set_xlim(10, 50)
axes.set_ylim(10, 50)
axes.set_zlim(0.0, 5.0)

axes.set_xticks([10, 20, 30, 40, 50])
axes.set_yticks([10, 20, 30, 40, 50])
axes.set_zticks([0, 0.5, 1, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0])

plt.show()
