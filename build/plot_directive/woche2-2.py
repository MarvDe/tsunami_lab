import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("dh20_h5_solution_1.csv")

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 5))

ax1.plot(df.iloc[:, 0], df.iloc[:, 2])
ax1.set_xlabel("X-Position")
ax1.set_ylabel("Y-Wasserhöhe")
ax1.set_title("Höhe links: 20, Höhe rechts: 5, Zeitschritt: 1")

ax2.plot(df.iloc[:, 0], df.iloc[:, 3])
ax2.set_xlabel("X-Position")
ax2.set_ylabel("Y-Wassermomentum")