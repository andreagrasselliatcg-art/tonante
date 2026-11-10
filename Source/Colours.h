#pragma once
#include <juce_graphics/juce_graphics.h>

// Tema Tonante: nero caldo + marrone cioccolato + accenti rame
namespace quiete::colours
{
const juce::Colour background { 0xff141010 };   // nero caldo
const juce::Colour panel      { 0xff2a1f19 };   // marrone cioccolato
const juce::Colour grid       { 0xff4a3a30 };   // bordi / tracce
const juce::Colour accent     { 0xffd98a3d };   // rame
const juce::Colour knobLight  { 0xff8a5a3a };   // corpo manopola (alto)
const juce::Colour knobDark   { 0xff3b2519 };   // corpo manopola (basso)
const juce::Colour reduction  { 0xffff7a59 };
const juce::Colour spectrum   { 0xff9fb0c4 };
const juce::Colour textMain   { 0xfff1e6d8 };   // crema
const juce::Colour textDim    { 0xffa8927f };   // sabbia
}
