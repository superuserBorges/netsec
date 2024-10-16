#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"

class MyVehicle : public BaseWaveApplLayer {
protected:
    virtual void initialize(int stage) override {
        BaseWaveApplLayer::initialize(stage);
        if (stage == 0) {
            // Inicialização específica do veículo
        }
    }

    virtual void handleSelfMsg(cMessage* msg) override {
        // Lógica para enviar mensagens
        if (msg->isSelfMessage()) {
            auto pkt = new cPacket("Hello");
            L3Address destAddr = L3AddressResolver().resolve("node");
            sendPacket(pkt, destAddr);
        }
    }

    virtual void handleLowerMsg(cMessage* msg) override {
        // Lógica para receber mensagens
        EV << "Received message: " << msg->getName() << endl;
        delete msg;
    }
};

Define_Module(MyVehicle);
