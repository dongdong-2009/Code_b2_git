#ifndef IEDITLISTOWNER_INCLUDE
#define IEDITLISTOWNER_INCLUDE

#include <string>

class IEditListOwner
{
public:
    virtual void valueChanged( const int nId, const int nRow, const int nCol, const std::string& strDisplayValue ) = 0;
    virtual void booleanValueChanged( const int nId, const int nRow, const int nCol, const bool bNewValue, std::string& strDisplayValue ) = 0;
    virtual void timeValueChanged( const int nId, const int nRow, const int nCol, const time_t tmNewValue, std::string& strDisplayValue ) = 0;
    virtual void getBooleanValue( const int nId, const int nRow, const int nCol, bool& bCurValue ) = 0;
    virtual void getTimeValue( const int nId, const int nRow, const int nCol, time_t& tmCurValue ) = 0;
    virtual void getStringValue( const int nId, const int nRow, const int nCol, std::string& strCurValue, size_t& szMaxLength ) = 0;
    virtual void popupList( const int nId, const int nRow, const int nCol ) = 0;
    virtual void popupTree( const int nId, const int nRow, const int nCol ) = 0;
    virtual void popupGrid( const int nId, const int nRow, const int nCol ) = 0;
    virtual void popupText( const int nId, const int nRow, const int nCol ) = 0;
	virtual bool isRowEditable( const int nRow ) { return true; }
};

#endif