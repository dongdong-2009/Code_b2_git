#pragma once

#include "bus\mfc_extensions\src\auto_sort_list_ctrl\autosortlistctrl.h"

class PaScheduleBroadcastListCtrl : public TA_Base_Bus::AutoSortListCtrl 
{
public:
    PaScheduleBroadcastListCtrl(void);
    virtual ~PaScheduleBroadcastListCtrl(void);

    void insertNewSchedule( const std::string& strName, time_t tmStartTime, time_t tmEndTime, int nFrequencyType, int nFrequencyDetail, bool bAutoDel );

protected:
    virtual void PreSubclassWindow();
    virtual afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

    CFont m_fntList;
};
