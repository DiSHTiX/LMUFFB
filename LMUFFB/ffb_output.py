class MockFFBOutput:
    def __init__(self):
        print("Initializing Mock FFB Output (DirectInput placeholder)")

    def send_force(self, force):
        """
        Sends the calculated force to the virtual wheel.
        Force is a float between -1.0 and 1.0
        """
        # In a real app, this would use pyvjoy or ctypes to call DirectInput
        # For this prototype, we just verify the value is reasonable.
        # print(f"FFB Output: {force:.4f}")
        pass

    def cleanup(self):
        print("Cleaning up FFB Output")
