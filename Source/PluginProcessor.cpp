#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Presets.h"

namespace
{
const juce::StringArray kRanges { "LO", "32'", "16'", "8'", "4'", "2'" };
const juce::StringArray kWaves  { "Triangle", "Tri-Saw", "Sawtooth", "Square", "Wide Pulse", "Narrow Pulse" };
const juce::StringArray kWaves3 { "Triangle", "Rev Saw", "Sawtooth", "Square", "Wide Pulse", "Narrow Pulse" };

// tutti gli ID dei parametri (per preset e lettura veloce)
const char* const kAllIds[] = {
    "tune","glide","glideOn","modMix","modAmount","oscMod","filterMod","osc3Kbd","bendRange","trigger","priority","drift","velocity",
    "o1Range","o1Wave","o2Range","o2Detune","o2Wave","o3Range","o3Detune","o3Wave",
    "o1On","o1Vol","o2On","o2Vol","o3On","o3Vol","noiseOn","noiseVol","noiseColor","fbOn","fbVol",
    "cutoff","emphasis","contour","kbd1","kbd2",
    "fAttack","fDecay","fSustain","aAttack","aDecay","aSustain","decaySw","volume" };
}

juce::AudioProcessorValueTreeState::ParameterLayout TonanteProcessor::createLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout l;
    auto knob = [&] (const char* id, const char* name, float lo, float hi, float def, const char* label = "")
    {
        l.add (std::make_unique<AudioParameterFloat> (ParameterID { id, 1 }, name, NormalisableRange<float> (lo, hi, 0.01f), def,
                                                      AudioParameterFloatAttributes().withLabel (label)));
    };
    auto sw = [&] (const char* id, const char* name, bool def) { l.add (std::make_unique<AudioParameterBool> (ParameterID { id, 1 }, name, def)); };
    auto choice = [&] (const char* id, const char* name, const StringArray& c, int def)
    { l.add (std::make_unique<AudioParameterChoice> (ParameterID { id, 1 }, name, c, def)); };

    // performance
    knob ("tune", "Master Tune", -1.0f, 1.0f, 0.0f, "st");
    knob ("glide", "Glide", 0.0f, 10.0f, 2.0f);
    sw   ("glideOn", "Glide On", false);
    knob ("modMix", "Mod Mix", 0.0f, 10.0f, 0.0f);
    knob ("modAmount", "Mod Amount", 0.0f, 10.0f, 0.0f);
    sw   ("oscMod", "Oscillator Modulation", false);
    sw   ("filterMod", "Filter Modulation", false);
    sw   ("osc3Kbd", "Osc 3 Keyboard Control", true);
    l.add (std::make_unique<AudioParameterInt> (ParameterID { "bendRange", 1 }, "Bend Range", 1, 12, 2));
    choice ("trigger", "Trigger", { "Single", "Multi" }, 0);
    choice ("priority", "Note Priority", { "Low", "Last", "High" }, 0);
    knob ("drift", "Analog Drift", 0.0f, 10.0f, 2.0f);
    knob ("velocity", "Velocity", 0.0f, 100.0f, 0.0f, "%");

    // oscillatori
    choice ("o1Range", "Osc 1 Range", kRanges, 3);  choice ("o1Wave", "Osc 1 Wave", kWaves, 2);
    choice ("o2Range", "Osc 2 Range", kRanges, 3);  knob ("o2Detune", "Osc 2 Frequency", -7.0f, 7.0f, 0.0f, "st");
    choice ("o2Wave", "Osc 2 Wave", kWaves, 2);
    choice ("o3Range", "Osc 3 Range", kRanges, 3);  knob ("o3Detune", "Osc 3 Frequency", -7.0f, 7.0f, 0.0f, "st");
    choice ("o3Wave", "Osc 3 Wave", kWaves3, 2);

    // mixer
    sw ("o1On", "Osc 1 On", true);   knob ("o1Vol", "Osc 1 Volume", 0.0f, 10.0f, 8.0f);
    sw ("o2On", "Osc 2 On", false);  knob ("o2Vol", "Osc 2 Volume", 0.0f, 10.0f, 7.0f);
    sw ("o3On", "Osc 3 On", false);  knob ("o3Vol", "Osc 3 Volume", 0.0f, 10.0f, 7.0f);
    sw ("noiseOn", "Noise On", false); knob ("noiseVol", "Noise Volume", 0.0f, 10.0f, 5.0f);
    choice ("noiseColor", "Noise Color", { "White", "Pink" }, 0);
    sw ("fbOn", "Feedback On", false); knob ("fbVol", "Feedback Amount", 0.0f, 10.0f, 3.0f);

    // filtro
    NormalisableRange<float> cut (20.0f, 20000.0f, 1.0f); cut.setSkewForCentre (800.0f);
    l.add (std::make_unique<AudioParameterFloat> (ParameterID { "cutoff", 1 }, "Cutoff", cut, 1000.0f,
                                                  AudioParameterFloatAttributes().withLabel ("Hz")));
    knob ("emphasis", "Emphasis", 0.0f, 10.0f, 2.0f);
    knob ("contour", "Contour Amount", 0.0f, 10.0f, 5.0f);
    sw ("kbd1", "Keyboard Control 1", true);
    sw ("kbd2", "Keyboard Control 2", false);

    // inviluppi
    knob ("fAttack", "Filter Attack", 0.0f, 10.0f, 0.0f);
    knob ("fDecay", "Filter Decay", 0.0f, 10.0f, 4.0f);
    knob ("fSustain", "Filter Sustain", 0.0f, 10.0f, 3.0f);
    knob ("aAttack", "Loudness Attack", 0.0f, 10.0f, 0.0f);
    knob ("aDecay", "Loudness Decay", 0.0f, 10.0f, 4.0f);
    knob ("aSustain", "Loudness Sustain", 0.0f, 10.0f, 10.0f);
    sw ("decaySw", "Decay (Release)", false);
    knob ("volume", "Volume", 0.0f, 10.0f, 7.0f);
    return l;
}

TonanteProcessor::TonanteProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "TonanteState", createLayout())
{
    for (auto* id : kAllIds) raw[id] = apvts.getRawParameterValue (id);
}

bool TonanteProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

void TonanteProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.prepare (sampleRate);
    mono.assign ((size_t) juce::jmax (samplesPerBlock, 4096), 0.0f);
    keyboardState.reset();
}

void TonanteProcessor::readParams (tonante::SynthParams& p) const
{
    p.masterTune = get ("tune");
    p.glide = get ("glide");               p.glideOn = get ("glideOn") > 0.5f;
    p.modMix = get ("modMix");             p.modAmount = get ("modAmount");
    p.oscMod = get ("oscMod") > 0.5f;      p.filterMod = get ("filterMod") > 0.5f;
    p.osc3Kbd = get ("osc3Kbd") > 0.5f;    p.bendRange = get ("bendRange");
    p.singleTrigger = get ("trigger") < 0.5f;
    p.priority = (int) get ("priority");
    p.drift = get ("drift");               p.velocity = get ("velocity") * 0.01f;

    p.range[0] = (int) get ("o1Range");    p.wave[0] = (int) get ("o1Wave");
    p.range[1] = (int) get ("o2Range");    p.wave[1] = (int) get ("o2Wave");   p.detune2 = get ("o2Detune");
    p.range[2] = (int) get ("o3Range");    p.wave[2] = (int) get ("o3Wave");   p.detune3 = get ("o3Detune");

    p.oscOn[0] = get ("o1On") > 0.5f;      p.oscVol[0] = get ("o1Vol");
    p.oscOn[1] = get ("o2On") > 0.5f;      p.oscVol[1] = get ("o2Vol");
    p.oscOn[2] = get ("o3On") > 0.5f;      p.oscVol[2] = get ("o3Vol");
    p.noiseOn = get ("noiseOn") > 0.5f;    p.noiseVol = get ("noiseVol");      p.pinkNoise = get ("noiseColor") > 0.5f;
    p.feedbackOn = get ("fbOn") > 0.5f;    p.feedbackVol = get ("fbVol");

    p.cutoffHz = get ("cutoff");           p.emphasis = get ("emphasis");      p.contour = get ("contour");
    p.kbd1 = get ("kbd1") > 0.5f;          p.kbd2 = get ("kbd2") > 0.5f;

    p.fAttack = get ("fAttack");  p.fDecay = get ("fDecay");  p.fSustain = get ("fSustain");
    p.aAttack = get ("aAttack");  p.aDecay = get ("aDecay");  p.aSustain = get ("aSustain");
    p.decaySwitch = get ("decaySw") > 0.5f;
    p.volume = get ("volume");
}

void TonanteProcessor::handleMidi (const juce::MidiMessage& m)
{
    if (m.isNoteOn())                  synth.noteOn (m.getNoteNumber(), m.getFloatVelocity());
    else if (m.isNoteOff())            synth.noteOff (m.getNoteNumber());
    else if (m.isPitchWheel())         synth.setPitchBend ((float) (m.getPitchWheelValue() - 8192) / 8192.0f);
    else if (m.isControllerOfType (1))
    {
        const float v = (float) m.getControllerValue() / 127.0f;
        synth.setModWheel (v);
        currentModWheel.store (v);
    }
    else if (m.isAllNotesOff() || m.isAllSoundOff()) synth.allNotesOff();
}

void TonanteProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    buffer.clear();

    keyboardState.processNextMidiBuffer (midi, 0, numSamples, true);   // note dalla tastiera a schermo

    if (guiBendChanged.exchange (false)) synth.setPitchBend (guiBend.load());
    if (guiModChanged.exchange (false))  { synth.setModWheel (guiMod.load()); currentModWheel.store (guiMod.load()); }

    tonante::SynthParams p;
    readParams (p);
    synth.setParams (p);

    if ((int) mono.size() < numSamples) return; // non dovrebbe succedere (allocato in prepareToPlay)

    // rendering accurato al campione tra un evento MIDI e l'altro
    int pos = 0;
    for (const auto meta : midi)
    {
        const int at = juce::jlimit (0, numSamples, meta.samplePosition);
        if (at > pos) { synth.render (mono.data() + pos, at - pos); pos = at; }
        handleMidi (meta.getMessage());
    }
    if (pos < numSamples) synth.render (mono.data() + pos, numSamples - pos);

    for (int c = 0; c < buffer.getNumChannels(); ++c)
        buffer.copyFrom (c, 0, mono.data(), numSamples);
}

// ------------------------------------------------------------------ Preset
int TonanteProcessor::getNumPrograms() { return (int) tonante::getPresets().size(); }

const juce::String TonanteProcessor::getProgramName (int index)
{
    const auto& ps = tonante::getPresets();
    return juce::isPositiveAndBelow (index, (int) ps.size())
               ? juce::String (ps[(size_t) index].category) + ": " + ps[(size_t) index].name : juce::String();
}

void TonanteProcessor::setCurrentProgram (int index)
{
    const auto& ps = tonante::getPresets();
    if (! juce::isPositiveAndBelow (index, (int) ps.size())) return;

    auto set = [this] (const juce::String& id, float value)
    {
        if (auto* param = apvts.getParameter (id))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost (param->convertTo0to1 (value));
            param->endChangeGesture();
        }
    };
    for (auto* id : kAllIds)                       // si parte dall'Init
        if (auto* param = apvts.getParameter (id))
            set (id, param->convertFrom0to1 (param->getDefaultValue()));
    for (auto& [id, value] : ps[(size_t) index].values)
        set (id, value);

    currentProgram.store (index);
    userPresetActive.store (false);
}

juce::File TonanteProcessor::getUserPresetFolder()
{
    auto dir = juce::File::getSpecialLocation (juce::File::userMusicDirectory).getChildFile ("Tonante Presets");
    if (! dir.exists()) dir.createDirectory();
    return dir;
}

bool TonanteProcessor::saveUserPreset (const juce::File& f)
{
    if (auto xml = apvts.copyState().createXml()) return xml->writeTo (f);
    return false;
}

bool TonanteProcessor::loadUserPreset (const juce::File& f)
{
    if (auto xml = juce::parseXML (f))
        if (xml->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
            userPresetName = f.getFileNameWithoutExtension();
            userPresetActive.store (true);
            return true;
        }
    return false;
}

void TonanteProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = apvts.copyState();
    state.setProperty ("program", currentProgram.load(), nullptr);
    if (auto xml = state.createXml()) copyXmlToBinary (*xml, dest);
}

void TonanteProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
        {
            auto state = juce::ValueTree::fromXml (*xml);
            currentProgram.store ((int) state.getProperty ("program", 0));
            apvts.replaceState (state);
        }
}

juce::AudioProcessorEditor* TonanteProcessor::createEditor() { return new TonanteEditor (*this); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new TonanteProcessor(); }
