/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution in any form.
*
* Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/IMessageSelectionListener.h $
* @author:   zhou yuan
* @version:  $Revision: #1 $
*
* Last modification: $DateTime: 2018/03/14 13:51:20 $
* Last modified by:  $Author: lim.cy $
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
