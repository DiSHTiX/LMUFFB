import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from pathlib import Path
from typing import Optional

def plot_slope_timeseries(
    df: pd.DataFrame, 
    output_path: Optional[str] = None,
    show: bool = True
) -> str:
    """
    Generate 4-panel time-series plot for slope detection analysis.
    """
    fig, axes = plt.subplots(4, 1, figsize=(14, 12), sharex=True)
    fig.suptitle('Slope Detection Analysis - Time Series', fontsize=14, fontweight='bold')
    
    time = df['Time'] if 'Time' in df.columns else np.arange(len(df)) * 0.01
    
    # Panel 1: Inputs (Lat G and Slip Angle)
    ax1 = axes[0]
    ax1.plot(time, df['LatAccel'] / 9.81, label='Lateral G', color='#2196F3', alpha=0.8)
    ax1.set_ylabel('Lateral G', color='#2196F3')
    ax1.tick_params(axis='y', labelcolor='#2196F3')
    ax1.legend(loc='upper left')
    ax1.grid(True, alpha=0.3)
    
    ax1_twin = ax1.twinx()
    if 'calc_slip_angle_front' in df.columns:
        ax1_twin.plot(time, df['calc_slip_angle_front'], label='Slip Angle', 
                      color='#FF9800', alpha=0.8)
    ax1_twin.set_ylabel('Slip Angle (rad)', color='#FF9800')
    ax1_twin.tick_params(axis='y', labelcolor='#FF9800')
    ax1_twin.legend(loc='upper right')
    ax1.set_title('Inputs: Lateral G and Slip Angle')
    
    # Panel 2: Derivatives
    ax2 = axes[1]
    if 'dG_dt' in df.columns:
        ax2.plot(time, df['dG_dt'], label='dG/dt', color='#2196F3', alpha=0.8)
    if 'dAlpha_dt' in df.columns:
        ax2.plot(time, df['dAlpha_dt'], label='dAlpha/dt', color='#FF9800', alpha=0.8)
        ax2.axhline(0.02, color='#F44336', linestyle='--', alpha=0.5, label='Threshold (0.02)')
        ax2.axhline(-0.02, color='#F44336', linestyle='--', alpha=0.5)
    ax2.set_ylabel('Derivative')
    ax2.legend(loc='upper right')
    ax2.grid(True, alpha=0.3)
    ax2.set_title('Derivatives: dG/dt and dAlpha/dt')
    
    # Panel 3: Slope
    ax3 = axes[2]
    if 'SlopeCurrent' in df.columns:
        ax3.plot(time, df['SlopeCurrent'], label='Slope (dG/dAlpha)', color='#9C27B0', linewidth=0.8)
        ax3.axhline(-0.3, color='#F44336', linestyle='--', alpha=0.5, label='Neg Threshold (-0.3)')
        ax3.axhline(0, color='#4CAF50', linestyle='-', alpha=0.3)
    ax3.set_ylabel('Slope (G/rad)')
    ax3.set_ylim(-15, 15)  # Clamp for visibility
    ax3.legend(loc='upper right')
    ax3.grid(True, alpha=0.3)
    ax3.set_title('Calculated Slope (dG/dAlpha)')
    
    # Panel 4: Grip Output
    ax4 = axes[3]
    grip_col = 'GripFactor' if 'GripFactor' in df.columns else 'SlopeSmoothed'
    if grip_col in df.columns:
        ax4.plot(time, df[grip_col], label='Grip Factor', color='#4CAF50', linewidth=1.0)
        ax4.axhline(0.2, color='#9E9E9E', linestyle='--', alpha=0.5, label='Floor (0.2)')
        ax4.axhline(1.0, color='#9E9E9E', linestyle='--', alpha=0.5)
    ax4.set_ylabel('Grip Factor')
    ax4.set_xlabel('Time (s)')
    ax4.set_ylim(0, 1.1)
    ax4.legend(loc='upper right')
    ax4.grid(True, alpha=0.3)
    ax4.set_title('Output: Grip Factor')
    
    # Add markers if present
    if 'Marker' in df.columns:
        marker_times = time[df['Marker'] == 1]
        for ax in axes:
            for mt in marker_times:
                ax.axvline(mt, color='#E91E63', linestyle='-', alpha=0.7, linewidth=2)
    
    plt.tight_layout()
    
    if output_path:
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        plt.close()
        return output_path
    
    if show:
        plt.show()
    
    return ""

def plot_slip_vs_latg(
    df: pd.DataFrame,
    output_path: Optional[str] = None,
    show: bool = True
) -> str:
    """
    Scatter plot of Slip Angle vs Lateral G (tire curve visualization).
    """
    fig, ax = plt.subplots(figsize=(10, 8))
    
    slip_col = 'calc_slip_angle_front' if 'calc_slip_angle_front' in df.columns else None
    if slip_col is None:
        return ""
    
    slip = np.abs(df[slip_col])
    lat_g = np.abs(df['LatAccel'] / 9.81) if 'LatAccel' in df.columns else None
    
    if lat_g is None:
        return ""
    
    # Color by speed
    speed = df['Speed'] * 3.6 if 'Speed' in df.columns else None
    
    scatter = ax.scatter(slip, lat_g, c=speed, cmap='viridis', alpha=0.3, s=2)
    
    ax.set_xlabel('Slip Angle (rad)')
    ax.set_ylabel('Lateral G')
    ax.set_title('Tire Curve: Slip Angle vs Lateral G')
    ax.grid(True, alpha=0.3)
    
    if speed is not None:
        cbar = plt.colorbar(scatter, ax=ax)
        cbar.set_label('Speed (km/h)')
    
    # Mark the theoretical peak region
    ax.axvline(0.08, color='#F44336', linestyle='--', alpha=0.5, label='Typical Peak (~0.08 rad)')
    ax.legend()
    
    plt.tight_layout()
    
    if output_path:
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        plt.close()
        return output_path
    
    if show:
        plt.show()
    
    return ""

def plot_dalpha_histogram(
    df: pd.DataFrame,
    output_path: Optional[str] = None,
    show: bool = True
) -> str:
    """
    Histogram of dAlpha/dt values (shows when slope calculation is "active").
    """
    if 'dAlpha_dt' not in df.columns:
        return ""
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    dalpha = df['dAlpha_dt'].values
    
    # Create histogram
    ax.hist(dalpha, bins=100, color='#2196F3', alpha=0.7, edgecolor='white')
    
    # Mark the threshold
    threshold = 0.02
    ax.axvline(threshold, color='#F44336', linestyle='--', linewidth=2, label=f'Threshold (+{threshold})')
    ax.axvline(-threshold, color='#F44336', linestyle='--', linewidth=2, label=f'Threshold (-{threshold})')
    
    # Calculate percentages
    above_threshold = (np.abs(dalpha) > threshold).mean() * 100
    
    ax.set_xlabel('dAlpha/dt (rad/s)')
    ax.set_ylabel('Frequency')
    ax.set_title(f'Distribution of dAlpha/dt\n{above_threshold:.1f}% of frames above threshold (active calculation)')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    if output_path:
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        plt.close()
        return output_path
    
    if show:
        plt.show()
    
    return ""

def plot_slope_correlation(
    df: pd.DataFrame,
    output_path: Optional[str] = None,
    show: bool = True
) -> str:
    """
    Scatter plot of dAlpha/dt vs SlopeCurrent to detect numerical instability.
    """
    if 'dAlpha_dt' not in df.columns or 'SlopeCurrent' not in df.columns:
        return ""
    
    # Downsample if too large for performance
    if len(df) > 20000:
        plot_df = df.sample(n=20000, random_state=42)
    else:
        plot_df = df
        
    fig, ax = plt.subplots(figsize=(10, 8))
    
    ax.scatter(plot_df['dAlpha_dt'], plot_df['SlopeCurrent'], 
               alpha=0.1, s=10, color='#9C27B0')
    
    # Annotate thresholds
    ax.axvline(0.02, color='#F44336', linestyle='--', alpha=0.5, label='Threshold (0.02)')
    ax.axvline(-0.02, color='#F44336', linestyle='--', alpha=0.5)
    
    ax.set_xlabel('dAlpha/dt (rad/s)')
    ax.set_ylabel('Slope (G/rad)')
    ax.set_title('Instability Check: dAlpha/dt vs SlopeCurrent')
    ax.set_ylim(-50, 50)  # Focus on the relevant range, even if outliers exist
    ax.grid(True, alpha=0.3)
    ax.legend()
    
    plt.tight_layout()
    
    if output_path:
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        plt.close()
        return output_path
    
    if show:
        plt.show()
    
    return ""
