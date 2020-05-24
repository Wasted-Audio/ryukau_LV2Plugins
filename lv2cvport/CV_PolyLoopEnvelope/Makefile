# Project name, used for binaries
NAME = CV_PolyLoopEnvelope

# Files to build
FILES_DSP = \
	plugin.cpp \
	parameter.cpp \
	dsp/dspcore.cpp \

FILES_UI = \
	ui.cpp \
	parameter.cpp \

OBJS_UI = \
	../../build/common/gui/style.o \
	../../build/common/gui/TinosBoldItalic.o \

# Do some magic
DPF_PATH ::= ../../lib/DPF
TARGET_DIR ::= ../../bin
BUILD_DIR ::= ../../build/$(NAME)
include ../../Makefile.plugins.mk

# Enable c++17.
ifeq ($(DEBUG),true)
BUILD_CXX_FLAGS += -std=c++17 -g -Wall
else
BUILD_CXX_FLAGS += -std=c++17 -O3 -Wall
endif

# Enable LV2 build.
ifeq ($(HAVE_OPENGL),true)
TARGETS += lv2_sep
else
TARGETS += lv2_dsp
endif

all: $(TARGETS)
