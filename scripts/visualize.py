#!/usr/bin/env python3
"""
Statistical Arbitrage Visualization Tool

This script visualizes results from the Statistical Arbitrage Simulator.
It loads results from CSV files and generates plots for portfolio performance,
spreads, z-scores, and trades.
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import argparse
import os
import sys

def load_data(filename):
    """Load backtest results from CSV file"""
    try:
        data = pd.read_csv(filename)
        return data
    except Exception as e:
        print(f"Error loading data: {e}")
        return None

def plot_portfolio_value(data, output_file=None):
    """Plot portfolio value over time"""
    plt.figure(figsize=(12, 6))
    plt.plot(data['Day'], data['PortfolioValue'])
    plt.title('Portfolio Value Over Time')
    plt.xlabel('Trading Days')
    plt.ylabel('Portfolio Value ($)')
    plt.grid(True)
    
    # Add initial value as horizontal line
    plt.axhline(y=data['PortfolioValue'].iloc[0], color='r', linestyle='--', 
                label=f'Initial Value: ${data["PortfolioValue"].iloc[0]:,.2f}')
    
    # Calculate drawdown
    peak = data['PortfolioValue'].cummax()
    drawdown = (peak - data['PortfolioValue']) / peak
    max_drawdown = drawdown.max()
    
    # Calculate returns
    total_return = (data['PortfolioValue'].iloc[-1] / data['PortfolioValue'].iloc[0]) - 1
    
    # Add metrics annotation
    plt.annotate(f'Total Return: {total_return:.2%}\nMax Drawdown: {max_drawdown:.2%}',
                 xy=(0.02, 0.95), xycoords='axes fraction',
                 bbox=dict(boxstyle="round,pad=0.5", fc="white", alpha=0.8))
    
    plt.legend()
    
    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Portfolio chart saved to {output_file}")
    else:
        plt.show()
    
    plt.close()

def plot_drawdown(data, output_file=None):
    """Plot drawdown over time"""
    # Calculate drawdown
    peak = data['PortfolioValue'].cummax()
    drawdown = (peak - data['PortfolioValue']) / peak
    
    plt.figure(figsize=(12, 6))
    plt.plot(data['Day'], drawdown * 100)
    plt.title('Portfolio Drawdown')
    plt.xlabel('Trading Days')
    plt.ylabel('Drawdown (%)')
    plt.grid(True)
    plt.gca().invert_yaxis()  # Invert y-axis so drawdowns go down
    
    # Add max drawdown line
    max_drawdown = drawdown.max()
    plt.axhline(y=max_drawdown * 100, color='r', linestyle='--', 
                label=f'Max Drawdown: {max_drawdown:.2%}')
    
    plt.legend()
    
    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Drawdown chart saved to {output_file}")
    else:
        plt.show()
    
    plt.close()

def plot_returns_distribution(data, output_file=None):
    """Plot distribution of daily returns"""
    # Calculate daily returns
    data['DailyReturn'] = data['PortfolioValue'].pct_change()
    
    # Create histogram
    plt.figure(figsize=(12, 6))
    plt.hist(data['DailyReturn'].dropna() * 100, bins=50, alpha=0.75)
    plt.title('Distribution of Daily Returns')
    plt.xlabel('Daily Return (%)')
    plt.ylabel('Frequency')
    plt.grid(True)
    
    # Add statistics
    mean_return = data['DailyReturn'].mean()
    std_return = data['DailyReturn'].std()
    sharpe = mean_return / std_return * np.sqrt(252)  # Annualized Sharpe ratio
    
    plt.annotate(f'Mean: {mean_return:.4%}\nStd Dev: {std_return:.4%}\nSharpe: {sharpe:.2f}',
                 xy=(0.02, 0.95), xycoords='axes fraction',
                 bbox=dict(boxstyle="round,pad=0.5", fc="white", alpha=0.8))
    
    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Returns distribution chart saved to {output_file}")
    else:
        plt.show()
    
    plt.close()

def main():
    parser = argparse.ArgumentParser(description='Visualize Statistical Arbitrage backtest results')
    parser.add_argument('results_file', help='CSV file containing backtest results')
    parser.add_argument('--output', '-o', help='Output directory for charts', default='.')
    
    args = parser.parse_args()
    
    # Load data
    data = load_data(args.results_file)
    if data is None:
        sys.exit(1)
    
    # Create output directory if it doesn't exist
    os.makedirs(args.output, exist_ok=True)
    
    # Generate plots
    plot_portfolio_value(data, os.path.join(args.output, 'portfolio_value.png'))
    plot_drawdown(data, os.path.join(args.output, 'drawdown.png'))
    plot_returns_distribution(data, os.path.join(args.output, 'returns_distribution.png'))
    
    print(f"All charts have been saved to {args.output}")

if __name__ == "__main__":
    main() 