#!/usr/bin/env python3
"""
Sample Data Generator for Statistical Arbitrage Simulator

This script generates synthetic price data for pairs trading:
1. Creates cointegrated pairs to test the strategy
2. Introduces mean-reversion and noise to simulate real data
3. Outputs a CSV file with date and price columns
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import argparse
import os
from datetime import datetime, timedelta

def generate_random_walk(n_steps, drift=0.0001, volatility=0.01):
    """Generate a random walk price series with drift"""
    # Generate random steps with drift
    steps = np.random.normal(loc=drift, scale=volatility, size=n_steps)
    
    # Compute price path
    path = 100 * np.cumprod(1 + steps)
    return path

def generate_cointegrated_series(base_series, coef=0.8, noise_vol=0.005, error_persistence=0.9):
    """Generate a cointegrated series based on a base series"""
    n = len(base_series)
    
    # Generate a mean-reverting error term
    error = np.zeros(n)
    for i in range(1, n):
        # AR(1) process
        error[i] = error_persistence * error[i-1] + np.random.normal(0, noise_vol)
    
    # Create cointegrated series
    cointegrated = coef * base_series + error
    
    # Ensure positive prices
    min_value = np.min(cointegrated)
    if min_value <= 0:
        cointegrated = cointegrated - min_value + 1.0
        
    return cointegrated

def generate_trading_dates(n_days, start_date=None):
    """Generate list of trading dates excluding weekends"""
    if start_date is None:
        start_date = datetime(2020, 1, 1)
    
    dates = []
    current_date = start_date
    
    while len(dates) < n_days:
        if current_date.weekday() < 5:  # Monday=0, Friday=4
            dates.append(current_date.strftime('%Y-%m-%d'))
        current_date += timedelta(days=1)
    
    return dates

def generate_sample_data(n_days=252, n_pairs=3, output_file='sample_data.csv'):
    """Generate sample data for pairs trading"""
    print(f"Generating {n_days} days of data with {n_pairs} cointegrated pairs...")
    
    # Generate dates
    dates = generate_trading_dates(n_days)
    
    # Initialize DataFrame with dates
    df = pd.DataFrame({'Date': dates})
    
    for i in range(n_pairs):
        # Generate base series
        base_series = generate_random_walk(n_days)
        
        # Generate cointegrated series with varying coefficients
        coef = 0.5 + np.random.rand() * 1.0  # Random coefficient between 0.5 and 1.5
        coint_series = generate_cointegrated_series(base_series, coef=coef)
        
        # Add to DataFrame
        base_symbol = f'A{i+1}'
        coint_symbol = f'B{i+1}'
        
        df[base_symbol] = base_series
        df[coint_symbol] = coint_series
        
        # Print info about the pair
        print(f"Generated pair {i+1}: {base_symbol}/{coint_symbol} with beta coefficient: {coef:.4f}")
    
    # Add a non-cointegrated stock for testing
    df['C1'] = generate_random_walk(n_days, drift=0.0003, volatility=0.015)
    
    # Save to CSV
    df.to_csv(output_file, index=False)
    print(f"Data saved to {output_file}")
    
    return df

def plot_sample_pair(data, pair_index=0, output_file=None):
    """Plot a sample pair to visualize cointegration"""
    base_symbol = f'A{pair_index+1}'
    coint_symbol = f'B{pair_index+1}'
    
    if base_symbol not in data.columns or coint_symbol not in data.columns:
        print(f"Pair {pair_index+1} not found in data")
        return
    
    # Normalize prices for comparison
    base_norm = data[base_symbol] / data[base_symbol].iloc[0] * 100
    coint_norm = data[coint_symbol] / data[coint_symbol].iloc[0] * 100
    
    # Calculate spread
    spread = data[base_symbol] - (data[coint_symbol] * data[base_symbol].iloc[0] / data[coint_symbol].iloc[0])
    
    # Plot prices
    plt.figure(figsize=(14, 10))
    
    # Price series
    plt.subplot(2, 1, 1)
    plt.plot(base_norm, label=base_symbol)
    plt.plot(coint_norm, label=coint_symbol)
    plt.title(f'Normalized Prices: {base_symbol} vs {coint_symbol}')
    plt.legend()
    plt.grid(True)
    
    # Spread
    plt.subplot(2, 1, 2)
    plt.plot(spread, label='Spread')
    plt.title(f'Price Spread: {base_symbol} - beta*{coint_symbol}')
    plt.grid(True)
    
    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Plot saved to {output_file}")
    else:
        plt.show()
    
    plt.close()

def main():
    parser = argparse.ArgumentParser(description='Generate sample data for Statistical Arbitrage backtesting')
    parser.add_argument('--days', type=int, default=252, help='Number of trading days (default: 252 = 1 year)')
    parser.add_argument('--pairs', type=int, default=3, help='Number of cointegrated pairs (default: 3)')
    parser.add_argument('--output', default='sample_data.csv', help='Output CSV file (default: sample_data.csv)')
    parser.add_argument('--plot', action='store_true', help='Plot sample pairs')
    
    args = parser.parse_args()
    
    # Generate data
    data = generate_sample_data(args.days, args.pairs, args.output)
    
    # Plot sample pair if requested
    if args.plot:
        plot_dir = 'plots'
        os.makedirs(plot_dir, exist_ok=True)
        
        for i in range(args.pairs):
            plot_file = os.path.join(plot_dir, f'pair_{i+1}.png')
            plot_sample_pair(data, i, plot_file)

if __name__ == "__main__":
    main() 