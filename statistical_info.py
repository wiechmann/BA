import numpy as np
import pandas as pd


# Statistical information for M. Burtscher's datasets
# https://userweb.cs.txstate.edu/~burtscher/research/datasets/FPdouble/
# https://userweb.cs.txstate.edu/~burtscher/research/datasets/FPsingle/
# including file size, total and unique values, first-order entropy, and randomness


datasets = [
    "msg_bt",
    "msg_lu",
    "msg_sp",
    "msg_sppm",
    "msg_sweep3d",
    "num_brain",
    "num_comet",
    "num_control",
    "num_plasma",
    "obs_error",
    "obs_info",
    "obs_spitzer",
    "obs_temp",
]

column_names_statistics = [
    "size (megabytes)",
    "floats (millions)",
    "unique values (percent)",
    "1st order entropy (bits)",
    "randomness (percent)",
]

column_names_entropies = [
    "Byte 0",
    "Byte 1",
    "Byte 2",
    "Byte 3",
    "Byte 4",
    "Byte 5",
    "Byte 6",
    "Byte 7",
]


def statistics(data):
    size = data.nbytes
    num_of_values = len(data)
    _, unique_values = np.unique(data, return_counts=True)
    num_of_unique_values = len(unique_values)
    probabilities = unique_values / num_of_values
    entropy = -sum(probabilities * np.log2(probabilities))
    randomness = entropy / np.log2(num_of_unique_values)
    return (
        size / np.exp2(20),  # size of file in MB
        num_of_values / 1000000,  # values in millions
        num_of_unique_values / num_of_values * 100,  # unique values in percent
        entropy,  # first order entropy in bits
        randomness * 100,  # randomness in percent
    )


def byte_entropies(data):
    size = data.nbytes
    num_of_values = len(data)
    bytes = size // num_of_values
    data_as_bytes = data.view(dtype=np.uint8)
    entropies = np.zeros(bytes)

    for i in range(bytes):
        byte_i = data_as_bytes[i::bytes]
        _, unique_values = np.unique(byte_i, return_counts=True)
        probabilities = unique_values / num_of_values
        entropies[i] = -sum(probabilities * np.log2(probabilities))
    return entropies


rows = len(datasets)

prefix = "res/data/single/"
suffix = ".sp"
column_names_statistics[1] = "singles (millions)"

statistical_info = np.zeros((rows, len(column_names_statistics)))
entropies = np.zeros((rows, 4))


for i in range(rows):
    with open(prefix + datasets[i] + suffix, "rb") as file:
        data = np.fromfile(file, dtype=np.float32)

    entropies[i] = byte_entropies(data)
    statistical_info[i] = statistics(data)


df = pd.DataFrame(
    statistical_info,
    index=datasets,
    columns=column_names_statistics,
)
# print(df)
df.to_csv("res/singles.csv", float_format="%.1f")

df = pd.DataFrame(
    entropies,
    index=datasets,
    columns=column_names_entropies[:4],
)
# print(df)
df.to_csv("res/singles_bytes.csv", float_format="%.1f")


prefix = "res/data/double/"
suffix = ".dp"
column_names_statistics[1] = "doubles (millions)"

entropies = np.zeros((rows, 8))


for i in range(rows):
    with open(prefix + datasets[i] + suffix, "rb") as file:
        data = np.fromfile(file, dtype=np.float64)

    entropies[i] = byte_entropies(data)
    statistical_info[i] = statistics(data)


df = pd.DataFrame(
    statistical_info,
    index=datasets,
    columns=column_names_statistics,
)
# print(df)
df.to_csv("res/doubles.csv", float_format="%.1f")

df = pd.DataFrame(
    entropies,
    index=datasets,
    columns=column_names_entropies,
)
# print(df)
df.to_csv("res/doubles_bytes.csv", float_format="%.1f")
