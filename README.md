# Tonante – three oscillator monosynth (Tune Talks)

Synth monofonico analogico-virtuale in formato **AU** (Logic Pro, GarageBand) e **VST3** (Ableton Live, Windows).
Architettura classica a sottrazione: 3 oscillatori → mixer → filtro ladder 24 dB → VCA, con due inviluppi.

## Installazione

Scarica dalla pagina **Releases** del repository:

- **macOS** (Apple Silicon + Intel): `Tonante-x.y.z-macOS.pkg`. Il pacchetto non è firmato da Apple: al primo avvio *Impostazioni di Sistema → Privacy e sicurezza → Apri comunque*.
- **Windows**: `Tonante-x.y.z-Windows-Setup.exe` (installa in `C:\Program Files\Common Files\VST3`) oppure lo zip del VST3. Se SmartScreen avvisa: *Ulteriori informazioni → Esegui comunque*.

Logic Pro: traccia *Strumento software* → AU Instruments → Tune Talks → Tonante.
Ableton Live: *Impostazioni → Plug-in → Use VST3 Plug-in System Folders → Rescan*, poi Plug-ins → VST3 → Tune Talks → Tonante.

## Sezioni e controlli

| Sezione | Controlli |
|---|---|
| **Controllers** | Tune (±1 semitono), Glide + interruttore, Mod Mix (Osc 3 ↔ rumore), Mod Amount (si somma alla mod wheel), Osc Mod (→ pitch Osc 1/2), Filter Mod (→ cutoff), Decay (il rilascio usa il tempo di decay) |
| **Oscillators** | Range LO / 32' / 16' / 8' / 4' / 2'; forme d'onda triangolo, tri-saw (Osc 3: dente di sega inverso), dente di sega, quadra, impulso largo, impulso stretto; Frequency ±7 semitoni su Osc 2/3 (in LO ±3 ottave); **KBD** sgancia Osc 3 dalla tastiera per usarlo come LFO |
| **Mixer** | Volume e on/off di Osc 1-3, rumore bianco/rosa, **Feedback** (anello di saturazione). Volumi alti mandano in saturazione il filtro, come sugli analogici |
| **Filter** | Cutoff, Emphasis (fino all'autoscillazione), Contour Amount, KBD 1 (1/3) e KBD 2 (2/3) di inseguimento tastiera, inviluppo Attack/Decay/Sustain |
| **Loudness** | Attack/Decay/Sustain, Volume |
| **Settings** | Bend range, Drift (instabilità analogica), Velocity, Priority (nota bassa/ultima/alta), Trigger (single = legato, multi) |

Tastiera a schermo, ruote **Pitch** (torna al centro) e **Mod**; MIDI: pitch bend, mod wheel (CC1), all notes off.

## Preset

30 preset originali divisi in Bass, Lead, Brass, Keys, Texture, FX, Drums, Init. **Save/Load** salvano i tuoi suoni in `~/Music/Tonante Presets` (`.tpreset`).

## Motore audio

Oscillatori PolyBLEP con oversampling 2x e decimatore half-band, filtro ladder ZDF a 4 poli con saturazione, rumore di fondo analogico (innesca l'autoscillazione), inviluppi a curva RC, glide esponenziale, drift lento indipendente per oscillatore. Test in `tests/test_synth.cpp` (intonazione, autoscillazione, inviluppi, glide, priorità, robustezza).

## Build da sorgente

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
JUCE 8 viene scaricato in automatico. La build su GitHub Actions crea `.pkg` per macOS e installer per Windows a ogni push su `main`.

Font: Space Grotesk (SIL Open Font License, `Source/fonts/OFL.txt`).
