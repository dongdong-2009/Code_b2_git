/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/pa/PAManager/src/TrainDvaStatusModel.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * TrainDvaStatusModel controls additional layer of business logic for DVA
  * specific broadcast dialog
  *
  */

#if !defined(TrainDvaStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define TrainDvaStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include "app/pa/PAManager/src/DvaStatusPage.h"
#include "app/pa/PAManager/src/DvaStatusModel.h"
	
class TrainDvaStatusModel
: 
public DvaStatusModel
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
    void    setHasOverrideOption(bool hasOption) { m_hasOverrideOption = hasOption; }

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
