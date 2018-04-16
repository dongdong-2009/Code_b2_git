#ifndef LIBRARYSYNCFAILEDPROCESSOR_H
#define LIBRARYSYNCFAILEDPROCESSOR_H

#include "core/naming/src/NamedObject.h"
#include "bus/signs_4669/TisManagerCorbaDef/src/ISTISManagerCorbaDef.h"
#include <vector>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace TA_IRS_App
{
	typedef TA_Base_Core::NamedObject < TA_Base_Bus::ISTISManagerCorbaDef,
	        TA_Base_Bus::ISTISManagerCorbaDef_ptr,
	        TA_Base_Bus::ISTISManagerCorbaDef_var > StisNamedObject;

    class WorkThread;
	class LibrarySyncFailedProcessor : boost::noncopyable
    {
        public:
            enum ProcessorType
            {
                STIS_SYNCHRONISE,
                STIS_UPGRADE,
                TTIS_SYNCHRONISE,
                TTIS_UPGRADE
            };
            LibrarySyncFailedProcessor(ProcessorType type, unsigned short repeatInterval);
            ~LibrarySyncFailedProcessor(void);
            void stopProcess();
            void startProcess();
            static std::string processTypeToString(ProcessorType type);
			void setSessionId(std::string sessionId);
        private:
			typedef boost::shared_ptr<StisNamedObject> StisNamedObjectPtr;
            bool processSTISSynchronise(StisNamedObject& stisCorbaObject);
            bool processSTISUpgrade(StisNamedObject& stisCorbaObject);
            bool processTTISSynchronise(StisNamedObject& stisCorbaObject);
            bool processTTISUpgrade(StisNamedObject& stisCorbaObject);
			unsigned long getNextSTISVersion(StisNamedObject& stisCorbaObject);
			unsigned long getCurrentSTISVersion(StisNamedObject& stisCorbaObject);
			unsigned long getNextTTISVersion(StisNamedObject& stisCorbaObject);
			unsigned long getCurrentTTISVersion(StisNamedObject& stisCorbaObject);
            void processFailedStations();
            boost::function<unsigned long ()> getCDBVersion;
            boost::function<unsigned long (StisNamedObject&)> getStationVersion;
            boost::function<bool (StisNamedObject&)> processFailedStation;
			std::vector<StisNamedObjectPtr> m_stisCorbaObjects;
            ProcessorType m_type;
            WorkThread* m_processThread;
            // Cache the library file for synchronise
            TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType m_libraryFile;
			unsigned short m_processRepeatInterval;
			std::string m_sessionID;
    };
}

#endif