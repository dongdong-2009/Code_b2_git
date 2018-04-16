/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/system_control/banner/pages/call_page/src/VettingManager.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Class to handle vetting requirements. It informs the TIS
  * Agent if vetting should be turned on or off, and listens
  * for messages from the TIS Agent in case the vetting status
  * changes.
  *
  */

#if !defined(AFX_VETTINGMANAGER_H__1C10B035_5B08_43A9_AD83_6E4BBBB6C0BD__INCLUDED_)
#define AFX_VETTINGMANAGER_H__1C10B035_5B08_43A9_AD83_6E4BBBB6C0BD__INCLUDED_

//#include "bus/signs/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/naming/src/NamedObject.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_IRS_App
{
    class IVettingCallback; // Foward declaration

    class VettingManager : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {
    public:

        /**
          * Constructor
          *
          * @param callback    The object to notify when the vetting state changes.
          * @param locationKey The location of the TIS Agent
          */
	    VettingManager(IVettingCallback& callback, unsigned long locationKey);


        /**
          * Destructor
          */
        virtual ~VettingManager();


        /**
          *  Called when an incoming RATIS message notification is received from the
          *  Message subsystem.
          */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);


        /** 
          * getVettingState
          *
          * Returns the current vetting state.
          *
          * @return  True if vetting is currently required, and false otherwise.
          */
        bool getVettingState()
        {
            return m_isVetting;
        }

        
        /** 
          * setVettingState
          *
          * Sets the new vetting state.
          *
          * @param vettingState  True if vetting is now required, and false otherwise.
          */
        void setVettingState(bool vettingState);

    private:

        // True if we are currently vetting, false otherwise.
        bool m_isVetting;

        // Object to notify if the vetting state changes.
        IVettingCallback& m_callback;
    };

} // namespace TA_IRS_App

#endif // !defined(AFX_VETTINGMANAGER_H__1C10B035_5B08_43A9_AD83_6E4BBBB6C0BD__INCLUDED_)
