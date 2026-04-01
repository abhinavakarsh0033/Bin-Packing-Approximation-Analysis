#!/usr/bin/env python3
"""
Bin Packing Algorithm Analysis - Visualization Module

This script reads experimental results from a CSV file and generates
visualizations comparing different bin packing algorithms.

Usage:
    python plot.py [results.csv] [output_dir]

Default:
    results.csv in current directory
    Plots saved to ./plots/
"""

import sys
import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

# Style configuration
plt.style.use('seaborn-v0_8-whitegrid')
COLORS = {
    'NextFit': '#e74c3c',    # Red
    'FirstFit': '#3498db',   # Blue
    'BestFit': '#2ecc71',    # Green
    'FFD': '#9b59b6',        # Purple
    'BFD': '#f39c12',        # Orange
}
MARKERS = {
    'NextFit': 'o',
    'FirstFit': 's',
    'BestFit': '^',
    'FFD': 'D',
    'BFD': 'v',
}


def load_data(csv_path):
    """Load and preprocess the experimental results."""
    df = pd.read_csv(csv_path)

    # Convert types
    df['n'] = df['n'].astype(int)
    df['bins_used'] = df['bins_used'].astype(int)
    df['waste'] = df['waste'].astype(float)
    df['runtime_ms'] = df['runtime_ms'].astype(float)
    df['approx_ratio'] = df['approx_ratio'].astype(float)

    return df


def plot_bins_vs_n(df, output_dir, test_type=None):
    """Plot bins used vs input size for each algorithm."""
    fig, ax = plt.subplots(figsize=(10, 6))

    # Filter by test type if specified
    data = df if test_type is None else df[df['test_type'] == test_type]

    for algo in COLORS.keys():
        algo_data = data[data['algorithm'] == algo]
        if len(algo_data) > 0:
            grouped = algo_data.groupby('n')['bins_used'].mean().reset_index()
            ax.plot(grouped['n'], grouped['bins_used'],
                   label=algo, color=COLORS[algo], marker=MARKERS[algo],
                   linewidth=2, markersize=8)

    ax.set_xlabel('Number of Items (n)', fontsize=12)
    ax.set_ylabel('Bins Used', fontsize=12)
    title = 'Bins Used vs Input Size'
    if test_type:
        title += f' ({test_type})'
    ax.set_title(title, fontsize=14, fontweight='bold')
    ax.legend(loc='best', fontsize=10)
    ax.grid(True, alpha=0.3)

    filename = 'bins_vs_n'
    if test_type:
        filename += f'_{test_type}'
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, f'{filename}.png'), dpi=150)
    plt.close()


def plot_runtime_vs_n(df, output_dir, test_type=None):
    """Plot runtime vs input size for each algorithm."""
    fig, ax = plt.subplots(figsize=(10, 6))

    data = df if test_type is None else df[df['test_type'] == test_type]

    for algo in COLORS.keys():
        algo_data = data[data['algorithm'] == algo]
        if len(algo_data) > 0:
            grouped = algo_data.groupby('n')['runtime_ms'].mean().reset_index()
            ax.plot(grouped['n'], grouped['runtime_ms'],
                   label=algo, color=COLORS[algo], marker=MARKERS[algo],
                   linewidth=2, markersize=8)

    ax.set_xlabel('Number of Items (n)', fontsize=12)
    ax.set_ylabel('Runtime (ms)', fontsize=12)
    ax.set_yscale('log')  # Log scale for runtime
    title = 'Runtime vs Input Size'
    if test_type:
        title += f' ({test_type})'
    ax.set_title(title, fontsize=14, fontweight='bold')
    ax.legend(loc='best', fontsize=10)
    ax.grid(True, alpha=0.3)

    filename = 'runtime_vs_n'
    if test_type:
        filename += f'_{test_type}'
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, f'{filename}.png'), dpi=150)
    plt.close()


def plot_waste_vs_n(df, output_dir, test_type=None):
    """Plot total waste vs input size for each algorithm."""
    fig, ax = plt.subplots(figsize=(10, 6))

    data = df if test_type is None else df[df['test_type'] == test_type]

    for algo in COLORS.keys():
        algo_data = data[data['algorithm'] == algo]
        if len(algo_data) > 0:
            grouped = algo_data.groupby('n')['waste'].mean().reset_index()
            ax.plot(grouped['n'], grouped['waste'],
                   label=algo, color=COLORS[algo], marker=MARKERS[algo],
                   linewidth=2, markersize=8)

    ax.set_xlabel('Number of Items (n)', fontsize=12)
    ax.set_ylabel('Total Waste (unused capacity)', fontsize=12)
    title = 'Waste vs Input Size'
    if test_type:
        title += f' ({test_type})'
    ax.set_title(title, fontsize=14, fontweight='bold')
    ax.legend(loc='best', fontsize=10)
    ax.grid(True, alpha=0.3)

    filename = 'waste_vs_n'
    if test_type:
        filename += f'_{test_type}'
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, f'{filename}.png'), dpi=150)
    plt.close()


def plot_approx_ratio(df, output_dir):
    """Plot approximation ratio for small inputs where optimal was computed."""
    # Filter to only rows with valid approximation ratio
    data = df[df['approx_ratio'] > 0]

    if len(data) == 0:
        print("No approximation ratio data available (no optimal solutions computed)")
        return

    fig, ax = plt.subplots(figsize=(10, 6))

    for algo in COLORS.keys():
        algo_data = data[data['algorithm'] == algo]
        if len(algo_data) > 0:
            grouped = algo_data.groupby('n')['approx_ratio'].mean().reset_index()
            ax.plot(grouped['n'], grouped['approx_ratio'],
                   label=algo, color=COLORS[algo], marker=MARKERS[algo],
                   linewidth=2, markersize=8)

    # Add theoretical bounds
    ax.axhline(y=1.0, color='black', linestyle='--', linewidth=1.5,
               label='Optimal (ratio=1.0)')
    ax.axhline(y=11/9, color='gray', linestyle=':', linewidth=1.5,
               label='FFD/BFD bound (11/9)')
    ax.axhline(y=1.7, color='lightgray', linestyle=':', linewidth=1.5,
               label='FF/BF bound (1.7)')

    ax.set_xlabel('Number of Items (n)', fontsize=12)
    ax.set_ylabel('Approximation Ratio (algo / optimal)', fontsize=12)
    ax.set_title('Approximation Ratio vs Input Size (Small Inputs)',
                fontsize=14, fontweight='bold')
    ax.legend(loc='best', fontsize=9)
    ax.grid(True, alpha=0.3)
    ax.set_ylim(0.95, 2.1)

    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'approx_ratio.png'), dpi=150)
    plt.close()


def plot_approx_ratio_by_test_type(df, output_dir):
    """Plot approximation ratio grouped by test type."""
    data = df[df['approx_ratio'] > 0]

    if len(data) == 0:
        return

    test_types = data['test_type'].unique()

    fig, axes = plt.subplots(2, 3, figsize=(15, 10))
    axes = axes.flatten()

    for idx, test_type in enumerate(test_types):
        if idx >= len(axes):
            break

        ax = axes[idx]
        type_data = data[data['test_type'] == test_type]

        for algo in COLORS.keys():
            algo_data = type_data[type_data['algorithm'] == algo]
            if len(algo_data) > 0:
                grouped = algo_data.groupby('n')['approx_ratio'].mean().reset_index()
                ax.plot(grouped['n'], grouped['approx_ratio'],
                       label=algo, color=COLORS[algo], marker=MARKERS[algo],
                       linewidth=2, markersize=6)

        ax.axhline(y=1.0, color='black', linestyle='--', linewidth=1, alpha=0.5)
        ax.set_xlabel('n', fontsize=10)
        ax.set_ylabel('Approx Ratio', fontsize=10)
        ax.set_title(test_type, fontsize=11, fontweight='bold')
        ax.grid(True, alpha=0.3)
        ax.set_ylim(0.95, 2.1)

    # Hide unused subplots
    for idx in range(len(test_types), len(axes)):
        axes[idx].set_visible(False)

    # Add legend to figure
    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc='upper center', ncol=5, fontsize=10,
               bbox_to_anchor=(0.5, 1.02))

    plt.tight_layout()
    plt.subplots_adjust(top=0.92)
    plt.savefig(os.path.join(output_dir, 'approx_ratio_by_type.png'), dpi=150)
    plt.close()


def plot_algorithm_comparison_bar(df, output_dir):
    """Create a bar chart comparing algorithms across metrics."""
    # Aggregate data
    agg_data = df.groupby('algorithm').agg({
        'bins_used': 'mean',
        'waste': 'mean',
        'runtime_ms': 'mean'
    }).reset_index()

    fig, axes = plt.subplots(1, 3, figsize=(15, 5))

    algorithms = list(COLORS.keys())
    colors = [COLORS[a] for a in algorithms]
    x = np.arange(len(algorithms))

    # Bins used
    bins_data = [agg_data[agg_data['algorithm'] == a]['bins_used'].values[0]
                 for a in algorithms if a in agg_data['algorithm'].values]
    axes[0].bar(x[:len(bins_data)], bins_data, color=colors[:len(bins_data)])
    axes[0].set_xticks(x[:len(bins_data)])
    axes[0].set_xticklabels(algorithms[:len(bins_data)], rotation=45)
    axes[0].set_ylabel('Avg Bins Used')
    axes[0].set_title('Average Bins Used', fontweight='bold')

    # Waste
    waste_data = [agg_data[agg_data['algorithm'] == a]['waste'].values[0]
                  for a in algorithms if a in agg_data['algorithm'].values]
    axes[1].bar(x[:len(waste_data)], waste_data, color=colors[:len(waste_data)])
    axes[1].set_xticks(x[:len(waste_data)])
    axes[1].set_xticklabels(algorithms[:len(waste_data)], rotation=45)
    axes[1].set_ylabel('Avg Waste')
    axes[1].set_title('Average Waste', fontweight='bold')

    # Runtime
    runtime_data = [agg_data[agg_data['algorithm'] == a]['runtime_ms'].values[0]
                    for a in algorithms if a in agg_data['algorithm'].values]
    axes[2].bar(x[:len(runtime_data)], runtime_data, color=colors[:len(runtime_data)])
    axes[2].set_xticks(x[:len(runtime_data)])
    axes[2].set_xticklabels(algorithms[:len(runtime_data)], rotation=45)
    axes[2].set_ylabel('Avg Runtime (ms)')
    axes[2].set_title('Average Runtime', fontweight='bold')

    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'algorithm_comparison.png'), dpi=150)
    plt.close()


def plot_heatmap_approx_ratio(df, output_dir):
    """Create a heatmap of approximation ratios by algorithm and test type."""
    data = df[df['approx_ratio'] > 0]

    if len(data) == 0:
        return

    # Create pivot table
    pivot = data.pivot_table(
        values='approx_ratio',
        index='test_type',
        columns='algorithm',
        aggfunc='mean'
    )

    # Reorder columns to match COLORS order
    cols = [c for c in COLORS.keys() if c in pivot.columns]
    pivot = pivot[cols]

    fig, ax = plt.subplots(figsize=(10, 6))

    im = ax.imshow(pivot.values, cmap='RdYlGn_r', aspect='auto',
                   vmin=1.0, vmax=2.0)

    ax.set_xticks(np.arange(len(pivot.columns)))
    ax.set_yticks(np.arange(len(pivot.index)))
    ax.set_xticklabels(pivot.columns, fontsize=11)
    ax.set_yticklabels(pivot.index, fontsize=10)

    # Add value annotations
    for i in range(len(pivot.index)):
        for j in range(len(pivot.columns)):
            val = pivot.values[i, j]
            if not np.isnan(val):
                text = ax.text(j, i, f'{val:.2f}', ha='center', va='center',
                              color='black' if val < 1.5 else 'white', fontsize=10)

    ax.set_title('Approximation Ratio Heatmap\n(lower is better, 1.0 is optimal)',
                fontsize=14, fontweight='bold')

    plt.colorbar(im, ax=ax, label='Approximation Ratio')
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'heatmap_approx_ratio.png'), dpi=150)
    plt.close()


def generate_summary_stats(df, output_dir):
    """Generate a text file with summary statistics."""
    with open(os.path.join(output_dir, 'summary.txt'), 'w') as f:
        f.write("=" * 60 + "\n")
        f.write("Bin Packing Algorithm Experimental Summary\n")
        f.write("=" * 60 + "\n\n")

        # Overall statistics
        f.write("OVERALL STATISTICS\n")
        f.write("-" * 40 + "\n")
        f.write(f"Total experiments: {len(df)}\n")
        f.write(f"Algorithms tested: {', '.join(df['algorithm'].unique())}\n")
        f.write(f"Input sizes: {sorted(df['n'].unique())}\n")
        f.write(f"Test types: {', '.join(df['test_type'].unique())}\n\n")

        # Per-algorithm summary
        f.write("PER-ALGORITHM SUMMARY\n")
        f.write("-" * 40 + "\n")
        for algo in df['algorithm'].unique():
            algo_data = df[df['algorithm'] == algo]
            f.write(f"\n{algo}:\n")
            f.write(f"  Avg bins used: {algo_data['bins_used'].mean():.2f}\n")
            f.write(f"  Avg waste: {algo_data['waste'].mean():.4f}\n")
            f.write(f"  Avg runtime: {algo_data['runtime_ms'].mean():.4f} ms\n")

            approx_data = algo_data[algo_data['approx_ratio'] > 0]
            if len(approx_data) > 0:
                f.write(f"  Avg approx ratio: {approx_data['approx_ratio'].mean():.4f}\n")
                f.write(f"  Max approx ratio: {approx_data['approx_ratio'].max():.4f}\n")

        # Approximation ratio summary
        f.write("\n\nAPPROXIMATION RATIO ANALYSIS\n")
        f.write("-" * 40 + "\n")
        approx_data = df[df['approx_ratio'] > 0]
        if len(approx_data) > 0:
            for algo in approx_data['algorithm'].unique():
                algo_approx = approx_data[approx_data['algorithm'] == algo]
                f.write(f"\n{algo}:\n")
                f.write(f"  Mean ratio: {algo_approx['approx_ratio'].mean():.4f}\n")
                f.write(f"  Std dev: {algo_approx['approx_ratio'].std():.4f}\n")
                f.write(f"  Min: {algo_approx['approx_ratio'].min():.4f}\n")
                f.write(f"  Max: {algo_approx['approx_ratio'].max():.4f}\n")

        f.write("\n" + "=" * 60 + "\n")
        f.write("End of Summary\n")


def main():
    # Parse arguments
    csv_path = sys.argv[1] if len(sys.argv) > 1 else 'results.csv'
    output_dir = sys.argv[2] if len(sys.argv) > 2 else 'plots'

    # Check if CSV exists
    if not os.path.exists(csv_path):
        print(f"Error: Results file '{csv_path}' not found.")
        print("Run the experiments first using the runner executable.")
        sys.exit(1)

    # Create output directory
    Path(output_dir).mkdir(parents=True, exist_ok=True)

    print(f"Loading data from: {csv_path}")
    df = load_data(csv_path)
    print(f"Loaded {len(df)} results")

    print(f"Generating plots in: {output_dir}")

    # Generate all plots
    print("  - Bins vs N plots...")
    plot_bins_vs_n(df, output_dir)
    for test_type in df['test_type'].unique():
        plot_bins_vs_n(df, output_dir, test_type)

    print("  - Runtime vs N plots...")
    plot_runtime_vs_n(df, output_dir)

    print("  - Waste vs N plots...")
    plot_waste_vs_n(df, output_dir)

    print("  - Approximation ratio plots...")
    plot_approx_ratio(df, output_dir)
    plot_approx_ratio_by_test_type(df, output_dir)

    print("  - Algorithm comparison bar chart...")
    plot_algorithm_comparison_bar(df, output_dir)

    print("  - Heatmap...")
    plot_heatmap_approx_ratio(df, output_dir)

    print("  - Summary statistics...")
    generate_summary_stats(df, output_dir)

    print(f"\nDone! All plots saved to '{output_dir}/'")
    print("Files generated:")
    for f in sorted(os.listdir(output_dir)):
        print(f"  - {f}")


if __name__ == '__main__':
    main()
