all: patch build generate_ttl

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

.PHONY: patch
patch:
	$(MAKE) -C patch

.PHONY: generate_ttl
generate_ttl: build
	./generate-ttl.sh

# DEBUG=true
# export DEBUG

LV2 ?= true
VST2 ?= true
JACK ?= true

ifeq ($(LV2),true)
INSTALL_TARGET += installLV2
endif
ifeq ($(VST2),true)
INSTALL_TARGET += installVST2
endif
ifeq ($(JACK),true)
INSTALL_TARGET += installJACK
endif

pkgname ?= UhhyouPlugins
PREFIX ?= /usr/local
lv2dir ?= $(DESTDIR)$(PREFIX)/lib/lv2
vst2dir ?= $(DESTDIR)$(PREFIX)/lib/lxvst
bindir ?= $(DESTDIR)$(PREFIX)/bin
sysconfdir ?= $(DESTDIR)$(PREFIX)/etc
datarootdir ?= $(DESTDIR)$(PREFIX)/share
datadir ?= $(datarootdir)
docdir ?= $(datarootdir)/doc

.PHONY: dpf
dpf:
	$(MAKE) -C lib/DPF dgl utils/lv2_ttl_generator

.PHONY: common
common: dpf
	$(MAKE) -C common

.PHONY: experimental
experimental: common
	$(MAKE) -C experimental

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
install: $(INSTALL_TARGET) installConfig installResource installDoc

.PHONY: installConfig
installConfig:
	mkdir -p $(sysconfdir)/$(pkgname)/style
	cp -r style/style.json $(sysconfdir)/$(pkgname)/style

.PHONY: installResource
installResource:
	mkdir -p $(datadir)/$(pkgname)/themes
	cp -r style/themes $(datadir)/$(pkgname)

.PHONY: installDoc
installDoc:
	mkdir -p $(docdir)/$(pkgname)
	cp -r License $(docdir)/$(pkgname)
	cp README.md $(docdir)/$(pkgname)
	cp style/ColorConfig.md $(docdir)/$(pkgname)

.PHONY: installLV2
installLV2:
	mkdir -p $(lv2dir)
	cp -r bin/*.lv2 $(lv2dir)

.PHONY: installVST2
installVST2:
	mkdir -p $(vst2dir)/$(pkgname)
	cp -r bin/*-vst.so $(vst2dir)/$(pkgname)

.PHONY: installJACK
installJACK:
	mkdir -p $(bindir)
	cp -r `find bin -maxdepth 1 -type f ! -name "*.*"` $(bindir)

.PHONY: installHome
installHome:
	mkdir -p ~/.lv2
	cp -r bin/*.lv2 ~/.lv2
	python3 install.py

.PHONY: installHomeVST2
installHomeVST2:
	mkdir -p ~/.lxvst
	cp -r bin/*-vst.so ~/.lxvst
	python3 install.py

.PHONY: clean
clean:
	rm -rf bin
	rm -rf build

.PHONY: cleanDPF
cleanDPF:
	$(MAKE) -C lib/DPF clean

.PHONY: cleanall
cleanall: cleanDPF clean
