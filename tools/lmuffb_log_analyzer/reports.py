import pandas as pd
from .models import SessionMetadata
from .analyzers.slope_analyzer import (
    analyze_slope_stability, 
    detect_oscillation_events,
    detect_singularities
)

def generate_text_report(metadata: SessionMetadata, df: pd.DataFrame) -> str:
    """
    Generate a formatted text report for the session.
    """
    slope_results = analyze_slope_stability(df)
    oscillations = detect_oscillation_events(df)
    singularity_count, worst_slope = detect_singularities(df)
    
    report = []
    report.append("=" * 60)
    report.append(" " * 15 + "LMUFFB DIAGNOSTIC REPORT")
    report.append("=" * 60)
    report.append("")
    
    report.append("SESSION INFORMATION")
    report.append("-" * 20)
    report.append(f"Driver:       {metadata.driver_name}")
    report.append(f"Vehicle:      {metadata.vehicle_name}")
    report.append(f"Track:        {metadata.track_name}")
    report.append(f"Date:         {metadata.timestamp}")
    report.append(f"App Version:  {metadata.app_version}")
    report.append("")
    
    report.append("SETTINGS")
    report.append("-" * 20)
    report.append(f"Gain:               {metadata.gain:.2f}")
    report.append(f"Understeer Effect:  {metadata.understeer_effect:.2f}")
    report.append(f"SOP Effect:          {metadata.sop_effect:.2f}")
    report.append(f"Slope Detection:    {'Enabled' if metadata.slope_enabled else 'Disabled'}")
    report.append(f"Slope Sensitivity:  {metadata.slope_sensitivity:.2f}")
    report.append(f"Slope Threshold:    {metadata.slope_threshold:.2f}")
    report.append("")
    
    report.append("SLOPE ANALYSIS")
    report.append("-" * 20)
    report.append(f"Slope Mean:       {slope_results['slope_mean']:.2f}")
    report.append(f"Slope Std Dev:    {slope_results['slope_std']:.2f}")
    report.append(f"Slope Range:      {slope_results['slope_min']:.1f} to {slope_results['slope_max']:.1f}")
    
    if slope_results.get('active_percentage') is not None:
        report.append(f"Active Time:      {slope_results['active_percentage']:.1f}%")
        
    if slope_results.get('floor_percentage') is not None:
        report.append(f"Floor Hits:       {slope_results['floor_percentage']:.1f}%")
        
    report.append(f"Oscillations:      {len(oscillations)} events detected")
    report.append(f"Singularities:     {singularity_count} events detected (Worst: {worst_slope:.1f})")
    report.append("")

    report.append("SIGNAL QUALITY & STABILITY")
    report.append("-" * 20)
    if slope_results.get('zero_crossing_rate') is not None:
        report.append(f"Zero-Crossing Rate: {slope_results['zero_crossing_rate']:.2f} Hz")
    if slope_results.get('binary_residence') is not None:
        report.append(f"Binary Residence:   {slope_results['binary_residence']:.1f}%")
    if slope_results.get('derivative_energy_ratio') is not None:
        report.append(f"D-Energy Ratio:     {slope_results['derivative_energy_ratio']:.2f}")
    report.append("")
    
    if slope_results['issues']:
        report.append("ISSUES DETECTED")
        report.append("-" * 20)
        for issue in slope_results['issues']:
            report.append(f"  [!] {issue}")
        report.append("")
    else:
        report.append("No significant issues detected in slope analysis.")
        report.append("")
        
    report.append("=" * 60)
    
    return "\n".join(report)
