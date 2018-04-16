#pragma once
#include "Singleton.h"
#include <ace/Sig_Handler.h>

struct SignalHandler : ACE_Event_Handler, Singleton<SignalHandler>
{
    SignalHandler();
    virtual int handle_signal(int signum, siginfo_t* = 0, ucontext_t* = 0);
    ACE_Sig_Handler m_handler;
};
