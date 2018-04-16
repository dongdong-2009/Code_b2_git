/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/ITrainBroadcastPage.h $
  * @author Jade Lee
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by: $Author: weikun.lin $
  * 
  * ITrainBroadcastPage simple interface which allows the model to determine where
  *     it needs to send state change notifications
  */

#if !defined(AFX_ITrainBroadcastPage_B3711608_2C1B_4371_9411_7B156F95E8B9__INCLUDED_)
#define AFX_ITrainBroadcastPage_B3711608_2C1B_4371_9411_7B156F95E8B9__INCLUDED_


#include "app/pa/PAManager/src/IGenericBroadcastPage.h"

namespace TA_IRS_Bus
{
    class TrainSelectorGUI;
}

class ITrainBroadcastPage : public IGenericBroadcastPage
{
public:

    /**
     * displayDvaStatusDialog
     *
     * Pops up a DVA status dialog window
     *
     * @param broadcastId the identifier that defines what information is shown in status dialog
     *
     */
    virtual void    displayDvaStatusDialog(const std::string& broadcastId) = 0;

    /**
     * displayLiveStatusDialog
     *
     * Pops up a live status dialog window
     *
     * @param broadcastId the identifier that defines what information is shown in status dialog
     *
     */
    virtual void    displayLiveStatusDialog(const std::string& broadcastId) = 0;

    /**
     * getTrainSelectorGUI
     *
     * Gets the underlying train selector
     */
    virtual TA_IRS_Bus::TrainSelectorGUI& getTrainSelectorGUI() = 0;
};

#endif // !defined(AFX_ITrainBroadcastPage_B3711608_2C1B_4371_9411_7B156F95E8B9__INCLUDED_)
