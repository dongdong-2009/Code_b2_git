/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/CommonContext.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	Base class for BooleanContext and RealNumberContext...
  *
  */

#ifndef COMMON_CONTEXT_H
#define COMMON_CONTEXT_H

#include <vector>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


namespace TA_Base_Bus
{
	class IExpressionNodeNotification;
    class EventDetailsFactory;
};


namespace TA_Base_Bus
{
	class InputAssociation;
};


namespace TA_Base_Bus
{

	class CommonContext : public IEntityUpdateEventProcessor
	{
	public:

		/**
		* Constructor, Destructor
		* 
		*/
		CommonContext();
		CommonContext ( IExpressionNodeNotification * userOfExpression );
		virtual ~CommonContext();


		//
		// operations specific to IEntityUpdateEventProcessor
		//


		virtual void processEntityUpdateEvent ( unsigned long entityKey, ScadaEntityUpdateType updateType );


		//
		// operations specific to CommonContext
		//


		/**
		*
		* enableCallback
		*
		* indicate if the online changes of the input entities associated with the
		* mathematical expression will be notifiying the user/owner of the expression
		*
		* @param    enabled		Indication if callback mechanism is enabled
		*
		*/
		virtual void enableCallback ( bool enabled );

		
		/**
		*
		* getCombinedStatus
		*
		* calculate and return the combined status from all input entities associated
		* with the mathematical expression.
		*
		*/
		virtual EQualityStatus getCombinedStatus() const;


		/**
		*
		* isGoodCombinedStatus
		*
		* return true if the combined status from all input entities associated
		* with the mathematical expression are either QUALITY_GOOD_NO_SPECIFIC_REASON
		* or QUALITY_GOOD_LOCAL_OVERRIDE, false otherwise
		*
		*/
		virtual bool isGoodCombinedStatus() const;


		/**
		*
		* getExpressionResultString
		*
		* Returns a string containing the result of the expression evaluation in
		* the following format:
		*
		* [PointDescription1] is [Value1], [PointDescription2] is [Value2], ...
		*
		*/
		virtual std::string getExpressionResultString();


	protected:


		virtual bool getBooleanValue ( const std::string& variableName );
		virtual double getDoubleValue ( const std::string& variableName );


	private:
		InputAssociation * findInputDependency ( const std::string & inputName );
		InputAssociation * createInputDependency ( const std::string & inputName );

        // Disable copy constructor and assignment operator
        CommonContext ( const CommonContext & obj );
		CommonContext & operator= ( const CommonContext & rhs );

	private:

		std::vector < InputAssociation * >  m_listOfInputDependencies;
		IExpressionNodeNotification *       m_userOfExpression;
		bool                                m_toCallback;
        std::string							m_invalidDependencyErrorString;
        //TD16581, lock for list of input dependencies
        TA_Base_Core::ReEntrantThreadLockable m_lock;
        //TD16211, use EventDetailFactory to retrieve asset_name of datapoint
        EventDetailsFactory *						m_eventDetailsFactory;
    };

} // close namespace TA_Base_Bus

#endif

