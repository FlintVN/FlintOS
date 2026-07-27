
MODULES         	:=  flintos.device
JC                  :=  javac
MODULE_SOURCE_PATH  :=  src
OUT_DIR          	?=  bin

RUN_OPT             :=
DEV_OPT             :=  -g
JFLAGS              :=  -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8

RUN_DIR				:=	$(OUT_DIR)/run
DEV_DIR				:= 	$(OUT_DIR)/dev

all: run dev

run: $(foreach m,$(MODULES),$(RUN_DIR)/$(m))

dev: $(foreach m,$(MODULES),$(DEV_DIR)/$(m))

$(RUN_DIR)/%:
	@echo Compiling $* for runtime mode
	@$(JC) $(RUN_OPT) $(JFLAGS) -d $(RUN_DIR) --module $* --module-source-path $(MODULE_SOURCE_PATH)
	@jar --create --file $(RUN_DIR)/$*.jar --manifest META-INF/MANIFEST.MF -0 -C $(RUN_DIR)/$* "."

$(DEV_DIR)/%:
	@echo Compiling $* for development mode
	@$(JC) $(DEV_OPT) $(JFLAGS) -d $(DEV_DIR) --module $* --module-source-path $(MODULE_SOURCE_PATH)
	@cp -r $(MODULE_SOURCE_PATH)/$* $(DEV_DIR)/$*/src
	@jar --create --file $(DEV_DIR)/$*.jar --manifest META-INF/MANIFEST.MF -C $(DEV_DIR)/$* "."

clean:
	@rm -rf $(OUT_DIR)
	@echo Clean complete
