#include <stdlib.h>
#include "MainEventReceiver.h"

bool MainEventReceiver::OnEvent(const SEvent& event)
{
    for (unsigned int i = 0; i < eventReceivers.size(); ++i)
    {
        if (eventReceivers[i]->OnEvent(event))
        {
            return true;
        }
    }

    return false;
}

void MainEventReceiver::AddEventReceiver(IEventReceiver* receiver)
{
    eventReceivers.push_back(receiver);
}

bool MainEventReceiver::RemoveEventReceiver(IEventReceiver* receiver)
{
    for (unsigned int i = 0; i < eventReceivers.size(); ++i)
    {
        if (eventReceivers[i] == receiver)
        {
            eventReceivers.erase(i);
            return true;
        }

        return false;
    }
}