#pragma once

namespace TA_IRS_App
{

// select row and auto adjudge the column width
// BASBaseListCtrl
class BASBaseListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(BASBaseListCtrl)

public:
	BASBaseListCtrl();
	virtual ~BASBaseListCtrl();
public:
	virtual void resizeLastColumnWidth();
	virtual void StartTimers();
	virtual void StopTimers();
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
private:
	DWORD setRowFullSelect();
	DWORD setDisplayGrid(bool bShow = true);
	void resizeColumn();
	
private:
	bool m_rowFullSelectFlag;
};

}