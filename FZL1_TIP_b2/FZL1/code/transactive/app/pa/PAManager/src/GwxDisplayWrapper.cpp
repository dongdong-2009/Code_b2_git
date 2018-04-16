/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/GwxDisplayWrapper.h"


GwxDisplayWrapper* GwxDisplayWrapper::s_pInstance = NULL;

GwxDisplayWrapper::GwxDisplayWrapper() : 
m_pGwxDisplay(),
m_bSchematicOpen( false ),
m_hPrevParent( NULL )
{
    // Initialize COM for embed Gwx32 window
    if ( FAILED( CoInitialize( NULL )))
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_FAILED_TO_INIT_COM );
        return;
    }

    m_pGwxDisplay.CreateInstance( GWX_DISPLAY_PROG_ID );
}

GwxDisplayWrapper::~GwxDisplayWrapper()
{
    if ( NULL != m_pGwxDisplay )
    {
        m_pGwxDisplay.Release();
    }

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

int GwxDisplayWrapper::loadSchematic( const std::string& strFileFullName, HWND hPosMarker, HWND hParent )
{
    try
    {
        if ( NULL == m_pGwxDisplay )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_FAILED_TO_INIT_GWX_COM );
            return ERROR_FAILED_TO_INIT_GWX_COM;
        }

        if ( m_bSchematicOpen )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_LOGIC );
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

        CRect rtMarker;
        ::GetWindowRect( hPosMarker, &rtMarker );
        ::ShowWindow( hPosMarker, SW_HIDE );

        POINT ptTopLeft = { rtMarker.left, rtMarker.top };
        if ( NULL != hParent )
        {
            ::ScreenToClient( hParent, &ptTopLeft );
        }
        hrRes = m_pGwxDisplay->SetWindowDimensionsPixels( ptTopLeft.x, ptTopLeft.y, rtMarker.Width(), rtMarker.Height() );
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

void GwxDisplayWrapper::changeSchematicSize(HWND hPosMarker, HWND hParent)
{
    if (NULL != m_pGwxDisplay)
    {
        CRect rtMarker;
        ::GetWindowRect( hPosMarker, &rtMarker );

        POINT ptTopLeft = { rtMarker.left, rtMarker.top };
        if ( NULL != hParent )
        {
            ::ScreenToClient( hParent, &ptTopLeft );
        }

        HRESULT hrRes = E_FAIL;
        hrRes = m_pGwxDisplay->SetWindowDimensionsPixels( ptTopLeft.x, ptTopLeft.y, rtMarker.Width(), rtMarker.Height() );
        if ( FAILED( hrRes ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set window dimensions pixels" );
        }

        hrRes = m_pGwxDisplay->ViewFitToWindow();
        if ( FAILED( hrRes ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to fit schematic to window" );
        }
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No have schematic!" );
    }
    
}

void GwxDisplayWrapper::closeSchematic()
{
    if ( !m_bSchematicOpen )
    {
        try
        {
            if ( m_pGwxDisplay->IsRuntimeMode() )
            {
                m_pGwxDisplay->CloseWindow();
            }
        }
        catch (...)
        {
        }
        return;
    }

    try
    {
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
    }
    catch (...)
    {
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