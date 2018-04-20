/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_log_viewer/src/IRequestSelectionListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
