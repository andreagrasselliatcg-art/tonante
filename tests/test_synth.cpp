// Test offline del motore Tonante (senza JUCE):
//   g++ -O2 -std=c++17 -ISource tests/test_synth.cpp -o t && ./t
#include "dsp/TonanteVoice.h"
#include <cstdio>
#include <vector>

using namespace tonante;

static double estimateFreq (const std::vector<float>& x, double sr, int from)
{
    // frequenza dai passaggi per lo zero (con interpolazione lineare)
    double first = -1, last = -1; int count = 0;
    for (size_t n = (size_t) from + 1; n < x.size(); ++n)
        if (x[n - 1] <= 0 && x[n] > 0)
        {
            const double t = (double) (n - 1) + (double) (-x[n - 1] / (x[n] - x[n - 1]));
            if (first < 0) first = t; else ++count;
            last = t;
        }
    return count > 0 ? count * sr / (last - first) : 0.0;
}

static double centsOff (double f, double ref) { return 1200.0 * std::log2 (f / ref); }

int main()
{
    const double sr = 48000;
    int fails = 0;
    auto check = [&] (bool ok, const char* what) { std::printf ("%-58s %s\n", what, ok ? "OK" : "FAIL"); if (! ok) ++fails; };

    // 1) intonazione: Osc 1 triangolo 8', filtro aperto, A4 = 440 Hz; poi 16' e 4'
    for (int r : { Range16, Range8, Range4 })
    {
        MonoSynth s; s.prepare (sr);
        SynthParams p; p.wave[0] = Triangle; p.cutoffHz = 20000; p.emphasis = 0; p.contour = 0;
        p.aSustain = 10; p.drift = 0; p.range[0] = r;
        s.setParams (p); s.noteOn (69, 1.0f);
        std::vector<float> buf ((size_t) sr);
        s.render (buf.data(), (int) buf.size());
        const double expected = 440.0 * std::pow (2.0, MonoSynth::rangeOffset (r) / 12.0);
        const double f = estimateFreq (buf, sr, 4800);
        char msg[128]; std::snprintf (msg, sizeof (msg), "Tuning range %d: %.2f Hz (expected %.2f, %+.2f cent)", r, f, expected, centsOff (f, expected));
        check (std::fabs (centsOff (f, expected)) < 2.0, msg);
    }

    // 2) Osc 2 a +7 semitoni (quinta)
    {
        MonoSynth s; s.prepare (sr);
        SynthParams p; p.oscOn[0] = false; p.oscOn[1] = true; p.wave[1] = Triangle; p.detune2 = 7.0f;
        p.cutoffHz = 20000; p.emphasis = 0; p.contour = 0; p.aSustain = 10; p.drift = 0;
        s.setParams (p); s.noteOn (57, 1.0f);
        std::vector<float> buf ((size_t) sr); s.render (buf.data(), (int) buf.size());
        const double f = estimateFreq (buf, sr, 4800), e = 220.0 * std::pow (2.0, 7.0 / 12.0);
        char msg[128]; std::snprintf (msg, sizeof (msg), "Osc 2 detune +7: %.2f Hz (expected %.2f)", f, e);
        check (std::fabs (centsOff (f, e)) < 2.0, msg);
    }

    // 3) Filtro in autoscillazione: nessun oscillatore, emphasis 10 → sinusoide vicino al cutoff
    for (float fc : { 220.0f, 1000.0f, 4000.0f })
    {
        MonoSynth s; s.prepare (sr);
        SynthParams p; p.oscOn[0] = false; p.emphasis = 10; p.cutoffHz = fc; p.contour = 0; p.aSustain = 10; p.drift = 0;
        p.noiseOn = true; p.noiseVol = 0.05f; // un filo di rumore per innescare
        s.setParams (p); s.noteOn (60, 1.0f);
        std::vector<float> buf ((size_t) sr); s.render (buf.data(), (int) buf.size());
        float peak = 0; for (size_t n = 24000; n < buf.size(); ++n) peak = std::max (peak, std::fabs (buf[n]));
        const double f = estimateFreq (buf, sr, 24000);
        char msg[128]; std::snprintf (msg, sizeof (msg), "Self-oscillation @%.0f Hz cutoff: %.1f Hz, level %.2f", fc, f, peak);
        check (peak > 0.1f && std::fabs (centsOff (f, fc)) < 120.0, msg);
    }

    // 4) Inviluppo: attack 0 → arriva subito; release con Decay switch off → silenzio in pochi ms
    {
        MonoSynth s; s.prepare (sr);
        SynthParams p; p.cutoffHz = 20000; p.aAttack = 0; p.aSustain = 10; p.drift = 0; p.decaySwitch = false;
        s.setParams (p); s.noteOn (60, 1.0f);
        std::vector<float> a (4800); s.render (a.data(), 4800);
        s.noteOff (60);
        std::vector<float> b (4800); s.render (b.data(), 4800);
        float tail = 0; for (int n = 2400; n < 4800; ++n) tail = std::max (tail, std::fabs (b[(size_t) n]));
        float body = 0; for (int n = 2400; n < 4800; ++n) body = std::max (body, std::fabs (a[(size_t) n]));
        char msg[128]; std::snprintf (msg, sizeof (msg), "Envelope: level %.2f while held, %.5f 50 ms after release", body, tail);
        check (body > 0.2f && tail < 0.001f, msg);
    }

    // 5) Glide: da 48 a 72 con glide 5 → a metà strada dopo un po', arrivo finale corretto
    {
        MonoSynth s; s.prepare (sr);
        SynthParams p; p.wave[0] = Triangle; p.cutoffHz = 20000; p.emphasis = 0; p.contour = 0; p.aSustain = 10; p.drift = 0;
        p.glideOn = true; p.glide = 5.0f; p.priority = LastNote;
        s.setParams (p);
        s.noteOn (48, 1.0f);
        std::vector<float> w (4800); s.render (w.data(), 4800);
        s.noteOn (72, 1.0f); // legato
        std::vector<float> early (2400), late ((size_t) sr * 4);
        s.render (early.data(), 2400);
        s.render (late.data(), (int) late.size());
        const double fe = estimateFreq (early, sr, 0), fl = estimateFreq (late, sr, (int) late.size() - 9600);
        char msg[128]; std::snprintf (msg, sizeof (msg), "Glide: %.0f Hz just after, %.1f Hz at the end (target 523.3)", fe, fl);
        check (fe < 300 && std::fabs (centsOff (fl, 523.25)) < 3.0, msg);
    }

    // 6) Priorità nota bassa (default): tenendo 60, la 67 non cambia l'altezza
    {
        MonoSynth s; s.prepare (sr);
        SynthParams p; p.wave[0] = Triangle; p.cutoffHz = 20000; p.contour = 0; p.emphasis = 0; p.aSustain = 10; p.drift = 0;
        p.priority = LowNote;
        s.setParams (p); s.noteOn (60, 1.0f); s.noteOn (67, 1.0f);
        std::vector<float> buf (24000); s.render (buf.data(), 24000);
        const double f = estimateFreq (buf, sr, 2400);
        char msg[128]; std::snprintf (msg, sizeof (msg), "Low-note priority: %.1f Hz (expected 261.6)", f);
        check (std::fabs (centsOff (f, 261.63)) < 3.0, msg);
        p.priority = LastNote; s.setParams (p); s.noteOn (67, 1.0f);
        s.render (buf.data(), 24000);
        const double f2 = estimateFreq (buf, sr, 2400);
        std::snprintf (msg, sizeof (msg), "Last-note priority: %.1f Hz (expected 392.0)", f2);
        check (std::fabs (centsOff (f2, 392.0)) < 3.0, msg);
    }

    // 7) Robustezza: tutto al massimo, nessun NaN e uscita limitata
    {
        MonoSynth s; s.prepare (96000);
        SynthParams p;
        for (int o = 0; o < 3; ++o) { p.oscOn[o] = true; p.oscVol[o] = 10; p.range[o] = Range2; p.wave[o] = NarrowPulse; }
        p.noiseOn = true; p.noiseVol = 10; p.feedbackOn = true; p.feedbackVol = 10; p.emphasis = 10; p.contour = 10;
        p.cutoffHz = 20000; p.kbd1 = p.kbd2 = true; p.oscMod = p.filterMod = true; p.modAmount = 10; p.volume = 10;
        s.setParams (p);
        std::vector<float> buf (96000);
        bool ok = true; float peak = 0;
        for (int note : { 24, 60, 108, 127 })
        {
            s.noteOn (note, 1.0f); s.render (buf.data(), (int) buf.size());
            for (float v : buf) { if (! std::isfinite (v)) ok = false; peak = std::max (peak, std::fabs (v)); }
        }
        char msg[128]; std::snprintf (msg, sizeof (msg), "Everything at max: finite=%d, peak %.2f", (int) ok, peak);
        check (ok && peak <= 1.0f, msg);
    }

    std::printf (fails ? "\n%d TEST FAILED\n" : "\nALL SYNTH TESTS PASSED\n", fails);
    return fails;
}
