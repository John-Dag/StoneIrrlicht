#ifndef INTERFACEEVENTRECEIVER_H
#define INTERFACEEVENTRECEIVER_H

#include "irrlicht.h"
#include "InterfaceConsole.h"

class InterfaceEventReceiver : public IEventReceiver
{
public:
    struct SAppContext
    {
        IrrlichtDevice* device;
    };

    InterfaceEventReceiver(IrrlichtDevice*, InterfaceConsole*);
    virtual bool OnEvent(const SEvent&);

    IVideoDriver* driver;
    ISceneManager* smgr;
    IGUIEnvironment* guienv;
    IrrlichtDevice* device;
    InterfaceConsole* _console;
    SAppContext context;

    const c8* cs;
    wchar_t* ws;

private:
    SAppContext Context;
};

#endif