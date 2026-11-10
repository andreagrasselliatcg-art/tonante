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
    setColour (juce::BubbleComponent::backgroundColourId, juce::Colour (0xff1a1411));
    setColour (juce::BubbleComponent::outlineColourId, accent);
    setColour (juce::TooltipWindow::textColourId, textMain);
    setColour (juce::ComboBox::outlineColourId, grid);
    setColour (juce::TextEditor::backgroundColourId, panel.darker (0.3f));
    setColour (juce::TextButton::textColourOffId, textMain);
}

void TonanteLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                                           float start, float end, juce::Slider& s)
{
    const auto b = juce::Rectangle<int> (x, y, w, h).toFloat();
    const auto c = b.getCentre();
    const float R = juce::jmin (b.getWidth(), b.getHeight()) * 0.5f - 2.0f;
    const bool stepped = (bool) s.getProperties().getWithDefault ("stepped", false);
    const bool showScale = R >= 26.0f;
    const float knobR = showScale ? R - 14.0f : R - 2.0f;
    const float angle = start + pos * (end - start);
    const juce::Colour ink = textMain.withAlpha (0.85f);

    // ---- scala stampata
    if (showScale)
    {
        g.setFont (quiete::uiFont (juce::jlimit (7.5f, 9.5f, R * 0.22f), true));
        if (stepped)
        {
            const auto names = juce::StringArray::fromTokens (s.getProperties().getWithDefault ("labels", "").toString(), "|", "");
            const int n = juce::jmax (2, names.size());
            for (int i = 0; i < names.size(); ++i)
            {
                const float a = start + (end - start) * (float) i / (float) (n - 1);
                const bool sel = std::abs (a - angle) < 0.05f;
                g.setColour (sel ? accent : ink);
                g.drawLine ({ c.getPointOnCircumference (knobR + 2.0f, a), c.getPointOnCircumference (knobR + 5.5f, a) }, 1.3f);
                const auto p = c.getPointOnCircumference (R - 3.0f, a);
                g.drawText (names[i], juce::Rectangle<float> (34.0f, 11.0f).withCentre (p), juce::Justification::centred);
            }
        }
        else
        {
            const bool bipolar = s.getMinimum() < 0.0 && s.getMaximum() > 0.0;
            for (int i = 0; i <= 10; ++i)
            {
                const float a = start + (end - start) * (float) i / 10.0f;
                const bool major = (i % 2) == 0;
                g.setColour (ink);
                g.drawLine ({ c.getPointOnCircumference (knobR + 2.0f, a), c.getPointOnCircumference (knobR + (major ? 6.0f : 4.0f), a) }, major ? 1.3f : 0.9f);
                if (! major) continue;
                juce::String t = juce::String (i);
                if (bipolar) t = i == 0 ? "-" : (i == 10 ? "+" : (i == 5 ? "0" : ""));
                if (t.isEmpty()) continue;
                g.drawText (t, juce::Rectangle<float> (18.0f, 11.0f).withCentre (c.getPointOnCircumference (R - 3.0f, a)), juce::Justification::centred);
            }
        }
    }

    // ---- base zigrinata
    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.fillEllipse (c.x - knobR + 1.5f, c.y - knobR + 3.0f, knobR * 2, knobR * 2);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3a2e27), c.x, c.y - knobR, juce::Colour (0xff0c0908), c.x, c.y + knobR, false));
    g.fillEllipse (c.x - knobR, c.y - knobR, knobR * 2, knobR * 2);
    g.setColour (juce::Colour (0xff5a4a3f).withAlpha (0.8f));
    for (int i = 0; i < 40; ++i)
    {
        const float a = juce::MathConstants<float>::twoPi * (float) i / 40.0f;
        g.drawLine ({ c.getPointOnCircumference (knobR - 0.5f, a), c.getPointOnCircumference (knobR - 3.0f, a) }, 0.8f);
    }

    // ---- "testa di pollo" (avorio sulle manopole continue, bachelite scura sui selettori)
    const juce::Colour capHi = stepped ? juce::Colour (0xff4a3b33) : juce::Colour (0xfff4ead6);
    const juce::Colour capLo = stepped ? juce::Colour (0xff120d0b) : juce::Colour (0xffc9b48f);
    const float body = knobR * 0.62f;
    juce::Path head;
    head.addEllipse (-body, -body, body * 2, body * 2);
    juce::Path pointer;
    pointer.startNewSubPath (-body * 0.55f, -body * 0.35f);
    pointer.lineTo (-knobR * 0.16f, -knobR * 0.98f);
    pointer.quadraticTo (0.0f, -knobR * 1.04f, knobR * 0.16f, -knobR * 0.98f);
    pointer.lineTo (body * 0.55f, -body * 0.35f);
    pointer.closeSubPath();
    head.addPath (pointer);
    const auto tr = juce::AffineTransform::rotation (angle).translated (c.x, c.y);
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.fillPath (head, tr.translated (1.5f, 2.5f));
    g.setGradientFill (juce::ColourGradient (capHi, c.x - knobR * 0.5f, c.y - knobR * 0.6f, capLo, c.x + knobR * 0.5f, c.y + knobR * 0.6f, false));
    g.fillPath (head, tr);
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.strokePath (head, juce::PathStrokeType (0.8f), tr);
    // linea indice incisa
    juce::Path idx;
    idx.addRoundedRectangle (-1.1f, -knobR * 0.94f, 2.2f, knobR * 0.62f, 1.1f);
    g.setColour (stepped ? accent : juce::Colour (0xff3a2a20));
    g.fillPath (idx, tr);
    // riflesso
    g.setColour (juce::Colours::white.withAlpha (stepped ? 0.08f : 0.35f));
    g.fillEllipse (c.x - body * 0.55f, c.y - body * 0.75f, body * 0.9f, body * 0.6f);
}

void TonanteLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool highlighted, bool)
{
    // interruttore a levetta + spia
    auto r = b.getLocalBounds().toFloat().reduced (1.0f);
    const bool on = b.getToggleState();
    g.setColour (juce::Colour (0xff1a1411));
    g.fillRoundedRectangle (r, 4.0f);
    g.setColour (highlighted ? grid.brighter (0.25f) : grid);
    g.drawRoundedRectangle (r, 4.0f, 1.0f);

    const float hgt = r.getHeight();
    const auto base = juce::Point<float> (r.getX() + hgt * 0.55f, r.getCentreY());
    const float br = hgt * 0.24f;
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffd8d2c8), base.x, base.y - br, juce::Colour (0xff6d665d), base.x, base.y + br, false));
    g.fillEllipse (base.x - br, base.y - br, br * 2, br * 2);
    const auto tip = base.translated (0.0f, on ? -hgt * 0.36f : hgt * 0.36f);
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.drawLine ({ base.translated (1.0f, 1.0f), tip.translated (1.0f, 1.0f) }, hgt * 0.16f);
    g.setColour (juce::Colour (0xffe9e4dc));
    g.drawLine ({ base, tip }, hgt * 0.14f);
    g.fillEllipse (tip.x - hgt * 0.1f, tip.y - hgt * 0.1f, hgt * 0.2f, hgt * 0.2f);

    // spia
    const float lr = hgt * 0.16f;
    const auto lamp = juce::Point<float> (r.getRight() - hgt * 0.45f, r.getCentreY());
    if (on)
    {
        g.setColour (accent.withAlpha (0.35f));
        g.fillEllipse (lamp.x - lr * 2.2f, lamp.y - lr * 2.2f, lr * 4.4f, lr * 4.4f);
    }
    g.setColour (on ? juce::Colour (0xffffb15c) : juce::Colour (0xff3a2a20));
    g.fillEllipse (lamp.x - lr, lamp.y - lr, lr * 2, lr * 2);

    g.setColour (on ? textMain : textDim);
    g.setFont (quiete::uiFont (juce::jmin (11.5f, hgt * 0.44f), true, 0.08f));
    g.drawText (b.getButtonText(), r.withTrimmedLeft (hgt * 1.0f).withTrimmedRight (hgt * 0.8f), juce::Justification::centred);
}

void TonanteLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h, float pos, float, float,
                                           juce::Slider::SliderStyle, juce::Slider& s)
{
    // ruota in gomma zigrinata dentro una fessura
    auto slot = juce::Rectangle<int> (x, y, w, h).toFloat().reduced (1.0f, 0.0f);
    g.setColour (juce::Colour (0xff0b0807));
    g.fillRoundedRectangle (slot, 5.0f);
    auto wheel = slot.reduced (3.0f, 4.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff0e0b0a), 0, wheel.getY(), juce::Colour (0xff0e0b0a), 0, wheel.getBottom(), false));
    juce::ColourGradient cyl (juce::Colour (0xff0d0a09), 0, wheel.getY(), juce::Colour (0xff0d0a09), 0, wheel.getBottom(), false);
    cyl.addColour (0.5, juce::Colour (0xff3d322c));
    g.setGradientFill (cyl);
    g.fillRoundedRectangle (wheel, 4.0f);
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    const float spacing = 5.0f;
    const float offset = std::fmod (pos, spacing);
    for (float yy = wheel.getY() + offset; yy < wheel.getBottom(); yy += spacing)
        g.drawHorizontalLine ((int) yy, wheel.getX() + 1.0f, wheel.getRight() - 1.0f);
    // tacca di riferimento
    g.setColour (s.getMinimum() < 0.0 ? textMain : accent);
    g.fillRoundedRectangle (wheel.getX() + 1.0f, pos - 2.0f, wheel.getWidth() - 2.0f, 4.0f, 2.0f);
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
    addKnob ("cutoff", "CUTOFF"); addKnob ("emphasis", "RESONANCE"); addKnob ("contour", "ENV AMOUNT");
    addSwitch ("kbd1", "TRACK 1/3"); addSwitch ("kbd2", "TRACK 2/3");
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
    k->slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    k->slider.setPopupDisplayEnabled (true, true, this, 900);
    k->slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    k->slider.setColour (juce::Slider::textBoxTextColourId, stepped ? textMain : textMain.withAlpha (0.85f));
    k->slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    k->slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    k->slider.getProperties().set ("stepped", stepped);
    if (stepped)
        k->slider.getProperties().set ("labels", id.endsWith ("Range") ? "LO|32|16|8|4|2"
                                                 : (id == "o3Wave" ? "TRI|REV|SAW|SQR|WIDE|NARR" : "TRI|T-SAW|SAW|SQR|WIDE|NARR"));
    k->label.setText (label, juce::dontSendNotification);
    k->label.setJustificationType (juce::Justification::centred);
    k->label.setFont (quiete::uiFont (10.5f, true, 0.12f));
    k->label.setColour (juce::Label::textColourId, textMain.withAlpha (0.8f));
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
    const float sx = (float) getWidth() / 1240.0f;

    // fondo in metallo spazzolato scuro (texture generata una volta)
    if (texture.getWidth() != getWidth() || texture.getHeight() != getHeight())
    {
        texture = juce::Image (juce::Image::RGB, juce::jmax (1, getWidth()), juce::jmax (1, getHeight()), true);
        juce::Graphics tg (texture);
        tg.fillAll (background);
        juce::Random rnd (1234);
        for (int yy = 0; yy < getHeight(); ++yy)
        {
            tg.setColour (juce::Colour (0xff2b221d).withAlpha (rnd.nextFloat() * 0.35f));
            tg.drawHorizontalLine (yy, 0.0f, (float) getWidth());
        }
    }
    g.drawImageAt (texture, 0, 0);

    auto screw = [&] (float cx, float cy)
    {
        const float r = 4.2f * sx;
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xffb9b1a6), cx - r, cy - r, juce::Colour (0xff4a443e), cx + r, cy + r, false));
        g.fillEllipse (cx - r, cy - r, r * 2, r * 2);
        g.setColour (juce::Colour (0xff221b17));
        g.drawLine (cx - r * 0.7f, cy + r * 0.2f, cx + r * 0.7f, cy - r * 0.2f, 1.2f * sx);
    };

    // targhetta in rame con il nome
    auto plate = juce::Rectangle<float> (16.0f * sx, 10.0f * sx, 300.0f * sx, 40.0f * sx);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffe0a16a), plate.getX(), plate.getY(), juce::Colour (0xff8a4f24), plate.getRight(), plate.getBottom(), false));
    g.fillRoundedRectangle (plate, 4.0f * sx);
    g.setColour (juce::Colour (0xff4a2a14));
    g.drawRoundedRectangle (plate.reduced (2.5f * sx), 3.0f * sx, 1.0f * sx);
    screw (plate.getX() + 10.0f * sx, plate.getCentreY());
    screw (plate.getRight() - 10.0f * sx, plate.getCentreY());
    g.setFont (quiete::uiFont (24.0f * sx, true, 0.22f));
    g.setColour (juce::Colour (0x66ffffff));
    g.drawText ("TONANTE", plate.translated (0, 1.0f * sx).withTrimmedRight (110.0f * sx), juce::Justification::centred);
    g.setColour (juce::Colour (0xff2a170b));
    g.drawText ("TONANTE", plate.withTrimmedRight (110.0f * sx), juce::Justification::centred);
    auto side = plate.withTrimmedLeft (192.0f * sx).withTrimmedRight (22.0f * sx).reduced (0.0f, 7.0f * sx);
    g.setColour (juce::Colour (0xff4a2a14));
    g.drawVerticalLine ((int) side.getX() - (int) (8.0f * sx), side.getY(), side.getBottom());
    g.setColour (juce::Colour (0xff2a170b));
    g.setFont (quiete::uiFont (9.5f * sx, true, 0.16f));
    g.drawText ("MONOSYNTH", side.removeFromTop (side.getHeight() * 0.5f), juce::Justification::centredLeft);
    g.setFont (quiete::uiFont (9.0f * sx, false, 0.12f));
    g.drawText ("TUNE TALKS", side, juce::Justification::centredLeft);

    // pannelli incisi con viti
    for (auto& s : sections)
    {
        const auto a = s.area.toFloat();
        g.setColour (juce::Colours::black.withAlpha (0.5f));
        g.fillRoundedRectangle (a.translated (0, 3.0f * sx), 6.0f * sx);
        g.setGradientFill (juce::ColourGradient (panel.brighter (0.06f), a.getX(), a.getY(), panel.darker (0.12f), a.getX(), a.getBottom(), false));
        g.fillRoundedRectangle (a, 6.0f * sx);
        g.setColour (juce::Colours::white.withAlpha (0.06f));
        g.drawHorizontalLine ((int) a.getY() + 1, a.getX() + 6, a.getRight() - 6);
        g.setColour (juce::Colours::black.withAlpha (0.45f));
        g.drawRoundedRectangle (a, 6.0f * sx, 1.0f);
        const float m = 9.0f * sx;
        screw (a.getX() + m, a.getY() + m); screw (a.getRight() - m, a.getY() + m);
        screw (a.getX() + m, a.getBottom() - m); screw (a.getRight() - m, a.getBottom() - m);

        if (s.title.isEmpty()) continue;
        // titolo inciso tra due filetti
        auto t = s.area.withHeight (juce::roundToInt (30 * sx)).toFloat();
        g.setFont (quiete::uiFont (12.0f * sx, true, 0.22f));
        const float tw = g.getCurrentFont().getStringWidthFloat (s.title) + 18.0f * sx;
        g.setColour (accent.withAlpha (0.55f));
        g.drawHorizontalLine ((int) t.getCentreY(), t.getX() + 22.0f * sx, t.getCentreX() - tw * 0.5f);
        g.drawHorizontalLine ((int) t.getCentreY(), t.getCentreX() + tw * 0.5f, t.getRight() - 22.0f * sx);
        g.setColour (juce::Colours::black.withAlpha (0.6f));
        g.drawText (s.title, t.translated (0, 1.0f), juce::Justification::centred);
        g.setColour (textMain);
        g.drawText (s.title, t, juce::Justification::centred);
    }

    g.setFont (quiete::uiFont (10.5f * sx, true, 0.12f));
    for (auto& [text, r] : rowLabels)
    {
        g.setColour (juce::Colours::black.withAlpha (0.6f));
        g.drawText (text, r.translated (0, 1), juce::Justification::centredLeft);
        g.setColour (textDim);
        g.drawText (text, r, juce::Justification::centredLeft);
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
    sections.push_back ({ "PERFORMANCE", R (12, top, 196, h) });
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
    sw ("osc3Kbd").button.setBounds (R (226, top + 34 + 2 * 118 + 66, 62, 24));

    // ---------------------------------------------------- MIXER
    sections.push_back ({ "MIXER", R (600, top, 190, h) });
    const char* mix[] = { "o1", "o2", "o3", "noise", "fb" };
    for (int i = 0; i < 5; ++i)
    {
        const int y = top + 40 + i * 68;
        const juce::String id (mix[i]);
        sw (id + "On").button.setBounds (R (610, y + 16, 104, 28));
        knob (id + "Vol").slider.setBounds (R (714, y - 4, 72, 70));
    }
    combos.at ("noiseColor")->box.setBounds (R (610, top + 40 + 3 * 68 + 47, 104, 20));

    // ---------------------------------------------------- FILTER
    sections.push_back ({ "FILTER", R (800, top, 236, h) });
    placeKnob ("cutoff",   R (810, top + 40, 118, 150));
    placeKnob ("emphasis", R (934, top + 34, 92, 96));
    placeKnob ("contour",  R (934, top + 132, 92, 96));
    sw ("kbd1").button.setBounds (R (816, top + 236, 100, 26));
    sw ("kbd2").button.setBounds (R (922, top + 236, 100, 26));
    rowLabels.push_back ({ "FILTER ENVELOPE", R (816, top + 270, 200, 18) });
    placeKnob ("fAttack",  R (808, top + 288, 74, 98));
    placeKnob ("fDecay",   R (882, top + 288, 74, 98));
    placeKnob ("fSustain", R (956, top + 288, 74, 98));

    // ---------------------------------------------------- LOUDNESS
    sections.push_back ({ "AMPLIFIER", R (1046, top, 182, h) });
    placeKnob ("aAttack",  R (1056, top + 36, 80, 104));
    placeKnob ("aDecay",   R (1140, top + 36, 80, 104));
    placeKnob ("aSustain", R (1056, top + 150, 80, 104));
    placeKnob ("volume",   R (1140, top + 150, 80, 104));
    rowLabels.push_back ({ "", R (0, 0, 0, 0) });

    // ---------------------------------------------------- SETTINGS + KEYBOARD
    const int y2 = top + h + 10;
    sections.push_back ({ "", R (12, y2, 1216, 214) });
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
