/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/TrainDvaStatusModel.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * TrainDvaStatusModel controls additional layer of business logic for DVA
  * specific broadcast dialog
  *
  */

#if !defined(TrainDvaStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define TrainDvaStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include "app/trains/TrainBorne_PA/src/GenericStatusPage.h"
#include "app/trains/TrainBorne_PA/src/DvaStatusModel.h"
	
class TrainDvaStatusModel : public DvaStatusModel
{
public:
    /**
     * TrainDvaStatusModel (constructor)
     *
     * @param page the view associated with this model
     *
     * @param broadcastId the broadcastId to be associated with this status model
     *
     */
    TrainDvaStatusModel(GenericStatusPage& page, const std::string& broadcastId);
       
    virtual ~TrainDvaStatusModel();

    /**
     * setHasOverrideOption (simple set function)
     */
    void setHasOverrideOption(bool hasOption);

    void removeAnnouncement();

protected:
    /**
     * getHasOverrideOption 
     *
     * @see DvaStatusModel::getHasOverrideOption base class for details
     *
     */
    virtual bool getHasOverrideOption();

private:
    // True if model is currnetly in override mode
    bool    m_hasOverrideOption;

};
#endif // !defined(TrainDvaStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
