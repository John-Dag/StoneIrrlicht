#include <iostream>
#include "InterfaceConsole.h"

using namespace irr;
using namespace std;

const SColor red(255, 255, 128, 64);

InterfaceConsole::InterfaceConsole(IrrlichtDevice* dev, LevelLoader* levelLoader)
{
    device = dev;
    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();
    _levelLoader = levelLoader;
    InitializeConsole();
}

InterfaceConsole::~InterfaceConsole()
{
    consoleOutput.clear();
    userEnteredCommand.clear();
}

void InterfaceConsole::InitializeConsole()
{
    //Set the console font
    if (guienv->getBuiltInFont())
    {
        HandleLogEvent(L"Console: Default font loaded.", false);
    }

    screenSize = driver->getScreenSize();

    //Set the background image
    try 
    {
        consoleImage = guienv->addImage(rect<s32>(SX(0), SY(0), SX(1024), SY(545)));
        consoleImage->setImage(driver->getTexture("media/Quake-Logo.jpg"));
        HandleLogEvent(L"Console: Background image loaded.", false);
    }
    catch (const exception& e)
    {
        cout << "Error: " << e.what() << endl;
    }
    
    consoleImage->setScaleImage(true);
    consoleImage->setVisible(false);
    consoleImage->setColor(SColor(0, 255, 255, 255));

    editBox = guienv->addEditBox(L"", rect<s32>(SX(0), SY(512), SX(1024), 
                                 SY(545)));
    
    editBox->setVisible(false);
    editBox->setDrawBackground(false);
    editBox->setDrawBorder(false);
    editBox->setOverrideColor(SColor(255, 255, 255, 255));
    
    font = guienv->getFont("media/myfont.xml");
    guienv->getSkin()->setFont(font);

    backgroundRect.UpperLeftCorner.X = SX(0);
    backgroundRect.UpperLeftCorner.Y = SY(-10000);
    backgroundRect.LowerRightCorner.X = SX(1024);
    backgroundRect.LowerRightCorner.Y = SY(545);

    isConsoleInit = true;
}

void InterfaceConsole::ToggleConsole()
{
    if (!consoleImage->isVisible())
    {
        consoleImage->setVisible(true);
        guienv->setFocus(editBox);
        editBox->setVisible(true);
        isToggled = true;
        device->getCursorControl()->setVisible(true);
        smgr->getActiveCamera()->setInputReceiverEnabled(false);
    }
    else
    {
        consoleImage->setVisible(false);
        guienv->removeFocus(editBox);
        editBox->setVisible(false);
        isToggled = false;
        device->getCursorControl()->setVisible(false);
        smgr->getActiveCamera()->setInputReceiverEnabled(true);
    }
}

void InterfaceConsole::RenderConsole()
{
    if (!isToggled)
        return;

    driver->draw2DRectangle(SColor(0, 204, 154, 84), backgroundRect);

    if (!isTextRendered)
    {
        currentLine = SY(480);
        staticText = guienv->addStaticText(L"", backgroundRect, false, true, consoleImage, -1, false);
        staticText->setOverrideColor(SColor(255, 255, 255, 255));
    }

    if (hasLogChanged)
    {
        RenderStaticText();
    }
}        

void InterfaceConsole::RenderStaticText()
{
    //Loop through the event receiver list and add new log events to console output
    iter = consoleOutput.begin();
    while (iter != consoleOutput.end())
    {
        if (!iter->isOutputRendered)
        {
            IGUIStaticText* tempStaticText;
            tempStaticText = guienv->addStaticText(L"", backgroundRect, false, true, consoleImage, -1, false);
            tempStaticText->setVisible(false);
            tempStaticText->setText(iter->output.c_str());

            s32 staticFontHeight = tempStaticText->getTextHeight();
            s32 numNewLines = staticFontHeight / lineHeight;
            s32 numLinesToScroll = numNewLines - 1;

            currentLine = (currentLine - (lineHeight  * numLinesToScroll));
            staticText->setRelativePosition(position2d<s32>(0, currentLine));
            consoleText.append(iter->output);
            staticText->setText(consoleText.c_str());
            iter->isOutputRendered = true;

            //Remove rendered log events from the receiver list 
            //consoleOutput.erase(iter);
            numLines++;
        }

        iter++;
    }

    isTextRendered = true;
    hasLogChanged = false;

    //Remove old log events for sake of video memory
    if (numLines > maxLines)
    {
        int temp = consoleText.find_first_of(L"\0");
        consoleText.erase(0, temp + 1);
    }
}

void InterfaceConsole::HandleLogEvent(wstring text, bool isUserEnteredCmd)
{
    if (text.empty())
        return;
    
    wstring temp = FormatLogEvent(text);
    logOutput log;
    log.output = L"\n[]" + temp + L"\0";
    log.isOutputRendered = false;

    //The latest log event a user entered command
    if (isUserEnteredCmd)
        userEnteredCommand.push_back(log);

    consoleOutput.push_back(log);
    
    //Do something with a user entered command
    HandleCommand(text);
    hasLogChanged = true;
}

void InterfaceConsole::HandleCommand(wstring text)
{
    if (text.empty())
        return;

    if (text.find(L"help") == 0)
    {
        DisplayHelp();
    }

    if (text.find(L"load") == 0)
    {
        if (text.length() < 5)
            return;
        wstring temp = text.substr(5, text.length());
        FormatLogEvent(temp);
        //temp.insert(0, L"media/");
        wcout << temp << endl;

        _levelLoader->LoadBSPLevel(temp);
    }

    if (text.find(L"collision") == 0)
    {
        if (_levelLoader->player->collisionResponder->isEnabled())
        {
            _levelLoader->player->collisionResponder->setEnabled(false);
            HandleLogEvent(L"Collision disabled", false);
        }
        else
        {
            _levelLoader->player->collisionResponder->setEnabled(true);
            HandleLogEvent(L"Collision enabled", false);
        }
    }

    if (text.find(L"gravity") == 0)
    {
        if (_levelLoader->player->collisionResponder->getGravity().equals(vector3df(0, -1000, 0)))
        {
            _levelLoader->player->collisionResponder->setGravity(vector3df(0, 0, 0));
            HandleLogEvent(L"Gravity disabled", false);
        }
        else
        {
            _levelLoader->player->collisionResponder->setGravity(vector3df(0, -1000, 0));
            HandleLogEvent(L"Gravity enabled", false);
        }
    }
}

//Formats all log events 
std::wstring InterfaceConsole::FormatLogEvent(wstring& text)
{
    if (text.empty())
        return L"";

    const std::wstring& whitespace = L" \t";
    std::wstring::size_type strBegin = text.find_first_not_of(whitespace);
    std::wstring::size_type strEnd = text.find_last_not_of(whitespace);

    if (strBegin != std::wstring::npos || strEnd != std::wstring::npos)
    {
        strBegin == std::wstring::npos ? 0 : strBegin;
        strEnd == std::wstring::npos ? text.size() : 0;

        const auto strRange = strEnd - strBegin + 1;
        text.substr(strBegin, strRange).swap(text);
    }
    else if (text[0] == ' ' || text[0] == '\t')
    {
        text = L"";
    }

    return text;
}

//Allows the user to access previously entered commands with arrow keys
void InterfaceConsole::ScrollHistory(EKEY_CODE code)
{
    if (!hasUserScrolled)
    {
        historyIter = userEnteredCommand.end();
        hasUserScrolled = true;
    }

    if (code == KEY_UP)
    {
        if (historyIter != userEnteredCommand.begin())
        {
            historyIter--;

            std::wstring temp(historyIter->output);
            temp.erase(temp.begin());
            temp.erase(temp.begin());
            temp.erase(temp.begin());
            editBox->setText(temp.c_str());
        }

        if (historyIter == userEnteredCommand.begin())
        {
            std::wstring temp(historyIter->output);
            temp.erase(temp.begin());
            temp.erase(temp.begin());
            temp.erase(temp.begin());
            editBox->setText(temp.c_str());
        }
    }

    if (code == KEY_DOWN)
    {
        if (historyIter != userEnteredCommand.end())
        {
            historyIter++;

            std::wstring temp(historyIter->output);
            temp.erase(temp.begin());
            temp.erase(temp.begin());
            temp.erase(temp.begin());
            editBox->setText(temp.c_str());
        }
    }
}

void InterfaceConsole::DisplayHelp()
{
    HandleLogEvent(L"---------------------------- HELP ----------------------------", false);
    HandleLogEvent(L"Press F1 to toggle the console.", false);
    HandleLogEvent(L"Setting values: [command] = value", false);
    HandleLogEvent(L"Returning a value: [command]", false);
    HandleLogEvent(L"Loading a map. Note, don't add file type: load mapname", false);
    HandleLogEvent(L"Use up and down arrow to scroll command history.", false);
    HandleLogEvent(L"---------------------------- HELP ----------------------------", false);
}