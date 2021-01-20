#include "InterfaceEventReceiver.h"
#include <iostream>
using namespace std;

//Handles irrlicht built-in log events and sends them to the console
bool InterfaceEventReceiver::OnEvent(const SEvent& event)
{
    if (event.EventType == EET_GUI_EVENT)
    {
        s32 id = event.GUIEvent.Caller->getID();
        guienv = Context.device->getGUIEnvironment();
    }

    if (event.EventType == EET_LOG_TEXT_EVENT)
    {
        cs = event.LogEvent.Text;
        size_t size = strlen(cs) + 1;
        ws = new wchar_t[size];
        size_t outSize;
        mbstowcs_s(&outSize, ws, size, cs, size-1);
        std::cout << "outSize: " << outSize << ", ws size: " << size << std::endl;
        ws[outSize-1] = 0;
        wstring temp(ws);
        _console->HandleLogEvent(temp, false);

        // OLD CODE
        //cs = event.LogEvent.Text;
        //ws = new wchar_t[strlen(cs) + 1];
        //int length = mbstowcs(ws, cs, strlen(cs));
        //ws[length] = 0;
        //wstring temp(ws);
        //_console->HandleLogEvent(temp, false);
    }

    return false;
}

InterfaceEventReceiver::InterfaceEventReceiver(IrrlichtDevice* dev, InterfaceConsole* console)
{
    device = dev;
    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();
    context.device = dev;
    Context = context;
    _console = console;
}