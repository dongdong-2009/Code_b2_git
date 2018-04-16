/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution in any form.
*
* Source:    $File: //depot/4669_TIP/4669/transactive/app/signs/ttis_manager/src/IMessageSelectionListener.h $
* @author:   zhou yuan
* @version:  $Revision: #1 $
*
* Last modification: $DateTime: 2008/10/31 12:15:01 $
* Last modified by:  $Author: grace.koh $
*
* Implemented by the GUI to be notified when message selection changes.
*
*/

#pragma once

#include "DisplayAttributeManager.h"

namespace TA_IRS_App
{
    class IMessageSelectionListener
    {
    public:
        virtual DisplayAttributeManager* getDisplayAttributeManager() = 0;
        virtual void messageSelectionChanged( bool messageIsSelected ) = 0;
    };

} // TA_IRS_App

