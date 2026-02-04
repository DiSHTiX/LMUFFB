import pytest
import pandas as pd
from pathlib import Path
from lmuffb_log_analyzer.loader import load_log
from lmuffb_log_analyzer.models import SessionMetadata

def test_load_log():
    sample_log = Path(__file__).parent / "sample_log.csv"
    metadata, df = load_log(str(sample_log))
    
    # Check metadata
    assert isinstance(metadata, SessionMetadata)
    assert metadata.driver_name == "Test Driver"
    assert metadata.vehicle_name == "Test Car"
    assert metadata.track_name == "Test Track"
    assert metadata.gain == 0.8
    assert metadata.slope_enabled is True
    
    # Check dataframe
    assert isinstance(df, pd.DataFrame)
    assert len(df) == 4
    assert "LatAccel" in df.columns
    assert "SlopeCurrent" in df.columns
    assert df.iloc[3]["Marker"] == 1
