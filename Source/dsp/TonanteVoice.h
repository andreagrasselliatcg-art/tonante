#pragma once
// ============================================================================
//  Tonante – motore di sintesi monofonico (indipendente da JUCE)
//
//  Architettura classica "3 VCO → mixer → filtro ladder 24 dB → VCA":
//   • 3 oscillatori PolyBLEP (2x oversampling) con range LO/32'/16'/8'/4'/2'
//     e 6 forme d'onda; Osc 2/3 con intonazione fine ±7 semitoni;
//     Osc 3 sganciabile dalla tastiera (per usarlo come LFO).
//   • Mixer: 3 osc + rumore bianco/rosa + feedback (anello di saturazione).
//   • Filtro ladder 4 poli ZDF non lineare con emphasis fino all'autoscillazione,
//     inviluppo dedicato, inseguimento tastiera 1/3 + 2/3.
//   • Due inviluppi ADS con interruttore "Decay" per il rilascio.
//   • Modulazione: mix Osc 3 / rumore → pitch oscillatori e/o cutoff,
//     quantità da manopola + mod wheel.
//   • Glide, priorità note (bassa / ultima / alta), single/multi trigger, drift.
// ============================================================================
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace tonante
{
constexpr double kPi = 3.14159265358979323846;

inline float fastTanh (float x) noexcept
{
    x = std::clamp (x, -3.0f, 3.0f);
    const float x2 = x * x;
    return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}

inline float polyBlep (float t, float dt) noexcept
{
    if (t < dt)        { t /= dt;               return t + t - t * t - 1.0f; }
    if (t > 1.0f - dt) { t = (t - 1.0f) / dt;   return t * t + t + t + 1.0f; }
    return 0.0f;
}

// ---------------------------------------------------------------- parametri
enum Wave { Triangle = 0, TriSaw, Saw, Square, WidePulse, NarrowPulse }; // su Osc 3: TriSaw = dente di sega inverso
enum Range { RangeLo = 0, Range32, Range16, Range8, Range4, Range2 };
enum Priority { LowNote = 0, LastNote, HighNote };

struct SynthParams
{
    // performance
    float masterTune = 0.0f;     // semitoni (±1)
    float glide = 0.0f;          // 0..10
    bool  glideOn = false;
    float modMix = 0.0f;         // 0 = Osc 3, 10 = rumore
    float modAmount = 0.0f;      // 0..10 (si somma alla mod wheel)
    bool  oscMod = false;        // modulazione → pitch Osc 1/2
    bool  filterMod = false;     // modulazione → cutoff
    bool  osc3Kbd = true;        // Osc 3 segue la tastiera
    float bendRange = 2.0f;      // semitoni
    bool  singleTrigger = true;  // legato: gli inviluppi non ripartono sulle note legate
    int   priority = LowNote;
    float drift = 2.0f;          // 0..10 instabilità analogica
    float velocity = 0.0f;       // 0..1 sensibilità alla dinamica

    // oscillatori
    int   range[3] = { Range8, Range8, Range8 };
    int   wave[3]  = { Saw, Saw, Saw };
    float detune2 = 0.0f, detune3 = 0.0f;   // semitoni ±7

    // mixer (0..10)
    bool  oscOn[3] = { true, false, false };
    float oscVol[3] = { 8.0f, 6.0f, 6.0f };
    bool  noiseOn = false;   float noiseVol = 0.0f;   bool pinkNoise = false;
    bool  feedbackOn = false; float feedbackVol = 0.0f;

    // filtro
    float cutoffHz = 1200.0f;
    float emphasis = 2.0f;       // 0..10
    float contour = 5.0f;        // 0..10
    bool  kbd1 = false, kbd2 = false;

    // inviluppi (0..10) e sustain (0..10)
    float fAttack = 0.0f, fDecay = 4.0f, fSustain = 4.0f;
    float aAttack = 0.0f, aDecay = 4.0f, aSustain = 8.0f;
    bool  decaySwitch = false;   // rilascio = decay

    float volume = 7.0f;         // 0..10
};

// ---------------------------------------------------------------- blocchi DSP
struct Oscillator
{
    double phase = 0.0;

    float process (double inc, int wave, bool osc3) noexcept
    {
        const float t  = (float) phase;
        const float dt = (float) std::min (inc, 0.49);
        float y = 0.0f;
        auto pulse = [&] (float duty)
        {
            float v = t < duty ? 1.0f : -1.0f;
            v += polyBlep (t, dt);
            float t2 = t + 1.0f - duty; if (t2 >= 1.0f) t2 -= 1.0f;
            v -= polyBlep (t2, dt);
            return v - (2.0f * duty - 1.0f); // accoppiamento AC
        };
        switch (wave)
        {
            case Triangle:  y = 1.0f - 4.0f * std::fabs (t - 0.5f); break;
            case TriSaw:
                if (osc3) y = 1.0f - 2.0f * t + polyBlep (t, dt);                       // dente di sega inverso
                else      y = 0.5f * ((1.0f - 4.0f * std::fabs (t - 0.5f)) + (2.0f * t - 1.0f - polyBlep (t, dt)));
                break;
            case Saw:         y = 2.0f * t - 1.0f - polyBlep (t, dt); break;
            case Square:      y = pulse (0.5f);  break;
            case WidePulse:   y = pulse (0.30f); break;
            case NarrowPulse: y = pulse (0.14f); break;
            default: break;
        }
        phase += inc;
        if (phase >= 1.0) phase -= std::floor (phase);
        return y;
    }
};

struct Noise
{
    uint32_t state = 0x12345678u;
    float b0 = 0, b1 = 0, b2 = 0;
    float white() noexcept
    {
        state ^= state << 13; state ^= state >> 17; state ^= state << 5;
        return (float) ((int32_t) state) * (1.0f / 2147483648.0f);
    }
    float process (bool pink) noexcept
    {
        const float w = white();
        if (! pink) return w;
        b0 = 0.99765f * b0 + w * 0.0990460f;   // filtro "economy" di P. Kellet
        b1 = 0.96300f * b1 + w * 0.2965164f;
        b2 = 0.57000f * b2 + w * 1.0526913f;
        return (b0 + b1 + b2 + w * 0.1848f) * 0.25f;
    }
};

// Ladder 4 poli, topologia ZDF (TPT) con saturazione all'ingresso del nodo di retroazione
struct Ladder
{
    float s[4] = { 0, 0, 0, 0 };
    void reset() noexcept { std::fill (std::begin (s), std::end (s), 0.0f); }

    float process (float x, float g, float k) noexcept
    {
        const float G = g / (1.0f + g), b = 1.0f - G;
        const float G2 = G * G, G3 = G2 * G, G4 = G2 * G2;
        const float S = b * (G3 * s[0] + G2 * s[1] + G * s[2] + s[3]);
        float u = (x * (1.0f + 0.45f * k) - k * S) / (1.0f + k * G4);
        u = fastTanh (u);
        for (int i = 0; i < 4; ++i)
        {
            const float v = (u - s[i]) * G;
            const float y = v + s[i];
            s[i] = y + v;
            u = y;
        }
        return u;
    }
};

struct Envelope
{
    enum Stage { Idle, Attack, Decay, Release };
    Stage stage = Idle;
    float value = 0.0f;

    void gateOn() noexcept  { stage = Attack; }        // riparte dal livello attuale (come un RC)
    void gateOff() noexcept { if (stage != Idle) stage = Release; }
    void reset() noexcept   { stage = Idle; value = 0.0f; }

    float process (float aCoef, float dCoef, float sustain, float rCoef) noexcept
    {
        switch (stage)
        {
            case Attack:
                value += (1.3f - value) * aCoef;        // carica verso 1.3: curva quasi lineare
                if (value >= 1.0f) { value = 1.0f; stage = Decay; }
                break;
            case Decay:   value += (sustain - value) * dCoef; break;
            case Release:
                value += (0.0f - value) * rCoef;
                if (value < 1.0e-5f) { value = 0.0f; stage = Idle; }
                break;
            case Idle: default: break;
        }
        return value;
    }
};

// Decimatore 2x (FIR half-band, finestra di Blackman)
struct HalfBand
{
    static constexpr int kTaps = 47;
    std::array<float, kTaps> h {};
    std::array<float, kTaps * 2> buf {};
    int pos = 0;

    HalfBand()
    {
        const int M = (kTaps - 1) / 2;
        double sum = 0;
        for (int n = 0; n < kTaps; ++n)
        {
            const int m = n - M;
            const double sinc = m == 0 ? 0.5 : std::sin (kPi * 0.5 * m) / (kPi * m);
            const double w = 0.42 - 0.5 * std::cos (2 * kPi * n / (kTaps - 1)) + 0.08 * std::cos (4 * kPi * n / (kTaps - 1));
            h[(size_t) n] = (float) (sinc * w);
            sum += h[(size_t) n];
        }
        for (auto& c : h) c = (float) (c / sum);
    }
    void reset() noexcept { buf.fill (0.0f); pos = 0; }
    void push (float x) noexcept
    {
        buf[(size_t) pos] = buf[(size_t) (pos + kTaps)] = x;
        if (++pos >= kTaps) pos = 0;
    }
    float output() const noexcept
    {
        float y = 0.0f;
        for (int n = 0; n < kTaps; ++n) y += h[(size_t) n] * buf[(size_t) (pos + n)];
        return y;
    }
};

// ---------------------------------------------------------------- voce monofonica
class MonoSynth
{
public:
    void prepare (double sampleRate)
    {
        fs = sampleRate > 0 ? sampleRate : 44100.0;
        fs2 = fs * 2.0;
        reset();
    }

    void reset()
    {
        for (auto& o : osc) o.phase = 0.0;
        ladder.reset(); fEnv.reset(); aEnv.reset(); dec.reset();
        numHeld = 0; gate = false;
        lastOut = 0; osc3Last = 0;
        smCutoff = -1.0f;
    }

    void setParams (const SynthParams& p) noexcept { params = p; }
    void setPitchBend (float v) noexcept   { bend = std::clamp (v, -1.0f, 1.0f); }   // -1..1
    void setModWheel (float v) noexcept    { wheel = std::clamp (v, 0.0f, 1.0f); }

    void noteOn (int note, float vel) noexcept
    {
        removeHeld (note);
        if (numHeld < kMaxHeld) { held[numHeld] = note; heldVel[numHeld] = vel; ++numHeld; }
        updateNote (true);
    }

    void noteOff (int note) noexcept
    {
        removeHeld (note);
        updateNote (false);
    }

    void allNotesOff() noexcept
    {
        numHeld = 0;
        gate = false;
        fEnv.gateOff(); aEnv.gateOff();
    }

    bool isSilent() const noexcept { return aEnv.stage == Envelope::Idle; }

    // genera numSamples campioni mono (frequenza di campionamento host)
    void render (float* out, int numSamples) noexcept
    {
        const auto& p = params;
        const float aA = envCoefAttack (p.aAttack), aD = envCoef (decayTime (p.aDecay));
        const float fA = envCoefAttack (p.fAttack), fD = envCoef (decayTime (p.fDecay));
        const float aR = p.decaySwitch ? aD : envCoef (0.006f), fR = p.decaySwitch ? fD : envCoef (0.006f);
        const float aS = std::clamp (p.aSustain / 10.0f, 0.0f, 1.0f), fS = std::clamp (p.fSustain / 10.0f, 0.0f, 1.0f);

        const float glideT = p.glideOn ? 5.0f * std::pow (std::clamp (p.glide, 0.0f, 10.0f) / 10.0f, 3.0f) : 0.0f;
        // glideT = tempo per arrivare (quasi) alla nota: costante di tempo = glideT / 5
        const float glideCoef = glideT > 1.0e-4f ? (float) (1.0 - std::exp (-5.0 / (glideT * fs))) : 1.0f;

        const float m = std::clamp (p.modAmount / 10.0f + wheel, 0.0f, 1.0f);
        const float mixN = std::clamp (p.modMix / 10.0f, 0.0f, 1.0f);
        const float vol[3] = { p.oscOn[0] ? p.oscVol[0] / 10.0f * 0.6f : 0.0f,
                               p.oscOn[1] ? p.oscVol[1] / 10.0f * 0.6f : 0.0f,
                               p.oscOn[2] ? p.oscVol[2] / 10.0f * 0.6f : 0.0f };
        const float nVol = p.noiseOn ? p.noiseVol / 10.0f * 0.6f : 0.0f;
        const float fb = p.feedbackOn ? p.feedbackVol / 10.0f * 1.6f : 0.0f;
        const float k = std::clamp (p.emphasis / 10.0f, 0.0f, 1.0f) * 4.6f;
        const float kbdTrack = (p.kbd1 ? 1.0f / 3.0f : 0.0f) + (p.kbd2 ? 2.0f / 3.0f : 0.0f);
        const float outVol = std::pow (std::clamp (p.volume / 10.0f, 0.0f, 1.0f), 2.0f);
        const float velAmt = std::clamp (p.velocity, 0.0f, 1.0f);
        const float velGain = 1.0f - velAmt + velAmt * velocity;
        const float baseOct = std::log2 (std::clamp (p.cutoffHz, 10.0f, 20000.0f));
        const float driftAmt = std::clamp (p.drift, 0.0f, 10.0f) * 0.008f; // fino a ±8 cent
        const float smC = (float) (1.0 - std::exp (-1.0 / (0.004 * fs)));

        for (int i = 0; i < numSamples; ++i)
        {
            // glide e drift (a frequenza host)
            pitch += (targetPitch - pitch) * glideCoef;
            if (--driftCounter <= 0)
            {
                driftCounter = 256;
                for (int o = 0; o < 3; ++o) driftTarget[o] = noise.white() * driftAmt;
            }
            for (int o = 0; o < 3; ++o) driftCur[o] += (driftTarget[o] - driftCur[o]) * 0.0005f;

            const float fe = fEnv.process (fA, fD, fS, fR);
            const float ae = aEnv.process (aA, aD, aS, aR);

            // sorgente di modulazione: Osc 3 ↔ rumore
            const float modSrc = (1.0f - mixN) * osc3Last + mixN * noise.white() * 0.7f;
            const float pitchMod = p.oscMod ? modSrc * m * m * 12.0f : 0.0f;

            const float common = pitch + bend * p.bendRange + p.masterTune;
            const float st1 = common + rangeOffset (p.range[0]) + driftCur[0] + pitchMod;
            const float st2 = common + rangeOffset (p.range[1]) + p.detune2 + driftCur[1] + pitchMod;
            float st3;
            const float d3 = p.range[2] == RangeLo ? p.detune3 * 5.0f : p.detune3;   // in LO la manopola copre ±3 ottave
            if (p.osc3Kbd) st3 = common + rangeOffset (p.range[2]) + d3 + driftCur[2];
            else           st3 = 48.0f + p.masterTune + rangeOffset (p.range[2]) + d3;

            const double inc1 = noteHz (st1) / fs2, inc2 = noteHz (st2) / fs2, inc3 = noteHz (st3) / fs2;

            // cutoff in ottave
            float octs = baseOct + fe * (p.contour / 10.0f) * 4.5f * (1.0f - velAmt + velAmt * velocity)
                       + kbdTrack * (pitch - 60.0f) / 12.0f
                       + (p.filterMod ? modSrc * m * 3.0f : 0.0f);
            if (smCutoff < 0.0f) smCutoff = octs;
            smCutoff += (octs - smCutoff) * smC;
            const float fc = std::clamp (std::exp2 (smCutoff), 8.0f, (float) (0.45 * fs2));
            const float g = (float) std::tan (kPi * fc / fs2);

            float o3 = 0.0f;
            for (int os = 0; os < 2; ++os)   // 2x oversampling
            {
                const float o1 = osc[0].process (inc1, p.wave[0], false);
                const float o2 = osc[1].process (inc2, p.wave[1], false);
                o3 = osc[2].process (inc3, p.wave[2], true);
                float x = vol[0] * o1 + vol[1] * o2 + vol[2] * o3 + nVol * noise.process (p.pinkNoise);
                x += fb * fastTanh (lastOut * 1.5f);           // anello di feedback (saturazione "calda")
                x += noise.white() * 2.0e-4f;                   // rumore di fondo analogico: innesca l'autoscillazione
                const float y = ladder.process (x, g, k);
                lastOut = y;
                dec.push (y);
            }
            osc3Last = o3;

            float y = dec.output() * ae * velGain * outVol * 1.4f * (1.0f + 0.18f * k);   // compensa in parte la perdita di volume con l'emphasis
            out[i] = fastTanh (y);                                // soft clip finale
        }
    }

    // utilità esposte per test
    static float noteHz (float semis) noexcept { return 440.0f * std::exp2 ((semis - 69.0f) / 12.0f); }
    static float rangeOffset (int r) noexcept
    {
        switch (r) { case RangeLo: return -84.0f; case Range32: return -24.0f; case Range16: return -12.0f;
                     case Range8: return 0.0f; case Range4: return 12.0f; case Range2: return 24.0f; default: return 0.0f; }
    }

private:
    static constexpr int kMaxHeld = 16;

    float envCoef (float seconds) const noexcept { return (float) (1.0 - std::exp (-1.0 / (std::max (seconds, 0.0005f) * fs))); }
    float envCoefAttack (float knob) const noexcept
    {
        const float t = 0.001f * std::pow (10.0f, std::clamp (knob, 0.0f, 10.0f) * 0.4f); // 1 ms .. 10 s
        return envCoef (t / 1.466f);                                                       // tempo per arrivare a 1
    }
    static float decayTime (float knob) noexcept { return 0.002f * std::pow (10.0f, std::clamp (knob, 0.0f, 10.0f) * 0.4f); } // 2 ms .. 20 s

    void removeHeld (int note) noexcept
    {
        for (int i = 0; i < numHeld; ++i)
            if (held[i] == note)
            {
                for (int j = i; j < numHeld - 1; ++j) { held[j] = held[j + 1]; heldVel[j] = heldVel[j + 1]; }
                --numHeld;
                return;
            }
    }

    int chooseNote (float& vel) const noexcept
    {
        int idx = numHeld - 1;                                   // ultima
        if (params.priority == LowNote)
            for (int i = 0; i < numHeld; ++i) { if (held[i] < held[idx]) idx = i; }
        else if (params.priority == HighNote)
            for (int i = 0; i < numHeld; ++i) { if (held[i] > held[idx]) idx = i; }
        vel = heldVel[idx];
        return held[idx];
    }

    void updateNote (bool isNoteOn) noexcept
    {
        if (numHeld == 0)
        {
            gate = false;
            fEnv.gateOff(); aEnv.gateOff();
            return;
        }
        float vel = 1.0f;
        const int note = chooseNote (vel);
        const bool wasGate = gate;
        const bool changed = note != targetNote;
        targetNote = note;
        targetPitch = (float) note;
        if (! wasGate)
        {
            velocity = vel;
            if (! params.glideOn || params.glide <= 0.0f || aEnv.stage == Envelope::Idle) pitch = targetPitch;
            fEnv.gateOn(); aEnv.gateOn();
        }
        else if (isNoteOn && changed && ! params.singleTrigger)
        {
            velocity = vel;
            fEnv.gateOn(); aEnv.gateOn();                       // multi trigger
        }
        if (! params.glideOn || params.glide <= 0.0f) pitch = targetPitch;
        gate = true;
    }

    double fs = 44100.0, fs2 = 88200.0;
    SynthParams params;
    std::array<Oscillator, 3> osc;
    Noise noise;
    Ladder ladder;
    Envelope fEnv, aEnv;
    HalfBand dec;

    int held[kMaxHeld] {};
    float heldVel[kMaxHeld] {};
    int numHeld = 0;
    bool gate = false;
    float pitch = 60.0f, targetPitch = 60.0f, velocity = 1.0f;
    int targetNote = -1;
    float bend = 0.0f, wheel = 0.0f;
    float lastOut = 0.0f, osc3Last = 0.0f, smCutoff = -1.0f;
    float driftTarget[3] {}, driftCur[3] {};
    int driftCounter = 0;
};
} // namespace tonante
