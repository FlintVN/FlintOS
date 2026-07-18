#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd -P)"
JDK_ROOT="$ROOT/flintjdk"
UI_ROOT="$ROOT/flintui"
MIDP_ROOT="$ROOT/flintmidp"
LIBRARY_ROOT="$ROOT/files/lib"
UI_OUTPUT="$ROOT/build/sdk/flint.ui"
DEVICE_OUTPUT="$ROOT/build/sdk/flintos.device"

for tool in javac jar; do
  command -v "$tool" >/dev/null 2>&1 || { echo "$tool was not found in PATH. Install JDK 17 or newer." >&2; exit 1; }
done
for script in "$JDK_ROOT/scripts/build.sh" "$MIDP_ROOT/scripts/build.sh"; do
  [[ -f "$script" ]] || { echo "Missing submodule build script: $script. Run git submodule update --init --recursive." >&2; exit 1; }
done

mkdir -p -- "$LIBRARY_ROOT"

echo 'Delegating FlintJDK build to its submodule...'
bash "$JDK_ROOT/scripts/build.sh"
cp -f -- "$JDK_ROOT/bin/run/"*.jar "$LIBRARY_ROOT/"

echo 'Building FlintUI from its dedicated submodule...'
[[ -f "$UI_ROOT/src/flint.ui/module-info.java" ]] || { echo 'Missing FlintUI submodule. Run git submodule update --init --recursive.' >&2; exit 1; }
rm -rf -- "$UI_OUTPUT"
mkdir -p -- "$UI_OUTPUT"
javac -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8 -d "$UI_OUTPUT" \
  --module-path "$LIBRARY_ROOT" --module-source-path "$UI_ROOT/src" --module flint.ui
jar cf0m "$LIBRARY_ROOT/flint.ui.jar" "$JDK_ROOT/META-INF/MANIFEST.MF" \
  -C "$UI_OUTPUT/flint.ui" .

echo 'Building FlintOS device API...'
rm -rf -- "$DEVICE_OUTPUT"
mkdir -p -- "$DEVICE_OUTPUT"
javac -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8 -d "$DEVICE_OUTPUT" \
  --module-path "$LIBRARY_ROOT" --module-source-path "$ROOT/java/src" --module flintos.device
jar cf0m "$LIBRARY_ROOT/flintos.device.jar" "$ROOT/java/META-INF/MANIFEST.MF" \
  -C "$DEVICE_OUTPUT/flintos.device" .

echo 'Delegating FlintMIDP build to its submodule...'
bash "$MIDP_ROOT/scripts/build.sh" --library-root "$LIBRARY_ROOT"
cp -f -- "$MIDP_ROOT/bin/run/midp.jar" "$LIBRARY_ROOT/midp.jar"
cp -f -- "$MIDP_ROOT/bin/run/flintos.midp.jar" "$LIBRARY_ROOT/flintos.midp.jar"
cp -f -- "$MIDP_ROOT/bin/run/m3g.jar" "$LIBRARY_ROOT/m3g.jar"

echo "SDK libraries are ready in: $LIBRARY_ROOT"
