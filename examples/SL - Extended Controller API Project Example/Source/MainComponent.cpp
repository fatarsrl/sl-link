/*
 ==============================================================================

 MainComponent.cpp
 Created: June 2024
 Author: Alessandro Petrolati
 SL - Extended Controller API
 STUDIOLOGIC/FATAR Srl
 alessandro.petrolati@studiologic-music.com
 www.studiologic-music.com

 ==============================================================================
 */
#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{   
    setSize(500, 300);

    //MIDI PANEL
    addAndMakeVisible(sysexPanel);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint(Graphics& g)
{
    g.fillAll(Colours::grey);    
}

void MainComponent::resized()
{   
    sysexPanel.setBounds(getLocalBounds());
}
