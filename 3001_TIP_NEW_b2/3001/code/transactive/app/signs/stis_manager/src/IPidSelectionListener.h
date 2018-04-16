/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/IPidSelectionListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Implemented by the GUI to be notified when PID selection changes.
  *
  */


#if !defined(IPidSelectionListener_H)
#define IPidSelectionListener_H


namespace TA_IRS_App
{
    class IPidSelectionListener
    {

    public:

        
        /** 
          * pidSelectionChanged
          *
          * Gets called when the PID selection is changed.
          *
          * @param pidsInList   true if at least one PID is in the list
          * @param pidSelectionExists   true if one PID in the list is selected
          */
        virtual void pidSelectionChanged(bool pidsInList, bool pidSelectionExists) = 0;

    };

} // TA_IRS_App

#endif // !defined(IPidSelectionListener_H)
