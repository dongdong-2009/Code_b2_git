/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution in any form.
*
* Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/IMessageSelectionListener.h $
* @author:   zhou yuan
* @version:  $Revision: #2 $
*
* Last modification: $DateTime: 2012/02/06 16:15:14 $
* Last modified by:  $Author: haijun.li $
*
* Implemented by the GUI to be notified when message selection changes.
*
*/

#if !defined(IMessageSelectionListener_H)
#define IMessageSelectionListener_H

#include "app/signs/ttis_manager/src/DisplayAttributeManager.h"

namespace TA_IRS_App
{
    class IMessageSelectionListener
    {
    public:
        virtual DisplayAttributeManager* getDisplayAttributeManager() = 0;
        virtual void messageSelectionChanged(bool messageIsSelected) = 0;
    };

} // TA_IRS_App

#endif // !defined(IMessageSelectionListener_H)
