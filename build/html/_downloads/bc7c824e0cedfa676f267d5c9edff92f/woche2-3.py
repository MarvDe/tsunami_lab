import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("h4u0_8solution_1.csv")

plt.plot(df.iloc[:, 0], df.iloc[:, 2])
plt.xlabel("X-Position")
plt.ylabel("Y-Wasserhöhe")
plt.title("Höhe: 4, Momentum: 0.8")