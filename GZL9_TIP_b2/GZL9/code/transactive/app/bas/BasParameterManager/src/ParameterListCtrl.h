#if !defined(TA_GF_3002_app_bas_BasParameterManager_CParameterListCtrl)
#define TA_GF_3002_app_bas_BasParameterManager_CParameterListCtrl
#pragma once

#include "app/bas/BasParameterManager/src/CommonDefine.h"
#include "app/bas/BasParameterManager/src/EquipmentManager.h"
namespace TA_IRS_App
{
	// CParameterListCtrl
	class EquipmentManager;
	class CParameterListCtrl : public CListCtrl
								,public IEquipmentUpdateInterface
	{
		DECLARE_DYNAMIC(CParameterListCtrl)

	public:
		CParameterListCtrl();
		virtual ~CParameterListCtrl();
	public:
		// the IEquipentUpdateInterface
		void UpdateDatapointValue(unsigned long datapointKey, const std::string& newValue);
		void UpdateDatapointStatus(unsigned long datapointKey, const std::string& strStatus);

		void setEquipmentValue(unsigned long locationKey, unsigned long typeKey, unsigned long entityKey, double value);

		bool getEquipmentLimitedValue(unsigned long locationKey, unsigned long typeKey, unsigned long entityKey, double& lowValue, double& highValue);

		void setExecuteStatus( unsigned long locationKey,unsigned long typeKey, unsigned long entityKey, const std::string& strStatus);

	public:
		void setupListCtrl();
		void refreshParameterValues(unsigned long locationKey, unsigned long valueType);
		void prepareToClose();
	protected:
		DECLARE_MESSAGE_MAP()
	protected:
		DWORD setRowFullSelect();
	public:
		static const std::string ColumnName_Location;
		static const std::string ColumnName_ValueType;
		static const std::string ColumnName_ValueTypeTag;
		static const std::string ColumnName_Value;
		static const std::string ColumnName_Status;
		static const int ColumnIndex_Location;
		static const int ColumnIndex_ValueType;
		static const int ColumnIndex_ValueTypeTag;
		static const int ColumnIndex_Value;
		static const int ColumnIndex_Status;

		static const std::string m_Status_Success;
		static const std::string m_Status_Processing;
		static const std::string m_Status_LCC_Failed;
		static const std::string m_Status_Comm_Failed;
		static const std::string m_Status_Failed;
	private:
		static unsigned long m_currentLocationKey;
		static unsigned long m_currentValueTypeKey;

		std::auto_ptr<EquipmentManager> m_equipmentManager;

		volatile bool m_isPrepareToExist;
    public:
        //afx_msg void OnSize(UINT nType, int cx, int cy);
    };

}

#endif //TA_GF_3002_app_bas_BasParameterManager_CParameterListCtrl
