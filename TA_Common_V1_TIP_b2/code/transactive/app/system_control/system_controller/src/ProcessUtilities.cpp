/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Cameron Rochester
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This singleton class performs a number of process-specific
  * utilities such as starting and killing processes.
  *
  */
#pragma warning (disable: 4996)
#include "ProcessUtilities.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/RunParams.h"
#include <boost/algorithm/string.hpp>
#include <string>
#include <sstream>
#include <iostream>

#ifdef WIN32
    #include <winsock2.h>
#else
    #include <unistd.h>
    #include <pwd.h>
    #include <signal.h>
    #include <sys/resource.h>    // For setpriority
#endif

#ifdef SOLARIS
    #include <sys/types.h>       //
    #include <sys/processor.h>   // All three needed for processor binding
    #include <sys/procset.h>     //
    #include <errno.h>
#endif

#define usrExec(mode) (((mode) & S_IXUSR) == S_IXUSR)
#define grpExec(mode) (((mode) & S_IXGRP) == S_IXGRP)
#define othExec(mode) (((mode) & S_IXOTH) == S_IXOTH)

#include <sys/types.h>
#include <sys/stat.h>
#ifdef LINUX
    #include <sys/resource.h>
#endif

TA_Base_Core::NonReEntrantThreadLockable TA_Base_App::ProcessUtilities::m_getpwnamLock;

using TA_Base_Core::DebugUtil;
namespace TA_Base_App
{
    //
    // Constructor
    //
    ProcessUtilities::ProcessUtilities()
    {
    }

    //
    // launchProcess
    //
    TA_Base_Core::ProcessId ProcessUtilities::launchProcess(const std::string& startPath,
                                                            const std::vector<std::string>& argList,
                                                            const std::string& userName,
                                                            const std::string& passWord)
    {
        FUNCTION_ENTRY("launchProcess");

        // Build up the executable name
        std::string executable = startPath + argList[0];

#ifdef WIN32

        if (executable.size() && (executable.length() <= 4 || executable.substr(executable.length() - 4) != ".exe"))
        {
            executable = executable + ".exe";
        }

#endif //#ifdef WIN32

        // Build up the command line args
#ifdef WIN32
        std::string commandline;
#else
        char**  argv = new char* [argList.size() + 1];
#endif //#ifdef WIN32

        std::stringstream commandlineForLog;
        std::stringstream argsForLog;

        for (unsigned int i = 0; i < argList.size(); i++)
        {
            commandlineForLog << argList[i] << " ";
            argsForLog << "argv[" << i << "]: " << argList[i] << std::endl;
#ifdef WIN32
            // Build up the command line for WIN32
            commandline += argList[i];
            commandline += " ";
#else
            // Build up the command line for SPARC
            argv[i] = new char[argList[i].length() + 1];
            strcpy(argv[i], argList[i].c_str());
#endif //#ifdef WIN32
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Number of args is %d:\n%s", argList.size(), argsForLog.str().c_str());

#ifndef WIN32
        // Splat the last member
        argv[argList.size()] = NULL;
#endif

#ifdef WIN32
        boost::trim(commandline);
#endif
        boost::trim(executable);

        // Make sure the executable exists, if not bomb out!
        // Generate alarms and all that fun stuff
        struct stat statbuf;

        if (stat(executable.c_str(), &statbuf) != 0)
        {
            // Log an error
            std::string out("Monitored Process not found: ");
            out += executable;
            out += " - Unable to execute.";
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, out.c_str());

            return 0;
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Launching process [%s]: Command line: [%s]", executable.c_str(), commandlineForLog.str().c_str());
        }

#ifdef WIN32

        // Construct the WIN32 start process information
        int pid = 0;
        STARTUPINFO         sInfo;
        sInfo.cb              = sizeof(STARTUPINFO);
        sInfo.lpReserved      = NULL;
        sInfo.lpReserved2     = NULL;
        sInfo.cbReserved2     = 0;
        sInfo.lpDesktop       = NULL;
        sInfo.lpTitle         = NULL;
        sInfo.dwFlags         = 0;
        sInfo.dwX             = 0;
        sInfo.dwY             = 0;
        sInfo.dwFillAttribute = 0;
        sInfo.wShowWindow     = SW_SHOW;

        PROCESS_INFORMATION pi;

        if (false == passWord.empty())
        {
            HANDLE hToken = NULL;
            LPTSTR lpBuffer = NULL;

            if (FALSE == LogonUser((char*)userName.c_str(), ".", (char*)passWord.c_str(), LOGON32_LOGON_BATCH, LOGON32_PROVIDER_DEFAULT, &hToken))
            {
                DWORD dwErrorCode = GetLastError();
                std::string systemErrMsg;

                if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                    NULL,
                                    dwErrorCode,
                                    0,
                                    (LPTSTR)&lpBuffer,
                                    0,
                                    NULL) > 0)
                {
                    systemErrMsg = lpBuffer;
                    ::LocalFree(lpBuffer);
                }
                else
                {
                    systemErrMsg = "Unknown";
                }

                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to logon as %s; Error: (%ld)%s",
                            userName.c_str(), dwErrorCode, systemErrMsg.c_str());
                return 0;
            }

            if (FALSE == CreateProcessAsUser(hToken,
                                             executable.c_str(),
                                             (char*)commandline.c_str(),
                                             NULL,
                                             NULL,
                                             FALSE,
                                             DETACHED_PROCESS,
                                             NULL,
                                             NULL,
                                             &sInfo,
                                             &pi))
            {
                DWORD dwErrorCode = GetLastError();
                std::string systemErrMsg;

                if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                    NULL,
                                    dwErrorCode,
                                    0,
                                    (LPTSTR)&lpBuffer,
                                    0,
                                    NULL) > 0)
                {
                    systemErrMsg = lpBuffer;
                    ::LocalFree(lpBuffer);
                }
                else
                {
                    systemErrMsg = "Unknown";
                }

                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to execute [%s]; Commandline: [%s] Error: (%ld)%s",
                            executable.c_str(), commandline.c_str(), dwErrorCode, systemErrMsg.c_str());

                CloseHandle(hToken);
                return 0;
            }

            pid = pi.dwProcessId;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Create Process with pid %d for %s", pid, commandline.c_str());

            CloseHandle(hToken);
        }
        else
        {
            CreateProcess(executable.c_str(),
                          (char*)commandline.c_str(),
                          NULL,
                          NULL,
                          TRUE,
                          DETACHED_PROCESS,
                          NULL,
                          NULL,
                          &sInfo,
                          &pi);
            pid = pi.dwProcessId;
        }

        if (pid < 0)
        {
            return 0;
        }

#else
        // Make sure the executable exists.

#ifdef LINUX
        passwd* userEntry = getpwnam(userName.c_str());
#endif

#ifdef SOLARIS
        const int bufferLength = 1024;
        char* buffer = new char[bufferLength];
        memset(buffer, 0, bufferLength);
        passwd* userEntry = new passwd;

        // getpwnam_r seems to require a lock around it, or it occasionally hangs.
        // This is presumably a Solaris bug.
        {
            TA_Base_Core::ThreadGuard guard(m_getpwnamLock);
            getpwnam_r(userName.c_str(), userEntry, buffer, bufferLength, &userEntry);
        }

        delete [] buffer;
        buffer = NULL;
#endif

        if (userEntry == NULL)
        {
            std::string err("Could not launch ");
            err += executable;
            err += " as user \"";
            err += userName;
            err += "\" because the user is unknown";
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, err.c_str());

            return 0;
        }

        if (!S_ISREG(statbuf.st_mode))
        {
            // Generate an alarm
            std::string out("Monitored Process is not a regular file: ");
            out += executable;
            out += " - Unable to execute.";
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, out.c_str());

            delete userEntry;
            return 0;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "Setting up permission table");

        // Set up a permission table
        bool error(false);

#ifdef LINUX
        short val = statbuf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
#else // SOLARIS
        short val = statbuf.st_mode & S_IAMB;
#endif

        if (userEntry->pw_uid == 0)
        {
            if (!(usrExec(val) || grpExec(val) || othExec(val)))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No execute permission for owner, group or other.");
                error = true;
            }
        }
        else
        {
            if (userEntry->pw_uid == statbuf.st_uid)
            {
                if (!usrExec(val))
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No execute permission for owner.");
                    error = true;
                }
            }
            else if (userEntry->pw_gid == statbuf.st_gid)
            {
                if (!grpExec(val))
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No execute permission for group.");
                    error = true;
                }
            }
            else
            {
                if (!othExec(val))
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No execute permission for other.");
                    error = true;
                }
            }
        }

        if (error)
        {
            // Log the error
            std::string out(userName);
            out += " does not have permission to execute [";
            out += executable + "]";
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, out.c_str());

            delete userEntry;
            return 0;
        }

        const char* errorFork = NULL;
        const char* errorGroup = NULL;
        const char* errorUser = NULL;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "About to fork process");
        pid_t pid = 0;

        switch (pid = vfork())
        {
        case -1:    // Error
            errorFork = "Could not fork:";
            break;

        case 0:     // Child
            //  Change to the required user & group IDs.
        {
            if (::setgid(userEntry->pw_gid) == -1)
            {
                errorGroup = "Unknown group for user";
            }

            if (::setuid(userEntry->pw_uid) == -1)
            {
                errorUser = "Unknown user";
            }

            struct rlimit limit;

            if (getrlimit(RLIMIT_NOFILE, &limit) != 0)
            {
                printf("getrlimit error for RLIMIT_NOFILE");
            }

            // Close all open file descriptors for the child
            for (unsigned int i = 3; i <= limit.rlim_cur; i++)
            {
                close(i);
            }

            //  Run the executable.
            if (execv(executable.c_str(), argv) == -1)
            {
                std::stringstream errStrm;
                errStrm << (errorGroup ? errorGroup : "") << std::endl
                    << (errorUser ? errorUser : "") << std::endl
                    << (errorFork ? errorFork : "") << std::endl
                    << " UserName=" << userName << std::endl
                    << "Monitored Process execution failed: " << executable << " - " << strerror(errno) << std::endl;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", errStrm.str().c_str());
                _exit(1);
            }

            if (errorGroup || errorUser || errorFork)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "UserName=%s error=%s %s",
                    userName.c_str(), (errorGroup ? errorGroup : ""), (errorUser ? errorUser : ""), (errorFork ? errorFork : ""));
            }

            _exit(0);
        }
        break;

        default:    // Parent
            break;
        }

        delete [] argv;
        delete userEntry;

        // check the pid exists, if not make the return pid = 0
        if (kill(pid, 0) != 0)
        {
            return 0;
        }

#endif //#ifdef WIN32

        FUNCTION_EXIT;
        return pid;
    }

    //
    // terminateProcess
    //
    bool ProcessUtilities::terminateProcess(int pid)
    {
#ifdef WIN32
        // Make a call to OpenProcess to get the process handle, then kill it!
        HANDLE blah = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)pid);

        if (TerminateProcess(blah, 0) == 0)
        {
            return false;
        }

#else

        // Make a call to the UNIX kill system command
        if (kill(pid, SIGTERM) != 0)
        {
            char out[128] = {0};
            sprintf(out, "PID %i not found, cannot send SIGTERM. Process assumed dead.", pid);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, out);
            return true;
        }

#endif //#ifdef WIN32
        return true;
    }

    //
    // killProcess
    //
    void ProcessUtilities::killProcess(int pid)
    {
#ifdef WIN32
        terminateProcess(pid);
#else

        // Make a kill system call
        if (kill(pid, SIGKILL) != 0)
        {
            char out[128] = {0};
            sprintf(out, "PID %i not found, cannot send SIGKILL. Process assumed dead.", pid);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, out);
        }

#endif //#ifdef WIN32
    }

    //
    // parseProcessAddress
    //
    void ProcessUtilities::parseProcessAddress(std::string& startPath, std::vector<std::string>& argList, const std::string& processAddress)
    {
        startPath.clear();
        boost::split(argList, boost::trim_copy<const std::string>(processAddress), boost::is_space(), boost::token_compress_on);
    }

    //
    // setPriority
    //
    void ProcessUtilities::setPriority(int pid, int priority)
    {
#ifndef WIN32

        errno = 0;
        int returnValue = setpriority(PRIO_PROCESS, pid, priority);

        if (returnValue == -1 && errno != 0)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Error setting the priority of process %d: %s", pid, strerror(errno));
        }

#endif
    }

    //
    // bindToProcessor
    //
    void ProcessUtilities::bindToProcessor(int pid, int processor)
    {
#ifdef SOLARIS

        int returnValue = processor_bind(P_PID, pid, processor, NULL);

        if (returnValue == 1)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Error binding process %d to a processor: %s", pid, strerror(errno));
        }

#endif
    }

#ifdef WIN32

    //
    // checkProcess
    //
    bool ProcessUtilities::checkProcess(int pid)
    {
        HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)pid);
        int out = WaitForSingleObject(handle, 0);
        return (out == WAIT_OBJECT_0 || out == WAIT_TIMEOUT);
    }

#endif //#ifdef WIN32
} // namespace TA_Base_App
