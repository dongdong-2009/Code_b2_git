#ifndef IACTIVITYPARAMETER_INCLUDE
#define IACTIVITYPARAMETER_INCLUDE

#include <string>
#include <vector>

struct TreeItem
{
    bool HasChild;
    unsigned int ID;
    unsigned int ParentID;
    std::string Item;
};

struct ListItem
{
    unsigned int ID;
    std::string Item;
};

struct GridItem
{
    // Row and Column starts from 0
    int Row;
    int Column;
    std::string Item;
};

struct GridColumn
{
    int Type;
    std::string Name;
    std::string DisplayName;
};

struct ItemPair
{
    int Type;
    unsigned int Order;
    bool Valid;
    std::string Name;
    std::string DisplayName;
    std::string Item;
};

class IListPicker
{
public:
    virtual void getInputListItems( const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0 ) = 0;
    // return true if parameter list (size/name and other values) update required
    virtual bool pickFromList( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0 ) = 0;
};

class ITreePicker
{
public:
    virtual void getInputTreeItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0 ) = 0;
    virtual void getChildItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId ) = 0;
    // return true if parameter list (size/name and other values) update required
    virtual bool pickFromTree( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0 ) = 0;
};

class IBoolEditor
{
public:
    // return true if parameter list (size/name and other values) update required
    virtual bool updateBooleanValue( const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 ) = 0;
    virtual void getBooleanValue( const std::string& strName, bool& bValue, const unsigned int nRow = 0 ) = 0;
};

class ITimeEditor
{
public:
    // return true if parameter list (size/name and other values) update required
    virtual bool updateTimeValue( const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 ) = 0;
    virtual void getTimeValue( const std::string& strName, time_t& tmValue, const unsigned int nRow = 0 ) = 0;
};

class IStringEditor
{
public:
    // return true if parameter list (size/name and other values) update required
    virtual bool updateValue( const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 ) = 0;
    virtual void getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow = 0 ) {}; // [ToDo] Implement this function to support max length of editbox
};

class IGridEditor
{
public:
    virtual void getGridItems( const std::string& strName, std::vector<GridColumn>& vecColumns, std::vector<GridItem>& vecKeyItems, bool& bRowFixed, const unsigned int nRow = 0 ) = 0;

    // Default there is no parameter as input type String, overwrite this function if there is
    virtual IStringEditor* getStringEditor( const std::string& strName ) { return 0; }
    // Default there is no parameter as input type Bool, overwrite this function if there is
    virtual IBoolEditor* getBoolEditor( const std::string& strName ) { return 0; }
    // Default there is no parameter as input type Time, overwrite this function if there is
    virtual ITimeEditor* getTimeEditor( const std::string& strName ) { return 0; }
    // Default there is no parameter as input type List, overwrite this function if there is
    virtual IListPicker* getListPicker( const std::string& strName ) { return 0; }
    // Default if there is no parameter as input type Tree, overwrite this function if there is
    virtual ITreePicker* getTreePicker( const std::string& strName ) { return 0; }
    // Default if there is no parameter as input type Grid, overwrite this function if there is
    virtual IGridEditor* getGridEditor( const std::string& strName ) { return 0; }
	
    // return true if parameter list (size/name and other values) update required
	virtual bool deleteRow( const unsigned int nRow ) = 0;
	virtual bool newRow() = 0;
};

class IBasicEditor
{
public:
    virtual void getBasicNameValuePairs(std::vector<ItemPair>& vecNameValues) = 0;
    virtual bool updateBasicBooleanValue( const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid ) = 0;
    virtual void getBasicBooleanValue( const std::string& strName, bool& bValue ) = 0;
    virtual bool updateBasicValue( const std::string& strName, const std::string& strDisplayValue, bool& bValid ) = 0;
    virtual void getBasicValue( const std::string& strName, std::string& strValue, size_t& szMaxLength ) = 0;
};

class IActivityParameter
{
public:
    // return true if parameter list (size/name exclude value) update required
    virtual void getNameValuePairs( std::vector<ItemPair>& vecNameValues ) = 0;
    //virtual void defineParameters() = 0;
    virtual std::string instanceName() = 0;
    virtual std::string displayValue( const std::string& strName ) = 0;

    // Default there is no parameter as input type String, overwrite this function if there is
    virtual IStringEditor* getStringEditor( const std::string& strName ) { return 0; }
    // Default there is no parameter as input type Bool, overwrite this function if there is
    virtual IBoolEditor* getBoolEditor( const std::string& strName ) { return 0; }
    // Default there is no parameter as input type Time, overwrite this function if there is
    virtual ITimeEditor* getTimeEditor( const std::string& strName ) { return 0; }
    // Default there is no parameter as input type List, overwrite this function if there is
    virtual IListPicker* getListPicker( const std::string& strName ) { return 0; }
    // Default if there is no parameter as input type Tree, overwrite this function if there is
    virtual ITreePicker* getTreePicker( const std::string& strName ) { return 0; }
    // Default if there is no parameter as input type Grid, overwrite this function if there is
    virtual IGridEditor* getGridEditor( const std::string& strName ) { return 0; }

    virtual IBasicEditor& basicEditor() = 0;
};

#endif