#if !defined(GF_3002_app_bas_BASAgent_BasModeEquipment_H)
#define GF_3002_app_bas_BASAgent_BasModeEquipment_H

#include "app/bas/BASAgent/src/IDataPointUpdateObserver.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/message/src/NameValuePair.h"
#include <string>

namespace TA_Base_Core
{
    class MessageType;
}

namespace TA_Base_Bus
{
	class DataPoint;
}

namespace TA_IRS_App
{
    class BasModeEquipment : IDataPointUpdateObserver
	{
	public:
		BasModeEquipment(const std::string& nodeName);
		~BasModeEquipment();

		void executeStationMode(unsigned long modeValue, bool canOverrideStationModeInProgress, const std::string& sessionID);
		
		unsigned long getCurrentModeValue();

        virtual void processDataPointControlStateUpdate(const unsigned long dpEntityKey, const int nControlState, const TA_Base_Bus::DataPointControl& controlState);
        virtual void processDataPointValueUpdate(const unsigned long dpEntityKey, const TA_Base_Bus::DataPointValue& updateValue );
        
	private:
		std::string m_nodeName;
		TA_Base_Bus::DataPoint* m_modeCtrlPt;
		TA_Base_Bus::DataPoint* m_modeAutoManual;
		TA_Base_Bus::DataPoint* m_modeInProgress;
		TA_Base_Bus::DataPoint* m_modeValue;
        TA_Base_Bus::DataPoint* m_modeRightCtrlPt;

        TA_Base_Core::ReEntrantThreadLockable m_modeValueInitLock;
        bool m_modeValueInit;
	};
}
#endif //GF_3002_app_bas_BASAgent_BasModeEquipment_H
