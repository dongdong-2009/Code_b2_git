/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/pa/PAManager/src/ChangeBroadcastId.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides a simple class that attempts to change a broadcast ID
  *     upon changing, cancel condition has been met (as in don't
  *     need to cancel operation anymore)
  *
  */

#pragma once

#include "app/pa/PAManager/src/IAsyncCancelCondition.h"
#include <string>

class DvaStatusModel;

class ChangeBroadcastId : public IAsyncCancelCondition
{
public:

    /**
     * Creates a new broadcast change object, which attempts to change the name
     *  of the broadcast in the input model to the target broadcast ID
     *
     * @param model the model of a broadcast status dialog that we're trying
     *          to change the name of
     *
     * @param newBroadcastId the target name for broadcast
     *
     */
    ChangeBroadcastId(DvaStatusModel& model, std::string newBroadcastId);

    /**
     * shouldCancel (interface implementation)
     * 
     * Attempts to change the broadcast ID
     *
     * @return true if the broadcast ID has been changed
     *
     */
    virtual bool shouldCancel();
    
    /**
     * successfullyChangedBroadcast
     *
     * @return true if this object has successfully changed the models broadcast ID name
     *
     */
    bool hasSuccessfullyChangedBroadcast();

private:

    DvaStatusModel& m_dvaStatusModel;
    std::string m_newBroadcastId;
    bool m_broadcastIdChangedSuccessfully;
};
