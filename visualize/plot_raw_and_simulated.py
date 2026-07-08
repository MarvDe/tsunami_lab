import argparse
import sys

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

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

def plot_gauges(df_raw, df_sim, gauges_raw, gauges_sim, title=None, save_path=None):
    fig, ax = plt.subplots(figsize=(10, 5))

    for gauge in gauges_raw:
        if gauge not in df_raw.columns:
            print(
                f"Warning: column '{gauge}' not found in file "
                f"(available: {list(df_raw.columns)}); skipping.",
                file=sys.stderr,
            )
            continue
        ax.plot(df_raw["Time"], df_raw[gauge], label=f"{gauge}_raw", linewidth=1.2, c="r")

    for gauge in gauges_sim:
        sub = df_sim[df_sim["name"] == gauge].sort_values("sim_time")
        ax.plot(sub["sim_time"], sub["eta"], label=f"{gauge}_sim", linewidth=1.2, c="g")

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

    df_raw = load_gauge_file(args.file_raw)
    df_sim = load_sim_file(args.file_sim)

    # allign the peaks
    m_r = np.argmax(df_raw[args.gauges[0]])
    t_r = df_raw["Time"][m_r]
    m_s = np.argmax(df_sim["eta"])
    t_s = df_sim["sim_time"][m_s]
    print(- t_s + t_r)
    df_sim["sim_time"] = df_sim["sim_time"] + 25.8412 #- t_s + t_r

    print(args.file_raw)
    plot_gauges(df_raw, df_sim, args.gauges, ["gauge_22"], save_path=args.save)

if __name__ == "__main__":
    main()