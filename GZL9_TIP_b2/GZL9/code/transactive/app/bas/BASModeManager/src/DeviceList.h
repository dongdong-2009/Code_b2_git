#pragma once

#include "app/bas/BASModeManager/src/CommonDef.h"
#include "app/bas/BASModeManager/src/BASBaseListCtrl.h"
#include "app/bas/BASModeManager/src/BasManagerDataHelper.h"
#include "app/bas/BASModeManager/src/DeviceEquipmentManager.h"

namespace TA_IRS_App
{
	// CDeviceList
	class CDeviceList : public BASBaseListCtrl
						,public IEquipmentUpdateInterface
	{
		DECLARE_DYNAMIC(CDeviceList)
		//like++. GF-78.
		typedef struct _data_point_info{
			unsigned long	deviceKey;
			std::string		newValue;
			bool			status;

		}DATAPOINTINFO, *PDATAPOINTINFO;
	public:
		CDeviceList();
		virtual ~CDeviceList();
	public:
		// the IEquipmentUpdateInterface
		void UpdateDatapointValue(unsigned long deviceKey, const std::string& newValue, bool status);

		// do message update the data point value. like++ GF-78.
		afx_msg LRESULT OnUpdateDatapointValue(WPARAM wParam, LPARAM lParam);

		void Initialise();
		void fillwithModeDevice(unsigned long locationKey, unsigned long modeKey);

		bool stopFillwithModeDevice();
	protected:
		void setupListCtrl();
		bool isDeviceRunningFailed(int item);
		void refreshItemGUI(int item);
		int getItemIndexByData(unsigned long data);
	protected:
		afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
		DECLARE_MESSAGE_MAP()
	private:
		static const unsigned long m_COLUMN_INDEX_DEVICENUM;
		static const unsigned long m_COLUMN_INDEX_DEVICENAME;
		static const unsigned long m_COLUMN_INDEX_AUTOMANUAL;
		static const unsigned long m_COLUMN_INDEX_CURRENTSTATUS;
		static const unsigned long m_COLUMN_INDEX_EXPECTSTATUS;

		static const std::string m_HEADER_DEVICENUM;
		static const std::string m_HEADER_DEVICENAME;
		static const std::string m_HEADER_AUTOMANUAL;
		static const std::string m_HEADER_CURRENTSTATUS;
		static const std::string m_HEADER_EXPECTSTATUS;

		volatile bool m_isStoploading;

		unsigned long m_locationKey;
		unsigned long m_modeKey;
	};
}


