// ============================================================================
// Hardware Component Inspectors - Implementation
// ============================================================================

#include "hardware_inspectors.h"

namespace inspector {

// ============================================================================
// Serial Component Inspector
// ============================================================================

void registerProperties(ecs::serial_component& comp, ComponentInspector& inspector) {
    inspector.addProperty("Device Path", &comp.devicePath);
    inspector.addProperty("Baud Rate", &comp.baudRate, 300, 115200);
    inspector.addProperty("Connected", &comp.connected);
    inspector.addProperty("Auto Reconnect", &comp.autoReconnect);
    
    if (comp.autoReconnect) {
        inspector.addProperty("Reconnect Interval", &comp.reconnectInterval, 1.0f, 60.0f);
    }
    
    inspector.addCustomProperty("Connection", [&]() {
        if (!comp.connected) {
            if (ImGui::Button("Connect")) {
                if (comp.serial.setup(comp.devicePath, comp.baudRate)) {
                    comp.connected = true;
                }
            }
            
            ImGui::Text("Available Devices:");
            auto devices = comp.listDevices();
            for (const auto& device : devices) {
                if (ImGui::Selectable(device.c_str())) {
                    comp.devicePath = device;
                }
            }
        } else {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Connected");
            ImGui::SameLine();
            if (ImGui::Button("Disconnect")) {
                comp.serial.close();
                comp.connected = false;
            }
            
            ImGui::Text("Available bytes: %d", comp.available());
        }
    });
    
    inspector.addCustomProperty("Send", [&]() {
        static char sendBuffer[256] = "";
        ImGui::InputText("Message", sendBuffer, 256);
        if (ImGui::Button("Send")) {
            if (comp.connected) {
                std::string data(sendBuffer);
                comp.serial.writeBytes((unsigned char*)data.c_str(), data.size());
            }
        }
        
        ImGui::SameLine();
        static int byteToSend = 0;
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##byte", &byteToSend, 0, 0);
        ImGui::SameLine();
        if (ImGui::Button("Send Byte")) {
            if (comp.connected) {
                comp.serial.writeByte((unsigned char)byteToSend);
            }
        }
    });
}

// ============================================================================
// OSC Component Inspector
// ============================================================================

void registerProperties(ecs::osc_component& comp, ComponentInspector& inspector) {
    inspector.addProperty("Send Host", &comp.sendHost);
    inspector.addProperty("Send Port", &comp.sendPort, 1, 65535);
    inspector.addProperty("Receive Port", &comp.receivePort, 1, 65535);
    inspector.addProperty("Send Enabled", &comp.sendEnabled);
    inspector.addProperty("Receive Enabled", &comp.receiveEnabled);
    
    inspector.addCustomProperty("Setup", [&]() {
        if (ImGui::Button("Enable Sender")) {
            comp.sendEnabled = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Enable Receiver")) {
            comp.receiveEnabled = true;
        }
    });
    
    inspector.addCustomProperty("Send Message", [&]() {
        static char address[128] = "/test";
        static float floatValue = 0.0f;
        static int intValue = 0;
        static char stringValue[256] = "hello";
        
        ImGui::InputText("Address", address, 128);
        
        ImGui::DragFloat("Float", &floatValue);
        if (ImGui::Button("Send Float")) {
            ecs::osc_component::OSCMessage msg;
            msg.address = address;
            msg.type = ecs::osc_component::OSCMessage::FLOAT;
            msg.floatValue = floatValue;
            comp.messageQueue.push_back(msg);
        }
        
        ImGui::InputInt("Int", &intValue);
        if (ImGui::Button("Send Int")) {
            ecs::osc_component::OSCMessage msg;
            msg.address = address;
            msg.type = ecs::osc_component::OSCMessage::INT;
            msg.intValue = intValue;
            comp.messageQueue.push_back(msg);
        }
        
        ImGui::InputText("String", stringValue, 256);
        if (ImGui::Button("Send String")) {
            ecs::osc_component::OSCMessage msg;
            msg.address = address;
            msg.type = ecs::osc_component::OSCMessage::STRING;
            msg.stringValue = stringValue;
            comp.messageQueue.push_back(msg);
        }
    });
    
    inspector.addCustomProperty("Status", [&]() {
        if (comp.sendEnabled) {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Sending to %s:%d", 
                comp.sendHost.c_str(), comp.sendPort);
        }
        if (comp.receiveEnabled) {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Receiving on port %d", 
                comp.receivePort);
        }
    });
}

// ============================================================================
// Audio Source Component Inspector
// ============================================================================

void registerProperties(ecs::audio_source_component& comp, ComponentInspector& inspector) {
    inspector.addProperty("Enabled", &comp.enabled);
    inspector.addProperty("Device Index", &comp.deviceIndex, 0, 16);
    inspector.addProperty("Buffer Size", &comp.bufferSize, 64, 4096);
    inspector.addProperty("Sample Rate", &comp.sampleRate, 8000, 192000);
    inspector.addProperty("Input Gain", &comp.inputGain, 0.0f, 4.0f);
    inspector.addProperty("Smoothing", &comp.smoothing, 0.0f, 0.999f);
    inspector.addProperty("Peak Decay", &comp.peakDecay, 0.9f, 0.9999f);
    inspector.addProperty("FFT Bins", &comp.fftBins, 64, 2048);

    inspector.addCustomProperty("Controls", [&]() {
        if (!comp.streamActive) {
            if (ImGui::Button("Start")) comp.start();
        } else {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Active");
            ImGui::SameLine();
            if (ImGui::Button("Stop")) comp.stop();
            ImGui::SameLine();
            if (ImGui::Button("Reset")) comp.reset();
        }
    });

    inspector.addCustomProperty("Volume", [&]() {
        ImGui::ProgressBar(comp.smoothedVolume, ImVec2(-1, 0), "");
        ImGui::SameLine(0, 4);
        ImGui::Text("Peak: %.2f", comp.peakVolume);
    });

    static const char* bandNames[] = { "Sub", "Bass", "LoMid", "Mid", "HiMid", "Pres", "Brill" };
    inspector.addCustomProperty("FFT Bands", [&]() {
        for (int i = 0; i < (int)ecs::FFTBand::COUNT; i++) {
            float v = comp.smoothedBandValues[i];
            ImGui::ProgressBar(v, ImVec2(-1, 10));
            ImGui::SameLine(0, 4);
            ImGui::TextUnformatted(bandNames[i]);
        }
    });

    inspector.addCustomProperty("Beat Detectors", [&]() {
        for (int i = 0; i < (int)ecs::FFTBand::COUNT; i++) {
            auto& bd = comp.beatDetectors[i];
            ImGui::PushID(i);
            ImGui::DragFloat("Thresh", &bd.threshold, 0.01f, 0.1f, 5.0f);
            ImGui::SameLine();
            ImGui::DragFloat("Decay", &bd.decay, 0.001f, 0.5f, 0.999f);
            ImGui::SameLine();
            if (bd.triggered) ImGui::TextColored(ImVec4(1,0.6f,0,1), "* %s", bandNames[i]);
            else             ImGui::TextDisabled("  %s", bandNames[i]);
            ImGui::PopID();
        }
    });
}

// ============================================================================
// MIDI Source Component Inspector
// ============================================================================

void registerProperties(ecs::midi_source_component& comp, ComponentInspector& inspector) {
    inspector.addProperty("Enabled", &comp.enabled);
    inspector.addProperty("Device Index", &comp.deviceIndex, 0, 16);
    inspector.addProperty("Listen Channel", &comp.listenChannel, -1, 16);
    inspector.addProperty("CC Smoothing", &comp.ccSmoothing, 0.0f, 0.999f);
    inspector.addProperty("Learn Mode", &comp.learnMode);

    inspector.addCustomProperty("Controls", [&]() {
        if (!comp.streamActive) {
            if (ImGui::Button("Start")) comp.start();
        } else {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Active");
            ImGui::SameLine();
            if (ImGui::Button("Stop")) comp.stop();
            ImGui::SameLine();
            if (ImGui::Button("Reset")) comp.reset();
        }
    });

    inspector.addCustomProperty("Status", [&]() {
        ImGui::Text("Notes active: %d  Last note: %d  Velocity: %d",
            comp.activeNoteCount, comp.lastNoteOn, comp.lastVelocity);
        ImGui::ProgressBar((comp.pitchBend + 1.0f) * 0.5f, ImVec2(-1, 6), "");
        ImGui::SameLine(0, 4); ImGui::TextDisabled("PB");
        ImGui::ProgressBar(comp.smoothedModWheel, ImVec2(-1, 6), "");
        ImGui::SameLine(0, 4); ImGui::TextDisabled("MW");
        ImGui::ProgressBar(comp.smoothedExpression, ImVec2(-1, 6), "");
        ImGui::SameLine(0, 4); ImGui::TextDisabled("Expr");
    });

    inspector.addCustomProperty("CC Monitor", [&]() {
        ImGui::BeginChild("##ccmon", ImVec2(0, 120), false, ImGuiWindowFlags_HorizontalScrollbar);
        for (int i = 0; i < 128; i++) {
            if (comp.smoothedCCValues[i] > 0.001f) {
                ImGui::Text("CC%3d: %.2f", i, comp.smoothedCCValues[i]);
                ImGui::SameLine();
                ImGui::ProgressBar(comp.smoothedCCValues[i], ImVec2(80, 10), "");
            }
        }
        ImGui::EndChild();
    });

    inspector.addCustomProperty("Learn", [&]() {
        static char learnName[64] = "";
        ImGui::InputText("Target Name", learnName, 64);
        ImGui::SameLine();
        if (ImGui::Button("Start Learn")) comp.startLearn(learnName);
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) comp.cancelLearn();
        if (!comp.learnTargetName.empty())
            ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "Learning: %s", comp.learnTargetName.c_str());
    });
}

} // namespace inspector
