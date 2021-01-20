#include <iostream>
#include "Manager.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif
#define _CRT_SECURE_NO_WARNINGS

void Manager::Init()
{
    //Set driver and resolution
    InitDrivers();

    //Initialize all event receivers and loaders
    device->setEventReceiver(&mainReceiver);
    inputReceiver.init();
    levelLoader = new LevelLoader(device);
    console = new InterfaceConsole(device, levelLoader);
    interfaceReceiver = new InterfaceEventReceiver(device, console);
    mainReceiver.AddEventReceiver(&inputReceiver);
    mainReceiver.AddEventReceiver(interfaceReceiver);

    //Start the main game loop
    RunStone();
}

int Manager::InitDrivers()
{
    //Ask the user to select a video driver
    video::E_DRIVER_TYPE driverType = DriverChoiceConsole();
    if (driverType == video::EDT_COUNT)
        return 1;

    dimension2d<u32> resolution = ResolutionChoiceConsole();

    device = createDevice(driverType, resolution, 32, false, false, false, 0);

    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    driver->setFog(video::SColor(0, 138, 125, 81), video::EFT_FOG_LINEAR, 250, 1000, .003f, true, false);
    if (device == 0)
        return 1;

    return 0;
}

void Manager::ProcessConsoleInput()
{
    if (inputReceiver.keyPressed(KEY_F1)) {
        console->ToggleConsole();
    }
    if (inputReceiver.keyPressed(KEY_ESCAPE)) {
        isDeviceRunning = false;
    }

    //Console input
    if (console->isToggled)
    {
        if (inputReceiver.keyPressed(KEY_RETURN) && console->isConsoleInit) {
            console->HandleLogEvent(console->editBox->getText(), true);
            console->editBox->setText(L"");
            console->historyIter = console->userEnteredCommand.end();
        }
        if (inputReceiver.keyPressed(KEY_UP)) {
            console->ScrollHistory(KEY_UP);
        }
        if (inputReceiver.keyPressed(KEY_DOWN)) {
            console->ScrollHistory(KEY_DOWN);
        }
    }

    //Player input
    else
    {
        if (inputReceiver.keyPressed(KEY_SPACE))
        {
            levelLoader->player->Jump();
        }
        if (inputReceiver.keyPressed(KEY_SPACE) && 
            levelLoader->player->collisionResponder->isFalling())
        {
            levelLoader->player->isJumping = true;
        }
        if (inputReceiver.keyUp(KEY_SPACE) && levelLoader->player->isJumping)
        {
            levelLoader->player->Jump();
        }
        if (inputReceiver.leftMousePressed())
        {
            levelLoader->player->FireWeapon();
        }
    }
}

// This is the new way with the update and render decoupled
int Manager::RunStone()
{
    // if no previous time set, get the current time in millis
    if (!previousTime) previousTime = device->getTimer()->getRealTime();

    levelLoader->LoadBSPLevel(L"20kdm2");

    device->getCursorControl()->setVisible(false);

    int lastFPS = -1;
    while (device->run())
    {
        isDeviceRunning = true;
        inputReceiver.endEventProcess();

        // Update Begin
        if (device->isWindowActive()) {
            ProcessConsoleInput();
            if (isDeviceRunning) {
                u32 millisPerUpdate = 2;    // you can experiment to find the best value
                if (!levelLoader->isLevelLoading) {
                    while (accumulator >= millisPerUpdate) {
                        smgr->updateAll();
                        accumulator -= millisPerUpdate;
                    }
                }
            }
        }
        // Update End

        // Render Begin
        if (device->isWindowActive()) {
            //ProcessConsoleInput();
            if (isDeviceRunning) {
                if (!levelLoader->isLevelLoading) {
                    // get the deltaTime
                    u32 currTime = device->getTimer()->getRealTime();
                    u32 deltaTime = currTime - previousTime;

                    u32 millisPerRender = 5; // you can experiment to find the best value
                    if (deltaTime > millisPerRender) {
                        driver->beginScene(true, true, SColor(0, 0, 0, 0));
                        smgr->renderAll();
                        device->getGUIEnvironment()->drawAll();
                        console->RenderConsole();
                        driver->endScene();

                        int fps = driver->getFPS();

                        if (lastFPS != fps) {
                            core::stringw str = "Stone - ";
                            str += driver->getName();
                            str += ", FPS: ";
                            str += fps;

                            device->setWindowCaption(str.c_str());
                            lastFPS = fps;
                        }
                        // add the time since render to the accumulator 
                        accumulator += deltaTime;
                        // set the previous time
                        previousTime = currTime;
                    }

                }
                else { device->yield(); }

                inputReceiver.startEventProcess();
            }
            else { break; }
        }
        // Render End
    }
    isDeviceRunning = false;
    device->drop();

    return 0;
}

// This is the old way where the update happens every frame
//int Manager::RunStone()
//{
//    // if no previous time set, get the current time in millis
//    if (!previousTime) previousTime = device->getTimer()->getRealTime();
//
//    levelLoader->LoadBSPLevel(L"20kdm2");
//
//    device->getCursorControl()->setVisible(false);
//
//    int lastFPS = -1;
//    while (device->run())
//    {
//        isDeviceRunning = true;
//        inputReceiver.endEventProcess();
//
//        // Render Begin
//        if (device->isWindowActive()) {
//            ProcessConsoleInput();
//            if (isDeviceRunning) {
//                if (!levelLoader->isLevelLoading) {
//                    // get the deltaTime
//                    u32 currTime = device->getTimer()->getRealTime();
//                    u32 deltaTime = currTime - previousTime;
//
//                    if (deltaTime > 5) {
//                        driver->beginScene(true, true, SColor(0, 0, 0, 0));
//                        smgr->drawAll();
//                        device->getGUIEnvironment()->drawAll();
//                        console->RenderConsole();
//                        driver->endScene();
//
//                        int fps = driver->getFPS();
//
//                        if (lastFPS != fps) {
//                            core::stringw str = "Stone - ";
//                            str += driver->getName();
//                            str += ", FPS: ";
//                            str += fps;
//
//                            device->setWindowCaption(str.c_str());
//                            lastFPS = fps;
//                        }
//                    }
//                }
//                else { device->yield(); }
//
//                inputReceiver.startEventProcess();
//            }
//            else { break; }
//        }
//        // Render End
//    }
//    isDeviceRunning = false;
//    device->drop();
//
//    return 0;
//}

video::E_DRIVER_TYPE Manager::DriverChoiceConsole()
{
    printf("Select a driver:\n"\
        " (a) OpenGL 1.5\n (b) Direct3D 9.0c\n" \
        " (d) Burning's Software Renderer\n (e) Software Renderer\n"\
        " (f) NullDevice\n (otherKey) exit\n\n");

    char i;
    std::cin >> i;

    video::E_DRIVER_TYPE driverType;
    switch (i)
    {
    case 'a':
        driverType = video::EDT_OPENGL;
        break;
    case 'b':
        driverType = video::EDT_DIRECT3D9;
        break;
    case 'd':
        driverType = video::EDT_BURNINGSVIDEO;
        break;
    case 'e':
        driverType = video::EDT_SOFTWARE;
        break;
    case 'f':
        driverType = video::EDT_NULL;
        break;
    default:
        driverType = video::EDT_COUNT;
    }

    return driverType;
}

dimension2d<u32> Manager::ResolutionChoiceConsole()
{
    printf("Select a resolution:\n"\
        " (a) 640x480\n (b) 800x600\n (c) 1280x720\n"\
        " (d) 1366x768\n (e) 1600x1200\n"\
        " (f) 1920x1080\n (otherKey) exit\n\n");

    char i;
    std::cin >> i;

    switch (i)
    {
    case 'a':
        return dimension2d<u32>(640, 480);
        break;
    case 'b':
        return dimension2d<u32>(800, 600);
        break;
    case 'c':
        return dimension2d<u32>(1280, 720);
        break;
    case 'd':
        return dimension2d<u32>(1366, 768);
        break;
    case 'e':
        return dimension2d<u32>(1600, 1200);
        break;
    case 'f':
        return dimension2d<u32>(1920, 1080);
        break;
    default:
        return dimension2d<u32>(640, 480);
    }
}