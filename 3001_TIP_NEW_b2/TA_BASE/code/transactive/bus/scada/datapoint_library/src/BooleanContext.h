/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/BooleanContext.h $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2012/02/06 16:15:14 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	Boolean Context...Required by the Mathematical_Evaluation library
  *
  */

#ifndef BOOLEAN_CONTEXT_H
#define BOOLEAN_CONTEXT_H

#include "bus/mathematical_evaluation/src/IBooleanContext.h"
#include "bus/scada/datapoint_library/src/CommonContext.h"

namespace TA_Base_Bus
{

	class BooleanContext : public TA_Base_Bus::IBooleanContext,
						   public CommonContext
	{

	public:

		/**
		* Constructor, Destructor
		* 
		*/
		BooleanContext();
		BooleanContext ( IExpressionNodeNotification * userOfExpression ); // for callback operation
		virtual ~BooleanContext();


		//
		// Operations required by IBooleanContext interface
		//


		virtual bool getValue ( const std::string & variableName ) const;
		
	};

} // close namespace TA_Base_Bus

#endif

