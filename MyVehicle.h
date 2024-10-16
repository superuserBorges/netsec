#ifndef MYVEHICLE_H
#define MYVEHICLE_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"

class MyVehicle : public BaseWaveApplLayer {
public:
    // Constructor
    MyVehicle();

    // Destructor
    virtual ~MyVehicle();

protected:
    // Initialization function
    virtual void initialize(int stage) override;

    // Function to handle self messages
    virtual void handleSelfMsg(cMessage* msg) override;

    // Function to handle messages from lower layers
    virtual void handleLowerMsg(cMessage* msg) override;
};

#endif // MYVEHICLE_H
