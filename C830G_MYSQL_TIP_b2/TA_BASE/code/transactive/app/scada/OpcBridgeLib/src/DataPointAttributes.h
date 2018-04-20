/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcBridgeLib/src/DataPointAttributes.h $
  * @author  DarrenS
  * @version $Revision: #4 $
  *
  * Last modification : $DateTime: 2013/07/12 15:49:47 $
  * Last modified by : $Author: haijun.li $
  *
  * The DataPointAttributes class maintains a list of attributes currently requested by the OPC Client
  * for a particular datapoint
  *
  */

#ifndef DATAPOINTATTRIBUTES_H
#define DATAPOINTATTRIBUTES_H

#pragma warning ( disable:4786 )

#include "app/scada/OpcBridgeLib/src/AbstractDataObjectAttributes.h"
#include "app/scada/OpcBridgeLib/src/DisplayDataPointProxy.h"
namespace TA_Base_Bus
{
    class DisplayDataPointProxy;
}

namespace TA_Base_App
{
	class DataPointAttributes : public AbstractDataObjectAttributes
	{

	public:

		/**
		 * DataPointAttributes
		 *
		 * Constructor for the DataPointAttributes class
		 *
		 * @param 	corbaName		The CorbaName of the DataPoint
		 * @param 	opcTagManager	The manager for all OPC Tags
		 *
		 */
		DataPointAttributes(TA_Base_Core::IEntityData* entityData);

		DataPointAttributes(DataPointCacheData& opcDpData);

		virtual ~DataPointAttributes();

        static bool isAttributeNameValid(const std::string & attributeName, EOPCATTRIBUTETYPE& attrType);

		virtual EOPCATTRIBUTETYPE getAttributeType( std::string& attrName );

        virtual OPC_ENTITY_TYPE getObjectTypeNo();

		void		setStatus(unsigned long newStatus);

		bool		updateState(TA_Base_Bus::DataPointDisplayState& newState);

		_variant_t	getVariantValue();
		bool		isInAlarmState();
		WORD		getStatus();
		TA_Base_Bus::DisplayDataPointValue getValue();
		TA_Base_Bus::EScadaAlarmAckState getAlarmAckState();

	protected:
		virtual void getPrecreateAttributes(std::vector<AttrData>& attrs);

		virtual Attribute* newAttribute(const std::string& tagName, EOPCATTRIBUTETYPE attrType);

		virtual bool isConfigAvailable();

		void		 initDisplayState();

	protected:
		bool								  m_configAvaliable;
		TA_Base_Bus::DataPointDisplayState    m_state;
		TA_Base_Core::ReEntrantThreadLockable m_stateLock;
    };
}

#endif // #ifndef DATAPOINTATTRIBUTES_H
