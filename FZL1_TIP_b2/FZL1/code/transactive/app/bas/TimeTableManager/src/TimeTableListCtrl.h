#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableListCtrl)
#define Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableListCtrl
#if _MSC_VER > 1000
#pragma once
#endif
#include "bus/mfc_extensions/src/editable_list_control/EditableListCtrl.h"
#include "app/bas/TimeTableManager/src/TimeTableDatapointPairManager.h"
namespace TA_IRS_App
{
	const int TimeTable_Column_Index_mode = 0;
	const int TimeTable_Column_Index_BeginTime = 1;

	class ILocationFilter
	{
	public:
		virtual unsigned long getLocationKey() = 0;
	};

	// TimeTableListCtrl
	#define ERROR_ITEM_DATA_VALUE 0xFFFFFFFF
	class TimeTableListCtrl : public CEditableListCtrl, ICellManager, IModeBeginTimeUpdateInterface
	{
		DECLARE_DYNAMIC(TimeTableListCtrl)

	public:
		TimeTableListCtrl();
		virtual ~TimeTableListCtrl();

	public:
		void setupList();
		void resizeHeaderCtrl();
		ILocationFilter* setLocationFilter(ILocationFilter* locationFilter);
        std::string formatNameString(const std::string& strModeName, const std::string& strModeDesc);
        std::string formatTimeString(const time_t tmTime);
		CString updateItemChange(CString strNewValue, bool isNew);
		bool isItemChanged(int item, bool isNew);
		bool isItemChanged(const std::string& strNewValue);
		std::string getOriginalModeNameString(const std::string& strNewValue, bool isNew);
		std::string getOriginalModeBeginTime(const std::string& strNewTime);
		CString getOriginalModeShortNameString(const CString& strNewValue);
		void updateModeNameToNormal();
		void canEditModeList(bool bEditable);
		void loadTimeTableFromAgent(unsigned long locationKey, bool todayTimeTable);
        void checkAndAppendInsertionItem();
        bool isInsertionItem( const int nItem );

	public:
		// ------------------------ The implement of IModeBeginTimeUpdateInterface ------------------------------
		void updateModeBeginTime(const unsigned long locationKey, const unsigned long entityKey);
	public:
		// ------------------------ The implement of ICellManager ------------------------------
		virtual ICellManager::CellType GetCellType(CEditableListCtrl* listCtrl, int item, int subItem);

		// Return the type of the specified table cell. For unlimited number of characters this method
		// should return 0. This method is only relevant to CT_TEXT and CT_NUMBER field types.
		virtual unsigned long GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem);

		// Return the set of values allowed for the specified table cell. Used with CT_SELECTION.
		virtual ICellManager::CellData GetCellData(CEditableListCtrl* listCtrl, int item, int subItem);

		// Return the mask to be applied to values entered into the specified table cell. Used with CT_MASK.
		// The mask is formatted so that each changeable character is inside the brackets, and all static 
		// characters (characters not entered by the user, such as a dash or a slash) are outside the brackets. 
		// Dashes can be used inside the brackets to indicate a range of characters (based on the ASCII value 
		// of the start and end character).
		// e.g. "[AB][0-9][0-9]-[0-9][0-9][5-9][A-Z]"
		virtual CString GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem);

		// Return the character to be used as a place holder for masked input. Used with CT_MASK.
		// e.g. Givem '_' as the prompt char, an empty masked edit cell using the above mask would contain "___-____".
		virtual char GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem);

		// Indicate whether the specified table cell can be updated to the specified value.
		virtual bool UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value);
		// ------------------------ The implement of CEditableListCtrl ------------------------------
	protected:
		afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM);
        afx_msg LRESULT OnRefresh(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnUpdateItem(WPARAM wParam, LPARAM lParam);
		afx_msg void MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
		DECLARE_MESSAGE_MAP()
	protected:
		DWORD setRowFullSelect();
        void insertModeTime(unsigned long entityKey, unsigned long locationKey, unsigned long modeValue, time_t strBeginTime);
	private:
		bool checkRowColumn(int item, int subItem);
	private:
        bool m_isTodayTimetable;
        bool m_canEditable;
        ILocationFilter* m_locationFilter;
	};

}


#endif //Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableListCtrl