BUILD_DIR = ../build/common

TINOS_CPP ::= gui/TinosBoldItalic.cpp
TINOS_OBJ ::= $(TINOS_CPP:%=$(BUILD_DIR)/%.o)

$(TINOS_OBJ): $(TINOS_CPP)
	@mkdir -p $(dir $(TINOS_OBJ))
	g++ -O3 -c -o $@ $<
