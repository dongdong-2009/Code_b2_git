#ifndef DATA_NODE_ACCESS_HELP_H
#define DATA_NODE_ACCESS_HELP_H

#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeAccess.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "bus/scada/proxy_library/module/DefineFile.h"

#include "string.h"

namespace TA_Base_Bus
{
	class DataNodeAccessHelp : public IDataNodeAccess
	{
	public:
		DataNodeAccessHelp()
		{	
		}
		~DataNodeAccessHelp()
		{	
		}


		virtual boost::shared_ptr<DataNodeStaticConfiguration> getConfiguration() 
																{
																	DataNodeStaticConfiguration* pDnsc = NULL;
																	boost::shared_ptr<DataNodeStaticConfiguration> temp( pDnsc );
																	return temp;
																}

		virtual void changeDataNodeTag(const ETagState tagState, 
			const EOutputInhibitState outputInhibitState, 
			const EInputInhibitState inputInhibitState, 
			const std::string& ptwId, 
			const std::string& tagComment,
			const std::string& sessionId) {}

		virtual void setInhibitMms(const std::string& sessionId, bool inhibitMms) {}


		virtual void setDataNodeNotes( const std::string& sessionId,const std::string& notes) {}

		virtual unsigned short getFlexibleTimeOutValue() {return 0;}

	};

	class DataNodeProxyHelp : public IDataNodeProxy
	{
	public:
		~DataNodeProxyHelp()
		{
			if (g_pDefine->DataNodeAccessHelp_delete == false)
			{
				::std::string str = "test exception";
				TA_THROW(TA_Base_Core::ScadaProxyException(str));
			}		
		}

		virtual bool							isConfigValid()	const 							{};
		virtual unsigned long					getEntityKey() const							{return 0;};
		virtual std::string 	                getAgentName() const                            {std::string str = "str";return str;};
		virtual std::string 	                getEntityName() const                           {std::string str = "str";return str;};
		virtual unsigned long			        getLocationKey() const                          {return 0;};
		virtual unsigned long	                getSubSystemKey() const                         {return 0;};
		virtual unsigned long	                getParentKey() const                            {return 0;};
		virtual unsigned long	                getPhysicalSubSystemKey() const                 {return 0;};
		virtual const std::string&				getDescription() const							{std::string str = "str";return str;};
		virtual const std::string&				getLabel() const								{std::string str = "str";return str;};
		virtual const std::string&				getShortLabel() const							{std::string str = "str";return str;};
		virtual const std::string&				getAssetName() const							{std::string str = "str";return str;};
		virtual TA_Base_Core::SharedIEntityDataList           getChildDataPointEntities() const			
																					{
																						//typedef std::vector< boost::shared_ptr<IEntityData> > SharedIEntityDataList;
																						TA_Base_Core::SharedIEntityDataList temp;
																						return temp;
																					};
		virtual TA_Base_Core::SharedIEntityDataList           getChildDataNodeEntities() const			{TA_Base_Core::SharedIEntityDataList temp;return temp;};
		virtual bool							isEquipmentNode() const							{return false;};

		// Accessors for Tag State
		virtual bool							isTagged() const								{return false;};
		virtual TA_Base_Bus::ETagState					    getTaggingState() const							{TA_Base_Bus::ETagState temp;return temp;};
		virtual TA_Base_Bus::InhibitState					getTagInhibitState() const						{TA_Base_Bus::InhibitState temp ;return temp;};
		virtual std::string						getPtwId() const								{std::string str = "str";return str;};
		virtual std::string						getTagComment() const							{std::string str = "str";return str;};
		virtual void							setTagState(const TA_Base_Bus::ETagState tagState, 
			const TA_Base_Bus::EOutputInhibitState outputInhibitState,
			const TA_Base_Bus::EInputInhibitState inputInhibitState,
			const std::string& ptwId,
			const std::string& tagComment,
			const std::string& sessionId)			{};

		virtual std::string						getLastStatusChangeSessionId()  const			{std::string str = "str";return str;};
		virtual unsigned long					getLastStatusChangeTimestamp() const			{return 0;};

		// Accessors for Notes
		virtual void							setNotes(const std::string& notes, 	std::string& sessionId)				{};
		virtual std::string						getNotes() const								{std::string str = "str";return str;};

		// Accessors for MMS Inhibit
		virtual void							setInhibitMms(bool inhibitMms,		const std::string& sessionId)		{};
		virtual bool							getInhibitMms() const							{return true;};

		//
		// DataNode Summaries
		//

		// Accessors for Alarm Summary
		virtual TA_Base_Bus::BooleanTag			getAlarmSummary() const							{TA_Base_Bus::BooleanTag temp;return temp;};
		virtual TA_Base_Bus::EnumTag			getAlarmAckStateSummary() const					{TA_Base_Bus::EnumTag temp;return temp;};

		// Accessors for Inhibit summary
		virtual TA_Base_Bus::BooleanTag			getControlInhibitSummary() const				{TA_Base_Bus::BooleanTag temp;return temp;};
		virtual TA_Base_Bus::BooleanTag			getAlarmInhibitSummary() const					{TA_Base_Bus::BooleanTag temp;return temp;};
		virtual TA_Base_Bus::BooleanTag			getScanInhibitSummary() const					{TA_Base_Bus::BooleanTag temp;return temp;};
		virtual TA_Base_Bus::BooleanTag			getMmsAlarmInhibitSummary() const				{TA_Base_Bus::BooleanTag temp;return temp;};

		// Accessor for Override Summary
		virtual TA_Base_Bus::BooleanTag			getOverrideSummary() const						{TA_Base_Bus::BooleanTag temp;return temp;};

		// Accessor for Quality Summary
		virtual TA_Base_Bus::EnumTag			getQualitySummary() const						{TA_Base_Bus::EnumTag temp;return temp;};

		// Accessor for Control Pending Summary
		virtual TA_Base_Bus::BooleanTag			getControlPendingSummary() const				{TA_Base_Bus::BooleanTag temp;return temp;};
	};
}
#endif //DATA_NODE_ACCESS_HELP_H
