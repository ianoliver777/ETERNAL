#pragma once
#include "PluginProcessor.h"
#include <array>
class EternalAudioProcessorEditor final:public juce::AudioProcessorEditor,private juce::Timer {
public:
 explicit EternalAudioProcessorEditor(EternalAudioProcessor&);
 ~EternalAudioProcessorEditor() override;
 void paint(juce::Graphics&) override;
 void resized() override;
private:
 void timerCallback() override;
 EternalAudioProcessor& processor;
 std::array<juce::Slider,5> sliders;
 std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>,5> attachments;
 juce::ToggleButton germanium{"GE / SI"},bypass{"BYPASS"};
 std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> geAttachment,bypassAttachment;
 juce::ComboBox presets;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EternalAudioProcessorEditor)
};
