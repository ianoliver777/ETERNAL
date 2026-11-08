#include "PluginEditor.h"
#include <cmath>
namespace{
const juce::Colour gold(0xffc6a064),cream(0xffeee5d2);
constexpr const char* ids[]={"sustain","blend","grit","treble","output"};
constexpr const char* labels[]={"SUSTAIN","BLEND","GRIT","TREBLE","OUTPUT"};
}
EternalAudioProcessorEditor::EternalAudioProcessorEditor(EternalAudioProcessor& p):juce::AudioProcessorEditor(&p),processor(p){
 for(int i=0;i<5;++i){
  auto& s=sliders[size_t(i)];
  s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,90,20);
  s.setColour(juce::Slider::rotarySliderFillColourId,gold);
  s.setColour(juce::Slider::thumbColourId,cream);
  s.setColour(juce::Slider::textBoxTextColourId,cream);
  s.setColour(juce::Slider::textBoxBackgroundColourId,juce::Colour(0xff181818));
  s.setColour(juce::Slider::textBoxOutlineColourId,gold.withAlpha(0.3f));
  addAndMakeVisible(s);
  attachments[size_t(i)]=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters,ids[i],s);
 }
 for(auto* b:{&germanium,&bypass}){
  addAndMakeVisible(*b);
  b->setColour(juce::ToggleButton::textColourId,cream);
  b->setColour(juce::ToggleButton::tickColourId,gold);
 }
 geAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters,"germanium",germanium);
 bypassAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters,"bypass",bypass);
 const char* names[]={"STUDIO SUSTAIN","CLOUD DRUMS","GHOST SAMPLES","VELVET BASS","SMOKED TAPE","CRUSHED DRUMS"};
 for(int i=0;i<6;++i)presets.addItem(names[i],i+1);
 presets.setSelectedId(1,juce::dontSendNotification);
 presets.onChange=[this]{
  struct Preset{float sustain,blend,grit,treble,output;};
  const Preset values[]={{50,75,15,0,0},{80,85,32,-2,-2},{90,75,20,-5,-3},{68,100,12,-3,-1},{65,65,75,-6,-4},{92,95,90,1,-8}};
  int n=presets.getSelectedId()-1;if(n<0||n>=6)return;
  float v[]={values[n].sustain,values[n].blend,values[n].grit,values[n].treble,values[n].output};
  for(int i=0;i<5;++i)if(auto* param=processor.parameters.getParameter(ids[i]))
   param->setValueNotifyingHost(param->convertTo0to1(v[i]));
 };
 presets.setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xff171717));
 presets.setColour(juce::ComboBox::outlineColourId,gold);
 presets.setColour(juce::ComboBox::textColourId,cream);
 addAndMakeVisible(presets);
 setSize(1000,600);setResizable(true,true);setResizeLimits(800,480,1600,960);
 startTimerHz(24);
}
EternalAudioProcessorEditor::~EternalAudioProcessorEditor(){stopTimer();}
void EternalAudioProcessorEditor::timerCallback(){repaint();}
void EternalAudioProcessorEditor::resized(){
 auto sx=float(getWidth())/1000.0f,sy=float(getHeight())/600.0f;
 auto rect=[&](float x,float y,float w,float h){
  return juce::Rectangle<int>(int(x*sx),int(y*sy),int(w*sx),int(h*sy));
 };
 presets.setBounds(rect(345,25,300,38));
 bypass.setBounds(rect(855,30,120,30));
 for(int i=0;i<5;++i)sliders[size_t(i)].setBounds(rect(28+195*i,325,164,195));
 germanium.setBounds(rect(432,530,150,38));
}
void EternalAudioProcessorEditor::paint(juce::Graphics& g){
 const auto w=float(getWidth()),h=float(getHeight());
 g.fillAll(juce::Colour(0xff080808));
 auto sx=w/1000.0f,sy=h/600.0f;
 juce::Graphics::ScopedSaveState ss(g);
 g.addTransform(juce::AffineTransform::scale(sx,sy));
 juce::ColourGradient metal(juce::Colour(0xff2a2827),0,90,juce::Colour(0xff0d0e11),0,590,false);
 g.setGradientFill(metal);g.fillRoundedRectangle(15,82,970,500,18);
 g.setColour(gold);g.drawRoundedRectangle(15,82,970,500,18,3);
 g.setColour(gold.withAlpha(0.55f));g.drawRoundedRectangle(24,91,952,482,14,1);
 for(auto x:{40.0f,960.0f})for(auto y:{107.0f,554.0f}){
  g.setColour(juce::Colours::black);g.fillEllipse(x-8,y-8,16,16);
  g.setColour(gold.darker());g.drawEllipse(x-7,y-7,14,14,1);
  g.drawLine(x-4,y-4,x+4,y+4,1);
 }
 g.setColour(cream);
 g.setFont(juce::Font(juce::FontOptions(31,juce::Font::bold)));
 g.drawText("TRIPLE SEVEN AUDIO",66,125,425,50,juce::Justification::centredLeft);
 g.setColour(gold);
 g.setFont(juce::Font(juce::FontOptions(46,juce::Font::bold)));
 g.drawText("ETERNAL",620,119,320,58,juce::Justification::centredRight);
 g.setFont(juce::Font(juce::FontOptions(16)));
 g.drawText("OPTICAL SUSTAIN ENGINE",623,179,314,30,juce::Justification::centredRight);
 g.setColour(juce::Colours::black);
 g.fillRoundedRectangle(390,205,220,110,11);
 g.setColour(gold);g.drawRoundedRectangle(390,205,220,110,11,2);
 g.setFont(juce::Font(juce::FontOptions(16)));
 g.drawText("GAIN REDUCTION",404,215,192,23,juce::Justification::centred);
 float amount=juce::jlimit(0.0f,20.0f,processor.reduction.load(std::memory_order_relaxed));
 g.setColour(juce::Colour(0xff34261b));g.fillRoundedRectangle(412,262,176,18,4);
 g.setColour(gold);g.fillRoundedRectangle(412,262,176*(amount/20.0f),18,4);
 g.setColour(cream);g.drawText(juce::String(amount,1)+" dB",422,286,156,20,juce::Justification::centred);
 for(int i=0;i<5;++i){
  g.setColour(cream);
  g.setFont(juce::Font(juce::FontOptions(21,juce::Font::bold)));
  g.drawText(labels[i],31+i*195,501,158,26,juce::Justification::centred);
 }
 g.setColour(gold.withAlpha(0.6f));g.drawLine(125,548,417,548,1);
 g.drawLine(590,548,870,548,1);
 g.setColour(cream);g.setFont(juce::Font(juce::FontOptions(12)));
 g.drawText("ENGINEERED TO FEEL.",320,563,360,20,juce::Justification::centred);
}
