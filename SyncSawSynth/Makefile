# Project name, used for binaries
NAME = SyncSawSynth

# Files to build
FILES_DSP = \
	plugin.cpp \
	parameter.cpp \
	dsp/dspcore.cpp \

FILES_UI  = \
	ui.cpp \
	parameter.cpp \

OBJS_UI = \
	../build/common/gui/style.o \
	../build/common/gui/TinosBoldItalic.o \

# Do some magic
include ../Makefile.plugins.mk

# Enable c++17.
ifeq ($(DEBUG),true)
BUILD_CXX_FLAGS += -std=c++17 -g -Wall
else
BUILD_CXX_FLAGS += -std=c++17 -O3 -Wall
endif

# Enable all possible plugin types
LV2 ?= true
VST2 ?= true
JACK ?= true

ifeq ($(JACK)$(HAVE_JACK)$(HAVE_OPENGL),truetruetrue)
TARGETS += jack
endif

ifeq ($(LV2),true)
ifeq ($(HAVE_OPENGL),true)
TARGETS += lv2_sep
else
TARGETS += lv2_dsp
endif
endif

ifeq ($(VST2),true)
TARGETS += vst
endif

all: $(TARGETS)
