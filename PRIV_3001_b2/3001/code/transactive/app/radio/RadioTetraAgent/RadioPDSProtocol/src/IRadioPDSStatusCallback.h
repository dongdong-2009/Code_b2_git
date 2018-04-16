#pragma once

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/TtisTypes.h"
namespace TA_IRS_App
{
	class IRadioPDSStatusCallback
	{

	public:
		virtual  void notifyPDSDownloadStatus (const TA_IRS_App::TtisTypes::SPDSDownloadDataResults & pdsDownloadResult) = 0;
	};

}
