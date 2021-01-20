#ifndef INTERFACECONSOLE_H
#define INTERFACECONSOLE_H

#include <string>
#include <list>
#include <irrlicht.h>
#include "LevelLoader.h"

//Use these for auto scaling at different resolutions
#define SX(val) ( (int)(device->getVideoDriver()->getScreenSize().Width * (float)(val)/1024.0f) )
#define SY(val) ( (int)(device->getVideoDriver()->getScreenSize().Height * (float)(val)/768.0f) )

class InterfaceConsole
{
public:
    struct logOutput
    {
        std::wstring output;
        bool isOutputRendered = false;
        bool isUserEnteredCmd = false;
    };

    InterfaceConsole(IrrlichtDevice*, LevelLoader*);
    ~InterfaceConsole();
    void InitializeConsole(void);
    void HandleLogEvent(std::wstring, bool);
    void ToggleConsole(void);
    void RenderConsole(void);
    void ScrollHistory(EKEY_CODE);

    ILogger* logger;
    IGUIImage* consoleImage;
    IGUIEditBox* editBox;
    IGUIFont* font;
    IGUIStaticText* staticText;
    rect<s32>* rectangle;

    IrrlichtDevice* device;
    IVideoDriver* driver;
    ISceneManager* smgr;
    IGUIEnvironment* guienv;
    LevelLoader* _levelLoader;

    std::list<logOutput> consoleOutput;
    std::list<logOutput> userEnteredCommand;
    std::list<logOutput>::iterator iter;
    std::list<logOutput>::iterator historyIter;
    std::wstring consoleText;
    dimension2d<u32> screenSize;
    rect<s32> backgroundRect;

    bool isToggled = false;
    bool isTextRendered = false;
    bool hasLogChanged = false;
    bool hasUserScrolled = false;
    bool isConsoleInit = false;
    int lineHeight = 18;
    int currentLine = 0;
    int numLines = 0;
    int maxLines = 50;

private:
    void DisplayHelp(void);
    void RenderStaticText(void);
    void HandleCommand(std::wstring);
    std::wstring FormatLogEvent(std::wstring&);
};

#endif