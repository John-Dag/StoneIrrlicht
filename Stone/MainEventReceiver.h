#ifndef MAINEVENTRECEIVER_H
#define MAINEVENTRECEIVER_H
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;
using namespace io;

class MainEventReceiver : public IEventReceiver
{
public:
    virtual bool OnEvent(const SEvent& event);
    void AddEventReceiver(IEventReceiver* receiver);
    bool RemoveEventReceiver(IEventReceiver* receiver);

private:
    core::array<IEventReceiver*> eventReceivers;
};

#endif