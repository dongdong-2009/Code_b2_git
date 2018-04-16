#pragma once

#include <string>

// TreePickerDialog dialog
class CWnd;
class ITreePicker;

class AFX_EXT_CLASS TreePickerDialog
{
public:
    TreePickerDialog( ITreePicker& refPicker, const std::string& strName, CWnd* pParent = NULL, const int nRow = 0 );   // standard constructor
	virtual ~TreePickerDialog();

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
    ITreePicker& m_refPicker;
};
