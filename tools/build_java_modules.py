import argparse
import shutil
import subprocess
from pathlib import Path

DEFAULT_MODULES = "flint.app,flint.io,flint.net,flint.os,flint.ui,flintos.launcher,flintos.settings,flintos.diagnostics,flintos.appstore,flintos.updater,flintos.filemanager,flintos.music"
DEFAULT_MODULE_SOURCE_PATH = "java/modules;java/system_apps/launcher/src;java/system_apps/settings/src;java/system_apps/diagnostics/src;java/system_apps/app_store/src;java/system_apps/updater/src;java/system_apps/file_manager/src;java/system_apps/music/src"


def main() -> int:
    parser = argparse.ArgumentParser(description="Compile FlintOS Java modules.")
    parser.add_argument("--output", type=Path, default=Path("build/java-check"))
    parser.add_argument("--modules", default=DEFAULT_MODULES)
    parser.add_argument("--module-source-path", default=DEFAULT_MODULE_SOURCE_PATH)
    parser.add_argument("--clean", action="store_true")
    args = parser.parse_args()

    if args.clean and args.output.exists():
        shutil.rmtree(args.output)

    args.output.mkdir(parents=True, exist_ok=True)
    command = [
        "javac",
        "--release",
        "9",
        "-d",
        str(args.output),
        "--module-source-path",
        args.module_source_path,
        "--module",
        args.modules,
    ]
    subprocess.run(command, check=True)
    print(f"OK {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
