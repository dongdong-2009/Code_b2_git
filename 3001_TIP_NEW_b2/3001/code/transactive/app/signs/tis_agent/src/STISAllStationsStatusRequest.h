#ifndef STISSTATIONSTATUSREQUESTOCC_H
#define STISSTATIONSTATUSREQUESTOCC_H

#include "STISStatusRequest.h"

namespace TA_IRS_App
{

    class STISAllStationsStatusRequest : public STISStatusRequest
    {
        public:
            STISAllStationsStatusRequest(void);
            virtual ~STISAllStationsStatusRequest(void);
			virtual std::vector<unsigned char> createProtocolMessage();
	    	virtual void processResponse();
    		virtual void log(bool sendSuccess);
		private:
			void updateStationConnectionLinkStatus(const std::string& reportSatation, bool status);
			void updateStationPidStatus(const std::string& reportSatation, const std::string& pidStatus);
			void updateCurrentLibraryVersion(const std::string& reportSatation, int version);
			void updateNextLibraryVersion(const std::string& reportSatation, int version);
    };

};

#endif
