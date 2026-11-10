#pragma once
// Font incorporato nel plugin: Space Grotesk (SIL Open Font License, vedi Source/fonts/OFL.txt)
// Stesso aspetto su Mac e Windows, indipendente dai font installati.
#include <juce_graphics/juce_graphics.h>
#include "BinaryData.h"

namespace quiete
{
struct FontSet
{
    juce::Typeface::Ptr regular = juce::Typeface::createSystemTypefaceFor (BinaryData::SpaceGroteskRegular_ttf,
                                                                           (size_t) BinaryData::SpaceGroteskRegular_ttfSize);
    juce::Typeface::Ptr bold    = juce::Typeface::createSystemTypefaceFor (BinaryData::SpaceGroteskBold_ttf,
                                                                           (size_t) BinaryData::SpaceGroteskBold_ttfSize);
};

inline juce::Font uiFont (float height, bool isBold = false, float kerning = 0.0f)
{
    juce::SharedResourcePointer<FontSet> fonts; // l'editor ne tiene una copia viva, quindi qui è economico
    return juce::Font (juce::FontOptions (isBold ? fonts->bold : fonts->regular).withHeight (height).withKerningFactor (kerning));
}

inline bool isUiBold (const juce::Font& f)
{
    juce::SharedResourcePointer<FontSet> fonts;
    return f.getTypefacePtr() == fonts->bold || f.isBold();
}
} // namespace quiete
