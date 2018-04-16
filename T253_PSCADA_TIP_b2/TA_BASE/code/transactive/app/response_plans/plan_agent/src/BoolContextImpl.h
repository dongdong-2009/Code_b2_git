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

#ifndef BOOL_CONTEXT_IMPL_H
#define BOOL_CONTEXT_IMPL_H

#include "bus/mathematical_evaluation/src/IBooleanContext.h"

namespace TA_Base_App
{

	class BoolContextImpl : public TA_Base_Bus::IBooleanContext
	{

	public:

		/**
		* Constructor, Destructor
		* 
		*/
		BoolContextImpl();
        BoolContextImpl( const std::map<std::string, std::string>& mapExchangingData );
		//StringContextImpl ( IExpressionNodeNotification * userOfExpression ); // for callback operation
		virtual ~BoolContextImpl();


		//
		// Operations required by IBooleanContext interface
		//


		virtual bool getValue(const std::string& name) const;
	private:
		std::map<std::string, std::string> m_mapExchangingData;
		
	};

} // close namespace TA_Base_App

#endif

