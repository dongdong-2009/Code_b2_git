/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/IMessageSelectionListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implemented by the GUI to be notified when message selection changes.
  *
  */

#if !defined(IMessageSelectionListener_H)
#define IMessageSelectionListener_H


namespace TA_IRS_App
{
    class IMessageSelectionListener
    {

    public:

        
        /** 
          * predefinedMessageSelected
          *
          * Called when the predefined message tab is selected.
          * Sets whether a message is selected and if so the details needed to
          * populate certain fields on the display page.
          *
          * This is also called when a message is selected/deselected.
          *
          * @param tabSwitched          only true when the tab has just been switched to
          * @param validMessageSelected
          * @param priority
          * @param repeatInterval
          *
          */
        virtual void predefinedMessageSelected( bool tabSwitched,
                                                bool validMessageSelected,
                                                unsigned short priority = 0,
                                                unsigned short repeatInterval = 0) = 0;


        /** 
          * adHocMessageSelected
          *
          * Called when the ad hoc message tab is selected.
          * Sets whether a message has been entered. 
          *
          * This is also called when a message is types/cleared.
          *
          * @param tabSwitched          only true when the tab has just been switched to
          * @param validMessageEntered
          * @param repeatInterval       only used to set the default when the tab is switched
          */
        virtual void adHocMessageSelected( bool tabSwitched,
                                           bool validMessageEntered,
                                           unsigned short repeatInterval ) = 0;


        /** 
          * ratisMessageSelected
          *
          * Called when the RATIS message tab is selected.
          * Sets whether a message has been entered. 
          *
          * This is also called when a message is typed/cleared.
          *
          * @param tabSwitched          only true when the tab has just been switched to
          * @param validMessageEntered
          */
        virtual void ratisMessageSelected( bool tabSwitched,
                                           bool validMessageEntered,
										   bool isClearMessage ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IMessageSelectionListener_H)
