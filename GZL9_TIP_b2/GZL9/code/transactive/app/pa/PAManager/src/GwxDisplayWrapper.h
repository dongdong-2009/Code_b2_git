#pragma once

#import "app/pa/PAManager/src/Gwx32.tlb" no_namespace

class GwxDisplayWrapper
{
public:
    virtual ~GwxDisplayWrapper();

    static bool createInstance();

    static void destroyInstance();

    // Static function for public invoke
    static GwxDisplayWrapper& getInstance();

    int loadSchematic( const std::string& strFileFullName, HWND hPosMarker, HWND hParent );

    void closeSchematic();

    void refreshSchematic();

protected:
    GwxDisplayWrapper();

private:
    // Disable operator = and copy constructor
    const GwxDisplayWrapper& operator=( GwxDisplayWrapper& );
    GwxDisplayWrapper( const GwxDisplayWrapper& );

private:
    static GwxDisplayWrapper* s_pInstance;
    IGwx32Ptr m_pGwxDisplay;
    bool m_bSchematicOpen;
    HWND m_hPrevParent;
};
