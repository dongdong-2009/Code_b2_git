#if defined ( STEPTYPE_POPUP_MFT_MESSAGE )

#ifndef ACTIVEPOPUPMFTMESSAGESTEP_H_INCLUDED
#define ACTIVEPOPUPMFTMESSAGESTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePopupMFTMessageStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */

#include "core/naming/src/NamedObject.h"
#include "bus/scada/common_library/IDL/src/IDataPointAccessCorbaDef.h"
#include "app/response_plans/plan_agent/src/ActiveStep.h"


namespace TA_Base_Core
{
    class CommsMessageSender;
}

namespace TA_Base_App
{
	//Class declaration
	class ActivePopupMFTMessageStep : public ActiveStep
	{
	// Public methods
	public:

	    ActivePopupMFTMessageStep( const TA_Base_Core::StepDetail& stepDetail,
                                      const TA_Base_Core::PopupMFTMessageParameters& params,
						      IActivePlanFlow& flowControl );
        virtual ~ActivePopupMFTMessageStep();

	protected:
	    virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:

		ActivePopupMFTMessageStep& operator=( const ActivePopupMFTMessageStep& );
		ActivePopupMFTMessageStep( const ActivePopupMFTMessageStep& );

		const TA_Base_Core::PopupMFTMessageParameters m_params;

		TA_Base_Core::NamedObject<TA_Base_Bus::IDataPointAccessCorbaDef,
						 TA_Base_Bus::IDataPointAccessCorbaDef_ptr,
						 TA_Base_Bus::IDataPointAccessCorbaDef_var> m_dataPoint;

        void sendActiveStepUpdateMessage(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);

        unsigned short m_sendPopupMessageTryTimes;
        TA_Base_Core::CommsMessageSender* m_commsSender;
	}; // ActivePopupMFTMessageStep
}; // TA_Base_App

#endif // ACTIVEPOPUPMFTMESSAGESTEP_H_INCLUDED


#endif //  STEPTYPE_POPUP_MFT_MESSAGE

