all: dpf \
	SevenDelay \
	SyncSawSynth \
	WaveCymbal \
	FDNCymbal \
	TrapezoidSynth \
	IterativeSinCluster \
	EnvelopedSine \
	EsPhaser \
	CubicPadSynth

# DEBUG=true
# export DEBUG

LV2 ?= true
VST2 ?= true
JACK ?= true

.PHONY: dpf
dpf:
	$(MAKE) -C lib/DPF

.PHONY: CubicPadSynth
CubicPadSynth: dpf
	$(MAKE) -C CubicPadSynth LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: EsPhaser
EsPhaser: dpf
	$(MAKE) -C EsPhaser LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: EnvelopedSine
EnvelopedSine: dpf
	$(MAKE) -C EnvelopedSine LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: IterativeSinCluster
IterativeSinCluster: dpf
	$(MAKE) -C IterativeSinCluster LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: TrapezoidSynth
TrapezoidSynth: dpf
	$(MAKE) -C TrapezoidSynth LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: FDNCymbal
FDNCymbal: dpf
	$(MAKE) -C FDNCymbal LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: WaveCymbal
WaveCymbal: dpf
	$(MAKE) -C WaveCymbal LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: SyncSawSynth
SyncSawSynth: dpf
	$(MAKE) -C SyncSawSynth LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: SevenDelay
SevenDelay: dpf
	$(MAKE) -C SevenDelay LV2=$(LV2) VST2=$(VST2) JACK=$(JACK)

.PHONY: generate_ttl
generate_ttl:
	./generate-ttl.sh

.PHONY: install
install: generate_ttl
	cp -r bin/*.lv2 ~/.lv2/

.PHONY: clean
clean:
	rm -r bin
	rm -r build

.PHONY: cleanDPF
cleanDPF:
	rm -r lib/DPF/bin
	rm -r lib/DPF/build

.PHONY: cleanall
cleanall: cleanDPF clean
