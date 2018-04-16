/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/trains/train_agent/src/ITrainManagerTrainEventFacade.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This interface provides access to the sendTrainCommand
  *
  */

///////////////////////////////////////////////////////////
//  ITrainManager.h
//  Implementation of the Interface ITrainManager
//  Created on:      26-Nov-2003 02:54:15 PM
///////////////////////////////////////////////////////////

#if !defined(ITrainManagerTEF_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_)
#define ITrainManagerTEF_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_


namespace TA_IRS_App
{
	class TrainEvent;

	/**
	 * This interface provides access to onTrainEvent()
	 *
	 * Used to re-inject SDS Events for further processing.
	 */
	class ITrainManagerTrainEventFacade
	{
		public:
            virtual void onTrainEvent(TrainEvent& event) = 0;
    };
};
#endif // !defined(ITrainManagerTEF_D5754A45_DFD3_467f_A35B_03604554F535__INCLUDED_)
