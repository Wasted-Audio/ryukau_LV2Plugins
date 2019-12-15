#include "note.hpp"

// Using fmod because if equalTemperament == 1, this returns 2^121 which is too large.
inline float semiToPitch(float semi, float equalTemperament)
{
  return fmodf(powf(2.0f, semi / equalTemperament), 1e5f);
}

inline float paramMilliToPitch(float semi, float milli, float equalTemperament)
{
  return fmodf(
    powf(2.0f, (1000.0f * floorf(semi) + milli) / (equalTemperament * 1000.0f)), 1e5f);
}

// https://en.wikipedia.org/wiki/Cent_(music)#Piecewise_linear_approximation
inline float centApprox(float cent) { return 1.0f + 0.0005946f * cent; }

#if INSTRSET == 2 // SSE2
void Note::setup(float sampleRate)
{
  this->sampleRate = sampleRate;

  for (auto &osc : oscillator) osc.setup(osc, sampleRate);
  chordPan.fill(0.5);

  gainEnvelope.setup(sampleRate);
}

Note::Note()
{
  for (auto &osc : oscillator) BiquadOsc::initMethod(osc);

  auto iset = instrset_detect();
  if (iset >= 10) { // AVX512 version
    noteOn = &NoteMethod::noteOn_AVX512;
  } else if (iset >= 8) { // AVX2 version
    noteOn = &NoteMethod::noteOn_AVX2;
  } else if (iset >= 5) { // SSE4.1 version
    noteOn = &NoteMethod::noteOn_SSE41;
  } else if (iset >= 2) { // SSE2 version
    noteOn = &NoteMethod::noteOn_SSE2;
  } else { // Error: lowest instruction set not supported
    std::cerr << "\nError: Instruction set SSE2 not supported on this computer";
    exit(EXIT_FAILURE);
  }
}

void Note::release()
{
  if (state == NoteState::rest) return;
  state = NoteState::release;
  gainEnvelope.release();
}

void Note::rest() { state = NoteState::rest; }

std::array<float, 2> Note::process()
{
  if (state == NoteState::rest) return {0, 0};

  std::array<float, 2> out{};
  for (size_t i = 0; i < nChord; ++i) {
    auto sig = oscillator[i].process(oscillator[i]);
    out[0] += sig * (float(1) - chordPan[i]);
    out[1] += sig * chordPan[i];
  }

  const auto gainEnv = gainEnvelope.process();
  if (gainEnvelope.isTerminated()) rest();

  gain = velocity
    * (gainEnv
       + gainEnvCurve
         * (juce::dsp::FastMathApproximations::tanh(2.0f * gainEnvCurve * gainEnv) - gainEnv));
  out[0] *= gain;
  out[1] *= gain;

  return out;
}
#endif

#if INSTRSET >= 10 // AVX512VL
#define NOTE_ON_FUNC noteOn_AVX512
#elif INSTRSET >= 8 // AVX2
#define NOTE_ON_FUNC noteOn_AVX2
#elif INSTRSET >= 5 // SSE4.1
#define NOTE_ON_FUNC noteOn_SSE41
#elif INSTRSET == 2 // SSE2
#define NOTE_ON_FUNC noteOn_SSE2
#else
#error Unsupported instruction set
#endif

void NoteMethod::NOTE_ON_FUNC(
  Note &note,
  int32_t noteId,
  float normalizedKey,
  float frequency,
  float velocity,
  GlobalParameter &param,
  White<float> &rng)
{
  using ID = ParameterID::ID;

  frequency
    *= somepow<float>(2, somefloor<float>(param.value[ID::masterOctave]->getFloat()));

  note.state = NoteState::active;
  note.id = noteId;
  note.normalizedKey = normalizedKey;
  note.frequency = frequency;
  note.velocity = velocity;

  const float semiSign = param.value[ID::negativeSemi]->getInt() ? float(-1) : float(1);
  const float eqTemp = param.value[ID::equalTemperament]->getInt();

  const float nyquist = note.sampleRate / 2;
  const float randGainAmt = 3 * param.value[ID::randomGainAmount]->getFloat();
  const float randFreqAmt = param.value[ID::randomFrequencyAmount]->getFloat();
  const float pitchMultiply = param.value[ID::pitchMultiply]->getFloat();
  const float pitchModulo = semiToPitch(param.value[ID::pitchModulo]->getFloat(), eqTemp);

  const float lowShelfFreq = frequency
    * semiToPitch(semiSign * param.value[ID::lowShelfPitch]->getFloat(), eqTemp);
  const float lowShelfGain = param.value[ID::lowShelfGain]->getFloat();
  const float highShelfFreq = frequency
    * semiToPitch(semiSign * param.value[ID::highShelfPitch]->getFloat(), eqTemp);
  const float highShelfGain = param.value[ID::highShelfGain]->getFloat();

  const auto enableAliasing = param.value[ID::aliasing]->getInt();

  std::array<float, nPitch> notePitch;
  std::array<float, nPitch> noteGain;
  for (size_t i = 0; i < nPitch; ++i) {
    notePitch[i] = paramMilliToPitch(
      semiSign * param.value[ID::semi0 + i]->getFloat(),
      param.value[ID::milli0 + i]->getFloat(), eqTemp);
    noteGain[i] = param.value[ID::gain0 + i]->getFloat();
  }

  Vec16f overtonePitch;
  Vec16f overtoneGain;
  for (size_t i = 0; i < nOvertone; ++i) {
    overtonePitch.insert(i, float(i + 1));
    overtoneGain.insert(i, float(param.value[ID::overtone0 + i]->getFloat()));
  }

  for (size_t chord = 0; chord < nChord; ++chord) {
    float chordFreq = frequency
      * paramMilliToPitch(
                        semiSign * param.value[ID::chordSemi0 + chord]->getFloat(),
                        param.value[ID::chordMilli0 + chord]->getFloat(), eqTemp);
    float chordGain = param.value[ID::chordGain0 + chord]->getFloat();
    for (size_t pitch = 0; pitch < nPitch; ++pitch) {
      // Equation to calculate a sine wave frquency.
      // freq = noteFreq * (overtone + 1) * (pitch + 1)
      //      = noteFreq * (1 + pitch + overtone + pitch * overtone);
      Vec16f rndPt = randFreqAmt
        * Vec16f(rng.process(), rng.process(), rng.process(), rng.process(),
                 rng.process(), rng.process(), rng.process(), rng.process(),
                 rng.process(), rng.process(), rng.process(), rng.process(),
                 rng.process(), rng.process(), rng.process(), rng.process());
      Vec16f modPt = pitchMultiply * (rndPt + overtonePitch + rndPt * overtonePitch);
      if (pitchModulo != 1) // Modulo operation. modf isn't available in vcl.
        modPt = modPt - pitchModulo * floor(modPt / pitchModulo);

      Vec16f oscFreq = chordFreq * notePitch[pitch] * (1.0f + modPt);
      note.oscillator[chord].frequency[pitch] = oscFreq;

      Vec16f shelving(1.0f);
      shelving = select(oscFreq <= lowShelfFreq, shelving * lowShelfGain, shelving);
      shelving = select(oscFreq >= highShelfFreq, shelving * highShelfGain, shelving);
      Vec16f rndGn(
        rng.process(), rng.process(), rng.process(), rng.process(), rng.process(),
        rng.process(), rng.process(), rng.process(), rng.process(), rng.process(),
        rng.process(), rng.process(), rng.process(), rng.process(), rng.process(),
        rng.process());

      Vec16f oscGain(overtoneGain);
      if (!enableAliasing) oscGain = select(oscFreq >= nyquist, 0.0f, oscGain);

      note.oscillator[chord].gain[pitch]
        = oscGain * chordGain * noteGain[pitch] * shelving * (1.0f + randGainAmt * rndGn);
    }
  }

  for (auto &osc : note.oscillator) osc.setup(osc, note.sampleRate);

  for (size_t i = 0; i < nChord; ++i)
    note.chordPan[i] = param.value[ID::chordPan0 + i]->getFloat();

  note.gainEnvelope.reset(
    param.value[ID::gainA]->getFloat(), param.value[ID::gainD]->getFloat(),
    param.value[ID::gainS]->getFloat(), param.value[ID::gainR]->getFloat(), frequency,
    param.value[ID::gainEnvelopeCurve]->getFloat());
  note.gainEnvCurve = param.value[ID::gainEnvelopeCurve]->getFloat();
}
