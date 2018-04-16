/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Jade Lee
  * @version $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by: $Author$
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
