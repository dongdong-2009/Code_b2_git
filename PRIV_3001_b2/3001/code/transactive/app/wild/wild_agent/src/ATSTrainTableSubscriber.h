/* 
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/3001/transactive/app/wild/wild_agent/src/ATSTrainTableSubscriber.h $
 * @author:  <Andy Siow>
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * This class is a dedicated message subscriber for the ATS Train Information.
 */

#if !defined(AFX_ATSTRAINTABLESUBSCRIBER_H__39256061_613C_451F_BC0B_1231120EB78A__INCLUDED_)
#define AFX_ATSTRAINTABLESUBSCRIBER_H__39256061_613C_451F_BC0B_1231120EB78A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

namespace TA_IRS_App
{
	//Forward declaration
	class WILDAgent;

	class ATSTrainTableSubscriber 
	: public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef> 
	{
	public:
		ATSTrainTableSubscriber();
		virtual ~ATSTrainTableSubscriber();

		virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef&  message);

	private:
		// Disable the copy constructor and assignment operator so the compiler won't create its own version.
		// Reference: Effective C++
		ATSTrainTableSubscriber( const ATSTrainTableSubscriber& ATSTrainTableSubscriber);
		ATSTrainTableSubscriber& operator=(const ATSTrainTableSubscriber &);

		//
		// getATSAgentKey
		//
		// This method returns the ATS Agent entity key given the type and location.
		//
		// @return Returns the entity key for the ATS Agent. 
		//
		// @exception Throws InvalidWILDConfigurationException
		// 
		unsigned long getATSAgentKey() const;

		// Member variables
		unsigned long m_atsLocationKey;
	};
} //TA_App

#endif // !defined(AFX_ATSTRAINTABLESUBSCRIBER_H__39256061_613C_451F_BC0B_1231120EB78A__INCLUDED_)
