/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/signs/pids_manager/src/StationPidUpdater.h $
  * @author:  Anggiono
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * This class is used to communicate directly with the PID Agent. It uses its
  * NamedObject member to resolve the agent and make calls on it.
  *
  * All of the arguments to the methods in this class take standard C++ types. This
  * class converts these types to equivalent CORBA types as necessary, makes the
  * request on the agent, and converts the CORBA types that are returned back into
  * C++ types (eg CORBA sequences into vectors).
  *
  * If an error occurs while communicating with the agent, and an exception is
  * returned, this class converts the exception into a PidAgentException, so that
  * it can be caught by the dialog and an appropriate error message shown.
  *
  * This class also allows dialogs to subscribe for updates to (adhoc and
  * predefined) messages from the PID Agent.
  */

#ifndef STATIONPIDUPDATER_H
#define STATIONPIDUPDATER_H

#include <vector>

#include "core/naming/src/NamedObject.h"
#include "core\data_access_interface\PidsManager\src\PidMessage.h"
#include "core\data_access_interface\PidsManager\src\IPidSettings.h"
#include "bus\signs\pids_agent\IDL\src\IPIDSManagerCorbaDef.h"

struct MessageTime
{
    int startTimeYears;
    int startTimeMonths;
    int startTimedays;
    int startTimeHours;
    int startTimeMins;
    int startTimeSecs;
    int stopTimeYears;
    int stopTimeMonths;
    int stopTimedays;
    int stopTimeHours;
    int stopTimeMins;
    int stopTimeSecs;
};
// forward declaration
class IPidMessageListener;
class IPidSettings;
namespace TA_IRS_App
{	
    typedef TA_Base_Core::NamedObject < TA_Base_Bus::IPIDSManagerCorbaDef,
            TA_Base_Bus::IPIDSManagerCorbaDef_ptr,
            TA_Base_Bus::IPIDSManagerCorbaDef_var >
            PidsAgentNamedObject;

    class StationPidUpdater// : public  IStationPidUpdater//, public virtual TA_Base_Core::CtaRuntime
    {
        public:

            /**
            * getInstance
            *
            * Returns an instance of the class
            *
            * @return the pointer to this singleton instance
            *
            */
            static StationPidUpdater* getInstance();

            /**
            * removeInstance
            *
            * Removes the instance of the class (if already created) and cleanup the members.  Primarily
            * used upon program termination (e.g. from main()) so that Purify does not consider this class
            * and its members to be leaks.
            *
            */
            static void removeInstance();

            /**
              * StationPidUpdater
              * @param : TA_Base_Core::IEntityData& MgrConfiguration
              * @exception <exceptions> Optional
              */
            StationPidUpdater( TA_Base_Core::IEntityDataPtr MgrConfiguration );


            /**
              * ~StationPidUpdater
              * @return virtual
              * @exception <exceptions> Optional
              */
            virtual ~StationPidUpdater();

            // Retrieves all adhoc messages for the current location.
            virtual std::vector<TA_IRS_Core::PidMessage> getPidMessages();

            //	virtual void submitDisplayRequest(vector<std::string>& pids,  TA_IRS_Core::PidMessage& pidMsg,
            //		bool isCyclicMessage, MessageTime& msgTime, int interval,
            //		bool msgType, bool showType, bool isTrainSeled);


            virtual long modifyPidMessage( const int id, const wstring& name, const wstring& value );

            //TD18260 AVteam++
            //Delete the adhoc message with the specified ID
            //virtual long deletePidMessage(int id);
            //TD18260 ++AVteam
            //	virtual void clearPidMessage(bool bTrainPids, std::vector<std::string>& pids);
            // Allows classes implementing the IAdhocMessageListener interface to be notified of updates to messages.
            virtual void addListener( IPidMessageListener* listener );

            virtual void InitCorbaObject();

            PidsAgentNamedObject& getPidsAgentObject()
            {
                return m_pidsAgent;
            }

            TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage convertBackChineseDesc( CString chineseDesc );
			TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage wstringToCorbaSequence(const wstring& ws);

        private:
            // disabled methods
            StationPidUpdater();
//		StationPidUpdater(const StationPidUpdater&);
            void operator=( const StationPidUpdater& );

            PidsAgentNamedObject m_pidsAgent;
            //	PidMessageSubscriber    m_messageSubscriber;
            TA_IRS_Core::IPidSettings* m_settings;

            // The one and only instance of this class.
            static StationPidUpdater* m_instance;
            // Protect singleton creation
            static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
    };
}
#endif