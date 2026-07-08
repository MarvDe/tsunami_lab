#!/usr/bin/env python3
"""Generate NTHMP BP01 figures for the Sphinx benchmark documentation."""

import bisect
import csv
import math
import os
from pathlib import Path

os.environ.setdefault("MPLCONFIGDIR", "/tmp/matplotlib-tsunami-lab")

import matplotlib.pyplot as plt


ROOT = Path(__file__).resolve().parents[1]
BP01 = ROOT / "benchmarks" / "nthmp_bp01"
OUT_DIR = ROOT / "benchmarks" / "nthmp_bp01"

G = 9.81
DEPTH = 1.0
SHORELINE_X = 50.0
SLOPE_INV = 19.85
WAVE_HEIGHT = 0.019
WAVE_CENTER_X = 90.0

STATIONS = [
    {
        "name": "nthmp_bp01_xd_0_25",
        "file": "nthmp_bp01_xd_0_25.csv",
        "x": 50.25,
        "label": "Station 1, x/d=0.25",
        "ref_id": 0,
    },
    {
        "name": "nthmp_bp01_xd_9_95",
        "file": "nthmp_bp01_xd_9_95.csv",
        "x": 59.95,
        "label": "Station 2, x/d=9.95",
        "ref_id": 1,
    },
]


def bathymetry(x):
    x_from_shore = x - SHORELINE_X
    if x_from_shore <= 0:
        return -x_from_shore / SLOPE_INV
    return -min(DEPTH, x_from_shore / SLOPE_INV)


def initial_eta(x):
    k = math.sqrt(3.0 * WAVE_HEIGHT / (4.0 * DEPTH**3))
    sech = 1.0 / math.cosh(k * (x - WAVE_CENTER_X))
    return WAVE_HEIGHT * sech * sech



def plot_setup_stations():
    xs = [45.0 + i * 0.05 for i in range(1501)]
    bath = [bathymetry(x) for x in xs]
    eta = [initial_eta(x) for x in xs]

    fig, ax = plt.subplots(figsize=(10.5, 4.8))
    ax.plot(xs, bath, color="#7f5539", linewidth=2.1, label="bathymetry b")
    ax.plot(xs, eta, color="#1f77b4", linewidth=1.8, label="initial free surface eta")
    ax.fill_between(xs, bath, eta, where=[e >= b for e, b in zip(eta, bath)], color="#9ecae1", alpha=0.45)
    ax.axvline(SHORELINE_X, color="black", linestyle="--", linewidth=1.0, label="initial shoreline")

    label_positions = {
        "nthmp_bp01_xd_0_25": (51.6, 0.27),
        "nthmp_bp01_xd_9_95": (62.0, 0.18),
    }
    for station in STATIONS:
        ax.axvline(station["x"], color="#d62728", linewidth=1.1, alpha=0.8)
        text_x, text_y = label_positions[station["name"]]
        ax.annotate(
            station["label"],
            xy=(station["x"], 0.02),
            xytext=(text_x, text_y),
            arrowprops={"arrowstyle": "->", "lw": 0.9, "color": "#d62728"},
            fontsize=8,
            ha="left",
            va="center",
        )

    ax.set_xlim(45, 120)
    ax.set_ylim(-1.08, 0.42)
    ax.set_xlabel("x [m]")
    ax.set_ylabel("vertical coordinate [m]")
    ax.set_title("NTHMP BP01 setup with station positions")
    ax.grid(True, alpha=0.3)
    ax.legend(loc="lower right", fontsize=8)
    fig.tight_layout()
    fig.savefig(OUT_DIR / "bp01_setup_stations.png", dpi=200)
    plt.close(fig)



def main():
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    plot_setup_stations()


if __name__ == "__main__":
    main()
