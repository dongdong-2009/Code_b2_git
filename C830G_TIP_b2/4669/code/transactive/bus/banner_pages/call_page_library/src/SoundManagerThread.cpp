// SoundManagerThread.cpp: implementation of the SoundManagerThread class.
//
//////////////////////////////////////////////////////////////////////

#include "SoundManagerThread.h"
#include "core\utilities\src\DebugUtilMacros.h"
#include <sstream>
#include "core\exceptions\src\SoundPlayerException.h"
using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_Bus
{
    SoundManagerThread::SoundManagerThread()
    {
        m_bSound = false;
        // init();
    }

    SoundManagerThread::~SoundManagerThread()
    {
        terminateAndWait();
    }

    void SoundManagerThread::processEvent( boost::shared_ptr< SoundEntity > newEvent )
    {
        std::string category = newEvent->getCatergory();
        int priority = newEvent->getPriority();

        try
        {
            if( !m_bSound )
            {
                init();
            }

            //std::auto_ptr< TA_Base_Bus::SoundManagerProxy > soundManagerProxy( new TA_Base_Bus::SoundManagerProxy ); //TODO
            //soundManagerProxy->setPriority(category, priority);
            if( !m_bSound )
            {
                return;
            }

            m_soundManagerProxy->setPriority( category , priority );
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "refreshSoundPriority(): set priority successfully with priority %d and catergory %s", priority ,category.c_str() );

        }
        catch(TA_Base_Core::SoundPlayerException& ex)
        {
            std::string error = "SoundPlayerException while attempting to set priority for category \"";
            error += category;
            error += "\" : ";
            error += ex.what();
            LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
        }
        catch (...)
        {
            std::ostringstream ostr;
            ostr << "Sound Manager exception setting priority for category '" << category << "'";
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", ostr.str().c_str());
        }
    }

    void SoundManagerThread::init()
    {
        m_bSound = false;
        try
        {
            m_soundManagerProxy.reset(new TA_Base_Bus::SoundManagerProxy ); //TODO
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Successfully init soundManagerProxy");
            m_bSound = true;
            // start();
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to init soundManagerProxy");
        }
    }
}
