#pragma once

#include <string>

// ListPickerDialog dialog
class CWnd;
class IListPicker;

class AFX_EXT_CLASS ListPickerDialog
{
public:
    ListPickerDialog( IListPicker& refPicker, const std::string& strName, CWnd* pParent = NULL, const int nRow = 0 );   // standard constructor
	virtual ~ListPickerDialog();

public:
    int DoModal();

public:
    std::string Title;
    std::string DisplayValue;
    bool ListUpdated;

private:
    const int m_nRow;
    const std::string m_strName;
    CWnd* m_pParent;
    IListPicker& m_refPicker;
};
