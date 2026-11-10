#pragma once
// Banca preset originale di Tonante (suoni in stile monosynth classico).
// Ogni preset parte dai valori di default dei parametri e cambia solo quelli elencati.
#include <initializer_list>
#include <utility>
#include <vector>

namespace tonante
{
struct Preset
{
    const char* category;
    const char* name;
    std::vector<std::pair<const char*, float>> values;
};

// Indici delle scelte:
//  range: 0 LO, 1 32', 2 16', 3 8', 4 4', 5 2'
//  wave : 0 Triangle, 1 Tri-Saw (Osc 3: Rev Saw), 2 Saw, 3 Square, 4 Wide Pulse, 5 Narrow Pulse
//  priority: 0 Low, 1 Last, 2 High     trigger: 0 Single, 1 Multi     noiseColor: 0 White, 1 Pink
inline const std::vector<Preset>& getPresets()
{
    static const std::vector<Preset> presets = {
        // ------------------------------------------------------------ BASS
        { "Bass", "Granite Bass", { {"o1Range",2},{"o1Wave",2},{"o2On",1},{"o2Range",2},{"o2Wave",3},{"o2Detune",0.08f},
              {"o3On",1},{"o3Vol",6},{"o3Range",1},{"o3Wave",2},{"cutoff",280},{"emphasis",3},{"contour",6.5f},
              {"fDecay",3.5f},{"fSustain",2},{"aDecay",4},{"aSustain",9},{"kbd1",1} } },
        { "Bass", "Rubber Floor", { {"o1Range",2},{"o1Wave",0},{"o2On",1},{"o2Range",2},{"o2Wave",2},{"o2Detune",0.06f},
              {"cutoff",180},{"emphasis",6.5f},{"contour",7},{"fDecay",3.2f},{"fSustain",0},{"aDecay",3.8f},{"aSustain",8} } },
        { "Bass", "Sub Pressure", { {"o1Range",1},{"o1Wave",0},{"o2On",1},{"o2Range",2},{"o2Wave",3},{"o2Vol",4},
              {"cutoff",400},{"emphasis",0},{"contour",2},{"fDecay",5},{"fSustain",5},{"aSustain",10} } },
        { "Bass", "Funk Pluck", { {"o1Range",2},{"o1Wave",3},{"o2On",1},{"o2Range",2},{"o2Wave",2},{"o2Detune",0.1f},
              {"cutoff",220},{"emphasis",5},{"contour",8},{"fDecay",2.6f},{"fSustain",0},{"aDecay",3.6f},{"aSustain",0},
              {"decaySw",1},{"trigger",1} } },
        { "Bass", "Driven Low End", { {"o1Range",2},{"o1Vol",10},{"o2On",1},{"o2Range",2},{"o2Vol",10},{"o2Detune",0.12f},
              {"o3On",1},{"o3Range",2},{"o3Vol",10},{"o3Detune",-0.1f},{"cutoff",350},{"emphasis",3},{"contour",5},
              {"fDecay",4},{"fSustain",3},{"fbOn",1},{"fbVol",4} } },
        { "Bass", "Glide Bass", { {"o1Range",2},{"o2On",1},{"o2Range",1},{"glideOn",1},{"glide",3.2f},{"priority",1},
              {"cutoff",300},{"emphasis",4},{"contour",6},{"fDecay",4.5f},{"fSustain",2} } },
        { "Bass", "Deep Knock", { {"o1Range",1},{"o1Wave",0},{"o2On",1},{"o2Range",2},{"o2Wave",0},{"cutoff",150},
              {"emphasis",8},{"contour",8},{"fDecay",2.4f},{"fSustain",0},{"aDecay",3.5f},{"aSustain",0},{"decaySw",1},{"trigger",1} } },
        { "Bass", "Acid Squelch", { {"o1Range",3},{"o1Wave",2},{"cutoff",250},{"emphasis",7.5f},{"contour",7},{"fDecay",3},
              {"fSustain",0},{"aSustain",9},{"glideOn",1},{"glide",1.5f},{"priority",1} } },

        // ------------------------------------------------------------ LEAD
        { "Lead", "Silver Lead", { {"o2On",1},{"o2Detune",0.1f},{"o3On",1},{"o3Range",4},{"o3Vol",5},{"cutoff",1800},
              {"emphasis",3},{"contour",4},{"fAttack",1},{"fDecay",5},{"fSustain",6},{"glideOn",1},{"glide",2.3f},{"priority",1} } },
        { "Lead", "Singing Lead", { {"o2On",1},{"o2Wave",3},{"o2Detune",0.08f},{"o3On",0},{"o3Range",0},{"o3Wave",0},
              {"o3Detune",1.2f},{"osc3Kbd",0},{"oscMod",1},{"cutoff",2400},{"emphasis",4},{"contour",3},{"fDecay",6},
              {"fSustain",7},{"glideOn",1},{"glide",2},{"priority",1} } },
        { "Lead", "Whistle", { {"o1Range",4},{"o1Wave",0},{"cutoff",3000},{"emphasis",7},{"kbd1",1},{"kbd2",1},{"contour",1},
              {"glideOn",1},{"glide",2},{"priority",1},{"oscMod",1},{"o3Range",0},{"o3Wave",0},{"osc3Kbd",0},{"o3Detune",2} } },
        { "Lead", "Screamer", { {"o1Vol",10},{"o2On",1},{"o2Vol",10},{"o2Detune",0.15f},{"o3On",1},{"o3Vol",8},{"o3Detune",-0.12f},
              {"emphasis",7},{"cutoff",1500},{"contour",5},{"fbOn",1},{"fbVol",6},{"kbd1",1},{"kbd2",1},{"priority",1} } },
        { "Lead", "Fifth Lead", { {"o2On",1},{"o2Detune",7},{"o2Vol",7},{"cutoff",1600},{"emphasis",3},{"contour",4},
              {"fDecay",5},{"fSustain",5},{"glideOn",1},{"glide",1.8f},{"priority",1} } },
        { "Lead", "Square Hero", { {"o1Wave",3},{"o2On",1},{"o2Wave",4},{"o2Detune",0.07f},{"o3On",1},{"o3Range",4},
              {"o3Wave",5},{"o3Vol",4},{"cutoff",2000},{"emphasis",2},{"contour",3},{"priority",1} } },
        { "Lead", "Hollow Pipe", { {"o1Wave",1},{"o2On",1},{"o2Wave",0},{"o2Range",4},{"o2Vol",5},{"cutoff",1400},
              {"emphasis",1},{"contour",2.5f},{"fAttack",2},{"aAttack",1.5f},{"aSustain",9},{"priority",1},{"glideOn",1},{"glide",1.2f} } },

        // ------------------------------------------------------------ BRASS & KEYS
        { "Brass", "Stab Brass", { {"o2On",1},{"o2Detune",0.14f},{"o3On",1},{"o3Detune",-0.1f},{"cutoff",500},{"emphasis",1},
              {"contour",7.5f},{"fAttack",2.2f},{"fDecay",4.8f},{"fSustain",4},{"aAttack",0.8f},{"aDecay",4},{"aSustain",8},{"decaySw",1},{"trigger",1} } },
        { "Brass", "Soft Horn", { {"o1Wave",1},{"o2On",1},{"o2Wave",1},{"o2Detune",0.09f},{"cutoff",700},{"emphasis",0},
              {"contour",5},{"fAttack",4},{"fDecay",6},{"fSustain",6},{"aAttack",3},{"aSustain",9},{"decaySw",1} } },
        { "Keys", "Clav Pluck", { {"o1Wave",5},{"o2On",1},{"o2Wave",3},{"o2Range",4},{"o2Vol",5},{"cutoff",800},{"emphasis",2},
              {"contour",6},{"fDecay",2},{"fSustain",0},{"aDecay",2.8f},{"aSustain",0},{"decaySw",1},{"trigger",1},{"velocity",60} } },
        { "Keys", "Organ Tone", { {"o1Wave",0},{"o2On",1},{"o2Wave",0},{"o2Range",4},{"o2Vol",7},{"o3On",1},{"o3Wave",0},
              {"o3Range",5},{"o3Vol",5},{"cutoff",5000},{"emphasis",0},{"contour",0},{"aSustain",10},{"trigger",1} } },

        // ------------------------------------------------------------ PAD / TEXTURE
        { "Texture", "Slow Bloom", { {"o2On",1},{"o2Detune",0.11f},{"o3On",1},{"o3Detune",-0.09f},{"o3Range",2},{"cutoff",400},
              {"emphasis",3},{"contour",6},{"fAttack",6},{"fDecay",7},{"fSustain",5},{"aAttack",5.5f},{"aDecay",7},{"aSustain",9},{"decaySw",1} } },
        { "Texture", "Wind Tunnel", { {"o1On",1},{"o1Wave",0},{"o1Vol",3},{"noiseOn",1},{"noiseVol",8},{"noiseColor",1},
              {"emphasis",8},{"cutoff",900},{"filterMod",1},{"o3On",0},{"o3Range",0},{"o3Wave",0},{"osc3Kbd",0},{"o3Detune",-3},
              {"modAmount",3},{"kbd1",1},{"kbd2",1},{"contour",2},{"aAttack",4},{"aSustain",10},{"decaySw",1},{"aDecay",6} } },

        // ------------------------------------------------------------ FX & PERCUSSION
        { "FX", "Laser Drop", { {"volume",8}, {"o1On",0},{"emphasis",10},{"cutoff",120},{"contour",10},{"fDecay",3},{"fSustain",0},
              {"aDecay",3.4f},{"aSustain",0},{"decaySw",1},{"trigger",1},{"kbd1",1},{"kbd2",1} } },
        { "FX", "Siren", { {"o1Wave",3},{"o3On",0},{"o3Range",0},{"o3Wave",0},{"osc3Kbd",0},{"o3Detune",-2},{"oscMod",1},
              {"modAmount",6},{"cutoff",3000},{"emphasis",1},{"contour",0} } },
        { "FX", "Thunder Noise", { {"volume",9}, {"o1On",0},{"noiseOn",1},{"noiseVol",10},{"cutoff",300},{"emphasis",6},{"contour",6},
              {"fDecay",6},{"fSustain",0},{"aDecay",6},{"aSustain",0},{"decaySw",1},{"trigger",1} } },
        { "Drums", "Analog Kick", { {"volume",10}, {"o1On",0},{"noiseOn",1},{"noiseVol",1},{"emphasis",10},{"cutoff",55},{"contour",8},
              {"fDecay",2.2f},{"fSustain",0},{"aDecay",3},{"aSustain",0},{"decaySw",1},{"trigger",1},{"kbd1",1},{"kbd2",1},{"velocity",70} } },
        { "Drums", "Snare Hit", { {"volume",10}, {"o1Wave",0},{"o1Vol",3},{"noiseOn",1},{"noiseVol",9},{"cutoff",2500},{"emphasis",3},{"contour",4},
              {"fDecay",2.6f},{"fSustain",0},{"aDecay",2.8f},{"aSustain",0},{"decaySw",1},{"trigger",1},{"velocity",70} } },
        { "FX", "Trill Machine", { {"o3On",0},{"o3Range",0},{"o3Wave",3},{"osc3Kbd",0},{"o3Detune",3},{"oscMod",1},{"modAmount",3.3f},
              {"cutoff",1500},{"emphasis",2},{"contour",3} } },
        { "FX", "Growl FM", { {"o2On",1},{"o2Detune",0.05f},{"o3On",0},{"o3Wave",3},{"o3Range",3},{"o3Detune",-5},{"filterMod",1},
              {"modAmount",5},{"cutoff",600},{"emphasis",5},{"contour",4},{"kbd1",1},{"priority",1} } },
        { "FX", "Noise Burble", { {"o1Wave",3},{"modMix",10},{"filterMod",1},{"modAmount",5},{"cutoff",700},{"emphasis",7},{"contour",2} } },

        // ------------------------------------------------------------ INIT
        { "Init", "Init Patch", { } },
    };
    return presets;
}
} // namespace tonante
