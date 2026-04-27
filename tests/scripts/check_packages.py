import subprocess
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SYSTEM_APPS = sorted((ROOT / "java" / "system_apps").glob("*/app.json"))


def main() -> int:
    if not SYSTEM_APPS:
        print("FAIL no system app manifests found")
        return 1

    subprocess.run(["python", str(ROOT / "tools" / "build_java_modules.py"), "--clean"], check=True)
    failed = False
    for manifest in SYSTEM_APPS:
        app_dir = manifest.parent
        output = ROOT / "build" / "packages" / f"{app_dir.name}.flintapp"
        subprocess.run([
            "python",
            str(ROOT / "tools" / "package_app.py"),
            str(app_dir),
            "--output",
            str(output),
        ], check=True)
        with zipfile.ZipFile(output) as package:
            names = set(package.namelist())
            if "app.json" not in names:
                print(f"FAIL {output}: missing app.json")
                failed = True
                continue
            if not any(name.startswith("modules/") for name in names):
                print(f"FAIL {output}: missing compiled modules")
                failed = True
                continue
            print(f"OK {output.relative_to(ROOT)}")

    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
