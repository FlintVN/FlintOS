#!/bin/bash
# Build FlintOS Java modules and copy JARs to files/lib/
# Requires: JDK 17+ with module support

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
FLINTOS_ROOT="$(dirname "$SCRIPT_DIR")"
JAVA_DIR="$FLINTOS_ROOT/java"
FLINTJ2ME_DIR="$FLINTOS_ROOT/FlintJ2ME"
FILES_LIB="$FLINTOS_ROOT/files/lib"

echo "=== Step 1: Building flintos.device + flintos.midp ==="
cd "$JAVA_DIR"
make clean
make all

echo ""
echo "=== Step 2: Building M3G 3D (m3g.jar) ==="
cd "$FLINTJ2ME_DIR"
make clean
make m3g

echo ""
echo "=== Copying JARs to files/lib ==="
mkdir -p "$FILES_LIB"

cp "$JAVA_DIR/bin/flintos.device.jar" "$FILES_LIB/flintos.device.jar"
echo "  flintos.device.jar"

cp "$JAVA_DIR/bin/flintos.midp.jar" "$FILES_LIB/flintos.midp.jar"
echo "  flintos.midp.jar"

if [ -f "$FLINTJ2ME_DIR/build/m3g.jar" ]; then
    cp "$FLINTJ2ME_DIR/build/m3g.jar" "$FILES_LIB/m3g.jar"
    echo "  m3g.jar"
fi

echo ""
echo "=== Done ==="
echo "JARs in $FILES_LIB:"
ls -la "$FILES_LIB"
