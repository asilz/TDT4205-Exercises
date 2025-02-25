VSLC := ../build/vslc

PS2_EXAMPLES := $(patsubst %.vsl, %.ast, $(wildcard ps2-parser/*.vsl))
PS2_GRAPHVIZ := $(patsubst %.vsl, %.svg, $(wildcard ps2-parser/*.vsl))
PS3_EXAMPLES := $(patsubst %.vsl, %.ast, $(wildcard ps3-simplify/*.vsl))
PS3_GRAPHVIZ := $(patsubst %.vsl, %.svg, $(wildcard ps3-simplify/*.vsl))
PS4_EXAMPLES := $(patsubst %.vsl, %.symbols, $(wildcard ps4-symbols/*.vsl))

PRINT_AST_OPTION := -T

.PHONY: all ps2 ps2-graphviz ps3 ps3-graphviz ps4 clean

all: ps2 ps3 ps4

ps2: $(PS2_EXAMPLES)
ps2-graphviz: $(PS2_GRAPHVIZ)

# Make sure ps2 uses the -t flag instead of -T
ps2-parser/%.ast: PRINT_AST_OPTION := -t
ps2-parser/%.svg: PRINT_AST_OPTION := -t

ps3: $(PS3_EXAMPLES)
ps3-graphviz: $(PS3_GRAPHVIZ)

ps4: $(PS4_EXAMPLES)

$(VSLC):
	@echo "You need to build $(VSLC) before testing"
	@exit 1

%.ast: %.vsl $(VSLC)
	$(VSLC) $(PRINT_AST_OPTION) < $< > $@

%.svg: %.vsl $(VSLC)
	GRAPHVIZ_OUTPUT=1 $(VSLC) $(PRINT_AST_OPTION) < $< | dot -Tsvg -o $@

%.symbols: %.vsl $(VSLC)
	$(VSLC) -s < $< > $@

clean:
	-rm -rf */*.ast */*.svg

.PHONY: ps2-check ps3-check ps4-check

ps2-check: ps2
	cd ps2-parser; \
	find * -wholename "expected/*.ast" | awk -F/ '{print $$0 " " $$2}' | xargs -L 1 diff -s --unified=0
	@echo "No differences found in PS2!"

ps3-check: ps3
	cd ps3-simplify; \
	find * -wholename "expected/*.ast" | awk -F/ '{print $$0 " " $$2}' | xargs -L 1 diff -s --unified=0
	@echo "No differences found in PS3!"

ps4-check: ps4
	cd ps4-symbols; \
	find * -wholename "expected/*.symbols" | awk -F/ '{print $$0 " " $$2}' | xargs -L 1 diff -s --unified=0
	@echo "No differences found in PS4!"
