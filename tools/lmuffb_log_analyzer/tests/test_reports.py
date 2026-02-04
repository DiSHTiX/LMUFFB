from lmuffb_log_analyzer.models import SessionMetadata
from lmuffb_log_analyzer.reports import generate_text_report
import pandas as pd
import numpy as np

def test_generate_text_report():
    metadata = SessionMetadata(
        log_version="1.0.0",
        timestamp="2026-02-04 22:00:00",
        app_version="v0.7.9",
        driver_name="Test Driver",
        vehicle_name="Test Car",
        track_name="Test Track",
        gain=0.8,
        understeer_effect=0.5,
        sop_effect=0.4,
        slope_enabled=True,
        slope_sensitivity=0.6,
        slope_threshold=-0.3
    )
    
    data = {
        'Time': np.arange(0, 1, 0.01),
        'LatAccel': np.zeros(100),
        'Speed': np.zeros(100),
        'SlopeCurrent': np.zeros(100),
        'SlopeSmoothed': np.ones(100),
        'GripFactor': np.ones(100),
        'Marker': np.zeros(100)
    }
    df = pd.DataFrame(data)
    
    report = generate_text_report(metadata, df)
    
    assert "LMUFFB DIAGNOSTIC REPORT" in report
    assert "Test Driver" in report
    assert "Test Car" in report
    assert "v0.7.9" in report
