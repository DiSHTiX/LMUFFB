import subprocess
import re
import sys
import os

def run_command(command, cwd=None, env=None):
    print(f"Running: {' '.join(command)}")
    result = subprocess.run(command, capture_output=True, text=True, cwd=cwd, env=env)
    return result

def main():
    root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    
    # 1. Run C++ tests
    cpp_exe = os.path.join(root, "build", "tests", "Release", "run_combined_tests.exe")
    cpp_result = run_command([cpp_exe], cwd=root)
    print(cpp_result.stdout)
    if cpp_result.stderr:
        print(cpp_result.stderr, file=sys.stderr)
    
    # 2. Run Python tests
    env = os.environ.copy()
    env["PYTHONPATH"] = os.path.join(root, "tools")
    py_command = [sys.executable, "-m", "pytest", "tests", "tools/lmuffb_log_analyzer/tests"]
    py_result = run_command(py_command, cwd=root, env=env)
    print(py_result.stdout)
    if py_result.stderr:
        print(py_result.stderr, file=sys.stderr)
        
    # --- Summary ---
    print("\n" + "="*46)
    print("           GLOBAL TEST SUMMARY              ")
    print("="*46)
    
    # Parse CPP summary
    # Sample line: "  TEST CASES   : 212/212"
    # Sample line: "  ASSERTIONS   : 968 passed, 0 failed"
    cpp_tc = re.search(r"TEST CASES\s+:\s+(\d+/\d+)", cpp_result.stdout)
    cpp_ass = re.search(r"ASSERTIONS\s+:\s+(\d+) passed,\s+(\d+) failed", cpp_result.stdout)
    
    print(" C++ Tests:")
    if cpp_tc:
        print(f"   Test Cases : {cpp_tc.group(1)}")
    if cpp_ass:
        print(f"   Assertions : {cpp_ass.group(1)} passed, {cpp_ass.group(2)} failed")
    else:
        print("   [!] Could not parse C++ summary.")
        
    # Parse Pytest summary
    # Sample line: "============================== 18 passed in 4.86s ==============================="
    # Sample line: "======================== 1 failed, 17 passed in 5.12s ==========================="
    # Sample line: "======================= 18 passed, 2 warnings in 5.00s ========================"
    py_summary_line = None
    # We look for the status line that looks like multiple equals or just contains the count
    for line in reversed(py_result.stdout.splitlines()):
        if ("passed" in line or "failed" in line) and " in " in line and line.strip().startswith("="):
            py_summary_line = line
            break
            
    print("\n Python Tests:")
    if py_summary_line:
        # Clean up the line (remove =====)
        clean_line = py_summary_line.replace("=", "").strip()
        print(f"   Summary    : {clean_line}")
    else:
        # Fallback: check if we have any line with "passed" and "in"
        for line in reversed(py_result.stdout.splitlines()):
            if "passed" in line and "in " in line:
                print(f"   Summary    : {line.strip()}")
                py_summary_line = line
                break
        if not py_summary_line:
            print("   [!] Could not parse Python summary.")
        
    print("="*46)
    
    if cpp_result.returncode != 0 or py_result.returncode != 0:
        sys.exit(1)

if __name__ == "__main__":
    main()
