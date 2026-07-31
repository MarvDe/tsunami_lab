"""
plot_sim_gauges.py

Reads a solver gauge-output CSV file with columns:
    sim_time,name,x,y,height,momentum_x,momentum_y,bathymetry

and plots the water surface elevation (eta = height + bathymetry)
vs. time for each gauge found in the file.

This is the simulated-side counterpart to plot_gauges.py (which reads
the NOAA lab data files); the "eta" column here is defined the same
way as the lab gauges' surface-elevation columns, so the two plots
can be overlaid directly for comparison.

Usage:
    python plot_sim_gauges.py sim_output.csv
    python plot_sim_gauges.py sim_output.csv --gauges gauge_6 gauge_9
    python plot_sim_gauges.py sim_output.csv --save eta_plot.png
"""

import argparse
import sys

import matplotlib.pyplot as plt
import pandas as pd


def load_sim_file(path):
    """
    Load a solver gauge CSV into a DataFrame and add an 'eta' column
    (water surface elevation = height + bathymetry).
    """
    df = pd.read_csv(path)

    required = {"sim_time", "name", "height", "bathymetry"}
    missing = required - set(df.columns)
    if missing:
        raise ValueError(
            f"Input file is missing expected column(s): {sorted(missing)}. "
            f"Found columns: {list(df.columns)}"
        )

    df["eta"] = df["height"] + df["bathymetry"]
    return df


def plot_eta(df, gauges=None, title=None, save_path=None):
    available = df["name"].unique().tolist()

    if gauges is None:
        gauges = available
    else:
        missing = [g for g in gauges if g not in available]
        if missing:
            print(
                f"Warning: requested gauge(s) not found in file: {missing}. "
                f"Available gauges: {available}",
                file=sys.stderr,
            )
        gauges = [g for g in gauges if g in available]

    fig, ax = plt.subplots(figsize=(10, 5))

    for gauge in gauges:
        sub = df[df["name"] == gauge].sort_values("sim_time")
        ax.plot(sub["sim_time"], sub["eta"], label=gauge, linewidth=1.2)

    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Surface elevation, height + bathymetry (m)")
    ax.set_title(title or "Simulated surface elevation vs. time")
    ax.axhline(0, color="gray", linewidth=0.6, linestyle="--")
    ax.legend()
    ax.grid(True, alpha=0.3)
    fig.tight_layout()

    if save_path:
        fig.savefig(save_path, dpi=150)
        print(f"Saved plot to {save_path}")
    else:
        plt.show()


def main():
    parser = argparse.ArgumentParser(
        description="Plot simulated water surface elevation (height + "
        "bathymetry) vs. time from a solver gauge-output CSV file."
    )
    parser.add_argument("file", help="Path to the solver gauge CSV file")
    parser.add_argument(
        "--gauges",
        nargs="+",
        default=None,
        help="Which gauge names to plot (default: all gauges found in the "
        "file, e.g. gauge_6 gauge_9 gauge_16 gauge_22)",
    )
    parser.add_argument(
        "--save",
        metavar="PATH",
        default=None,
        help="Save the figure to this path instead of displaying it "
        "interactively",
    )
    args = parser.parse_args()

    df = load_sim_file(args.file)
    plot_eta(df, gauges=args.gauges, save_path=args.save)


if __name__ == "__main__":
    main()