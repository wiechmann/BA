import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


# a heatmap of byte entropies for M. Burtschers's datasets
# https://userweb.cs.txstate.edu/~burtscher/research/datasets/FPdouble/
# https://userweb.cs.txstate.edu/~burtscher/research/datasets/FPsingle/


# both doubles and singles in a single combined plot:

fig, (ax1, ax2) = plt.subplots(1, 2)

# ax1.set_title("binary64")
# ax2.set_title("binary32")
# ax1.set_title("doubles")
# ax2.set_title("singles")

color_map = "PuBu"
textcolors = ["black", "white"]
threshold = 0.75
labels = [
    "Byte 7",
    "Byte 6",
    "Byte 5",
    "Byte 4",
    "Byte 3",
    "Byte 2",
    "Byte 1",
    "Byte 0",
]

df_doubles = pd.read_csv("res/doubles_bytes.csv", index_col=0).loc[:, ::-1]
datasets = df_doubles.index
im = ax1.imshow(df_doubles, cmap=color_map)
ax1.set_xticks(np.arange(8), labels=labels)

plt.setp(ax1.get_xticklabels(), rotation=45, ha="right", rotation_mode="anchor")
ax1.set_yticks(np.arange(len(datasets)), labels=datasets)
ax1.tick_params(axis="y", which="both", length=0, pad=10)

for i in range(len(datasets)):
    for j in range(8):
        text = ax1.text(
            j,
            i,
            df_doubles.iat[i, j].round(1),
            ha="center",
            va="center",
            color=textcolors[int(im.norm(df_doubles.iat[i, j]) > threshold)],
        )


df_singles = pd.read_csv("res/singles_bytes.csv", index_col=0).loc[:, ::-1]
im = ax2.imshow(df_singles, cmap=color_map)
ax2.set_xticks(np.arange(4), labels=labels[4:])
plt.setp(ax2.get_xticklabels(), rotation=45, ha="right", rotation_mode="anchor")
ax2.set_yticks([])

for i in range(len(datasets)):
    for j in range(4):
        text = ax2.text(
            j,
            i,
            df_singles.iat[i, j].round(1),
            ha="center",
            va="center",
            color=textcolors[int(im.norm(df_singles.iat[i, j]) > threshold)],
        )
fig.tight_layout(pad=0.5)
plt.savefig("res/heatmap.png", dpi=600)
# plt.show()


# heatmap for doubles only

fig, ax = plt.subplots()

# ax.set_title("binary64")
# ax.set_title("doubles")

im = ax.imshow(df_doubles, cmap=color_map)
ax.set_xticks(np.arange(8), labels=labels)

plt.setp(ax.get_xticklabels(), rotation=45, ha="right", rotation_mode="anchor")
ax.set_yticks(np.arange(len(datasets)), labels=datasets)
ax.tick_params(axis="y", which="both", length=0, pad=10)

for i in range(len(datasets)):
    for j in range(8):
        text = ax.text(
            j,
            i,
            df_doubles.iat[i, j].round(1),
            ha="center",
            va="center",
            color=textcolors[int(im.norm(df_doubles.iat[i, j]) > threshold)],
        )

fig.tight_layout(pad=0.5)
plt.savefig("res/heatmap_doubles.png", dpi=600)
# plt.show()


# heatmap for singles only

fig, ax = plt.subplots()

# ax.set_title("binary32")
# ax.set_title("singles")

im = ax.imshow(df_singles, cmap=color_map)
ax.set_xticks(np.arange(4), labels=labels[4:])
plt.setp(ax.get_xticklabels(), rotation=45, ha="right", rotation_mode="anchor")
ax.set_yticks(np.arange(len(datasets)), labels=datasets)
ax.tick_params(axis="y", which="both", length=0, pad=10)

for i in range(len(datasets)):
    for j in range(4):
        text = ax.text(
            j,
            i,
            df_singles.iat[i, j].round(1),
            ha="center",
            va="center",
            color=textcolors[int(im.norm(df_singles.iat[i, j]) > threshold)],
        )
fig.tight_layout(pad=0.5)
plt.savefig("res/heatmap_singles.png", dpi=600)
# plt.show()
