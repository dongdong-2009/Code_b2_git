/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/IRightsChangeListener.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Simple interface for receipt of rights changed notifications
  *
  */

interface IRightsChangeListener
{
public:

    /**
     * onRightsChanged
     *
     * Called whenever there is a change in rights.
     *
     * @param changedRightsGlobalIds an array of the rights that have been changed
     *          Each element represents the *global* identifier of the changed right, 
     *          as defined in AccessControlledActions (ie TA_IRS_Bus::aca_PA_BROADCAST_TRAIN_LIVE)
     *
     */
    virtual void onRightsChanged(const std::vector<unsigned long>& changedRightsGlobalIds) = 0;
};
