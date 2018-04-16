#ifndef EDSV_DN_UPDATE_REQUEST_H
#define EDSV_DN_UPDATE_REQUEST_H

#include <string>

#include "app/scada/EquipmentStatusViewer/src/CustomDefines.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_App
{
	class DnUpdateRequest
	{

	public:
		DnUpdateRequest( unsigned long entityKey, Node_Item nodeItem)
		:
        m_entityKey( entityKey ),
        m_nodeItem(nodeItem)
		{
		};

		~DnUpdateRequest()
		{
		};

		bool operator==( DnUpdateRequest &other )
		{
			return (m_entityKey == other.m_entityKey && m_nodeItem == other.m_nodeItem);
		};

		unsigned long getEntityKey()
		{
			return m_entityKey;
		}

		Node_Item getNodeItem()
		{
			return m_nodeItem;
		}
	private:

		unsigned long			m_entityKey;

		Node_Item				m_nodeItem;
	};
}
#endif	
