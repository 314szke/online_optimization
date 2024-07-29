import matplotlib as mpl
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns


data = pd.read_csv("instance_1_scenarios.csv", sep=';')
data['nb_terminals'] = data.filter(regex=("V.*")).sum(axis=1)
sns.histplot(data, x='nb_terminals', kde=True)
plt.show()

vertex_data = data.filter(regex=("V.*")).sum().reset_index()
vertex_data.columns = ['Vertex', 'Occurence']
sns.barplot(vertex_data, x='Vertex', y='Occurence')
plt.show()

fig, ax = plt.subplots(figsize=(12, 6))
edge_data = data.filter(regex=("E.*")).sum().reset_index()
edge_data.columns = ['Edge', 'Occurence']
used_edges = edge_data.loc[edge_data['Occurence'] > 0]
sns.barplot(used_edges, x='Edge', y='Occurence', ax=ax)
ax.tick_params(axis='x', labelrotation=45)
plt.show()
