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

# ---- JDK 8 rt.jar location (required for FlintMIDP Java 8 compilation) ----
JDK8_RT="${JDK8_RT:-}"
if [ -z "$JDK8_RT" ]; then
  for candidate in \
    "C:/Program Files/Eclipse Adoptium/jdk-8.0.492.9-hotspot/jre/lib/rt.jar" \
    "C:/Program Files/Java/jdk1.8.0_202/jre/lib/rt.jar" \
    "/usr/lib/jvm/java-8-openjdk/jre/lib/rt.jar"; do
    if [ -f "$candidate" ]; then
      JDK8_RT="$candidate"
      break
    fi
  done
fi

# ---- Prerequisite checks ----
for tool in javac jar; do
  command -v "$tool" >/dev/null 2>&1 || { echo "$tool was not found in PATH. Install JDK 17 or newer." >&2; exit 1; }
done

for dir in "$JDK_ROOT/src" "$UI_ROOT/src" "$MIDP_ROOT/src" "$ROOT/java/src"; do
  [ -d "$dir" ] || { echo "Missing source directory: $dir. Run git submodule update --init --recursive." >&2; exit 1; }
done

if [ -z "$JDK8_RT" ] || [ ! -f "$JDK8_RT" ]; then
  echo "JDK 8 rt.jar not found. Set JDK8_RT environment variable to the path of rt.jar." >&2
  echo "Example: export JDK8_RT=\"C:/Program Files/Eclipse Adoptium/jdk-8.0.492.9-hotspot/jre/lib/rt.jar\"" >&2
  exit 1
fi

mkdir -p -- "$LIBRARY_ROOT"

# ---- FlintJDK ----
echo 'Building FlintJDK (java.base, flint.io, flint.net, flint.drawing)...'
JDK_BUILD_DIR="$JDK_ROOT/bin/run"
rm -rf -- "$JDK_BUILD_DIR"
for mod in java.base flint.io flint.net flint.drawing; do
  echo "  Compiling module $mod..."
  javac -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8 \
    -d "$JDK_BUILD_DIR" --module "$mod" --module-source-path "$JDK_ROOT/src"
  jar --create --file "$JDK_BUILD_DIR/$mod.jar" \
    --manifest "$JDK_ROOT/META-INF/MANIFEST.MF" -0 -C "$JDK_BUILD_DIR/$mod" .
done
cp -f -- "$JDK_BUILD_DIR"/*.jar "$LIBRARY_ROOT/"
echo 'FlintJDK build complete.'

# ---- FlintUI ----
echo 'Building FlintUI from its dedicated submodule...'
[ -f "$UI_ROOT/src/flint.ui/module-info.java" ] || { echo 'Missing FlintUI submodule. Run git submodule update --init --recursive.' >&2; exit 1; }
rm -rf -- "$UI_OUTPUT"
mkdir -p -- "$UI_OUTPUT"
javac -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8 -d "$UI_OUTPUT" \
  --module-path "$LIBRARY_ROOT" --module-source-path "$UI_ROOT/src" --module flint.ui
jar cf0m "$LIBRARY_ROOT/flint.ui.jar" "$ROOT/java/META-INF/flint.ui.MF" \
  -C "$UI_OUTPUT/flint.ui" .
echo 'FlintUI build complete.'

# ---- FlintOS device API ----
echo 'Building FlintOS device API...'
rm -rf -- "$DEVICE_OUTPUT"
mkdir -p -- "$DEVICE_OUTPUT"
javac -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8 -d "$DEVICE_OUTPUT" \
  --module-path "$LIBRARY_ROOT" --module-source-path "$ROOT/java/src" --module flintos.device
jar cf0m "$LIBRARY_ROOT/flintos.device.jar" "$ROOT/java/META-INF/MANIFEST.MF" \
  -C "$DEVICE_OUTPUT/flintos.device" .
echo 'FlintOS device API build complete.'

# ---- FlintMIDP ----
echo 'Building FlintMIDP...'
MIDP_SOURCES=$(find "$MIDP_ROOT/src" -name '*.java' | sort)
MIDP_TOOLS_DIR="$MIDP_ROOT/bin/tools"
MIDP_RUN_DIR="$MIDP_ROOT/bin/run"

rm -rf -- "$MIDP_RUN_DIR" "$MIDP_TOOLS_DIR"
mkdir -p -- "$MIDP_RUN_DIR/midp" "$MIDP_TOOLS_DIR"

# Build classpath from already-built JARs in files/lib
MIDP_CP="$LIBRARY_ROOT/flint.drawing.jar:$LIBRARY_ROOT/flintos.device.jar"
# midp.jar may not exist on first build; include it if present for incremental builds
if [ -f "$LIBRARY_ROOT/midp.jar" ]; then
  MIDP_CP="$MIDP_CP:$LIBRARY_ROOT/midp.jar"
fi

echo "  Compiling MIDP sources (Java 8 target)..."
javac -source 8 -target 8 -XDstringConcat=inline \
  -bootclasspath "$JDK8_RT" \
  -Xlint:all,-serial,-options -encoding UTF-8 \
  -classpath "$MIDP_CP" \
  -d "$MIDP_RUN_DIR/midp" $MIDP_SOURCES

echo "  Packaging MIDP JARs..."
javac --release 17 -encoding UTF-8 -d "$MIDP_TOOLS_DIR" "$MIDP_ROOT/tools/MidpJarPackager.java"
java -cp "$MIDP_TOOLS_DIR" MidpJarPackager \
  "$MIDP_RUN_DIR/midp" "$MIDP_ROOT/src" \
  "$MIDP_ROOT/META-INF/MANIFEST.MF" \
  "$MIDP_ROOT/config/jsr118-api-classes.txt" \
  "$MIDP_RUN_DIR" run

cp -f -- "$MIDP_RUN_DIR/midp.jar" "$LIBRARY_ROOT/midp.jar"
cp -f -- "$MIDP_RUN_DIR/flintos.midp.jar" "$LIBRARY_ROOT/flintos.midp.jar"
cp -f -- "$MIDP_RUN_DIR/m3g.jar" "$LIBRARY_ROOT/m3g.jar"
echo 'FlintMIDP build complete.'

# ---- Validate ----
java "$SCRIPT_DIR/ValidateJarManifests.java" "$LIBRARY_ROOT"

echo "SDK libraries are ready in: $LIBRARY_ROOT"
