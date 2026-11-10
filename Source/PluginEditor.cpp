#include "PluginEditor.h"
#include "Colours.h"
#include "Presets.h"

using namespace quiete::colours;

// ================================================================= LookAndFeel
TonanteLookAndFeel::TonanteLookAndFeel()
{
    setColourScheme (getDarkColourScheme());
    setColour (juce::Slider::textBoxTextColourId, textMain);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Label::textColourId, textMain);
    setColour (juce::ComboBox::backgroundColourId, panel.darker (0.35f));
    setColour (juce::ComboBox::outlineColourId, grid);
    setColour (juce::ComboBox::textColourId, textMain);
    setColour (juce::ComboBox::arrowColourId, accent);
    setColour (juce::PopupMenu::backgroundColourId, panel.darker (0.2f));
    setColour (juce::PopupMenu::highlightedBackgroundColourId, accent.withAlpha (0.35f));
    setColour (juce::PopupMenu::textColourId, textMain);
    setColour (juce::PopupMenu::highlightedTextColourId, textMain);
    setColour (juce::TextButton::buttonColourId, panel);
    setColour (juce::ComboBox::outlineColourId, grid);
    setColour (juce::TextEditor::backgroundColourId, panel.darker (0.3f));
    setColour (juce::TextButton::textColourOffId, textMain);
}

void TonanteLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                                           float start, float end, juce::Slider& s)
{
    const auto b = juce::Rectangle<int> (x, y, w, h).toFloat().reduced (3.0f);
    const float radius = juce::jmin (b.getWidth(), b.getHeight()) * 0.5f;
    const auto c = b.getCentre();
    const float lw = juce::jmax (2.5f, radius * 0.11f);
    const float arcR = radius - lw * 0.5f;
    const float angle = start + pos * (end - start);

    // tacche per i selettori a scatti
    const bool stepped = (bool) s.getProperties().getWithDefault ("stepped", false);
    if (stepped)
    {
        const int steps = (int) std::round (s.getMaximum() - s.getMinimum());
        g.setColour (textDim);
        for (int i = 0; i <= steps; ++i)
        {
            const float a = start + (end - start) * (float) i / (float) juce::jmax (1, steps);
            const juce::Point<float> p1 = c.getPointOnCircumference (radius + 1.0f, a), p2 = c.getPointOnCircumference (radius - 3.0f, a);
            g.drawLine ({ p1, p2 }, 1.4f);
        }
    }
    else
    {
        juce::Path track;
        track.addCentredArc (c.x, c.y, arcR, arcR, 0.0f, start, end, true);
        g.setColour (grid);
        g.strokePath (track, juce::PathStrokeType (lw, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        juce::Path value;
        const bool bipolar = s.getMinimum() < 0.0 && s.getMaximum() > 0.0;
        const float from = bipolar ? (start + end) * 0.5f : start;
        value.addCentredArc (c.x, c.y, arcR, arcR, 0.0f, juce::jmin (from, angle), juce::jmax (from, angle), true);
        g.setColour (accent);
        g.strokePath (value, juce::PathStrokeType (lw, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    const float bodyR = radius - (stepped ? 6.0f : lw * 2.1f);
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.fillEllipse (c.x - bodyR, c.y - bodyR + 2.5f, bodyR * 2, bodyR * 2);
    g.setGradientFill (juce::ColourGradient (stepped ? juce::Colour (0xff3a3330) : knobLight, c.x, c.y - bodyR,
                                             stepped ? juce::Colour (0xff0d0a09) : knobDark, c.x, c.y + bodyR, false));
    g.fillEllipse (c.x - bodyR, c.y - bodyR, bodyR * 2, bodyR * 2);
    g.setColour (accent.withAlpha (0.35f));
    g.drawEllipse (c.x - bodyR + 1.0f, c.y - bodyR + 1.0f, bodyR * 2 - 2.0f, bodyR * 2 - 2.0f, 1.0f);

    juce::Path pointer;
    pointer.addRoundedRectangle (-1.4f, -bodyR + 2.5f, 2.8f, bodyR * 0.48f, 1.4f);
    g.setColour (stepped ? accent : textMain);
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (c.x, c.y));
}

void TonanteLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool highlighted, bool)
{
    auto r = b.getLocalBounds().toFloat().reduced (1.0f);
    const bool on = b.getToggleState();
    // "LED" + testo
    g.setColour (on ? accent : (highlighted ? panel.brighter (0.08f) : panel.darker (0.3f)));
    g.fillRoundedRectangle (r, 5.0f);
    g.setColour (on ? accent : grid.darker (0.1f));
    g.drawRoundedRectangle (r, 5.0f, 1.0f);
    const float led = juce::jmin (7.0f, r.getHeight() * 0.3f);
    g.setColour (on ? juce::Colour (0xfffff3e0) : grid.brighter (0.1f));
    g.fillEllipse (r.getX() + 8.0f, r.getCentreY() - led * 0.5f, led, led);
    g.setColour (on ? background : textMain);
    g.setFont (quiete::uiFont (juce::jmin (12.0f, r.getHeight() * 0.48f), true, 0.04f));
    g.drawText (b.getButtonText(), r.withTrimmedLeft (12.0f + led), juce::Justification::centred);
}

void TonanteLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h, float pos, float, float,
                                           juce::Slider::SliderStyle, juce::Slider& s)
{
    // ruote pitch / mod
    auto r = juce::Rectangle<int> (x, y, w, h).toFloat().reduced (2.0f, 0.0f);
    g.setColour (background);
    g.fillRoundedRectangle (r, 6.0f);
    g.setColour (grid.darker (0.1f));
    g.drawRoundedRectangle (r, 6.0f, 1.0f);
    const bool bipolar = s.getMinimum() < 0.0;
    if (bipolar)
    {
        g.setColour (grid.darker (0.2f));
        g.drawHorizontalLine ((int) r.getCentreY(), r.getX() + 3, r.getRight() - 3);
    }
    else
    {
        g.setColour (accent.withAlpha (0.35f));
        g.fillRoundedRectangle (r.withTop (pos).reduced (3.0f, 0.0f).withTrimmedBottom (3.0f), 4.0f);
    }
    const auto thumb = juce::Rectangle<float> (r.getX() + 2.0f, pos - 7.0f, r.getWidth() - 4.0f, 14.0f);
    g.setGradientFill (juce::ColourGradient (knobLight, 0, thumb.getY(), knobDark, 0, thumb.getBottom(), false));
    g.fillRoundedRectangle (thumb, 4.0f);
}

juce::Font TonanteLookAndFeel::getLabelFont (juce::Label& l)
{
    const auto f = l.getFont();
    return quiete::uiFont (f.getHeight(), quiete::isUiBold (f), f.getExtraKerningFactor());
}
juce::Font TonanteLookAndFeel::getComboBoxFont (juce::ComboBox& b) { return quiete::uiFont (juce::jmin (15.0f, (float) b.getHeight() * 0.5f)); }
juce::Font TonanteLookAndFeel::getPopupMenuFont() { return quiete::uiFont (15.0f); }
juce::Font TonanteLookAndFeel::getTextButtonFont (juce::TextButton&, int h) { return quiete::uiFont (juce::jmin (15.0f, (float) h * 0.5f)); }

// ================================================================= Editor
TonanteEditor::TonanteEditor (TonanteProcessor& p)
    : AudioProcessorEditor (&p), proc (p), keyboard (p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel (&lnf);

    // performance
    addKnob ("tune", "TUNE"); addKnob ("glide", "GLIDE"); addKnob ("modMix", "MOD MIX"); addKnob ("modAmount", "MOD AMOUNT");
    addSwitch ("glideOn", "GLIDE"); addSwitch ("decaySw", "DECAY");
    addSwitch ("oscMod", "OSC MOD"); addSwitch ("filterMod", "FILTER MOD");
    // oscillatori
    for (int o = 1; o <= 3; ++o)
    {
        const juce::String n (o);
        addKnob ("o" + n + "Range", "RANGE", true);
        addKnob ("o" + n + "Wave", "WAVEFORM", true);
        if (o > 1) addKnob ("o" + n + "Detune", "FREQUENCY");
    }
    addSwitch ("osc3Kbd", "KBD");
    // mixer
    addSwitch ("o1On", "OSC 1"); addKnob ("o1Vol", "");
    addSwitch ("o2On", "OSC 2"); addKnob ("o2Vol", "");
    addSwitch ("o3On", "OSC 3"); addKnob ("o3Vol", "");
    addSwitch ("noiseOn", "NOISE"); addKnob ("noiseVol", "");
    addSwitch ("fbOn", "FEEDBACK"); addKnob ("fbVol", "");
    addCombo ("noiseColor", "");
    // filtro
    addKnob ("cutoff", "CUTOFF"); addKnob ("emphasis", "EMPHASIS"); addKnob ("contour", "CONTOUR AMT");
    addSwitch ("kbd1", "KBD 1"); addSwitch ("kbd2", "KBD 2");
    addKnob ("fAttack", "ATTACK"); addKnob ("fDecay", "DECAY"); addKnob ("fSustain", "SUSTAIN");
    // ampiezza
    addKnob ("aAttack", "ATTACK"); addKnob ("aDecay", "DECAY"); addKnob ("aSustain", "SUSTAIN"); addKnob ("volume", "VOLUME");
    // impostazioni
    addKnob ("bendRange", "BEND"); addKnob ("drift", "DRIFT"); addKnob ("velocity", "VELOCITY");
    addCombo ("priority", "PRIORITY"); addCombo ("trigger", "TRIGGER");

    // formattazione valori
    for (auto& [id, k] : knobs)
    {
        auto& s = k->slider;
        if (id == "cutoff")
            s.textFromValueFunction = [] (double v) { return v >= 1000.0 ? juce::String (v / 1000.0, 2) + " kHz" : juce::String ((int) std::round (v)) + " Hz"; };
        else if (id == "tune" || id.endsWith ("Detune"))
            s.textFromValueFunction = [] (double v) { return (v > 0 ? "+" : "") + juce::String (v, 2) + " st"; };
        else if (id == "velocity")
            s.textFromValueFunction = [] (double v) { return juce::String ((int) std::round (v)) + " %"; };
        else if (id == "bendRange")
            s.textFromValueFunction = [] (double v) { return juce::String (juce::CharPointer_UTF8 ("\xc2\xb1")) + juce::String ((int) v) + " st"; };
        else if (! id.endsWith ("Range") && ! id.endsWith ("Wave"))
            s.textFromValueFunction = [] (double v) { return juce::String (v, 1); };
        s.valueFromTextFunction = [] (const juce::String& t) { return t.retainCharacters ("-0123456789.").getDoubleValue(); };
        s.updateText();
    }

    // preset
    const auto& presets = tonante::getPresets();
    juce::String lastCat;
    for (int i = 0; i < (int) presets.size(); ++i)
    {
        if (lastCat != presets[(size_t) i].category)
        {
            lastCat = presets[(size_t) i].category;
            presetBox.addSectionHeading (lastCat.toUpperCase());
        }
        presetBox.addItem (presets[(size_t) i].name, i + 1);
    }
    presetBox.onChange = [this]
    {
        const int id = presetBox.getSelectedId();
        if (id > 0 && (id - 1 != proc.getCurrentProgram() || proc.userPresetActive.load())) proc.setCurrentProgram (id - 1);
    };
    addAndMakeVisible (presetBox);
    for (auto* b : { &prevButton, &nextButton, &saveButton, &loadButton }) addAndMakeVisible (*b);
    prevButton.onClick = [this] { stepPreset (-1); };
    nextButton.onClick = [this] { stepPreset (+1); };
    saveButton.onClick = [this]
    {
        chooser = std::make_unique<juce::FileChooser> ("Save preset", TonanteProcessor::getUserPresetFolder().getChildFile ("My Sound.tpreset"), "*.tpreset");
        chooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting,
                              [this] (const juce::FileChooser& fc)
                              {
                                  auto f = fc.getResult();
                                  if (f == juce::File()) return;
                                  f = f.withFileExtension ("tpreset");
                                  if (proc.saveUserPreset (f)) { proc.userPresetName = f.getFileNameWithoutExtension(); proc.userPresetActive.store (true); }
                              });
    };
    loadButton.onClick = [this]
    {
        chooser = std::make_unique<juce::FileChooser> ("Load preset", TonanteProcessor::getUserPresetFolder(), "*.tpreset");
        chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [this] (const juce::FileChooser& fc)
                              {
                                  if (fc.getResult().existsAsFile()) proc.loadUserPreset (fc.getResult());
                                  shownProgram = -2;
                              });
    };

    // ruote e tastiera
    for (auto* w : { &pitchWheel, &modWheel })
    {
        w->setSliderStyle (juce::Slider::LinearVertical);
        w->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible (*w);
    }
    pitchWheel.setRange (-1.0, 1.0, 0.001);
    pitchWheel.setValue (0.0, juce::dontSendNotification);
    pitchWheel.setDoubleClickReturnValue (true, 0.0);
    pitchWheel.onValueChange = [this] { proc.setGuiPitchBend ((float) pitchWheel.getValue()); };
    pitchWheel.onDragEnd = [this] { pitchWheel.setValue (0.0); };         // torna al centro come una ruota a molla
    modWheel.setRange (0.0, 1.0, 0.001);
    modWheel.onValueChange = [this] { proc.setGuiModWheel ((float) modWheel.getValue()); };
    for (auto [l, t] : { std::pair<juce::Label*, const char*> { &pitchLabel, "PITCH" }, { &modLabel, "MOD" } })
    {
        l->setText (t, juce::dontSendNotification);
        l->setJustificationType (juce::Justification::centred);
        l->setFont (quiete::uiFont (10.0f, true, 0.08f));
        l->setColour (juce::Label::textColourId, textDim);
        addAndMakeVisible (*l);
    }
    keyboard.setAvailableRange (24, 96);
    keyboard.setOctaveForMiddleC (4);
    keyboard.setColour (juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour (0xfff3ebe0));
    keyboard.setColour (juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour (0xff120e0c));
    keyboard.setColour (juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colour (0xffbfae9c));
    keyboard.setColour (juce::MidiKeyboardComponent::keyDownOverlayColourId, accent.withAlpha (0.75f));
    keyboard.setColour (juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, accent.withAlpha (0.18f));
    keyboard.setColour (juce::MidiKeyboardComponent::shadowColourId, juce::Colours::transparentBlack);
    keyboard.setColour (juce::MidiKeyboardComponent::upDownButtonBackgroundColourId, panel);
    keyboard.setColour (juce::MidiKeyboardComponent::upDownButtonArrowColourId, accent);
    keyboard.setWantsKeyboardFocus (false);
    addAndMakeVisible (keyboard);

    setResizable (true, true);
    setResizeLimits (1000, 580, 2000, 1160);
    getConstrainer()->setFixedAspectRatio (1240.0 / 700.0);
    setSize (1240, 700);
    startTimerHz (15);
}

TonanteEditor::~TonanteEditor() { setLookAndFeel (nullptr); }

TonanteEditor::Knob& TonanteEditor::knob (const juce::String& id) { return *knobs.at (id); }
TonanteEditor::Switch& TonanteEditor::sw (const juce::String& id) { return *switches.at (id); }

TonanteEditor::Knob& TonanteEditor::addKnob (const juce::String& id, const juce::String& label, bool stepped)
{
    auto k = std::make_unique<Knob>();
    k->slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    k->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 16);
    k->slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    k->slider.setColour (juce::Slider::textBoxTextColourId, stepped ? textMain : textMain.withAlpha (0.85f));
    k->slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    k->slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    k->slider.getProperties().set ("stepped", stepped);
    k->label.setText (label, juce::dontSendNotification);
    k->label.setJustificationType (juce::Justification::centred);
    k->label.setFont (quiete::uiFont (10.5f, true, 0.07f));
    k->label.setColour (juce::Label::textColourId, textDim);
    addAndMakeVisible (k->slider);
    if (label.isNotEmpty()) addAndMakeVisible (k->label);
    k->att = std::make_unique<APVTS::SliderAttachment> (proc.apvts, id, k->slider);
    auto& ref = *k;
    knobs[id] = std::move (k);
    return ref;
}

TonanteEditor::Switch& TonanteEditor::addSwitch (const juce::String& id, const juce::String& text)
{
    auto s = std::make_unique<Switch>();
    s->button.setButtonText (text);
    addAndMakeVisible (s->button);
    s->att = std::make_unique<APVTS::ButtonAttachment> (proc.apvts, id, s->button);
    auto& ref = *s;
    switches[id] = std::move (s);
    return ref;
}

TonanteEditor::Combo& TonanteEditor::addCombo (const juce::String& id, const juce::String& label)
{
    auto c = std::make_unique<Combo>();
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*> (proc.apvts.getParameter (id)))
        c->box.addItemList (param->choices, 1);
    c->label.setText (label, juce::dontSendNotification);
    c->label.setFont (quiete::uiFont (10.5f, true, 0.07f));
    c->label.setColour (juce::Label::textColourId, textDim);
    c->label.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (c->box);
    if (label.isNotEmpty()) addAndMakeVisible (c->label);
    c->att = std::make_unique<APVTS::ComboBoxAttachment> (proc.apvts, id, c->box);
    auto& ref = *c;
    combos[id] = std::move (c);
    return ref;
}

void TonanteEditor::placeKnob (const juce::String& id, juce::Rectangle<int> r)
{
    auto& k = knob (id);
    if (k.label.getText().isNotEmpty()) k.label.setBounds (r.removeFromTop (16));
    k.slider.setBounds (r);
}

void TonanteEditor::timerCallback()
{
    const bool user = proc.userPresetActive.load();
    const int prog = proc.getCurrentProgram();
    if (prog != shownProgram || user != shownUser)
    {
        shownProgram = prog; shownUser = user;
        if (user) presetBox.setText ("User: " + proc.userPresetName, juce::dontSendNotification);
        else      presetBox.setSelectedId (prog + 1, juce::dontSendNotification);
    }
    if (! modWheel.isMouseButtonDown())
        modWheel.setValue (proc.getModWheel(), juce::dontSendNotification);
}

void TonanteEditor::stepPreset (int delta)
{
    const int n = proc.getNumPrograms();
    proc.setCurrentProgram ((proc.getCurrentProgram() + delta + n) % n);
}

void TonanteEditor::paint (juce::Graphics& g)
{
    g.fillAll (background);
    const float sx = (float) getWidth() / 1240.0f;

    // intestazione
    auto header = getLocalBounds().removeFromTop (juce::roundToInt (58 * sx)).reduced (juce::roundToInt (20 * sx), 0);
    g.setColour (textMain);
    g.setFont (quiete::uiFont (30.0f * sx, true, 0.1f));
    g.drawText ("TONANTE", header.removeFromLeft (juce::roundToInt (190 * sx)), juce::Justification::centredLeft);
    auto sub = header.removeFromLeft (juce::roundToInt (260 * sx)).reduced (0, juce::roundToInt (12 * sx));
    g.setColour (accent);
    g.setFont (quiete::uiFont (13.0f * sx, false, 0.04f));
    g.drawText ("three oscillator monosynth", sub.removeFromTop (sub.getHeight() / 2), juce::Justification::bottomLeft);
    g.setColour (textDim);
    g.setFont (quiete::uiFont (11.0f * sx, true, 0.12f));
    g.drawText ("BY TUNE TALKS", sub, juce::Justification::topLeft);

    // sezioni
    for (auto& s : sections)
    {
        g.setColour (panel);
        g.fillRoundedRectangle (s.area.toFloat(), 10.0f * sx);
        auto t = s.area.withHeight (juce::roundToInt (30 * sx)).reduced (juce::roundToInt (14 * sx), 0);
        g.setColour (accent);
        g.fillRoundedRectangle ((float) t.getX(), (float) t.getCentreY() - 2.0f * sx, 4.0f * sx, 4.0f * sx, 2.0f * sx);
        g.setColour (textMain);
        g.setFont (quiete::uiFont (12.0f * sx, true, 0.16f));
        g.drawText (s.title, t.withTrimmedLeft (juce::roundToInt (10 * sx)), juce::Justification::centredLeft);
    }
    // etichette di riga (OSC 1/2/3, contour...)
    g.setFont (quiete::uiFont (11.0f * sx, true, 0.1f));
    for (auto& [text, r] : rowLabels)
    {
        g.setColour (textDim);
        g.drawText (text, r, juce::Justification::centredLeft);
    }
    // linee di segnale tra le sezioni (flusso)
    g.setColour (grid.darker (0.1f));
    for (size_t i = 1; i + 1 < sections.size(); ++i)
    {
        const auto a = sections[i].area, b = sections[i + 1].area;
        if (a.getY() != b.getY()) continue;
        const float y = (float) a.getY() + 15.0f * sx;
        g.drawArrow ({ (float) a.getRight() + 1.0f, y, (float) b.getX() - 1.0f, y }, 1.2f, 5.0f * sx, 5.0f * sx);
    }
}

void TonanteEditor::resized()
{
    const float sx = (float) getWidth() / 1240.0f;
    auto S = [sx] (int v) { return juce::roundToInt ((float) v * sx); };
    auto R = [S] (int x, int y, int w, int h) { return juce::Rectangle<int> (S (x), S (y), S (w), S (h)); };
    sections.clear(); rowLabels.clear();

    // intestazione
    auto header = R (20, 12, 1200, 34);
    loadButton.setBounds (header.removeFromRight (S (64)));
    header.removeFromRight (S (6));
    saveButton.setBounds (header.removeFromRight (S (64)));
    header.removeFromRight (S (14));
    nextButton.setBounds (header.removeFromRight (S (34)));
    presetBox.setBounds (header.removeFromRight (S (250)).reduced (S (4), 0));
    prevButton.setBounds (header.removeFromRight (S (34)));

    const int top = 66, h = 392;
    // ---------------------------------------------------- PERFORMANCE
    sections.push_back ({ "CONTROLLERS", R (12, top, 196, h) });
    placeKnob ("tune",      R (24, top + 38, 82, 96));
    placeKnob ("glide",     R (114, top + 38, 82, 96));
    placeKnob ("modMix",    R (24, top + 146, 82, 96));
    placeKnob ("modAmount", R (114, top + 146, 82, 96));
    sw ("glideOn").button.setBounds   (R (24, top + 262, 82, 28));
    sw ("decaySw").button.setBounds   (R (114, top + 262, 82, 28));
    sw ("oscMod").button.setBounds    (R (24, top + 298, 172, 28));
    sw ("filterMod").button.setBounds (R (24, top + 334, 172, 28));

    // ---------------------------------------------------- OSCILLATORS
    sections.push_back ({ "OSCILLATORS", R (218, top, 372, h) });
    for (int o = 1; o <= 3; ++o)
    {
        const juce::String n (o);
        const int y = top + 34 + (o - 1) * 118;
        rowLabels.push_back ({ "OSC " + n, R (232, y + 40, 50, 20) });
        placeKnob ("o" + n + "Range", R (282, y, 86, 106));
        if (o > 1) placeKnob ("o" + n + "Detune", R (378, y, 86, 106));
        placeKnob ("o" + n + "Wave", R (474, y, 108, 106));
    }
    sw ("osc3Kbd").button.setBounds (R (228, top + 34 + 2 * 118 + 66, 52, 24));

    // ---------------------------------------------------- MIXER
    sections.push_back ({ "MIXER", R (600, top, 190, h) });
    const char* mix[] = { "o1", "o2", "o3", "noise", "fb" };
    for (int i = 0; i < 5; ++i)
    {
        const int y = top + 40 + i * 68;
        const juce::String id (mix[i]);
        sw (id + "On").button.setBounds (R (612, y + 16, 92, 28));
        knob (id + "Vol").slider.setBounds (R (712, y - 2, 70, 66));
    }
    combos.at ("noiseColor")->box.setBounds (R (612, top + 40 + 3 * 68 + 47, 92, 20));

    // ---------------------------------------------------- FILTER
    sections.push_back ({ "FILTER", R (800, top, 236, h) });
    placeKnob ("cutoff",   R (810, top + 40, 118, 150));
    placeKnob ("emphasis", R (934, top + 34, 92, 96));
    placeKnob ("contour",  R (934, top + 132, 92, 96));
    sw ("kbd1").button.setBounds (R (816, top + 236, 100, 26));
    sw ("kbd2").button.setBounds (R (922, top + 236, 100, 26));
    rowLabels.push_back ({ "FILTER CONTOUR", R (816, top + 270, 200, 18) });
    placeKnob ("fAttack",  R (808, top + 288, 74, 98));
    placeKnob ("fDecay",   R (882, top + 288, 74, 98));
    placeKnob ("fSustain", R (956, top + 288, 74, 98));

    // ---------------------------------------------------- LOUDNESS
    sections.push_back ({ "LOUDNESS", R (1046, top, 182, h) });
    placeKnob ("aAttack",  R (1056, top + 36, 80, 104));
    placeKnob ("aDecay",   R (1140, top + 36, 80, 104));
    placeKnob ("aSustain", R (1056, top + 150, 80, 104));
    placeKnob ("volume",   R (1140, top + 150, 80, 104));
    rowLabels.push_back ({ "", R (0, 0, 0, 0) });

    // ---------------------------------------------------- SETTINGS + KEYBOARD
    const int y2 = top + h + 10;
    sections.push_back ({ "SETTINGS", R (12, y2, 1216, 214) });
    placeKnob ("bendRange", R (150, y2 + 4, 76, 70));
    placeKnob ("drift",     R (236, y2 + 4, 76, 70));
    placeKnob ("velocity",  R (322, y2 + 4, 76, 70));
    for (auto [id, x] : { std::pair<const char*, int> { "priority", 420 }, { "trigger", 540 } })
    {
        auto& c = *combos.at (id);
        c.label.setBounds (R (x, y2 + 6, 110, 16));
        c.box.setBounds (R (x, y2 + 26, 110, 30));
    }

    pitchLabel.setBounds (R (22, y2 + 82, 34, 14));
    modLabel.setBounds (R (62, y2 + 82, 34, 14));
    pitchWheel.setBounds (R (24, y2 + 98, 30, 106));
    modWheel.setBounds (R (64, y2 + 98, 30, 106));
    keyboard.setBounds (R (106, y2 + 80, 1110, 124));
    keyboard.setKeyWidth ((float) keyboard.getWidth() / 42.0f);   // 6 ottave = 42 tasti bianchi
}
