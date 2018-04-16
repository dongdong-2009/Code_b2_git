/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/ITrainBroadcastPage.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * ITrainBroadcastPage simple interface which allows the model to determine where
  *     it needs to send state change notifications
  */

#if !defined(AFX_ITrainBroadcastPage_B3711608_2C1B_4371_9411_7B156F95E8B9__INCLUDED_)
#define AFX_ITrainBroadcastPage_B3711608_2C1B_4371_9411_7B156F95E8B9__INCLUDED_


#include "app/trains/TrainBorne_PA/src/IGenericBroadcastPage.h"

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

    virtual void updateTrainLiveStaticGroupNames( const std::vector<std::string>& trainListNames ) = 0;

};

#endif // !defined(AFX_ITrainBroadcastPage_B3711608_2C1B_4371_9411_7B156F95E8B9__INCLUDED_)
