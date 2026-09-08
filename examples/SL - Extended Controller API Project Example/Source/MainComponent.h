/*
 ==============================================================================
 
 MainComponent.h
 Created: June 2024
 Author: Alessandro Petrolati
 SL - Extended Controller API
 STUDIOLOGIC/FATAR Srl
 alessandro.petrolati@studiologic-music.com
 www.studiologic-music.com
 
 ==============================================================================
 */
#pragma once

#include <JuceHeader.h>
#include "SysexComponent.h"

class MainComponent  : public Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    //==============================================================================
    
    SysexComponent sysexPanel;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
