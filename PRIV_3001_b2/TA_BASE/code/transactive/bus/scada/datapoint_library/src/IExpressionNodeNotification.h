/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/scada/datapoint_library/src/IExpressionNodeNotification.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
