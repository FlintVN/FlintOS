import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate FlintOS HIL matrix without flashing hardware.")
    parser.add_argument("matrix", type=Path)
    args = parser.parse_args()

    matrix = json.loads(args.matrix.read_text(encoding="utf-8"))
    tests = matrix.get("include", [])
    if not tests:
        print("FAIL HIL matrix has no tests")
        return 1

    failed = False
    for test in tests:
        name = test.get("name")
        pass_pattern = test.get("pass_pattern")
        if not name or not pass_pattern:
            print(f"FAIL invalid HIL entry: {test}")
            failed = True
            continue
        sample_log = f"FLINTOS_TEST_START {name}\n{pass_pattern}\nFLINTOS_TEST_END {name}\n"
        subprocess.run([
            "python",
            str(ROOT / "tests" / "scripts" / "serial_monitor.py"),
            "--test-name",
            name,
        ], input=sample_log, text=True, check=True)

    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
