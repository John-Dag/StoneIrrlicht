#ifndef Manager_h
#define Manager_h
#define _CRT_SECURE_NO_WARNINGS

#include <irrlicht.h>

#include "InputEventReceiver.cpp"
#include "InterfaceConsole.h"
#include "InterfaceEventReceiver.h"
#include "MainEventReceiver.h"
#include "LevelLoader.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;
using namespace io;

class Manager
{
public:
    void Init(void);

    IrrlichtDevice* device;
    IVideoDriver* driver;
    ISceneManager* smgr;
    IGUIEnvironment* guienv;

    MainEventReceiver mainReceiver;
    InputEventReceiver inputReceiver;
    LevelLoader* levelLoader;
    InterfaceConsole* console;
    InterfaceEventReceiver* interfaceReceiver;

    bool isDeviceRunning = false;

private:
    int InitDrivers();
    void ProcessConsoleInput();
    int RunStone();
    video::E_DRIVER_TYPE DriverChoiceConsole();
    dimension2d<u32> ResolutionChoiceConsole();

    u32 accumulator = 10;
    u32 previousTime = 0;
};

#endif