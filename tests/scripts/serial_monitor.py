import argparse
import sys
from pathlib import Path

START = "FLINTOS_TEST_START"
PASS = "FLINTOS_TEST_PASS"
FAIL = "FLINTOS_TEST_FAIL"
END = "FLINTOS_TEST_END"


def scan_lines(lines: list[str], test_name: str) -> int:
    saw_start = False
    saw_pass = False
    saw_end = False
    for line in lines:
        if f"{START} {test_name}" in line:
            saw_start = True
        if f"{FAIL} {test_name}" in line:
            print(line.strip())
            return 1
        if f"{PASS} {test_name}" in line:
            saw_pass = True
        if f"{END} {test_name}" in line:
            saw_end = True

    if saw_start and saw_pass and saw_end:
        print(f"OK {test_name}")
        return 0

    print(f"FAIL {test_name}: missing expected serial markers")
    return 1


def main() -> int:
    parser = argparse.ArgumentParser(description="Scan FlintOS HIL serial log markers.")
    parser.add_argument("--test-name", default="boot_smoke")
    parser.add_argument("--log", type=Path)
    args = parser.parse_args()

    if args.log is None:
        lines = sys.stdin.readlines()
    else:
        lines = args.log.read_text(encoding="utf-8").splitlines()
    return scan_lines(lines, args.test_name)


if __name__ == "__main__":
    raise SystemExit(main())
