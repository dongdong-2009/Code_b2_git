/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/stis_manager/src/IPidSelectionListener.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
