#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"
#include "Fonts.h"

class TonanteLookAndFeel : public juce::LookAndFeel_V4
{
public:
    TonanteLookAndFeel();
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos, float start, float end, juce::Slider&) override;
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool highlighted, bool down) override;
    void drawLinearSlider (juce::Graphics&, int x, int y, int w, int h, float pos, float minPos, float maxPos,
                           juce::Slider::SliderStyle, juce::Slider&) override;
    juce::Font getLabelFont (juce::Label&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override;
    juce::Font getPopupMenuFont() override;
    juce::Font getTextButtonFont (juce::TextButton&, int) override;

private:
    juce::SharedResourcePointer<quiete::FontSet> fontSet;
};

class TonanteEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit TonanteEditor (TonanteProcessor&);
    ~TonanteEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;
    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<APVTS::SliderAttachment> att;
    };
    struct Switch
    {
        juce::ToggleButton button;
        std::unique_ptr<APVTS::ButtonAttachment> att;
    };
    struct Combo
    {
        juce::ComboBox box;
        juce::Label label;
        std::unique_ptr<APVTS::ComboBoxAttachment> att;
    };

    Knob& knob (const juce::String& id);
    Switch& sw (const juce::String& id);
    Knob& addKnob (const juce::String& id, const juce::String& label, bool stepped = false);
    Switch& addSwitch (const juce::String& id, const juce::String& text);
    Combo& addCombo (const juce::String& id, const juce::String& label);
    void placeKnob (const juce::String& id, juce::Rectangle<int> r);
    void timerCallback() override;
    void stepPreset (int delta);

    TonanteProcessor& proc;
    TonanteLookAndFeel lnf;

    std::map<juce::String, std::unique_ptr<Knob>> knobs;
    std::map<juce::String, std::unique_ptr<Switch>> switches;
    std::map<juce::String, std::unique_ptr<Combo>> combos;

    juce::ComboBox presetBox;
    juce::TextButton prevButton { "<" }, nextButton { ">" }, saveButton { "Save" }, loadButton { "Load" };
    std::unique_ptr<juce::FileChooser> chooser;
    int shownProgram = -2; bool shownUser = false;

    juce::Slider pitchWheel, modWheel;
    juce::Label pitchLabel, modLabel;
    juce::MidiKeyboardComponent keyboard;

    struct Section { juce::String title; juce::Rectangle<int> area; };
    std::vector<Section> sections;
    juce::Image texture;
    std::vector<std::pair<juce::String, juce::Rectangle<int>>> rowLabels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TonanteEditor)
};
