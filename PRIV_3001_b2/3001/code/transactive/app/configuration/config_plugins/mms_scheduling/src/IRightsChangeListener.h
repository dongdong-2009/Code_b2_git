/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/mms_scheduling/src/IRightsChangeListener.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
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
