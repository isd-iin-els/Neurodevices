Import("env")

import sys 
import subprocess

print("\n========== ESPTOOL DIAGNOSTIC ==========")

print("Python:", sys.executable)

try:
    result = subprocess.run( [sys.executable, "-m", "esptool", "version"], capture_output=True, text=True )

    print("Esptool version:")
    print(result.stdout)
    print(result.stderr)


except Exception as e: 
    print("Error:", e)

print("========================================\n")