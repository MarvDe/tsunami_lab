#!/usr/bin/env python3
"""
Wave Visualization Script
Reads multiple CSV files (files/*.csv) with columns: x, y, height, momentum_x[, bathymetry]
The bathymetry column is optional: if present it is plotted alongside the wave height,
otherwise only the height is shown.
Generates two animated GIFs:
  1. wave_height.gif  — wave height (+ bathymetry if available) vs. x
  2. wave_momentum.gif — momentum_x vs. x
"""

import glob
import os
import sys

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import pandas as pd


# ── Configuration ─────────────────────────────────────────────────────────────

CSV_PATTERN = "../build/*.csv"
HEIGHT_GIF  = "wave_height.gif"
MOMENTUM_GIF = "wave_momentum.gif"
FPS = 5                 # frames per second in the output GIFs
DPI = 120


# ── Load data ─────────────────────────────────────────────────────────────────

def load_files(pattern: str):
    import re
    paths = sorted(glob.glob(pattern),
                   key=lambda p: int(re.search(r'_(\d+)\.csv$', p).group(1))
                       if re.search(r'_(\d+)\.csv$', p) else p)
    if not paths:
        sys.exit(f"No CSV files found matching '{pattern}'. "
                 "Make sure the files/ directory exists and contains *.csv files.")
    frames = []
    for p in paths:
        df = pd.read_csv(p)
        required = {"x", "height", "momentum_x"}
        missing = required - set(df.columns)
        if missing:
            sys.exit(f"File '{p}' is missing columns: {missing}")
        frames.append(df.sort_values("x").reset_index(drop=True))

    # Bathymetry is optional: only plot it when ALL files provide the column
    has_bathymetry = all("bathymetry" in df.columns for df in frames)
    any_bathy = any("bathymetry" in df.columns for df in frames)
    if has_bathymetry:
        print("Bathymetry column detected — will be plotted alongside wave height.")
    elif any_bathy:
        print("Warning: 'bathymetry' column is absent in some files — "
              "bathymetry will not be plotted.")
    else:
        print("No bathymetry column found — plotting height only.")

    print(f"Loaded {len(frames)} file(s).")
    return frames, paths, has_bathymetry


# ── Axis limits (computed once so the animation doesn't jump around) ──────────

def global_limits(frames: list[pd.DataFrame], has_bathymetry: bool):
    x_min = min(df["x"].min() for df in frames)
    x_max = max(df["x"].max() for df in frames)

    if has_bathymetry:
        h_min = min(min(df["height"].min(), df["bathymetry"].min()) for df in frames)
        h_max = max(max(df["height"].max(), df["bathymetry"].max()) for df in frames)
    else:
        h_min = min(df["height"].min() for df in frames)
        h_max = max(df["height"].max() for df in frames)

    m_min = min(df["momentum_x"].min() for df in frames)
    m_max = max(df["momentum_x"].max() for df in frames)

    # Add a small margin
    def pad(lo, hi, frac=0.05):
        rng = hi - lo or 1.0
        return lo - frac * rng, hi + frac * rng

    return pad(x_min, x_max), pad(h_min, h_max), pad(m_min, m_max)


# ── GIF builder ───────────────────────────────────────────────────────────────

def build_gif(frames, paths, xlim, ylim, output_path,
              plot_fn, ylabel, title_prefix, has_bathymetry=False):
    """
    Generic GIF builder.
    plot_fn(ax, df, has_bathymetry) draws the desired data onto ax.
    """
    fig, ax = plt.subplots(figsize=(9, 4), dpi=DPI)
    fig.patch.set_facecolor("#0d1117")
    ax.set_facecolor("#161b22")
    for spine in ax.spines.values():
        spine.set_color("#30363d")
    ax.tick_params(colors="#8b949e")
    ax.xaxis.label.set_color("#8b949e")
    ax.yaxis.label.set_color("#8b949e")

    ax.set_xlim(*xlim)
    ax.set_ylim(*ylim)
    ax.set_autoscaley_on(False)   # prevent plotted data from expanding the y-axis
    ax.set_xlabel("x")
    ax.set_ylabel(ylabel)

    artists = []   # one list of artists per frame

    for i, (df, path) in enumerate(zip(frames, paths)):
        frame_artists = plot_fn(ax, df, has_bathymetry)
        ax.set_ylim(*ylim)   # re-apply after plotting to undo any matplotlib auto-expansion
        # Create a new Text object each frame so ArtistAnimation can swap them
        title_obj = ax.text(0.5, 1.01,
                            f"{title_prefix} - frame {i+1}/{len(frames)}",
                            transform=ax.transAxes,
                            ha="center", va="bottom",
                            color="#e6edf3", fontsize=11)
        artists.append(frame_artists + [title_obj])

    ani = animation.ArtistAnimation(fig, artists, interval=1000 // FPS,
                                    blit=False, repeat=True)
    ani.save(output_path, writer="pillow", fps=FPS, dpi=DPI)
    plt.close(fig)
    print(f"Saved → {output_path}")


# ── Plot functions ─────────────────────────────────────────────────────────────

def plot_height(ax, df, has_bathymetry=False):
    """Returns list of Artist objects for one frame (height, + bathymetry if available)."""
    artists = []

    line_h, = ax.plot(df["x"], df["height"],
                      color="#58a6ff", linewidth=1.8,
                      label="height", zorder=3)
    artists.append(line_h)

    if has_bathymetry:
        fill_h = ax.fill_between(df["x"], df["height"],
                                  df["bathymetry"],
                                  where=(df["height"] >= df["bathymetry"]),
                                  color="#58a6ff", alpha=0.15, zorder=2)
        line_b, = ax.plot(df["x"], df["bathymetry"],
                          color="#f0883e", linewidth=1.5,
                          linestyle="--", label="bathymetry", zorder=3)
        fill_b = ax.fill_between(df["x"], df["bathymetry"],
                                  ax.get_ylim()[0],
                                  color="#f0883e", alpha=0.25, zorder=1)
        artists += [fill_h, line_b, fill_b]
    else:
        fill_h = ax.fill_between(df["x"], df["height"],
                                  ax.get_ylim()[0],
                                  color="#58a6ff", alpha=0.15, zorder=2)
        artists.append(fill_h)

    return artists


def plot_momentum(ax, df, has_bathymetry=False):
    """Returns list of Artist objects for one frame (momentum_x)."""
    line_m, = ax.plot(df["x"], df["momentum_x"],
                      color="#3fb950", linewidth=1.8, zorder=3)
    fill_m = ax.fill_between(df["x"], df["momentum_x"], 0,
                              where=(df["momentum_x"] >= 0),
                              color="#3fb950", alpha=0.20, zorder=2)
    fill_mn = ax.fill_between(df["x"], df["momentum_x"], 0,
                               where=(df["momentum_x"] < 0),
                               color="#f85149", alpha=0.20, zorder=2)
    ax.axhline(0, color="#30363d", linewidth=0.8, zorder=1)
    return [line_m, fill_m, fill_mn]


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    frames, paths, has_bathymetry = load_files(CSV_PATTERN)
    xlim, hlim, mlim = global_limits(frames, has_bathymetry)

    ylabel_h = "height / bathymetry" if has_bathymetry else "height"
    title_h  = "Wave Height & Bathymetry" if has_bathymetry else "Wave Height"
    build_gif(frames, paths,
              xlim=xlim, ylim=hlim,
              output_path=HEIGHT_GIF,
              plot_fn=plot_height,
              ylabel=ylabel_h,
              title_prefix=title_h,
              has_bathymetry=has_bathymetry)

    build_gif(frames, paths,
              xlim=xlim, ylim=mlim,
              output_path=MOMENTUM_GIF,
              plot_fn=plot_momentum,
              ylabel="momentum_x",
              title_prefix="Wave Momentum (x)")

    print("Done.")


if __name__ == "__main__":
    main()