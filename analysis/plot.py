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

# Categorize test types
RANDOM_TYPES = ['random_uniform', 'random_normal']
ADVERSARIAL_TYPES = ['adversarial_over_half', 'adversarial_harmonic', 'adversarial_mixed']
BENCHMARK_TYPES = ['Falkenauer_U', 'Falkenauer_T']


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


def get_category(test_type):
    """Categorize test type into Random, Adversarial, or Benchmark."""
    if test_type in RANDOM_TYPES:
        return 'Random'
    elif test_type in ADVERSARIAL_TYPES:
        return 'Adversarial'
    elif test_type in BENCHMARK_TYPES:
        return 'Benchmark'
    return 'Other'


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
    ax.legend(loc='upper left', fontsize=10, framealpha=0.9)
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
    ax.legend(loc='upper left', fontsize=10, framealpha=0.9)
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
    ax.legend(loc='upper left', fontsize=10, framealpha=0.9)
    ax.grid(True, alpha=0.3)

    filename = 'waste_vs_n'
    if test_type:
        filename += f'_{test_type}'
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, f'{filename}.png'), dpi=150)
    plt.close()


def plot_approx_ratio_by_category(df, output_dir, category, test_types, filename_suffix):
    """Plot approximation ratio for a specific category of test types."""
    data = df[(df['approx_ratio'] > 0) & (df['test_type'].isin(test_types))]

    if len(data) == 0:
        return

    n_types = len([t for t in test_types if t in data['test_type'].unique()])
    if n_types == 0:
        return

    # Create subplots
    ncols = min(n_types, 3)
    nrows = (n_types + ncols - 1) // ncols

    fig, axes = plt.subplots(nrows, ncols, figsize=(5 * ncols, 5 * nrows), squeeze=False)
    axes = axes.flatten()

    actual_types = [t for t in test_types if t in data['test_type'].unique()]
    
    for idx, test_type in enumerate(actual_types):
        ax = axes[idx]
        type_data = data[data['test_type'] == test_type]

        for algo in COLORS.keys():
            algo_data = type_data[type_data['algorithm'] == algo]
            if len(algo_data) > 0:
                grouped = algo_data.groupby('n')['approx_ratio'].mean().reset_index()
                ax.plot(grouped['n'], grouped['approx_ratio'],
                       label=algo, color=COLORS[algo], marker=MARKERS[algo],
                       linewidth=2, markersize=6)

        # Add reference lines
        ax.axhline(y=1.0, color='black', linestyle='--', linewidth=1.5, alpha=0.7, label='Optimal')
        ax.axhline(y=11/9, color='gray', linestyle=':', linewidth=1.5, alpha=0.7, label='FFD bound (11/9)')

        ax.set_xlabel('Number of Items (n)', fontsize=10)
        ax.set_ylabel('Approximation Ratio', fontsize=10)
        ax.set_title(test_type, fontsize=12, fontweight='bold')
        ax.grid(True, alpha=0.3)
        ax.set_ylim(0.95, max(2.1, type_data['approx_ratio'].max() * 1.05))
        ax.legend(loc='upper right', fontsize=8, framealpha=0.9)

    # Hide unused subplots
    for idx in range(len(actual_types), len(axes)):
        axes[idx].set_visible(False)

    plt.suptitle(f'{category} Test Cases - Approximation Ratio', fontsize=14, fontweight='bold', y=1.02)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, f'approx_ratio_{filename_suffix}.png'), dpi=150, bbox_inches='tight')
    plt.close()


def plot_algorithm_comparison_by_category(df, output_dir):
    """Create separate bar charts for each category of test types."""
    categories = [
        ('Random', RANDOM_TYPES, 'random'),
        ('Adversarial', ADVERSARIAL_TYPES, 'adversarial'),
        ('Benchmark (Falkenauer)', BENCHMARK_TYPES, 'benchmark'),
    ]

    for cat_name, cat_types, cat_suffix in categories:
        cat_data = df[df['test_type'].isin(cat_types)]
        if len(cat_data) == 0:
            continue

        # Filter to only data with valid approx_ratio for benchmark
        approx_data = cat_data[cat_data['approx_ratio'] > 0]

        fig, axes = plt.subplots(1, 3, figsize=(15, 5))
        fig.suptitle(f'{cat_name} - Algorithm Comparison', fontsize=14, fontweight='bold')

        algorithms = list(COLORS.keys())
        colors = [COLORS[a] for a in algorithms]
        x = np.arange(len(algorithms))

        # Bins used
        agg_data = cat_data.groupby('algorithm')['bins_used'].mean()
        bins_data = [agg_data.get(a, 0) for a in algorithms]
        bars1 = axes[0].bar(x, bins_data, color=colors)
        axes[0].set_xticks(x)
        axes[0].set_xticklabels(algorithms, rotation=45, ha='right')
        axes[0].set_ylabel('Avg Bins Used', fontsize=11)
        axes[0].set_title('Average Bins Used', fontweight='bold')
        axes[0].grid(True, alpha=0.3, axis='y')

        # Waste
        agg_waste = cat_data.groupby('algorithm')['waste'].mean()
        waste_data = [agg_waste.get(a, 0) for a in algorithms]
        bars2 = axes[1].bar(x, waste_data, color=colors)
        axes[1].set_xticks(x)
        axes[1].set_xticklabels(algorithms, rotation=45, ha='right')
        axes[1].set_ylabel('Avg Waste', fontsize=11)
        axes[1].set_title('Average Waste', fontweight='bold')
        axes[1].grid(True, alpha=0.3, axis='y')

        # Approximation Ratio (if available)
        if len(approx_data) > 0:
            agg_ratio = approx_data.groupby('algorithm')['approx_ratio'].mean()
            ratio_data = [agg_ratio.get(a, 0) for a in algorithms]
            bars3 = axes[2].bar(x, ratio_data, color=colors)
            axes[2].axhline(y=1.0, color='black', linestyle='--', linewidth=1.5, label='Optimal')
            axes[2].set_xticks(x)
            axes[2].set_xticklabels(algorithms, rotation=45, ha='right')
            axes[2].set_ylabel('Avg Approx Ratio', fontsize=11)
            axes[2].set_title('Average Approximation Ratio', fontweight='bold')
            axes[2].legend(loc='upper right', fontsize=9)
            axes[2].grid(True, alpha=0.3, axis='y')
        else:
            agg_runtime = cat_data.groupby('algorithm')['runtime_ms'].mean()
            runtime_data = [agg_runtime.get(a, 0) for a in algorithms]
            bars3 = axes[2].bar(x, runtime_data, color=colors)
            axes[2].set_xticks(x)
            axes[2].set_xticklabels(algorithms, rotation=45, ha='right')
            axes[2].set_ylabel('Avg Runtime (ms)', fontsize=11)
            axes[2].set_title('Average Runtime', fontweight='bold')
            axes[2].grid(True, alpha=0.3, axis='y')

        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, f'algorithm_comparison_{cat_suffix}.png'), dpi=150)
        plt.close()


def plot_heatmap_approx_ratio(df, output_dir):
    """Create a heatmap of approximation ratios by algorithm and test type (all types)."""
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

    # Sort rows by category
    def sort_key(test_type):
        if test_type in RANDOM_TYPES:
            return (0, test_type)
        elif test_type in ADVERSARIAL_TYPES:
            return (1, test_type)
        elif test_type in BENCHMARK_TYPES:
            return (2, test_type)
        return (3, test_type)

    sorted_index = sorted(pivot.index, key=sort_key)
    pivot = pivot.reindex(sorted_index)

    fig, ax = plt.subplots(figsize=(12, max(6, len(pivot) * 0.8)))

    im = ax.imshow(pivot.values, cmap='RdYlGn_r', aspect='auto',
                   vmin=1.0, vmax=min(2.0, pivot.values.max() * 1.05))

    ax.set_xticks(np.arange(len(pivot.columns)))
    ax.set_yticks(np.arange(len(pivot.index)))
    ax.set_xticklabels(pivot.columns, fontsize=12, fontweight='bold')
    ax.set_yticklabels(pivot.index, fontsize=10)

    # Add value annotations
    for i in range(len(pivot.index)):
        for j in range(len(pivot.columns)):
            val = pivot.values[i, j]
            if not np.isnan(val):
                text = ax.text(j, i, f'{val:.3f}', ha='center', va='center',
                              color='black' if val < 1.4 else 'white', fontsize=10, fontweight='bold')

    ax.set_xlabel('Algorithm', fontsize=12, fontweight='bold')
    ax.set_ylabel('Test Type', fontsize=12, fontweight='bold')
    ax.set_title('Approximation Ratio Heatmap (All Test Types)\n(lower is better, 1.0 is optimal)',
                fontsize=14, fontweight='bold')

    cbar = plt.colorbar(im, ax=ax, shrink=0.8)
    cbar.set_label('Approximation Ratio', fontsize=11)

    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'heatmap_approx_ratio.png'), dpi=150)
    plt.close()


def plot_overall_comparison(df, output_dir):
    """Create an overall summary comparison with separate sections."""
    data = df[df['approx_ratio'] > 0]
    if len(data) == 0:
        return

    # Group by algorithm and test type category
    data = data.copy()
    data['category'] = data['test_type'].apply(get_category)

    fig, ax = plt.subplots(figsize=(12, 7))

    algorithms = list(COLORS.keys())
    categories = ['Random', 'Adversarial', 'Benchmark']
    x = np.arange(len(algorithms))
    width = 0.25

    for i, cat in enumerate(categories):
        cat_data = data[data['category'] == cat]
        if len(cat_data) > 0:
            ratios = [cat_data[cat_data['algorithm'] == a]['approx_ratio'].mean() 
                     if a in cat_data['algorithm'].values else 0 for a in algorithms]
            bars = ax.bar(x + i * width, ratios, width, label=cat, alpha=0.85)

    ax.axhline(y=1.0, color='black', linestyle='--', linewidth=2, label='Optimal (1.0)')
    ax.axhline(y=11/9, color='gray', linestyle=':', linewidth=1.5, label='FFD bound (11/9 ≈ 1.22)')

    ax.set_xlabel('Algorithm', fontsize=12, fontweight='bold')
    ax.set_ylabel('Average Approximation Ratio', fontsize=12, fontweight='bold')
    ax.set_title('Algorithm Performance by Test Category', fontsize=14, fontweight='bold')
    ax.set_xticks(x + width)
    ax.set_xticklabels(algorithms, fontsize=11)
    ax.legend(loc='upper right', fontsize=10, framealpha=0.95)
    ax.grid(True, alpha=0.3, axis='y')
    ax.set_ylim(0.95, max(1.8, data['approx_ratio'].max() * 1.05))

    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'overall_comparison.png'), dpi=150)
    plt.close()


def generate_summary_stats(df, output_dir):
    """Generate a text file with summary statistics."""
    with open(os.path.join(output_dir, 'summary.txt'), 'w') as f:
        f.write("=" * 70 + "\n")
        f.write("Bin Packing Algorithm Experimental Summary\n")
        f.write("=" * 70 + "\n\n")

        # Overall statistics
        f.write("OVERALL STATISTICS\n")
        f.write("-" * 50 + "\n")
        f.write(f"Total experiments: {len(df)}\n")
        f.write(f"Algorithms tested: {', '.join(df['algorithm'].unique())}\n")
        f.write(f"Input sizes: {sorted(df['n'].unique())}\n")
        f.write(f"Test types: {', '.join(df['test_type'].unique())}\n\n")

        # Per-category summary
        categories = [
            ('RANDOM TESTS', RANDOM_TYPES),
            ('ADVERSARIAL TESTS', ADVERSARIAL_TYPES),
            ('BENCHMARK TESTS (Falkenauer)', BENCHMARK_TYPES),
        ]

        for cat_name, cat_types in categories:
            cat_data = df[df['test_type'].isin(cat_types)]
            if len(cat_data) == 0:
                continue

            f.write(f"\n{cat_name}\n")
            f.write("-" * 50 + "\n")

            for algo in df['algorithm'].unique():
                algo_data = cat_data[cat_data['algorithm'] == algo]
                if len(algo_data) == 0:
                    continue
                f.write(f"\n  {algo}:\n")
                f.write(f"    Avg bins used: {algo_data['bins_used'].mean():.2f}\n")
                f.write(f"    Avg waste: {algo_data['waste'].mean():.4f}\n")
                f.write(f"    Avg runtime: {algo_data['runtime_ms'].mean():.4f} ms\n")

                approx_data = algo_data[algo_data['approx_ratio'] > 0]
                if len(approx_data) > 0:
                    f.write(f"    Avg approx ratio: {approx_data['approx_ratio'].mean():.4f}\n")
                    f.write(f"    Max approx ratio: {approx_data['approx_ratio'].max():.4f}\n")

        f.write("\n" + "=" * 70 + "\n")
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
    print(f"Test types found: {df['test_type'].unique()}")

    print(f"\nGenerating plots in: {output_dir}")

    # Generate per-test-type plots
    print("  - Bins vs N plots (per test type)...")
    for test_type in df['test_type'].unique():
        plot_bins_vs_n(df, output_dir, test_type)

    print("  - Runtime vs N plots (per test type)...")
    for test_type in df['test_type'].unique():
        plot_runtime_vs_n(df, output_dir, test_type)

    print("  - Waste vs N plots (per test type)...")
    for test_type in df['test_type'].unique():
        plot_waste_vs_n(df, output_dir, test_type)

    # Approximation ratio plots by category
    print("  - Approximation ratio plots (by category)...")
    plot_approx_ratio_by_category(df, output_dir, 'Random', RANDOM_TYPES, 'random')
    plot_approx_ratio_by_category(df, output_dir, 'Adversarial', ADVERSARIAL_TYPES, 'adversarial')
    plot_approx_ratio_by_category(df, output_dir, 'Benchmark', BENCHMARK_TYPES, 'benchmark')

    # Algorithm comparison by category
    print("  - Algorithm comparison bar charts (by category)...")
    plot_algorithm_comparison_by_category(df, output_dir)

    # Heatmap with all test types
    print("  - Heatmap (all test types)...")
    plot_heatmap_approx_ratio(df, output_dir)

    # Overall comparison
    print("  - Overall comparison chart...")
    plot_overall_comparison(df, output_dir)

    # Summary statistics
    print("  - Summary statistics...")
    generate_summary_stats(df, output_dir)

    print(f"\nDone! All plots saved to '{output_dir}/'")
    print("Files generated:")
    for f in sorted(os.listdir(output_dir)):
        print(f"  - {f}")


if __name__ == '__main__':
    main()
