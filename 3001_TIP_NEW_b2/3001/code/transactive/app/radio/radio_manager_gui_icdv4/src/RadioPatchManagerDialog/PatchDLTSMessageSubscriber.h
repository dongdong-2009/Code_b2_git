/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioPatchManagerDialog/PatchDLTSMessageSubscriber.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */

#if !defined(AFX_PatchDLTSMessageSubscriber_H__B2453CFA_7548_454D_A58C_478D204A05CB__INCLUDED_)
#define AFX_PatchDLTSMessageSubscriber_H__B2453CFA_7548_454D_A58C_478D204A05CB__INCLUDED_


#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

class PatchDLTSMessageSubscriber : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
{
	public:

        /**
         *  Constructor/Destructor
         */
		PatchDLTSMessageSubscriber();
		virtual ~PatchDLTSMessageSubscriber();

        /**
         *  Called when a Telephone notification is received from the
         *  Message subsystem.
         */
        virtual void receiveSpecialisedMessage(
            const TA_Base_Core::CommsMessageCorbaDef& message);		

	private:

		void initialise();


	private:

		static TA_Base_Core::ReEntrantThreadLockable m_lock;
		
		static const std::string TELEPHONE_SERVER_AGENT_ENTITY_NAME;
};

#endif 
