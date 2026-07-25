
# FlintOS Java modules — flintos.device + flintos.midp
#
# flintos.device: JPMS module
# flintos.midp:   classpath compilation
#   Compile order:
#     1. RawImage (flint.drawing dependency)
#     2. flintos.midp bridge (DisplayBridge, TouchBridge, PngDecoder, ResourceLoader)
#     3. MIDletLifecycle (before MIDlet, no MIDlet dependency)
#     4. ALL javax.microedition.* together (flintj2me standard + flintos.midp FlintOS-specific)

JC              := javac
JAR             := jar
SRC             := src
OUT_DIR         := bin
LIB             := ../files/lib
FLINTJ2ME       := ../FlintJ2ME

DEPS_DIR        := $(OUT_DIR)/deps

.PHONY: all device midp clean

all: device midp

device:
	@echo "=== Compiling flintos.device ==="
	@mkdir -p $(OUT_DIR)/device
	@$(JC) -Xlint:all -encoding UTF-8 -nowarn -d $(OUT_DIR)/device \
		--module-source-path $(SRC) --module flintos.device
	@$(JAR) --create --file $(OUT_DIR)/flintos.device.jar -C $(OUT_DIR)/device/flintos.device .

midp: $(DEPS_DIR).timestamp
	@echo "=== Compiling flintos.midp ==="
	@mkdir -p $(OUT_DIR)/midp

	@echo "  [1/4] flint.drawing.RawImage..."
	@$(JC) -Xlint:all -encoding UTF-8 -nowarn -source 8 -target 8 \
		-cp "$(DEPS_DIR)" -d $(OUT_DIR)/midp \
		$(SRC)/flintos.midp/flint/drawing/RawImage.java

	@echo "  [2/4] flintos.midp bridge..."
	@$(JC) -Xlint:all -encoding UTF-8 -nowarn -source 8 -target 8 \
		-cp "$(DEPS_DIR);$(OUT_DIR)/midp" -d $(OUT_DIR)/midp \
		$(SRC)/flintos.midp/flintos/midp/DisplayBridge.java \
		$(SRC)/flintos.midp/flintos/midp/TouchBridge.java \
		$(SRC)/flintos.midp/flintos/midp/PngDecoder.java \
		$(SRC)/flintos.midp/flintos/midp/ResourceLoader.java

	@echo "  [3/4] All javax.* + MIDletLifecycle together..."
	@$(JC) -Xlint:all -encoding UTF-8 -nowarn -source 8 -target 8 \
		-cp "$(DEPS_DIR);$(OUT_DIR)/midp" -d $(OUT_DIR)/midp \
		$(shell find $(FLINTJ2ME)/src/javax $(SRC)/flintos.midp/javax $(SRC)/flintos.midp/flintos/midp/MIDletLifecycle.java -name "*.java" 2>/dev/null || \
		find $(FLINTJ2ME)/src/javax $(SRC)/flintos.midp/javax -name "*.java") \
		$(SRC)/flintos.midp/flintos/midp/MIDletLifecycle.java

	@echo "  [4/4] Packaging..."
	@$(JAR) --create --file $(OUT_DIR)/flintos.midp.jar -C $(OUT_DIR)/midp .

$(DEPS_DIR).timestamp: $(LIB)/flintos.device.jar $(LIB)/flint.drawing.jar $(LIB)/java.base.jar
	@mkdir -p $(DEPS_DIR)
	@cd $(DEPS_DIR) && jar xf "$(abspath $(LIB))/flintos.device.jar"
	@cd $(DEPS_DIR) && jar xf "$(abspath $(LIB))/flint.drawing.jar"
	@cd $(DEPS_DIR) && jar xf "$(abspath $(LIB))/java.base.jar"
	@cd $(DEPS_DIR) && rm -rf META-INF module-info.class
	@touch $@

clean:
	@rm -rf $(OUT_DIR)
	@echo "Clean complete"
