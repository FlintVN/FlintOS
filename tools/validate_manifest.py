import argparse
import json
import re
from pathlib import Path

REQUIRED_FIELDS = {
    "id",
    "name",
    "version",
    "type",
    "runtime",
    "mainClass",
    "permissions",
    "minOs",
    "minJdk",
}

APP_ID_PATTERN = re.compile(r"^[a-z][a-z0-9]*(\.[a-z][a-z0-9]*)+$")
MAIN_CLASS_PATTERN = re.compile(r"^[A-Za-z_$][A-Za-z0-9_$]*(\.[A-Za-z_$][A-Za-z0-9_$]*)+$")
VALID_TYPES = {"system", "user"}


def validate_manifest(path: Path) -> None:
    manifest = json.loads(path.read_text(encoding="utf-8"))

    missing = REQUIRED_FIELDS - manifest.keys()
    if missing:
        raise ValueError(f"missing required fields: {sorted(missing)}")

    if manifest["runtime"] != "flintjvm":
        raise ValueError("runtime must be flintjvm")

    if manifest["type"] not in VALID_TYPES:
        raise ValueError(f"type must be one of {sorted(VALID_TYPES)}")

    if not APP_ID_PATTERN.fullmatch(manifest["id"]):
        raise ValueError("id must be a dotted lowercase app id")

    if not MAIN_CLASS_PATTERN.fullmatch(manifest["mainClass"]):
        raise ValueError("mainClass must be a fully qualified Java class name")

    if not isinstance(manifest["permissions"], list):
        raise ValueError("permissions must be a list")

    for permission in manifest["permissions"]:
        if not isinstance(permission, str) or not permission:
            raise ValueError("permissions must contain non-empty strings")


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate FlintOS app manifests.")
    parser.add_argument("manifests", nargs="+", type=Path)
    args = parser.parse_args()

    failed = False
    for manifest_path in args.manifests:
        try:
            validate_manifest(manifest_path)
            print(f"OK {manifest_path}")
        except (OSError, json.JSONDecodeError, ValueError) as error:
            failed = True
            print(f"FAIL {manifest_path}: {error}")

    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
