#pragma once
#include <JuceHeader.h>
#include <atomic>
class EternalAudioProcessor final : public juce::AudioProcessor {
public:
 EternalAudioProcessor();
 void prepareToPlay(double sampleRate,int samplesPerBlock) override;
 void releaseResources() override {}
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor() const override {return true;}
 const juce::String getName() const override{return "ETERNAL";}
 bool acceptsMidi() const override{return false;}
 bool producesMidi() const override{return false;}
 bool isMidiEffect() const override{return false;}
 double getTailLengthSeconds() const override{return 0;}
 int getNumPrograms() override{return 1;}
 int getCurrentProgram() override{return 0;}
 void setCurrentProgram(int) override {}
 const juce::String getProgramName(int) override{return {};}
 void changeProgramName(int,const juce::String&) override {}
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 void getStateInformation(juce::MemoryBlock&) override;
 void setStateInformation(const void*,int) override;
 juce::AudioProcessorValueTreeState parameters;
 std::atomic<float> reduction{0};
private:
 static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
 double sampleRateHz=44100;
 float envelope=0,gainCell=1;
 std::array<juce::dsp::IIR::Filter<float>,2> tone;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EternalAudioProcessor)
};
