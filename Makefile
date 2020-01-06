all: dpf \
	SevenDelay \
	SyncSawSynth \
	WaveCymbal \
	FDNCymbal \
	TrapezoidSynth \
	IterativeSinCluster \
	EnvelopedSine

# DEBUG=true
# export DEBUG

.PHONY: dpf
dpf:
	$(MAKE) -C lib/DPF

.PHONY: EnvelopedSine
EnvelopedSine: dpf
	$(MAKE) -C EnvelopedSine

.PHONY: IterativeSinCluster
IterativeSinCluster: dpf
	$(MAKE) -C IterativeSinCluster

.PHONY: TrapezoidSynth
TrapezoidSynth: dpf
	$(MAKE) -C TrapezoidSynth

.PHONY: FDNCymbal
FDNCymbal: dpf
	$(MAKE) -C FDNCymbal

.PHONY: WaveCymbal
WaveCymbal: dpf
	$(MAKE) -C WaveCymbal

.PHONY: SyncSawSynth
SyncSawSynth: dpf
	$(MAKE) -C SyncSawSynth

.PHONY: SevenDelay
SevenDelay: dpf
	$(MAKE) -C SevenDelay

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
