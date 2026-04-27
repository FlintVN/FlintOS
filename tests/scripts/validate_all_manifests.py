from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

from validate_manifest import validate_manifest


def main() -> int:
    manifests = sorted((ROOT / "java" / "system_apps").glob("*/app.json"))
    if not manifests:
        print("FAIL no app manifests found")
        return 1

    failed = False
    for manifest in manifests:
        try:
            validate_manifest(manifest)
            print(f"OK {manifest.relative_to(ROOT)}")
        except Exception as error:
            failed = True
            print(f"FAIL {manifest.relative_to(ROOT)}: {error}")

    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
