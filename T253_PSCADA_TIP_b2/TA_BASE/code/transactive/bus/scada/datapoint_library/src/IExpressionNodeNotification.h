/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/datapoint_library/src/IExpressionNodeNotification.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
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
