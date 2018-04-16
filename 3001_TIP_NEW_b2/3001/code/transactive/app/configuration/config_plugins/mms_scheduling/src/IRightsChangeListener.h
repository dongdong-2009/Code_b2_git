/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/mms_scheduling/src/IRightsChangeListener.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Simple interface for receipt of rights changed notifications
  *
  */

namespace TA_IRS_App
{

    interface IRightsChangeListener
    {
    public:
    
        /**
         * onRightsChanged
         *
         * Called whenever there is a change in rights.
         *
         */
        virtual void onRightsChanged() = 0;
    };

} // end namespace TA_IRS_App
