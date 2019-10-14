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
	gui/TinosBoldItalic.cpp \

# Do some magic
include ../Makefile.plugins.mk

# Enable c++17.
ifeq ($(DEBUG),true)
BUILD_CXX_FLAGS += -std=c++17 -g -Wall
else
BUILD_CXX_FLAGS += -std=c++17 -O3 -Wall
endif

# Enable all possible plugin types
ifeq ($(HAVE_JACK),true)
ifeq ($(HAVE_OPENGL),true)
TARGETS += jack
endif
endif

# ifeq ($(LINUX),true)
# ifeq ($(HAVE_LIBLO),true)
# ifeq ($(HAVE_OPENGL),true)
# TARGETS += ladspa
# TARGETS += dssi
# endif
# endif
# endif

ifeq ($(HAVE_OPENGL),true)
TARGETS += lv2_sep
else
TARGETS += lv2_dsp
endif

TARGETS += vst

all: $(TARGETS)
