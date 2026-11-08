#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
EternalAudioProcessor::EternalAudioProcessor():
 AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),
 parameters(*this,nullptr,"ETERNAL",makeLayout()) {}
juce::AudioProcessorValueTreeState::ParameterLayout EternalAudioProcessor::makeLayout(){
 juce::AudioProcessorValueTreeState::ParameterLayout p;
 auto range=juce::NormalisableRange<float>(0,100,0.1f);
 p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"sustain",1},"Sustain",range,50));
 p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"blend",1},"Blend",range,75));
 p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"grit",1},"Grit",range,15));
 p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"treble",1},"Treble",juce::NormalisableRange<float>(-12,12,0.1f),0));
 p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"output",1},"Output",juce::NormalisableRange<float>(-24,18,0.1f),0));
 p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"germanium",1},"Germanium",false));
 p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"bypass",1},"Bypass",false));
 return p;
}
bool EternalAudioProcessor::isBusesLayoutSupported(const BusesLayout& b) const {
 auto o=b.getMainOutputChannelSet();return (o==juce::AudioChannelSet::mono()||o==juce::AudioChannelSet::stereo())&&o==b.getMainInputChannelSet();
}
void EternalAudioProcessor::prepareToPlay(double rate,int){
 sampleRateHz=rate;envelope=0;gainCell=1;
 auto c=juce::dsp::IIR::Coefficients<float>::makeHighShelf(rate,2000,0.7071,1.0);
 for(auto& t:tone){t.reset();t.coefficients=c;}
 reduction.store(0);
}
void EternalAudioProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer& m){
 juce::ignoreUnused(m);juce::ScopedNoDenormals nd;
 const int nc=juce::jmin(2,b.getNumChannels()),n=b.getNumSamples();
 if(nc==0)return;
 if(*parameters.getRawParameterValue("bypass")>0.5f){reduction.store(0);return;}
 auto sustain=*parameters.getRawParameterValue("sustain")*0.01f;
 auto blend=*parameters.getRawParameterValue("blend")*0.01f;
 auto grit=*parameters.getRawParameterValue("grit")*0.01f;
 auto treble=*parameters.getRawParameterValue("treble");
 auto volume=juce::Decibels::decibelsToGain(*parameters.getRawParameterValue("output"));
 bool ge=*parameters.getRawParameterValue("germanium")>0.5f;
 auto c=juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRateHz,2000,0.7071,juce::Decibels::decibelsToGain(treble));
 for(auto& f:tone)f.coefficients=c;
 const float threshold=-8-34*sustain,ratio=2+6*sustain,makeup=juce::Decibels::decibelsToGain(1+12*sustain);
 const float atk=std::exp(-1.0f/(float(sampleRateHz)*0.012f));
 const float rel=std::exp(-1.0f/(float(sampleRateHz)*(0.2f+0.9f*sustain)));
 float gr=0;
 for(int i=0;i<n;++i){
  float power=0;
  for(int ch=0;ch<nc;++ch){float x=b.getSample(ch,i);power+=x*x;}
  power/=float(nc);
  float ec=power>envelope?atk:rel;envelope=power+ec*(envelope-power);
  float level=juce::Decibels::gainToDecibels(std::sqrt(juce::jmax(envelope,1e-12f)),-120.0f);
  float above=level-threshold;
  float red=above>3?(1-1/ratio)*above:above > -3?(1-1/ratio)*(above+3)*(above+3)/12:0;
  float target=juce::Decibels::decibelsToGain(-red);
  float gc=target<gainCell?atk:rel;gainCell=target+gc*(gainCell-target);
  gr=juce::jmax(gr,-juce::Decibels::gainToDecibels(gainCell,-90.0f));
  for(int ch=0;ch<nc;++ch){
   float x=b.getSample(ch,i),wet=x*gainCell*makeup;
   float drive=1.0f+grit*(ge?8.0f:12.0f);
   float clipped=ge?(std::tanh(drive*(wet+0.08f))-std::tanh(drive*0.08f)):std::tanh(drive*wet);
   wet=wet*(1-grit*0.9f)+clipped*(grit*0.9f);
   wet=tone[size_t(ch)].processSample(wet);
   b.setSample(ch,i,volume*(x*(1-blend)+wet*blend));
  }
 }
 reduction.store(juce::jmin(gr,30.0f),std::memory_order_relaxed);
}
void EternalAudioProcessor::getStateInformation(juce::MemoryBlock& b){
 if(auto xml=parameters.copyState().createXml())copyXmlToBinary(*xml,b);
}
void EternalAudioProcessor::setStateInformation(const void* data,int size){
 if(auto xml=getXmlFromBinary(data,size))if(xml->hasTagName(parameters.state.getType()))
 parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessorEditor* EternalAudioProcessor::createEditor(){return new EternalAudioProcessorEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new EternalAudioProcessor();}
