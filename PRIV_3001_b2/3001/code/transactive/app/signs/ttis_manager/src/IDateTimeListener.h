/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/IDateTimeListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
