# Project name, used for binaries
NAME = IterativeSinCluster

# SIMD related variables.
FILES_SIMD = dsp/oscillator.cpp dsp/note.cpp
OBJ_DIR_SIMD ::= $(addsuffix /simd,../build/$(NAME))

NAME_SIMD ::= $(FILES_SIMD:.cpp=)

OBJ_AVX512 ::= $(addprefix $(OBJ_DIR_SIMD)/,$(addsuffix .avx512.o,$(NAME_SIMD)))
OBJ_AVX2 ::= $(addprefix $(OBJ_DIR_SIMD)/,$(addsuffix .avx2.o,$(NAME_SIMD)))
OBJ_SSE41 ::= $(addprefix $(OBJ_DIR_SIMD)/,$(addsuffix .sse41.o,$(NAME_SIMD)))
OBJ_SSE2 ::= $(addprefix $(OBJ_DIR_SIMD)/,$(addsuffix .sse2.o,$(NAME_SIMD)))

OBJ_SIMD ::= $(OBJ_AVX512) $(OBJ_AVX2) $(OBJ_SSE41) $(OBJ_SSE2)

OBJS_DSP += $(OBJ_SIMD)

INSTRSET_DETECT ::= ../lib/vcl/instrset_detect.cpp

# Files to build
FILES_DSP = \
	$(INSTRSET_DETECT) \
	plugin.cpp \
	parameter.cpp \
	dsp/dspcore.cpp \

FILES_UI  = \
	ui.cpp \
	parameter.cpp \
	gui/TinosBoldItalic.cpp \

# Do some magic
include ../Makefile.plugins.mk

# Enable c++17 and avx2.
ifeq ($(DEBUG),true)
BUILD_CXX_FLAGS += -std=c++17 -g -Wall
else
BUILD_CXX_FLAGS += -std=c++17 -O3 -Wall -Wno-unused-but-set-parameter
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

# Entry point.
all: simd $(TARGETS)

# SIMD rules.
simd: mkdir_build $(OBJ_AVX512) $(OBJ_AVX2) $(OBJ_SSE41) $(OBJ_SSE2)

mkdir_build:
	@mkdir -p $(OBJ_DIR_SIMD)/dsp

DPF_INCLUDE_PATH = -I. -I$(DPF_PATH)/distrho -I$(DPF_PATH)/dgl

$(OBJ_DIR_SIMD)/%.avx512.o: %.cpp
	@echo $@: $<
	@$(CXX) $(DPF_INCLUDE_PATH) -O3 -fPIC -mavx512f -mfma -mavx512vl -mavx512bw -mavx512dq -std=c++17 -c $< -o$@
$(OBJ_DIR_SIMD)/%.avx2.o: %.cpp
	@echo $@: $<
	@$(CXX) $(DPF_INCLUDE_PATH) -O3 -fPIC -mavx2 -mfma -std=c++17 -c $< -o$@
$(OBJ_DIR_SIMD)/%.sse41.o: %.cpp
	@echo $@: $<
	@$(CXX) $(DPF_INCLUDE_PATH) -O3 -fPIC -msse4.1 -std=c++17 -c $< -o$@
$(OBJ_DIR_SIMD)/%.sse2.o: %.cpp
	@echo $@: $<
	@$(CXX) $(DPF_INCLUDE_PATH) -O3 -fPIC -msse2 -std=c++17 -c $< -o$@
