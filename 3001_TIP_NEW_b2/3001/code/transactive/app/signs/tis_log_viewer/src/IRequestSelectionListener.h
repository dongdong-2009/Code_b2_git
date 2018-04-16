/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_log_viewer/src/IRequestSelectionListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Implemented by the GUI to be notified when display request selection changes.
  */


#if !defined(IRequestSelectionListener_H)
#define IRequestSelectionListener_H


namespace TA_IRS_App
{
    class IRequestSelectionListener
    {

    public:

        enum RequestType
        {
            None,
            TtisFreeText,
            TtisPredefined,
            StisFreeText,
            StisPredefined,
            Ratis_In_New ,
            Ratis_Out_New ,
            Ratis_In_Update,
            Ratis_Out_Update,
            Ratis_In_Clear,
            Ratis_Out_Clear
        };

        /**
          * requestSelectionChanged
          *
          * Gets called when a display request is selected or deselected.
          *
          * @param requestType     The type of display request that is selected.
          * @param isReinstatable  True if the display request can be reinstated, false otherwise.
          */
        virtual void requestSelectionChanged(RequestType requestType, bool isReinstatable) = 0;

    };

} // TA_App

#endif // !defined(IRequestSelectionListener_H)
