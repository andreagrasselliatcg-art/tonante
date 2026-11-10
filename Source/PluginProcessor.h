#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "dsp/TonanteVoice.h"

class TonanteProcessor : public juce::AudioProcessor
{
public:
    TonanteProcessor();
    ~TonanteProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override  { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentProgram.load(); }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    bool saveUserPreset (const juce::File&);
    bool loadUserPreset (const juce::File&);
    static juce::File getUserPresetFolder();

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    juce::AudioProcessorValueTreeState apvts;
    juce::MidiKeyboardState keyboardState;     // tastiera a schermo

    // ruote a schermo (thread GUI → audio)
    void setGuiPitchBend (float v) noexcept { guiBend.store (v); guiBendChanged.store (true); }
    void setGuiModWheel (float v) noexcept  { guiMod.store (v);  guiModChanged.store (true); }
    float getModWheel() const noexcept { return currentModWheel.load(); }

    std::atomic<bool> userPresetActive { false };
    juce::String userPresetName;

private:
    void readParams (tonante::SynthParams&) const;
    void handleMidi (const juce::MidiMessage&);

    tonante::MonoSynth synth;
    std::atomic<int> currentProgram { 0 };
    std::atomic<float> guiBend { 0.0f }, guiMod { 0.0f }, currentModWheel { 0.0f };
    std::atomic<bool> guiBendChanged { false }, guiModChanged { false };
    std::vector<float> mono;

    struct P { std::atomic<float>* v = nullptr; };
    std::map<juce::String, std::atomic<float>*> raw;
    float get (const char* id) const { return raw.at (id)->load(); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TonanteProcessor)
};
