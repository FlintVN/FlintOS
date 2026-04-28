import argparse
import json
import shutil
import subprocess
from pathlib import Path

SYSTEM_APPS = {
    "launcher": "flintos.launcher",
    "settings": "flintos.settings",
    "diagnostics": "flintos.diagnostics",
    "updater": "flintos.updater",
    "app_store": "flintos.appstore",
    "file_manager": "flintos.filemanager",
    "music": "flintos.music",
}
SHARED_MODULES = ["flint.app", "flint.os", "flint.ui", "flint.io", "flint.net"]
JDK_JARS = ["java.base.jar", "flint.io.jar", "flint.net.jar"]


def add_module_args(command: list[str], java_out: Path, module_name: str) -> None:
    module_dir = java_out / module_name
    if module_dir.exists():
        command.extend(["-C", str(module_dir), "."])


def create_app_jar(java_out: Path, staging: Path, app_name: str, module_name: str) -> None:
    app_dir = Path("java") / "system_apps" / app_name
    manifest = json.loads((app_dir / "app.json").read_text(encoding="utf-8"))
    jar_path = staging / "system" / "apps" / manifest["id"] / "FlintApp.jar"
    jar_path.parent.mkdir(parents=True, exist_ok=True)

    command = [
        "jar",
        "--create",
        "--file",
        str(jar_path),
        "--main-class",
        manifest["mainClass"],
        "-0",
    ]
    add_module_args(command, java_out, module_name)
    for shared_module in SHARED_MODULES:
        add_module_args(command, java_out, shared_module)
    subprocess.run(command, check=True)
    shutil.copy2(app_dir / "app.json", jar_path.parent / "app.json")


def main() -> int:
    parser = argparse.ArgumentParser(description="Build FlintOS Java runtime storage tree.")
    parser.add_argument("--output", type=Path, default=None, help="Output staging directory. Defaults to build/storage-root.")
    args = parser.parse_args()

    root = Path(__file__).resolve().parents[1]
    java_out = root / "build" / "java-runtime"
    staging = args.output if args.output is not None else root / "build" / "storage-root"

    subprocess.run([
        "python",
        str(root / "tools" / "build_java_modules.py"),
        "--clean",
        "--output",
        str(java_out),
    ], check=True)

    if staging.exists():
        shutil.rmtree(staging)
    (staging / "system" / "apps").mkdir(parents=True, exist_ok=True)
    lib_dir = staging / "lib"
    lib_dir.mkdir(parents=True, exist_ok=True)
    for jar_name in JDK_JARS:
        shutil.copy2(root.parent / "FlintESPJVM" / "BUILD_SYSTEM" / "Resources" / "lib" / jar_name, lib_dir / jar_name)

    for app_name, module_name in SYSTEM_APPS.items():
        create_app_jar(java_out, staging, app_name, module_name)

    launcher_jar = staging / "system" / "apps" / "flintos.launcher" / "FlintApp.jar"
    shutil.copy2(launcher_jar, staging / "FlintApp.jar")
    print(f"OK {staging}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
