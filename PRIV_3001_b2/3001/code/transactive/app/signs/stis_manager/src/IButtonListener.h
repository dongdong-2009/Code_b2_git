/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/stis_manager/src/IButtonListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Implemented by the GUI to be notified when PID selection changes.
  *
  */


#if !defined(IButtonListener_H)
#define IButtonListener_H

namespace TA_IRS_App
{
    class CallbackButton;
}


namespace TA_IRS_App
{
    class IButtonListener
    {

    public:

        /** 
          * buttonPressed
          *
          * Gets called when a button is pressed.
          *
          * @param button   the button that was pressed
          */
        virtual void buttonPressed(CallbackButton* button) = 0;

    };

} // TA_IRS_App

#endif // !defined(IButtonListener_H)
