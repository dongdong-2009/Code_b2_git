#ifndef RADIO_MFT_RESPONSE_H_INCLUDED
#define RADIO_MFT_RESPONSE_H_INCLUDED
#include "app/radio/RadioTetraAgent/src/RemoteAPI.h"
#include "app/radio/RadioTetraAgent/src/MccProtocolHelper.h"
#include "core/utilities/src/AlarmClock.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <string>

using namespace std;
class RadioMFTHandler;

class RadioMFTResponse : public virtual POA_MCC7500SDK::IAPIResponse{
	friend class RadioMFTHandler;
public:
	RadioMFTResponse(RadioMFTHandler& rm, TA_Base_Core::AlarmClock& timeout);
	~RadioMFTResponse();

	virtual void  CallBack(const ::MCC7500SDK::Parameters& message);
	void setOperationMode(bool mode); 
	void setMccReady(int id);
private:
	RadioMFTHandler& m_connection;
	TA_Base_Core::AlarmClock& m_timeoutThread;
	bool operationMode;
	bool mccReady[2];//PERIPHERAL is not needed anymore 
	bool initialized;

	static TA_Base_Core::ReEntrantThreadLockable m_lock;
	static unsigned int KEEPALIVE_TIMEOUT;
	/*static unsigned short PERIPHERAL_STATUS;*///PERIPHERAL is not needed anymore in MCC Initialization
	static unsigned short AUDIO_PROCESSING_STATUS;
	static unsigned short DATABASE_STATUS; 
	static unsigned short SYSTEM_NOT_READY;
};
	
#endif