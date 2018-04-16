#include <time.h>
#include <sstream>
#include <assert.h>
#include "core/message/src/StructuredEventConsumer.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/RunParams.h"

#if defined( SOLARIS )
#include "core/utilities/src/StdPubUtility.h"
#define _stricmp TA_Base_Core::CStdPubUtility::stricmp
#endif

#include "NSAModuleCommandParser.h"

namespace TA_Base_Core
{

    const int LEVEL_FEDERATION_EVENT_CHNANEL = 10;
    const int LEVEL_EVENT_CHANNEL = LEVEL_FEDERATION_EVENT_CHNANEL-1;
    const int LEVEL_NONE = 0;

    struct MyHandler : public TA_Base_Core::AbstractMessageSubscriber
    {
        // Implementation of AbstractMessageSubscriber
        virtual void receiveMessage(const CORBA::Any& message) {TA_Assert(0);} // Dummy implementation is NOT CALLED
        virtual bool onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events)
        {
            for (unsigned int i = 0; i < events.size(); ++i)
            {
                TA_Assert(events[i] != NULL);

                std::ostringstream stm;
                stm << "EVT " << i << ": " << TA_Base_Core::gStringFromEventHeader(*(events[i]));
                TA_Base_Core::MonitorStaticLib::outputDebugString(stm.str().c_str());
            }

            return true;
        }
    };

    NSAModuleCommandParser::NSAModuleCommandParser(void)
        :CommonModuleCommandParser()
        ,m_currChannelLocator(TA_Base_Core::IChannelLocator::_nil())
        ,m_currChannel(CosNotifyChannelAdmin::EventChannel::_nil())
        ,m_currTargetObj(AttNotification::Interactive::_nil())
        ,m_currentChannelObj(AttNotification::Interactive::_nil())
        ,m_channelName("")
    {
        m_moduleType = TA_Base_Core::InnerMonitorConsts::MODULE_TYPE_NSA;
        m_currentClassLevel = LEVEL_NONE;
    }

    NSAModuleCommandParser::~NSAModuleCommandParser(void)
    {
       m_currChannelLocator = TA_Base_Core::IChannelLocator::_nil();
       m_currIscsInteractiveObj = TA_Base_Core::ISCSInteractive::_nil();

       m_currChannel = CosNotifyChannelAdmin::EventChannel::_nil();
       m_currentChannelObj = AttNotification::Interactive::_nil();
       m_currTargetObj = AttNotification::Interactive::_nil();
    }

    void NSAModuleCommandParser::postInit()
    {
        char agentLocBuffer[1024];
        agentLocBuffer[0] = 0x0;
        sprintf(agentLocBuffer, "corbaloc::%s/ChannelLocator", m_processedAgentEndPoint.c_str(), m_processedAgentEntityName.c_str());

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "try to get channel coarba reference: corbaLocString <%s>", agentLocBuffer); 

        CORBA::Object_var tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(agentLocBuffer);
        m_currChannelLocator = TA_Base_Core::IChannelLocator::_narrow(tmpVar);
        m_currIscsInteractiveObj = TA_Base_Core::ISCSInteractive::_narrow(tmpVar);
    }

    bool NSAModuleCommandParser::processCommand(const std::vector<std::string> & vtCommandArgs)
    {
        if ( CommonModuleCommandParser::processCommand(vtCommandArgs) )
        {
            return true;
        }

        bool fProcessed = false;
        try
        {
            if (_stricmp(vtCommandArgs[0].c_str(),"listchannels") == 0)
            {
                fProcessed = true;
                listChannelCommand(vtCommandArgs);
            }
            else if (_stricmp(vtCommandArgs[0].c_str(),"setchannel") == 0)
            {
                fProcessed = true;
                setChannelCommand(vtCommandArgs);
            }
            else if ( (_stricmp(vtCommandArgs[0].c_str(),"addflag") == 0) || (_stricmp(vtCommandArgs[0].c_str(),"removeflag") == 0) )
            {
                fProcessed = true;
                processFlagCommand(vtCommandArgs);
            }
            else if (_stricmp(vtCommandArgs[0].c_str(),"channel") == 0)
            {
                fProcessed = true;
                channelCommand(vtCommandArgs, NULL);
            }
            else if (_stricmp(vtCommandArgs[0].c_str(),"loop") == 0)
            {
                fProcessed = true;
                processLoopCommand(vtCommandArgs);
            }
        }
        catch ( const CORBA::Exception& cex )
        {
            std::ostringstream stm;
            stm << "Error: CORBA::Exception caught: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex);
            TA_Base_Core::MonitorStaticLib::outputDebugString(stm.str().c_str());
        }

        
        return fProcessed;
    }


    std::string NSAModuleCommandParser::getStateDesc()
    {
        std::string baseContent = CommonModuleCommandParser::getStateDesc();

        std::ostringstream ostr;
        ostr << baseContent.c_str();
        if (m_channelName.length()>0)
        {
            ostr << ", channel name: " << m_channelName.c_str();
        }
        return ostr.str();
    }

    void NSAModuleCommandParser::displayUsage()
    {
        CommonModuleCommandParser::displayUsage();

        std::cout <<
            " listchannels \n" 
            " setchannel <channelName> \n" 
            " addflag param\n"
            " removeflag param\n"
            " loop second command param -- will be removed\n"
            "\n" 
            "Additional commands for event channel object: \n" 
            " channel help \n" 
            " channel current \n" 
            " channel up \n" 
            " channel go <object> \n" 
            " channel attach - displays messages passing through the current event channel\n" 
            "         detach - exit from message listening\n" 
            " channel flags \n"  << std::endl; 
    }

    void NSAModuleCommandParser::listChannelCommand(const std::vector<std::string> & vtCommandArgs)
    {
        TA_Base_Core::ChannelMappingSeq* channels = m_currChannelLocator->getChannels( );
        std::ostringstream stm;
        for ( unsigned int i = 0; i < channels->length(); i++)
        {
            TA_Base_Core::ChannelMapping& mapping = (*channels)[i];
            stm << " " << mapping.channelName.in() << std::endl;
        }
        delete channels;						

        TA_Base_Core::MonitorStaticLib::outputDebugString(stm.str().c_str());

    }

    void NSAModuleCommandParser::setChannelCommand(const std::vector<std::string> & vtCommandArgs)
    {
        TA_Base_Core::ChannelMappingSeq* channels = m_currChannelLocator->getChannels( );

        AttNotification::Interactive_var newTargetObj;
        CosNotifyChannelAdmin::EventChannel_var newChannel;

        for ( unsigned int i = 0; i < channels->length(); i++)
        {
            TA_Base_Core::ChannelMapping& mapping = (*channels)[i];
            if (mapping.channelName.in() == vtCommandArgs[1])
            {
                newChannel = CosNotifyChannelAdmin::EventChannel::_duplicate(mapping.channel);
                
                //CosNotifyChannelAdmin::SupplierAdmin_ptr ptr = newChannel->default_supplier_admin(); 

                newTargetObj = AttNotification::Interactive::_narrow(newChannel);
                break;
            }
        }

        delete channels;						

        if (!CORBA::is_nil(newTargetObj))
        {
            TA_Assert(!CORBA::is_nil(newChannel));

            m_currChannel = newChannel;
            m_currTargetObj = newTargetObj;
            m_currentChannelObj = newTargetObj;
            m_channelName = vtCommandArgs[1];
            m_currentClassLevel = LEVEL_FEDERATION_EVENT_CHNANEL;
            std::cout << " --> Federation EventChannel: " << m_channelName.c_str() << std::endl;
        }
        else
        {
            std::cout << " Error: Invalid currChannel name" << std::endl;
        }

    }

    void NSAModuleCommandParser::channelCommand(const std::vector<std::string> & vtCommandArgs, FILE * pFile)
    {
        if (CORBA::is_nil(m_currChannel))
        {
            std::cout << "Error: Channel has not been set"  << std::endl;
            return;
        }

        if(vtCommandArgs.size() < 2)
        {
            std::cout << "command: "  << vtCommandArgs[0].c_str() << " has grammar error" << std::endl;
            return;
        }

        TA_Assert(!CORBA::is_nil(m_currChannelLocator));

        if ( _stricmp( "current", vtCommandArgs[1].c_str() ) == 0 )
        {
            std::cout << " --> Current Federation EventChannel: " << m_channelName.c_str() << std::endl;
            return;
        }

        if ( _stricmp( "attach", vtCommandArgs[1].c_str() ) == 0 )
         {
            return attachEventChannelCommand(vtCommandArgs);
        }

        if ( !TA_Base_Core::RunParams::getInstance().isSet("IsOldManagedProcess") )  
        {

            if ( m_currentClassLevel == LEVEL_FEDERATION_EVENT_CHNANEL+1  && _stricmp(vtCommandArgs[1].c_str(),"go") == 0 && vtCommandArgs.size() == 3 )
            {
                std::vector<std::string> tmpCommand;
                tmpCommand.push_back("setchannel");
                tmpCommand.push_back(vtCommandArgs[2]);
                setChannelCommand(tmpCommand);
                return;
            }

            if ( m_currentClassLevel == LEVEL_EVENT_CHANNEL && _stricmp(vtCommandArgs[1].c_str(),"up") == 0 )
            {
                m_currTargetObj = m_currentChannelObj;
                m_currentClassLevel = LEVEL_FEDERATION_EVENT_CHNANEL;
                std::cout << " --> Federation EventChannel: " << m_channelName.c_str() << std::endl;
                return;
            }
        }

        // Pass the command to the current target object
        CORBA::Boolean success = false;
        CORBA::Boolean target_changed = false;
        AttNotification::Interactive_ptr newTargetObj;

        std::ostringstream ostr;
        int argSize = vtCommandArgs.size();
        assert(argSize >= 2);
        for(size_t i=1; i<argSize-1; i++)
        {
            ostr << vtCommandArgs[i].c_str() << " ";
        }
        ostr << vtCommandArgs[argSize-1].c_str();

        char* result = m_currTargetObj->do_command( ostr.str().c_str(), success, target_changed, newTargetObj);
        std::cout << result << std::endl;

        int buffsize = strlen(result);
        if (pFile != NULL)
        {
            struct tm   *nowTime;
            time_t      szClock;

            // Get time in seconds
            time( &szClock );

            // Convert time to struct tm form 
            nowTime = localtime( &szClock );

            // Print local time as a string.
            char timeBuffer[100];
            sprintf( timeBuffer,"\nCurrent date and time: %s", asctime( nowTime ) );

            fputs(timeBuffer, pFile);

            fputs(result, pFile);
            fflush(pFile);
        }

        CORBA::string_free(result);


        if (target_changed)
        {
            m_currTargetObj = newTargetObj;
            if ( _stricmp(vtCommandArgs[1].c_str(),"up") == 0 )
            {
                m_currentClassLevel++;
            }
            else if ( _stricmp(vtCommandArgs[1].c_str(),"go") == 0 )
            {
                m_currentClassLevel--;
            }
        } //end if target_changed

    } //end function

    void NSAModuleCommandParser::attachEventChannelCommand(const std::vector<std::string> & vtCommandArgs)
    {
        if (CORBA::is_nil(m_currChannel))
        {
            std::cout << " Error: Channel has not been set" << std::endl;
            return;
        }

        TA_Base_Core::StructuredEventConsumer* consumer = new TA_Base_Core::StructuredEventConsumer(new TA_Base_Core::DefaultAdminProvider);
        consumer->addConnection(m_processedAgentEndPoint, m_currChannel);

        CosNotification::EventTypeSeq seq;
        seq.length(1);
        seq[0].domain_name = CORBA::string_dup(TA_Base_Core::WILDCARD_STR);
        seq[0].type_name = CORBA::string_dup(TA_Base_Core::WILDCARD_STR);
        TA_Base_Core::SubscriptionFilter* filter = new TA_Base_Core::SubscriptionFilter(seq, "true", NULL);
        TA_Base_Core::IEventHandler* handler = new MyHandler;

        consumer->attachSubscriberIfCompatible(handler, filter);
        m_isCatchInput = true;

        std::string cmd;
        while(1)
        {
            std::cin >> cmd;
            if (cmd == "detach")
            {
                m_isCatchInput = false;
                break;
            }
            TA_Base_Core::Thread::sleep(1000);
        }

        consumer->detachSubscriber(filter);
        consumer->disconnectAllEventChannels();
        delete consumer;

        delete filter;
        delete handler;

    }

    void NSAModuleCommandParser::processFlagCommand(const std::vector<std::string> & vtCommandArgs)
    {
        assert( !CORBA::is_nil(m_currIscsInteractiveObj) );
        assert( vtCommandArgs.size() >= 2 );

        CORBA::Boolean fResult = false;
        CORBA::Boolean targetChanged = false;
        TA_Base_Core::ISCSInteractive_ptr newTargetObj;

        char * response = m_currIscsInteractiveObj->processCommand(vtCommandArgs[0].c_str(), vtCommandArgs[1].c_str(), fResult,
            targetChanged, newTargetObj);

        std::cout << response << std::endl;
        CORBA::string_free(response);
    }

    //" loop second command param\n"
    void NSAModuleCommandParser::processLoopCommand(const std::vector<std::string> & vtCommandArgs)
    {
        assert( !CORBA::is_nil(m_currIscsInteractiveObj) );
        if( vtCommandArgs.size() < 3 )
        {
            std::cout << " wrong command" << std::endl;
            return;
        }

        FILE * pfile = fopen("nsa_channel_report.txt", "w");
        fputs("--------------------- start ---------------------------\n\n", pfile);

        int tmpSeconds = atoi(vtCommandArgs[1].c_str());
        int msec = tmpSeconds > 0 ? tmpSeconds*1000 : 1000; 
        
        std::vector<std::string> vtCurrentCommand;
        vtCurrentCommand.push_back("channel");
        for(int i=2; i<vtCommandArgs.size(); i++ )
        {
            vtCurrentCommand.push_back(vtCommandArgs[i]);
        }

        std::string cmd;
        while(1)
        {
            channelCommand(vtCurrentCommand, pfile);
            TA_Base_Core::Thread::sleep(msec);
        }

        CORBA::Boolean fResult = false;
        CORBA::Boolean targetChanged = false;
        TA_Base_Core::ISCSInteractive_ptr newTargetObj;

        char * response = m_currIscsInteractiveObj->processCommand(vtCommandArgs[0].c_str(), vtCommandArgs[1].c_str(), fResult,
            targetChanged, newTargetObj);

        std::cout << response << std::endl;
        CORBA::string_free(response);        
    }

}