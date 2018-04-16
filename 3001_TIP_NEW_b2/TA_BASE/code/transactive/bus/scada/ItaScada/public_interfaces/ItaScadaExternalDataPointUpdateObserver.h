/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/ItaScada/public_interfaces/ItaScadaExternalDataPointUpdateObserver.h $
  * @author  Sean Liew
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2012/02/06 16:15:14 $
  * Last modified by : $Author: haijun.li $
  *
  * Implementation of the Interface ItaScadaExternalDataPointUpdateObserver
  *
  */

#if !defined(ItaScadaExternalDataPointUpdateObserver_6F7DA565_77E9_418c_8842_1125F6E1BBD2__INCLUDED_)
#define ItaScadaExternalDataPointUpdateObserver_6F7DA565_77E9_418c_8842_1125F6E1BBD2__INCLUDED_

/**
 * Interface to allow the observation of datapoint value and quality updates for
 * an externally hosted datapoint.
 * @version 1.0
 * @created 04-Mar-2005 10:31:29 AM
 */


#include "core/types/public_types/Constants.h"

namespace TA_Base_Bus
{
	class DpValue;

	class ItaScadaExternalDataPointUpdateObserver
	{

	public:
		/**
		 * Method to receive the datapoint updates
		 * @param name    Name of updated external datapoint
		 * @param value    The updated datapoint value.
		 * @param quality    The updated datapoint quality
		 * 
		 */
		virtual void onExternalDataPointUpdate(const std::string& name, const TA_Base_Bus::DpValue& dpValue, TA_Base_Core::EDataPointQuality quality) =0;

	};
}

#endif // !defined(ItaScadaExternalDataPointUpdateObserver_6F7DA565_77E9_418c_8842_1125F6E1BBD2__INCLUDED_)