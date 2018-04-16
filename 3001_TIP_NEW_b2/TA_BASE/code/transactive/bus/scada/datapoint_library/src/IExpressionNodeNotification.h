/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/IExpressionNodeNotification.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#ifndef IEXPRESSION_NODE_NOTIFICATION_H
#define IEXPRESSION_NODE_NOTIFICATION_H


namespace TA_Base_Bus
{

	class IExpressionNodeNotification
	{
	public:

		/**
		 * processExpressionChanges
		 *
		 * Call back when the expression node has changed.
		 *
		 */
		virtual void processExpressionChanges() = 0;
	};

}

#endif
