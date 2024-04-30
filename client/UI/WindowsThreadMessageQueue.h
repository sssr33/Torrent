#pragma once

#include <Helpers/WinApiEvent.h>

// WindowsThreadMessageQueue will process all messages for all windows where this window created
class WindowsThreadMessageQueue {
public:
    WindowsThreadMessageQueue();

    void ProcessQueuedMessages();
    void WaitAndProcessQueuedMessages();

private:
    bool queueReceivedQuit = false;
    Helpers::WinApiEvent wakeUpQueueEvent;
};
