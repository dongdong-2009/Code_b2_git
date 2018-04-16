

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventHelper.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#ifndef CADIEVENTHELPER_H
#define CADIEVENTHELPER_H

#include <string>
#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADITypes.h"

namespace TA_IRS_App {

	class CADIEventHelper  
	{
	public:
		CADIEventHelper();
		virtual ~CADIEventHelper();

		static std::string getCADCommandReturnCode(int event);
		static std::string getRadioEventQualifiers(unsigned long event);
		static std::string getAckQualifier(unsigned short event);
		static std::string getCmdStatus(unsigned short event);
		static std::string getSessionStatus(unsigned short event);
		static std::string getZCStatus(unsigned short event);
		static std::string getIZTrunkingStatus(unsigned short event);
		static std::string getRejectCodeQualifier(unsigned short event);

	private:
		// disabled methods
		CADIEventHelper(const CADIEventHelper&);
		void operator=(const CADIEventHelper&);

	};
};

#endif
