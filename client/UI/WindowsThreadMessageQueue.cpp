#include "WindowsThreadMessageQueue.h"

#include <cassert>

WindowsThreadMessageQueue::WindowsThreadMessageQueue()
    : wakeUpQueueEvent(Arg::ManualReset(false), Arg::InitialSet(false))
{}

void WindowsThreadMessageQueue::ProcessQueuedMessages() {
    MSG msg = {};

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            // may be any window
            this->queueReceivedQuit = true;
            break;
        }
        else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void WindowsThreadMessageQueue::WaitAndProcessQueuedMessages() {
    HANDLE events[] = {
        this->wakeUpQueueEvent.GetEventHandle()
    };
    constexpr DWORD eventCount = static_cast<DWORD>(std::size(events));

    DWORD evt = MsgWaitForMultipleObjectsEx(
        eventCount,
        events,
        INFINITE,
        QS_ALLEVENTS,
        MWMO_ALERTABLE);

    if (evt == WAIT_OBJECT_0 + eventCount) {
        this->ProcessQueuedMessages();
    }
    else if ((evt >= WAIT_OBJECT_0) && (evt < WAIT_OBJECT_0 + eventCount)) {
        DWORD eventIndex = evt - WAIT_OBJECT_0;

    }
    else {
        switch (evt) {
        case WAIT_IO_COMPLETION:
        case WAIT_TIMEOUT:
            // dont care
            break;
        default:
            // handle the error and possibly exit
            // If happens check why and possibly add some handling
            assert(false);
            break;
        }
    }
}
