#include "StdAfx.h"
#include "SignalHandler.h"
#include "ProcessManager.h"

SignalHandler::SignalHandler()
{
    m_handler.register_handler(SIGINT, this);
    m_handler.register_handler(SIGBREAK, this);
}

int SignalHandler::handle_signal(int signum, siginfo_t*, ucontext_t*)
{
    ProcessManager::getInstance().terminateAndWaitForAll();
    return 0;
}
