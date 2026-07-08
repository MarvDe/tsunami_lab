"""
plot_gauges.py

Reads a NOAA/NTHMP conical-island benchmark gauge file (e.g. TS2A.txt,
TS2B.txt, TS2C.txt) and plots surface elevation vs. time for the
requested gauges.

Usage:
    python plot_gauges.py path/to/TS2A.txt
    python plot_gauges.py path/to/TS2A.txt --gauges g6_m g9_m g16_m g22_m
    python plot_gauges.py path/to/TS2A.txt --save output.png
"""

import argparse
import sys

import matplotlib.pyplot as plt
import pandas as pd


def load_gauge_file(path):
    """
    Load a TS2*.txt gauge file into a DataFrame.

    These files have a few lines of free-text header, then a header
    row of column names, then whitespace-delimited data. Missing
    samples are marked with the literal letter 'M'.
    """
    # Find the header row (the one starting with "Time")
    with open(path, "r") as f:
        lines = f.readlines()

    header_idx = None
    for i, line in enumerate(lines):
        if line.strip().startswith("Time"):
            header_idx = i
            break

    if header_idx is None:
        raise ValueError(
            f"Could not find a header row starting with 'Time' in {path}"
        )

    df = pd.read_csv(
        path,
        sep=r"\s+",
        skiprows=header_idx,
        header=0,
        na_values=["M"],
    )

    return df


def plot_gauges(df, gauges, title=None, save_path=None):
    fig, ax = plt.subplots(figsize=(10, 5))

    for gauge in gauges:
        if gauge not in df.columns:
            print(
                f"Warning: column '{gauge}' not found in file "
                f"(available: {list(df.columns)}); skipping.",
                file=sys.stderr,
            )
            continue
        ax.plot(df["Time"], df[gauge], label=gauge, linewidth=1.2)

    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Surface elevation (m)")
    ax.set_title(title or "Gauge surface elevation vs. time")
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
        description="Plot surface elevation vs. time from a NOAA conical "
        "island benchmark gauge file."
    )
    parser.add_argument("file_raw", help="Path to the gauge data file (e.g. TS2A.txt)")
    parser.add_argument("file_sim", help="Path to the solver gauge CSV file")
    parser.add_argument(
        "--gauges",
        nargs="+",
        default=["g6_m", "g9_m", "g16_m", "g22_m"],
        help="Which gauge from the raw file columns to plot (default: the four island gauges "
        "g6_m g9_m g16_m g22_m)",
    )
    parser.add_argument(
        "--save",
        metavar="PATH",
        default=None,
        help="Save the figure to this path instead of displaying it "
        "interactively",
    )
    args = parser.parse_args()

    df = load_gauge_file(args.file)
    plot_gauges(df, args.gauges, save_path=args.save)


if __name__ == "__main__":
    main()