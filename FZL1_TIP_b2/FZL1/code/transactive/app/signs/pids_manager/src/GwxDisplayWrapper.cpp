#include "StdAfx.h"
#include "PidManagerConsts.h"
//#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "GwxDisplayWrapper.h"


GwxDisplayWrapper* GwxDisplayWrapper::s_pInstance = NULL;

GwxDisplayWrapper::GwxDisplayWrapper() : 
m_pGwxDisplay(),
m_bSchematicOpen( false ),
m_hPrevParent( NULL )
{
    // Initialize COM for embed Gwx32 window
    CoInitialize(NULL);

    m_pGwxDisplay.CreateInstance( GWX_DISPLAY_PROG_ID );
}

GwxDisplayWrapper::~GwxDisplayWrapper()
{
    m_pGwxDisplay.Release();

    // Uninitialize COM
    CoUninitialize();
}

bool GwxDisplayWrapper::createInstance()
{
    if ( NULL == s_pInstance )
    {
        s_pInstance = new GwxDisplayWrapper();
    }

    if ( NULL == s_pInstance )
    {
        return false;
    }

    return true;
}

void GwxDisplayWrapper::destroyInstance()
{
    if ( NULL != s_pInstance )
    {
        delete s_pInstance;
        s_pInstance = NULL;
    }
}

GwxDisplayWrapper& GwxDisplayWrapper::getInstance()
{
    return (*s_pInstance);
}

int GwxDisplayWrapper::loadSchematic( const std::string& strFileFullName, const CRect& rtPosition, HWND hParent )
{
    try
    {
        if ( NULL == m_pGwxDisplay )
        {
            //PaErrorHelper::logErrorMsg( ERROR_UNEXPECTED_NULL_POINTER );
            return ERROR_UNEXPECTED_NULL_POINTER;
        }

        if ( m_bSchematicOpen )
        {
            //PaErrorHelper::logErrorMsg( ERROR_UNEXPECTED_LOGIC );
            return ERROR_UNEXPECTED_LOGIC;
        }

        if ( strFileFullName.empty() )
        {
            return ERROR_FAILED_TO_LOCATE_SCHEMATIC_FILE;
        }

        HRESULT hrRes = E_FAIL;

        hrRes = m_pGwxDisplay->StartRuntime();
        if ( FAILED( hrRes ))
        {
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }

        if ( !m_pGwxDisplay->FileOpen( _bstr_t(strFileFullName.c_str()) ))
        {
            return ERROR_GWX_FAILED_TO_OPEN_SCHEMATIC_FILE;
        }

        hrRes = m_pGwxDisplay->CancelRuntimeFocus();
        if ( FAILED( hrRes ))
        {
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }
        hrRes = m_pGwxDisplay->StopRuntime();
        if ( FAILED( hrRes ))
        {
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }
        hrRes = m_pGwxDisplay->StartRuntime();
        if ( FAILED( hrRes ))
        {
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }

        POINT ptTopLeft = { rtPosition.left, rtPosition.top };
        if ( NULL != hParent )
        {
            ::ScreenToClient( hParent, &ptTopLeft );
        }
        hrRes = m_pGwxDisplay->SetWindowDimensionsPixels( ptTopLeft.x, ptTopLeft.y, rtPosition.Width(), rtPosition.Height() );
        if ( FAILED( hrRes ))
        {
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }

        hrRes = m_pGwxDisplay->ViewFitToWindow();
        if ( FAILED( hrRes ))
        {
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }

        HWND hSchematic = reinterpret_cast<HWND>(m_pGwxDisplay->GetFrameWindowHandle());
        if (( NULL != hParent ) && ( NULL != hSchematic ))
        {
            m_hPrevParent = ::SetParent( hSchematic, hParent );
        }

        hrRes = m_pGwxDisplay->ShowWindow();
        if ( FAILED( hrRes ))
        {
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }

        m_bSchematicOpen = true;
    }
    catch ( ... )
    {
    }
    return SUCCESS_NO_ERROR;
}

void GwxDisplayWrapper::closeSchematic()
{
    if ( !m_bSchematicOpen )
    {
        return;
    }

    HWND hSchematic = reinterpret_cast<HWND>(m_pGwxDisplay->GetFrameWindowHandle());
    if ( NULL != hSchematic )
    {
        m_pGwxDisplay->HideWindow();
        if ( NULL != m_hPrevParent )
        {
            ::SetParent( hSchematic, m_hPrevParent );
        }
        m_pGwxDisplay->CloseAllPopupWindows();
        m_pGwxDisplay->StopRuntime();
        m_pGwxDisplay->CloseWindow();
    }

    m_bSchematicOpen = false;
}

void GwxDisplayWrapper::refreshSchematic()
{
    if (( NULL == m_pGwxDisplay ) || !m_bSchematicOpen )
    {
        return;
    }

    m_pGwxDisplay->RefreshWindow();
}