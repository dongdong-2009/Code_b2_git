/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/IDateTimeListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
