/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/IDateTimeListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Implemented by the GUI to be notified date time controls change.
  *
  */


#if !defined(IDateTimeListener_H)
#define IDateTimeListener_H

namespace TA_IRS_App
{
    class CallbackDateTimeCtrl;
}


namespace TA_IRS_App
{
    class IDateTimeListener
    {

    public:

        /** 
          * dateTimeChanged
          *
          * Gets called when a date/time is changed.
          *
          * @param control   the control that was changed
          */
        virtual void dateTimeChanged(CallbackDateTimeCtrl* control) = 0;

    };

} // TA_IRS_App

#endif // !defined(IDateTimeListener_H)
