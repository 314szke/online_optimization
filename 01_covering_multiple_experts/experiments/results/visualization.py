import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

result_file = 'dummy_wc_result.csv'
data = pd.read_csv(result_file, sep=';')

figure, (ax1, ax2, ax3) = plt.subplots(nrows=1, ncols=3)
ax2.axis("off")

ax2 = plt.axes(projection="3d")
ax2.scatter3D(data['N'], data['K'], data['objective'], c=list(data['color']))

ax2.set_xlabel("variables")
ax2.set_ylabel("experts")
ax2.set_zlabel("objective value")

sns.lineplot(data=data, x='N', y='objective', hue='type', ax=ax1)
sns.lineplot(data=data, x='K', y='objective', hue='type', ax=ax3)

ax1.set_xlabel('variables')
ax1.set_ylabel('objective value')
ax3.set_xlabel('experts')
ax3.set_ylabel('objective value')
ax3.yaxis.set_label_position("right")
ax3.yaxis.tick_right()

ax1.set_xlim(9, 51)
ax1.set_ylim(0, 5.0)
ax3.set_xlim(9, 51)
ax3.set_ylim(0, 5.0)

ax1.set_xticks([10, 20, 30, 40, 50])
ax1.set_yticks([0, 0.5, 1, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0])
ax3.set_xticks([10, 20, 30, 40, 50])
ax3.set_yticks([0, 0.5, 1, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0])

ax2.set_xticks([10, 20, 30, 40, 50])
ax2.set_yticks([10, 20, 30, 40, 50])
ax2.set_zticks([2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0])

ax2.set_xlim(10, 50)
ax2.set_ylim(10, 50)
ax2.set_zlim(2.0, 5.0)

ax1.legend(title='Algorithm')
ax3.legend(title='Algorithm', loc='lower right')

plt.show()
