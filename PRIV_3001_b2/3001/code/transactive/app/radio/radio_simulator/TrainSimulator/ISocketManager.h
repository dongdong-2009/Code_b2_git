
#ifndef _I_SOCKET_MANAGER_H_
#define _I_SOCKET_MANAGER_H_
#include "app/radio/radio_simulator/TrainSimulator/UDPSocketManager.h"

namespace TA_IRS_App
{
	class ISocketManager
	{
	public:
		virtual TA_IRS_App::CUDPSocketManager* getUDPSocketManager() = 0;
	};
}
#endif