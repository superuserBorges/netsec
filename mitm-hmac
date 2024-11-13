#include "veins/modules/application/traci/MITMAttackApp.h"
#include <algorithm>
#include <sstream>
#include <openssl/hmac.h>

using namespace veins;

Define_Module(MITMAttackApp);

// Define shared secret key for HMAC
const std::string sharedSecretKey = "shared_secret_key";

// Helper function: Generate HMAC
std::string generateHMAC(const std::string& message, const std::string& key) {
    unsigned char* digest;
    digest = HMAC(EVP_sha256(),
                  key.c_str(), key.length(),
                  (unsigned char*)message.c_str(), message.length(),
                  NULL, NULL);

    // Convert HMAC to hex string
    char mdString[65];
    for (int i = 0; i < 32; i++) {
        sprintf(&mdString[i * 2], "%02x", (unsigned int)digest[i]);
    }
    return std::string(mdString);
}

// Helper function: Verify HMAC
bool verifyHMAC(const std::string& message, const std::string& receivedHMAC, const std::string& key) {
    std::string computedHMAC = generateHMAC(message, key);
    return computedHMAC == receivedHMAC;
}

void MITMAttackApp::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0) {
        // Get configuration parameters
        nodeId = getParentModule()->getIndex(); // Get the vehicle's index as its ID
        isAttacker = par("isAttacker").boolValue(); // Define attacker based on node ID
        attackTime = par("attackTime").doubleValue();
        senderNode = par("senderNode").intValue();

        // Initialize status
        hasAttacked = false;
        hasReceivedWarning = false;
        hasSentMessage = false;

        if (isAttacker) {
            findHost()->getDisplayString().setTagArg("i", 1, "red");

            EV_INFO << "MITM Attack Vehicle initialized as Node " << nodeId << " at " << simTime() << endl;
        }
        else {
            // Only sender vehicle creates timer
            if (nodeId == senderNode) {
                sendMessageEvt = new cMessage("sendMessage");
                // Schedule message slightly before attack time
                scheduleAt(simTime() + (attackTime - 1), sendMessageEvt);

                // Make sender vehicle visually distinct
                findHost()->getDisplayString().setTagArg("i", 1, "blue");
                EV_INFO << "Sender vehicle " << nodeId << " initialized" << endl;
            }
        }
    }
}

void MITMAttackApp::handleSelfMsg(cMessage* msg)
{
    if (msg == sendMessageEvt && !hasSentMessage && nodeId == senderNode) {
        sendNormalMessage();
        hasSentMessage = true;
        EV_INFO << "Vehicle " << nodeId << " sent normal message at " << simTime() << endl;
    }
}

void MITMAttackApp::sendNormalMessage()
{
    std::string message = "STATUS:normal";

    // Generate HMAC for the message
    std::string hmac = generateHMAC(message, sharedSecretKey);

    // Combine message and HMAC
    std::string messageWithHMAC = message + "|" + hmac;

    // Send the message with HMAC
    TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
    populateWSM(wsm);
    wsm->setDemoData(messageWithHMAC);

    sendDown(wsm);
    EV_INFO << "Message sent with HMAC: " << hmac << endl;
}

void MITMAttackApp::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
    std::string receivedData = wsm->getDemoData();

    // Split the message and HMAC
    auto delimiter = receivedData.find("|");
    std::string message = receivedData.substr(0, delimiter);
    std::string receivedHMAC = receivedData.substr(delimiter + 1);

    // Verify the HMAC
    if (verifyHMAC(message, receivedHMAC, sharedSecretKey)) {
        EV_INFO << "Message verified successfully: " << message << endl;

        // Process the message
        if (isAttacker && !hasAttacked) {
            if (message == "STATUS:normal") {
                // Modify and forward the message
                std::string modifiedMessage = "WARNING:accident";

                // Generate new HMAC for the modified message
                std::string modifiedHMAC = generateHMAC(modifiedMessage, sharedSecretKey);
                std::string modifiedMessageWithHMAC = modifiedMessage + "|" + modifiedHMAC;

                TraCIDemo11pMessage* modifiedMsg = wsm->dup();
                modifiedMsg->setDemoData(modifiedMessageWithHMAC);
                sendDown(modifiedMsg);

                hasAttacked = true;
                EV_INFO << "Vehicle " << nodeId << " (Attacker) modified and forwarded message at " << simTime() << endl;
            }
        }
        else if (!isAttacker && !hasReceivedWarning && nodeId != senderNode) {
            if (message == "WARNING:accident") {
                hasReceivedWarning = true;

                // Visual feedback
                findHost()->getDisplayString().setTagArg("i", 1, "red");

                if (traciVehicle) {
                    traciVehicle->setSpeed(0);
                    traciVehicle->setColor(TraCIColor(255, 0, 0, 255));
                }

                EV_INFO << "Vehicle " << nodeId << " stopped due to accident warning at " << simTime() << endl;
            }
        }
    } else {
        EV_WARN << "Message verification failed! Possible tampering detected." << endl;
    }
}

void MITMAttackApp::finish()
{
    if (isAttacker) {
        EV_INFO << "Attacker vehicle " << nodeId << " attack completed at " << simTime() << endl;
    }
    else if (hasReceivedWarning) {
        EV_INFO << "Vehicle " << nodeId << " was affected by the attack" << endl;
    }

    DemoBaseApplLayer::finish();
}
