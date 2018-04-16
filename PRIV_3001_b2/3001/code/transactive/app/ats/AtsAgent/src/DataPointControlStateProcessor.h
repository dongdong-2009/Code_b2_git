/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/DataPointControlStateProcessor.h $
  * @author:   zhou yuan
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */

#if !defined(AFX_DATAPOINTCONTROLSTATEPROCESSOR_H__EBE02C7A_EE2E_4D7D_8686_3D76C43CF949__INCLUDED_)
#define AFX_DATAPOINTCONTROLSTATEPROCESSOR_H__EBE02C7A_EE2E_4D7D_8686_3D76C43CF949__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "bus/scada/common_library/src/EventProcessor.h"
#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"

#include "app/ats/AtsAgent/src/AtsPsdData.h"

namespace TA_IRS_App
{
	class DataPointControlStateProcessor : public EventProcessor< TA_Base_Bus::DataPointWriteRequest >
	{
	public:
		DataPointControlStateProcessor();
		virtual ~DataPointControlStateProcessor();

		void InitialisePsdData(std::vector< AtsPsdData* > psdData);

	private:
		
		/**
		* processEvent()
		*
		* Generates an Event based on the request, and logs it
		*
		* @param newEvent	the request for the new event
		*
		*/
        virtual void processEvent( TA_Base_Bus::DataPointWriteRequest * newDataPointWriteRequest );

	private:
		TA_Base_Bus::DataPointWriteQueue*	m_writeQ;
		std::vector< AtsPsdData* >			m_psdData;
	};
}
#endif // !defined(AFX_DATAPOINTCONTROLSTATEPROCESSOR_H__EBE02C7A_EE2E_4D7D_8686_3D76C43CF949__INCLUDED_)
