#!/usr/bin/env python3
"""
plot_last_row.py

Reads a CSV file with n rows and at least 4 columns (no header).
Plots distance in metres on the x-axis (250 m per sample) against the
value in the fourth column on the y-axis.

Usage:
    python plot_last_row.py <input.csv>
"""

import sys
import pandas as pd
import matplotlib.pyplot as plt


# ── Input ─────────────────────────────────────────────────────────────────────

if len(sys.argv) != 2:
    sys.exit("Usage: python plot_last_row.py <input.csv>")

csv_path = sys.argv[1]

df = pd.read_csv(csv_path, header=None)

if df.shape[1] < 4:
    sys.exit(f"Expected at least 4 columns, found {df.shape[1]}.")

x = df.index * 250    # convert row index to metres (250 m per sample)
y = df.iloc[:, 3]     # fourth column


# ── Plot ──────────────────────────────────────────────────────────────────────

fig, ax = plt.subplots(figsize=(9, 4), dpi=120)
fig.patch.set_facecolor("#0d1117")
ax.set_facecolor("#161b22")
for spine in ax.spines.values():
    spine.set_color("#30363d")
ax.tick_params(colors="#8b949e")
ax.xaxis.label.set_color("#8b949e")
ax.yaxis.label.set_color("#8b949e")
ax.set_title("Distance vs Height", color="#e6edf3", fontsize=11)

# All rows
ax.plot(x, y, color="#58a6ff", linewidth=1.8, zorder=2)
ax.fill_between(x, y, ax.get_ylim()[0], color="#58a6ff", alpha=0.10, zorder=1)

ax.set_xlabel("Distance (m)")
ax.set_ylabel("Height (m)")
# Let matplotlib choose tick spacing so labels are never suppressed
ax.xaxis.set_major_locator(plt.MaxNLocator(nbins=10, integer=True))
ax.xaxis.set_major_formatter(plt.FuncFormatter(lambda val, _: f"{int(val):,}"))

output_path = csv_path.rsplit(".", 1)[0] + "_plot.png"
plt.tight_layout()
plt.savefig(output_path, dpi=120)
plt.close()
print(f"Saved → {output_path}")