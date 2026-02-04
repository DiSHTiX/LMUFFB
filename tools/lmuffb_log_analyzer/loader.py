import pandas as pd
from pathlib import Path
from typing import Tuple, Optional
from datetime import datetime
from .models import SessionMetadata

def load_log(filepath: str) -> Tuple[SessionMetadata, pd.DataFrame]:
    """
    Load lmuFFB telemetry log file.
    
    Returns:
        Tuple of (SessionMetadata, DataFrame with telemetry data)
    """
    path = Path(filepath)
    if not path.exists():
        raise FileNotFoundError(f"Log file not found: {filepath}")
    
    # Parse header comments
    metadata = _parse_header(path)
    
    # Find data start line (first non-comment line)
    data_start = 0
    with open(path, 'r') as f:
        for i, line in enumerate(f):
            if not line.startswith('#'):
                data_start = i
                break
    
    # Load CSV data
    df = pd.read_csv(filepath, skiprows=data_start)
    
    return metadata, df

def _parse_datetime(date_str: str) -> datetime:
    """Parse datetime from log header"""
    try:
        return datetime.strptime(date_str, "%Y-%m-%d %H:%M:%S")
    except ValueError:
        return datetime.now()

def _safe_float(val: Optional[str]) -> Optional[float]:
    """Safely convert string to float"""
    if val is None or val.lower() == 'none' or val == '':
        return None
    try:
        return float(val)
    except ValueError:
        return None

def _parse_header(path: Path) -> SessionMetadata:
    """Extract metadata from header comments"""
    header_data = {}
    
    with open(path, 'r') as f:
        for line in f:
            if not line.startswith('#'):
                break
            
            line = line.lstrip('# ').strip()
            if ':' in line:
                key, value = line.split(':', 1)
                header_data[key.strip().lower().replace(' ', '_')] = value.strip()
            elif 'LMUFFB Telemetry Log' in line:
                # Handle the first line differently if needed
                parts = line.split(':')
                if len(parts) > 1:
                    header_data['log_version'] = parts[1].strip()
                else:
                    # Alternative format "LMUFFB Telemetry Log: 1.0.0" (actually the plan says # LMUFFB Telemetry Log: 1.0.0)
                    pass

    return SessionMetadata(
        log_version=header_data.get('lmuffb_telemetry_log', 'unknown'),
        timestamp=_parse_datetime(header_data.get('date', '')),
        app_version=header_data.get('app_version', 'unknown'),
        driver_name=header_data.get('driver', 'Unknown'),
        vehicle_name=header_data.get('vehicle', 'Unknown'),
        track_name=header_data.get('track', 'Unknown'),
        gain=float(header_data.get('gain', 1.0)),
        understeer_effect=float(header_data.get('understeer_effect', 1.0)),
        sop_effect=float(header_data.get('sop_effect', 1.0)),
        slope_enabled=header_data.get('slope_detection', '').lower() == 'enabled',
        slope_sensitivity=float(header_data.get('slope_sensitivity', 0.5)),
        slope_threshold=float(header_data.get('slope_threshold', -0.3)),
        slope_alpha_threshold=_safe_float(header_data.get('slope_alpha_threshold')),
        slope_decay_rate=_safe_float(header_data.get('slope_decay_rate')),
    )
