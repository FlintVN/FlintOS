import argparse
import shutil
import zipfile
from pathlib import Path

from validate_manifest import validate_manifest


def add_tree(package: zipfile.ZipFile, source: Path, prefix: str) -> None:
    if not source.exists():
        return

    for path in source.rglob("*"):
        if path.is_file():
            package.write(path, Path(prefix) / path.relative_to(source))


def package_app(app_dir: Path, output: Path, compiled_modules: Path | None) -> None:
    manifest = app_dir / "app.json"
    validate_manifest(manifest)

    output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(output, "w", compression=zipfile.ZIP_DEFLATED) as package:
        package.write(manifest, "app.json")
        add_tree(package, app_dir / "classes", "classes")
        add_tree(package, app_dir / "modules", "modules")
        add_tree(package, app_dir / "assets", "assets")
        if compiled_modules is not None:
            add_tree(package, compiled_modules, "modules")


def main() -> int:
    parser = argparse.ArgumentParser(description="Package a FlintOS Java app as .flintapp.")
    parser.add_argument("app_dir", type=Path)
    parser.add_argument("-o", "--output", type=Path)
    parser.add_argument("--compiled-modules", type=Path, default=Path("build/java-check"))
    args = parser.parse_args()

    output = args.output
    if output is None:
        output = Path("build/packages") / f"{args.app_dir.name}.flintapp"

    compiled_modules = args.compiled_modules if args.compiled_modules.exists() else None
    package_app(args.app_dir, output, compiled_modules)
    print(f"OK {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
