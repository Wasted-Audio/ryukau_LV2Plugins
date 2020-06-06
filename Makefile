all: build generate_ttl

build: common \
	lv2cvport \
	CubicPadSynth \
	EnvelopedSine \
	EsPhaser \
	FDNCymbal \
	FoldShaper \
	IterativeSinCluster \
	L3Reverb \
	L4Reverb \
	LatticeReverb \
	LightPadSynth \
	ModuloShaper \
	OddPowShaper \
	SevenDelay \
	SoftClipper \
	SyncSawSynth \
	TrapezoidSynth \
	WaveCymbal \

.PHONY: generate_ttl
generate_ttl: build
	./generate-ttl.sh

# DEBUG=true
# export DEBUG

LV2 ?= true
VST2 ?= true
JACK ?= true

LV2_PATH ?= $(HOME)/.lv2
VST2_PATH ?= $(HOME)/.lxvst

.PHONY: dpf
dpf:
	$(MAKE) -C lib/DPF dgl utils/lv2_ttl_generator

.PHONY: common
common: dpf
	$(MAKE) -C common

.PHONY: lv2cvport
lv2cvport: common
	$(MAKE) -C lv2cvport

.PHONY: CubicPadSynth
CubicPadSynth: common
	$(MAKE) -C CubicPadSynth LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: EnvelopedSine
EnvelopedSine: common
	$(MAKE) -C EnvelopedSine LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: EsPhaser
EsPhaser: common
	$(MAKE) -C EsPhaser LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: FDNCymbal
FDNCymbal: common
	$(MAKE) -C FDNCymbal LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: FoldShaper
FoldShaper: common
	$(MAKE) -C FoldShaper LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: IterativeSinCluster
IterativeSinCluster: common
	$(MAKE) -C IterativeSinCluster LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: L3Reverb
L3Reverb: common
	$(MAKE) -C L3Reverb LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: L4Reverb
L4Reverb: common
	$(MAKE) -C L4Reverb LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: LatticeReverb
LatticeReverb: common
	$(MAKE) -C LatticeReverb LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: LightPadSynth
LightPadSynth: common
	$(MAKE) -C LightPadSynth LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: ModuloShaper
ModuloShaper: common
	$(MAKE) -C ModuloShaper LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: OddPowShaper
OddPowShaper: common
	$(MAKE) -C OddPowShaper LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: SevenDelay
SevenDelay: common
	$(MAKE) -C SevenDelay LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: SoftClipper
SoftClipper: common
	$(MAKE) -C SoftClipper LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: SyncSawSynth
SyncSawSynth: common
	$(MAKE) -C SyncSawSynth LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: TrapezoidSynth
TrapezoidSynth: common
	$(MAKE) -C TrapezoidSynth LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: WaveCymbal
WaveCymbal: common
	$(MAKE) -C WaveCymbal LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: install
install:
	mkdir -p $(LV2_PATH)
	cp -r bin/*.lv2 $(LV2_PATH)/
	python3 install.py $(CONFIG_PATH)

.PHONY: installVST2
installVST2:
	mkdir -p $(VST2_PATH)/UhhyouPlugins
	cp -r bin/*-vst.so $(VST2_PATH)/UhhyouPlugins/
	python3 install.py $(CONFIG_PATH)

.PHONY: clean
clean:
	rm -rf bin
	rm -rf build

.PHONY: cleanDPF
cleanDPF:
	$(MAKE) -C lib/DPF clean

.PHONY: cleanall
cleanall: cleanDPF clean
