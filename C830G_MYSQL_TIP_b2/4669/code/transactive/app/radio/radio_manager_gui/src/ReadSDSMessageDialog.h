/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/ReadSDSMessageDialog.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides class to view and (optionally) confirm/acknowledge SDS Messages.
  * In accordance with TES #497:
  * SDS Messages should be displayed in a non-modal dialog.
  * Only 1 SDS Message can be displayed at a time.
  * If attempt to display a second SDS Message, the first is automatically cancelled
  *     (that is, NOT acknowledged, so it remains in call stack)
  *
  * The associated resource has the capacity to display 140 'W' characters within
  * the text area.  If need more than this, need to expand size of the 
  * IDD_VIEW_TEXT_MESSAGE resource
  *
  * The message is shown from a modeless dialog box, only call methods from the 
  * primary windows thread (this is not a threadsafe class, shouldn't need to be)
  *
  * We don't need to worry about this call getting acknowledged from an external
  * source (before we acknowledge it from the dialog), because for us to
  * be viewing the SDS message, we must own it (and will be the only party that
  * can acknowledge it)
  */

#pragma once

#include "resource.h"

namespace TA_IRS_App
{
    class ReadSDSMessageDialog 
        : 
        public CDialog
    {
    public:

        /**
         * Should instantiate this class from the main thread, doesn't require
         *  it's own thread (uses modeless dialog box).
         */
        ReadSDSMessageDialog();

        /**
         * displays a new SDS Message, as defined by the input call ID.  If there
         *  is already an SDS message being displayed, that will be hidden (and new
         *  one displayed).
         *
         * @param callID defines the SDS call ID containing the call details
         */
        void displaySDSMessage(long callID);

    protected:

        virtual void OnOK();
        virtual void OnCancel();

    private:

        /**
         * Confirms that class is being operated from the main thread (as is not threadsafe)
         */
        void confirmInMainThread();

        long m_callID;
    };

}   // end namespace TA_IRS_App
