
MODULES             :=  flintos midp
JC                  :=  javac
JDK_MODULE_SRC_PATH := ../../flintjdk/src
MODULE_SRC_PATH     :=  src
OUT_DIR             ?=  bin

RUN_OPT             :=
DEV_OPT             :=  -g
JFLAGS              :=  -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8

RUN_OUT_DIR         :=  $(OUT_DIR)/run
DEV_OUT_DIR         :=  $(OUT_DIR)/dev

RUN_JAR_OUT_DIR     ?=  $(RUN_OUT_DIR)
DEV_JAR_OUT_DIR     ?=  $(DEV_OUT_DIR)

all: run dev

run: $(foreach m,$(MODULES),$(RUN_OUT_DIR)/$(m))

dev: $(foreach m,$(MODULES),$(DEV_OUT_DIR)/$(m))

$(RUN_OUT_DIR)/%: $(shell find $(MODULE_SRC_PATH)/$* -name *.java)
	@echo Compiling $* for runtime mode
	@$(JC) $(RUN_OPT) $(JFLAGS) -d $(RUN_OUT_DIR) --module $* --module-source-path "$(MODULE_SRC_PATH);$(JDK_MODULE_SRC_PATH)"
	@jar --create --file $(RUN_JAR_OUT_DIR)/$*.jar --manifest META-INF/MANIFEST.MF -0 -C $(RUN_OUT_DIR)/$* "."

$(DEV_OUT_DIR)/%: $(shell find $(MODULE_SRC_PATH)/$* -name *.java)
	@echo Compiling $* for development mode
	@$(JC) $(DEV_OPT) $(JFLAGS) -d $(DEV_OUT_DIR) --module $* --module-source-path "$(MODULE_SRC_PATH);$(JDK_MODULE_SRC_PATH)"
	@cp -r $(MODULE_SRC_PATH)/$* $(DEV_OUT_DIR)/$*/src
	@jar --create --file $(DEV_JAR_OUT_DIR)/$*.jar --manifest META-INF/MANIFEST.MF -C $(DEV_OUT_DIR)/$* "."

clean:
	@rm -rf $(OUT_DIR)
	@echo Clean complete
