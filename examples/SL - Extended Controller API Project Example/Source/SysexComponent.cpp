/*
 ==============================================================================
 
 SysexComponent.cpp
 Created: June 2024
 Author: Alessandro Petrolati
 SL - Extended Controller API
 STUDIOLOGIC/FATAR Srl
 alessandro.petrolati@studiologic-music.com
 www.studiologic-music.com
 
 ==============================================================================
 */

#include "SysexComponent.h"

const uint8_t FATAR_SYSEX_ID[3] { 0x00, 0x20, 0x1A };
const uint8_t PRODUCT_ID = 0x16;
const uint8_t HOST_ID = 0x03; // 0x01 NumaPlayer; 0x02 Camelot; 0x03 MyDevice etc.
const uint8_t DEVICE_ID = 0x00;
const String MY_DEVICE_NAME = "MY DEVICE";

SysexComponent::SysexComponent()
: audioSetupComp(audioDeviceManager, 0, 0, 0, 0, true, true, false, false)
{
    addAndMakeVisible(audioSetupComp);
    
    MessageManager::callAsync([&]() mutable
                              {
        deviceName = String();
        audioDeviceManager.addMidiInputDeviceCallback({}, this);
        audioDeviceManager.initialise(0, 0, nullptr, true, deviceName, nullptr);
    });
    
    startTimer(TIMER_MIDI_PORT, 1500);
    startTimer(TIMER_IDENTITY, 1500);
    startTimer(TIMER_MIDI_OUT, 1);
    startTimer(TIMER_MIDI_IN, 1);
}

SysexComponent::~SysexComponent()
{
    audioDeviceManager.removeMidiInputDeviceCallback({}, this);
    
    stopTimer(TIMER_MIDI_PORT);
    stopTimer(TIMER_IDENTITY);
    stopTimer(TIMER_MIDI_OUT);
    stopTimer(TIMER_MIDI_IN);
    
    // Clear MIDI output queue
    queueOut.clear();
    
    // Send logout message and flush pendin MIDI
    logoutRequest();
    
    // Flush MIDI queue directly to output, without timer
    for (auto sysMsg : queueOut)
    {
        MidiOutput* mOut = audioDeviceManager.getDefaultMidiOutput();
        if(mOut) mOut->sendMessageNow(sysMsg);
    }
}

void SysexComponent::paint(Graphics& g)
{
    g.fillAll(Colours::dimgrey);
    
    g.setColour(Colours::black);
    g.setFont(20);
    g.drawText("The IN/OUT ports are automatically selected.", 10, getHeight()-30, getWidth()-20, 20, Justification::centred);
}

void SysexComponent::resized()
{
    audioSetupComp.setBounds(0, 0, 500, 500);
}

void SysexComponent::sendIdentity()
{
    // Start with the manufacturer ID
    std::vector<uint8_t> sysexMessage {
        FATAR_SYSEX_ID[0],
        FATAR_SYSEX_ID[1],
        FATAR_SYSEX_ID[2],
        PRODUCT_ID,
        HOST_ID,
        DEVICE_ID
    };

    // tipo item System Login
    sysexMessage.push_back(ITEM_SYSTEM_SYSTEM);
    
    /* function:
     0 → login request,
     1 → login confirmation,
     2 → logout request,
     3 → logout confirmation,
     4 → standby notification (app is connected but can’t drive SL),
     5 → restart notification (app can restart its activity but first it must do a complete display refresh)
     */
    sysexMessage.push_back(SYSTEM_LOGIN_REQUEST);
    
    String text = MY_DEVICE_NAME;
    
    // caratteri della stringa
    for (char c : text)
    {
        sysexMessage.push_back(static_cast<uint8_t>(c));
    }
    
    // terminatore di stringa
    sysexMessage.push_back(0x00);
    
    // Making a sysex message, it means to prepend 0xf0 and append 0xf7
    MidiMessage sysMsg = MidiMessage::createSysExMessage(sysexMessage.data(), int(sysexMessage.size()));
    
    // Send the message to the MIDI output
    sendMidiMessage(sysMsg);
}

void SysexComponent::sendMidiMessage(const MidiMessage& sysMsg)
{
    queueOut.push_back(sysMsg);
}

void SysexComponent::sendIcon(uint8_t packet_num)
{
    // Start with the manufacturer ID
    std::vector<uint8_t> sysexMessage {
        FATAR_SYSEX_ID[0],
        FATAR_SYSEX_ID[1],
        FATAR_SYSEX_ID[2],
        PRODUCT_ID,
        HOST_ID,
        DEVICE_ID
    };

    sysexMessage.push_back(ITEM_SYSTEM_SYSTEM);
    sysexMessage.push_back(SYSTEM_SEND_ICON);
    sysexMessage.push_back(packet_num);

    // Based on the packet number we choose the starting row of the image
    const uint16_t* slice = &(Icon[32 * packet_num * 2]);
    
    for (uint16_t i = 0; i < (2*32); i++)
    {
        uint8_t msb = slice[i] >> 14;
        uint8_t mmsb = (slice[i] >> 7) & 0x7F;
        uint8_t lsb = slice[i] & 0x7F;

        sysexMessage.push_back(msb);
        sysexMessage.push_back(mmsb);
        sysexMessage.push_back(lsb);
    }

    // Making a sysex message, it means to prepend 0xf0 and append 0xf7
    MidiMessage sysMsg = MidiMessage::createSysExMessage(sysexMessage.data(), int(sysexMessage.size()));

    // Send the message to the MIDI output
    sendMidiMessage(sysMsg);
    //DBG("---- Icon " + String(packet_num) + " sent----");
}

void SysexComponent::logoutRequest()
{
    // Start with the manufacturer ID
    std::vector<uint8_t> sysexMessage {
        FATAR_SYSEX_ID[0],
        FATAR_SYSEX_ID[1],
        FATAR_SYSEX_ID[2],
        PRODUCT_ID,
        HOST_ID,
        DEVICE_ID
    };
    
    // codice per identificare un messaggio di sistema
    sysexMessage.push_back(ITEM_SYSTEM_SYSTEM);
    
    // function: logout request
    sysexMessage.push_back(SYSTEM_LOGOUT_REQUEST);
    
    // Making a sysex message, it means to prepend 0xf0 and append 0xf7
    MidiMessage sysMsg = MidiMessage::createSysExMessage(sysexMessage.data(), int(sysexMessage.size()));
    
    // Send the message to the MIDI output
    MidiOutput* mOut = audioDeviceManager.getDefaultMidiOutput();
    if (mOut)
    {
        mOut->sendMessageNow(sysMsg);
    }
}

#pragma mark -
#pragma mark CALLBACKs
void SysexComponent::handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message)
{
    // Il MIDI input, come l'output, è gestito da una coda temporizzata
    queueIn.push_back(message);
}

void SysexComponent::timerCallback(int timerID)
{
    switch (timerID)
    {
        case TIMER_IDENTITY:
        {
            sendIdentity();
        }
            break;
            
        case TIMER_MIDI_PORT:
        {
            /* Select SL default MIDI port */
            //  MIDI INPUT
            Array<MidiDeviceInfo> itemsIn = MidiInput::getAvailableDevices();
            for (auto item : itemsIn)
            {
                if (item.name == LINK_PORT_IN)
                {
                    if (!audioDeviceManager.isMidiInputDeviceEnabled(item.identifier))
                    {
                        DBG("Connect MIDI Input: " << LINK_PORT_IN);
                        audioDeviceManager.setMidiInputDeviceEnabled(item.identifier, true);
                    }
                }
            }
            
            //  MIDI OUTPUT
            Array<MidiDeviceInfo> itemsOut = MidiOutput::getAvailableDevices();
            for (auto item : itemsOut)
            {
                if (item.name == LINK_PORT_OUT)
                {
                    if (item.identifier != audioDeviceManager.getDefaultMidiOutputIdentifier())
                    {
                        DBG("Connect MIDI Output: " << LINK_PORT_OUT);
                        audioDeviceManager.setDefaultMidiOutputDevice(item.identifier);
                    }
                }
            }
        }
            break;
            
        case TIMER_MIDI_OUT:
        {
            if (queueOut.size() > 0)
            {
                MidiMessage sysMsg = queueOut.front();
                queueOut.erase(queueOut.begin());
                
                MidiOutput* mOut = audioDeviceManager.getDefaultMidiOutput();
                if (mOut) mOut->sendMessageNow(sysMsg);
            }
        }
            break;
            
        case TIMER_MIDI_IN:
        {
            if (queueIn.size() > 0)
            {
                MidiMessage sysMsg = queueIn.front();
                queueIn.erase(queueIn.begin());
                parseMidiInput(sysMsg);
            }
        }
            break;
    }
}

void SysexComponent::parseMidiInput(const MidiMessage& message)
{
    // Check if the message is a system exclusive (SysEx) message
    if (!message.isSysEx()) return;
    
    // Ensure the SysEx data size is sufficient
    if (message.getSysExDataSize() < 7) return;
    
    // Extract the SysEx data
    uint8_t fatar1 = message.getSysExData()[0];
    uint8_t fatar2 = message.getSysExData()[1];
    uint8_t fatar3 = message.getSysExData()[2];
    uint8_t prodId = message.getSysExData()[3];
    uint8_t hostId = message.getSysExData()[4];
    //uint8_t deviceId = message.getSysExData()[5];
    
    if (fatar1 != FATAR_SYSEX_ID[0] ||
        fatar2 != FATAR_SYSEX_ID[1] ||
        fatar3 != FATAR_SYSEX_ID[2] ||
        prodId != PRODUCT_ID ||
        hostId != HOST_ID)
    {
        return;
    }

    // Extract specific elements of the message
    uint8_t item_id = message.getSysExData()[6];
    uint8_t item_num = message.getSysExData()[7];
    uint8_t item_val = message.getSysExData()[8];
    
    // Handle the message elements based on their ID
    switch (item_id)
    {
        case ITEM_SYSTEM_SYSTEM:
            // Handle system messages
            handleSystemMessage(item_num, item_val);
            break;
            
        case ITEM_BUTTON:
            // Handle button messages
            handleButtonMessage(item_num, item_val);
            break;
            
        case ITEM_ENCODER:
            // Handle encoder messages
            handleEncoderMessage(item_num, item_val);
            break;
    }
}

void SysexComponent::handleSystemMessage(uint8_t item_num, uint8_t item_val)
{
    switch (item_num)
    {
        case SYSTEM_LOGIN_REQUEST:
            DBG("login request");
            break;
            
        case SYSTEM_LOGIN_CONFIRM:
            DBG("login confirmation");
            drawScreen();
            sendIcon(0);
            break;
            
        case SYSTEM_LOGOUT_REQUEST:
            DBG("logout request");
            // clean up
            break;
            
        case SYSTEM_LOGOUT_CONFIRM:
            DBG("logout confirmation");
            break;
            
        case SYSTEM_STANDBY:
            DBG("standby notification");
            break;
            
        case SYSTEM_RESTART:
            DBG("restart notification");
            drawScreen();
            break;
            
        case SYSTEM_LOGIN_RECALL:
            DBG("login recall");
            drawScreen();
            break;

        case SYSTEM_ICON_ACK:
            DBG("icon ack " + String(item_val));
            if (item_val < 15)
            {
                sendIcon(item_val + 1);
            }
            else
            {
                drawScreen();
            }
            break;

        case SYSTEM_ICON_NACK:
            DBG("icon ack " + String(item_val));
            sendIcon(item_val);
            break;
    }
}

void SysexComponent::handleButtonMessage(uint8_t item_num, uint8_t item_val)
{
    switch (item_num)
    {
        case JOYSTICK_BUTTON_MAIN:
            if (item_val == 1)
            {
                DBG("JOYSTICK PUSH");
            }
            else if (item_val == 2)
            {
                DBG("JOYSTICK LONG PUSH");
            }
            break;
            
        case APPLY_BUTTON:
            if (item_val == 1)
            {
                DBG("APPLY_BUTTON PUSH");
            }
            else if (item_val == 2)
            {
                DBG("APPLY_BUTTON LONG PUSH");
            }
            break;
            
        case ENCODER_BUTTON_1:
            if (item_val == 1)
            {
                DBG("ENCODER_BUTTON_1 PUSH");
            }
            else if (item_val == 2)
            {
                DBG("ENCODER_BUTTON_1 LONG PUSH");
            }
            break;
            
        case SELECT_ZOOM_BUTTON_1:
            if (item_val == 1)
            {
                DBG("SELECT_ZOOM_BUTTON_1 PUSH");
            }
            else if (item_val == 2)
            {
                DBG("SELECT_ZOOM_BUTTON_1 LONG PUSH");
            }
            break;
    }
}

void SysexComponent::handleEncoderMessage(uint8_t item_num, uint8_t item_val)
{
    if (item_num == ENCODER_1)
    {
        int relativeTick = item_val - 64;
        myEncoderValue += relativeTick;
        myEncoderValue = std::clamp(myEncoderValue, -6, 6);
        DBG(myEncoderValue);
        
        PLOT_TEXT(String(myEncoderValue), 10, 120, 300, TEXT_ALIGN_CENTER, TEXT_SIZE_BIG,
                  0, 0, 0,
                  11, 111, 111);

        // Maps from [-6, 6] to [0, 12] (i.e. Knob's IIDX)
        int imageIndex = (myEncoderValue + 6);

        PLOT_BITMAP(50,
                    LCD_HEIGHT-60,
                    IMG_KNOB_CENTERED,
                    imageIndex,
                    255, 0, 0,
                    0, 0, 0);
    }
    else if (item_num == JOYSTICK_ENCODER)
    {
        DBG("JOYSTICK_ENCODER");
    }
}

#pragma mark -
#pragma mark Send Sysex to SL
void SysexComponent::LED_RGB(uint8_t num, uint8_t r, uint8_t g, uint8_t b, uint8_t brigh)
{
    // Convert from 8 8 8 to 7 7 7
    r = r >> 1;
    g = g >> 1;
    b = b >> 1;
    
    // Start with the manufacturer ID
    std::vector<uint8_t> sysexMessage {
        FATAR_SYSEX_ID[0],
        FATAR_SYSEX_ID[1],
        FATAR_SYSEX_ID[2],
        PRODUCT_ID,
        HOST_ID,
        DEVICE_ID
    };
    
    // tipo item
    sysexMessage.push_back(ITEM_LED_RGB);
    
    // numero item
    sysexMessage.push_back(num);
    
    //  Colours RGB
    sysexMessage.push_back(r);
    sysexMessage.push_back(g);
    sysexMessage.push_back(b);
    
    // luminosità
    sysexMessage.push_back(brigh);
    
    // Making a sysex message, it means to prepend 0xf0 and append 0xf7
    MidiMessage sysMsg = MidiMessage::createSysExMessage(sysexMessage.data(), int(sysexMessage.size()));
    
    // Send the message to the MIDI output
    sendMidiMessage(sysMsg);
}

void SysexComponent::LED_WHITE(uint8_t num, uint8_t state)
{
    // Start with the manufacturer ID
    std::vector<uint8_t> sysexMessage {
        FATAR_SYSEX_ID[0],
        FATAR_SYSEX_ID[1],
        FATAR_SYSEX_ID[2],
        PRODUCT_ID,
        HOST_ID,
        DEVICE_ID
    };
    
    // tipo di item: led
    sysexMessage.push_back(ITEM_LED);
    
    // numero di led
    sysexMessage.push_back(num);
    
    // stato del led: 0 = spento, 1 = acceso
    sysexMessage.push_back(state);
    
    // Making a sysex message, it means to prepend 0xf0 and append 0xf7
    MidiMessage sysMsg = MidiMessage::createSysExMessage(sysexMessage.data(), int(sysexMessage.size()));
    
    // Send the message to the MIDI output
    sendMidiMessage(sysMsg);
}

void SysexComponent::PLOT_TEXT(String text,
                               uint16_t x,
                               uint16_t y,
                               uint16_t width,
                               uint8_t txIndent,
                               uint8_t fontSize,
                               uint8_t r_fore, uint8_t g_fore, uint8_t b_fore,
                               uint8_t r_back, uint8_t g_back, uint8_t b_back)
{
    // Convert from 8 8 8 to 7 7 7
    r_fore = r_fore >> 1;
    g_fore = g_fore >> 1;
    b_fore = b_fore >> 1;
    
    r_back = r_back >> 1;
    g_back = g_back >> 1;
    b_back = b_back >> 1;
    
    // Start with the manufacturer ID
    std::vector<uint8_t> sysexMessage {
        FATAR_SYSEX_ID[0],
        FATAR_SYSEX_ID[1],
        FATAR_SYSEX_ID[2],
        PRODUCT_ID,
        HOST_ID,
        DEVICE_ID
    };
    
    // tipo di item: display LCD
    sysexMessage.push_back(ITEM_LCD);
    
    // function: 0 = scrivi testo
    sysexMessage.push_back(0x00);
    
    // coordinata x LSB MSB
    MsbLsb X = get7BitMsbLsb(x);
    sysexMessage.push_back(X.msb);
    sysexMessage.push_back(X.lsb);
    
    // coordinata y LSB MSB
    MsbLsb Y = get7BitMsbLsb(y);
    sysexMessage.push_back(Y.msb);
    sysexMessage.push_back(Y.lsb);
    
    // width LSB MSB
    // 0 = segue solo la lunghezza del testo,
    // evita il drawing del background rect
    MsbLsb W = get7BitMsbLsb(width);
    sysexMessage.push_back(W.msb);
    sysexMessage.push_back(W.lsb);
    
    // allineamento: SEE text_indent enumerator
    sysexMessage.push_back(txIndent);
    
    // font size: SEE text_font enumerator
    sysexMessage.push_back(fontSize);
    
    //  Colours RGB
    sysexMessage.push_back(r_fore);
    sysexMessage.push_back(g_fore);
    sysexMessage.push_back(b_fore);
    
    sysexMessage.push_back(r_back);
    sysexMessage.push_back(g_back);
    sysexMessage.push_back(b_back);
    
    // caratteri della stringa
    for (char c : text)
    {
        sysexMessage.push_back(static_cast<uint8_t>(c));
    }
    
    // terminatore di stringa
    sysexMessage.push_back(0x00);
    
    // Making a sysex message, it means to prepend 0xf0 and append 0xf7
    MidiMessage sysMsg = MidiMessage::createSysExMessage(sysexMessage.data(), int(sysexMessage.size()));
    
    // Send the message to the MIDI output
    sendMidiMessage(sysMsg);
}

void SysexComponent::PLOT_RECT(uint16_t x,
                               uint16_t y,
                               uint16_t w,
                               uint16_t h,
                               uint8_t r, uint8_t g, uint8_t b
                               )
{
    // Convert from 8 8 8 to 7 7 7
    r = r >> 1;
    g = g >> 1;
    b = b >> 1;
    
    // Start with the manufacturer ID
    std::vector<uint8_t> sysexMessage {
        FATAR_SYSEX_ID[0],
        FATAR_SYSEX_ID[1],
        FATAR_SYSEX_ID[2],
        PRODUCT_ID,
        HOST_ID,
        DEVICE_ID
    };
    
    // tipo di item: display LCD
    sysexMessage.push_back(ITEM_LCD);
    
    //  function: 2 = disegna rettangolo
    sysexMessage.push_back(0x02);
    
    // coordinata x LSB MSB
    MsbLsb X = get7BitMsbLsb(x);
    sysexMessage.push_back(X.msb);
    sysexMessage.push_back(X.lsb);
    
    // coordinata y LSB MSB
    MsbLsb Y = get7BitMsbLsb(y);
    sysexMessage.push_back(Y.msb);
    sysexMessage.push_back(Y.lsb);
    
    // coordinata w LSB MSB
    MsbLsb W = get7BitMsbLsb(w);
    sysexMessage.push_back(W.msb);
    sysexMessage.push_back(W.lsb);
    
    // coordinata h LSB MSB
    MsbLsb H = get7BitMsbLsb(h);
    sysexMessage.push_back(H.msb);
    sysexMessage.push_back(H.lsb);
    
    // colore: RGB
    sysexMessage.push_back(r);
    sysexMessage.push_back(g);
    sysexMessage.push_back(b);
    
    // Making a sysex message, it means to prepend 0xf0 and append 0xf7
    MidiMessage sysMsg = MidiMessage::createSysExMessage(sysexMessage.data(), int(sysexMessage.size()));
    
    // Send the message to the MIDI output
    sendMidiMessage(sysMsg);
}

void SysexComponent::PLOT_BITMAP(uint16_t x,
                                 uint16_t y,
                                 uint8_t bmp_gdx,
                                 uint8_t bmp_idx,
                                 uint8_t r_fore, uint8_t g_fore, uint8_t b_fore,
                                 uint8_t r_back, uint8_t g_back, uint8_t b_back)
{
    // Convert from 8 8 8 to 7 7 7
    r_fore = r_fore >> 1;
    g_fore = g_fore >> 1;
    b_fore = b_fore >> 1;
    
    r_back = r_back >> 1;
    g_back = g_back >> 1;
    b_back = b_back >> 1;
    
    // Start with the manufacturer ID
    std::vector<uint8_t> sysexMessage {
        FATAR_SYSEX_ID[0],
        FATAR_SYSEX_ID[1],
        FATAR_SYSEX_ID[2],
        PRODUCT_ID,
        HOST_ID,
        DEVICE_ID
    };
    
    // tipo di item: display LCD
    sysexMessage.push_back(ITEM_LCD);
    
    //  function: 3 = disegna bitmap
    sysexMessage.push_back(0x03);
    
    // coordinata x LSB MSB
    MsbLsb X = get7BitMsbLsb(x);
    sysexMessage.push_back(X.msb);
    sysexMessage.push_back(X.lsb);
    
    // coordinata y LSB MSB
    MsbLsb Y = get7BitMsbLsb(y);
    sysexMessage.push_back(Y.msb);
    sysexMessage.push_back(Y.lsb);
    
    // Bitmap Group index, see Appendix A
    sysexMessage.push_back(bmp_gdx);
    
    // Bitmap Icon index, see Appendix A
    sysexMessage.push_back(bmp_idx);
    
    //  Colours RGB
    sysexMessage.push_back(r_fore);
    sysexMessage.push_back(g_fore);
    sysexMessage.push_back(b_fore);
    
    sysexMessage.push_back(r_back);
    sysexMessage.push_back(g_back);
    sysexMessage.push_back(b_back);
    
    // Making a sysex message, it means to prepend 0xf0 and append 0xf7
    MidiMessage sysMsg = MidiMessage::createSysExMessage(sysexMessage.data(), int(sysexMessage.size()));
    
    // Send the message to the MIDI output
    sendMidiMessage(sysMsg);
}

void SysexComponent::CLEAR_LCD(uint8_t r, uint8_t g, uint8_t b)
{
    // Convert from 8 8 8 to 7 7 7
    r = r >> 1;
    g = g >> 1;
    b = b >> 1;
    
    // Start with the manufacturer ID
    std::vector<uint8_t> sysexMessage {
        FATAR_SYSEX_ID[0],
        FATAR_SYSEX_ID[1],
        FATAR_SYSEX_ID[2],
        PRODUCT_ID,
        HOST_ID,
        DEVICE_ID
    };
    
    // tipo di item: display LCD
    sysexMessage.push_back(ITEM_LCD);
    
    // function: 1 = cancella tutto il display
    sysexMessage.push_back(0x01);
    
    // colore: RGB
    sysexMessage.push_back(r);
    sysexMessage.push_back(g);
    sysexMessage.push_back(b);
    
    // Making a sysex message, it means to prepend 0xf0 and append 0xf7
    MidiMessage sysMsg = MidiMessage::createSysExMessage(sysexMessage.data(), int(sysexMessage.size()));
    
    // Send the message to the MIDI output
    sendMidiMessage(sysMsg);
}

void SysexComponent::drawScreen()
{
    CLEAR_LCD();

    PLOT_RECT(0, 0, LCD_WIDTH, 100,
        155, 127, 64);
    PLOT_TEXT("Hello device, turn encoder 1", 10, 40, 300,
              TEXT_ALIGN_CENTER,
              TEXT_SIZE_MIDDLE,
              0, 0, 0,
              155, 127, 64);

    PLOT_RECT(10, 120, 300, 28,
              11, 111, 111);

    PLOT_BITMAP(277, 197, 0x7F, 0,
                0, 0, 0,
                0, 0, 0);
}
