import json
import shutil
import subprocess
from pathlib import Path


def main() -> int:
    root = Path(__file__).resolve().parents[1]
    java_out = root / "build" / "java-runtime"
    staging = root / "build" / "storage-root"
    jar_path = staging / "FlintApp.jar"
    manifest = json.loads((root / "java" / "system_apps" / "launcher" / "app.json").read_text(encoding="utf-8"))

    subprocess.run([
        "python",
        str(root / "tools" / "build_java_modules.py"),
        "--clean",
        "--output",
        str(java_out),
    ], check=True)

    if staging.exists():
        shutil.rmtree(staging)
    (staging / "lib").mkdir(parents=True, exist_ok=True)

    subprocess.run([
        "jar",
        "--create",
        "--file",
        str(jar_path),
        "--main-class",
        manifest["mainClass"],
        "-C",
        str(java_out / "flintos.launcher"),
        ".",
        "-C",
        str(java_out / "flint.app"),
        ".",
        "-C",
        str(java_out / "flint.os"),
        ".",
        "-C",
        str(java_out / "flint.ui"),
        ".",
    ], check=True)

    print(f"OK {jar_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
