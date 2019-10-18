Requires libsndfile.

```bash
g++ -g -lsndfile -DTEST_BUILD ../SyncSawSynth/dsp/dspcore.cpp ../SyncSawSynth/parameter.cpp main.cpp
valgrind --read-inline-info=yes --tool=callgrind ./a.out
```
