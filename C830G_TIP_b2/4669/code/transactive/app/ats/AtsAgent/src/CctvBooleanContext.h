/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/CctvBooleanContext.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	Boolean Context...Required by the Mathematical_Evaluation library
  *
  */

#ifndef CCTVBOOLEANCONTEXT_H
#define CCTVBOOLEANCONTEXT_H

#include "bus/mathematical_evaluation/src/IBooleanContext.h"

namespace TA_IRS_App
{
	class AtsCctvData;

	class CctvBooleanContext : public TA_Base_Bus::IBooleanContext
	{

	public:

		/**
		* Constructor, Destructor
		* 
		*/
		
		CctvBooleanContext ( AtsCctvData& m_cctvData );
		virtual ~CctvBooleanContext();


		//
		// Operations required by ICctvBooleanContext interface
		//


		virtual bool getValue ( const std::string & variableName ) const;

	private:

		CctvBooleanContext();

	protected:

		AtsCctvData&	m_cctvData;

		
	};

} // close namespace TA_Bus

#endif

