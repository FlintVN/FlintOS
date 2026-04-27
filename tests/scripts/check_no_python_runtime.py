from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ALLOWED_PREFIXES = {
    "tools",
    "tests",
}
RUNTIME_PREFIXES = {
    "main",
    "native",
    "java",
}


def is_allowed(path: Path) -> bool:
    rel = path.relative_to(ROOT)
    first = rel.parts[0] if rel.parts else ""
    if first in ALLOWED_PREFIXES:
        return True
    if first not in RUNTIME_PREFIXES:
        return True
    return False


def main() -> int:
    violations = []
    for path in ROOT.rglob("*.py"):
        if ".git" in path.parts or "build" in path.parts:
            continue
        if not is_allowed(path):
            violations.append(path.relative_to(ROOT))

    for violation in violations:
        print(f"FORBIDDEN_RUNTIME_PYTHON {violation}")

    if violations:
        return 1

    print("OK no Python runtime files in active runtime tree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
