#include "app/cctv/video_display_manager/src/StdAfx.h"

#include "app/cctv/video_display_manager/src/GwxDisplayWrapper.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
GwxDisplayWrapper* GwxDisplayWrapper::s_pInstance = NULL;
const char* const GWX_DISPLAY_PROG_ID = "Gwx32.Display";
const int GWX_MENU_HEIGHT=20;

GwxDisplayWrapper::GwxDisplayWrapper() : 
m_pGwxDisplay(),
m_bSchematicOpen( false ),
m_hPrevParent( NULL )
{
    // Initialize COM for embed Gwx32 window
    if ( FAILED( CoInitialize( NULL )))
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]", ERROR_FAILED_TO_INIT_COM );
        return;
    }

    m_pGwxDisplay.CreateInstance( GWX_DISPLAY_PROG_ID );
}

GwxDisplayWrapper::~GwxDisplayWrapper()
{
    if ( NULL != m_pGwxDisplay )
    {

		destroyInstance();
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

int GwxDisplayWrapper::loadStationDiagram( const std::string& strFileFullName, const CRect& rtPosition, HWND hParent )
{
    try
    {
        if ( NULL == m_pGwxDisplay )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]", ERROR_FAILED_TO_INIT_GWX_COM );
            return ERROR_FAILED_TO_INIT_GWX_COM;
        }

        if ( m_bSchematicOpen )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]", ERROR_UNEXPECTED_LOGIC );
			
            return ERROR_UNEXPECTED_LOGIC;
        }

        if ( strFileFullName.empty() )
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]", ERROR_FAILED_TO_LOCATE_SCHEMATIC_FILE );
            return ERROR_FAILED_TO_LOCATE_SCHEMATIC_FILE;
        }

        HRESULT hrRes = E_FAIL;
		
			hrRes = m_pGwxDisplay->StartRuntime();
			if ( FAILED( hrRes ))
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]",ERROR_FAILED_TO_INVOKE_GWX_INTERFACE);
				return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
			}
		

        if ( !m_pGwxDisplay->FileOpen( _bstr_t(strFileFullName.c_str()) ))
        {
			std::string str="Fail to open .gdf file:";
			str=str+strFileFullName;
			AfxMessageBox(str.c_str());
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]",ERROR_GWX_FAILED_TO_OPEN_SCHEMATIC_FILE);
            return ERROR_GWX_FAILED_TO_OPEN_SCHEMATIC_FILE;
		}

        hrRes = m_pGwxDisplay->CancelRuntimeFocus();
        if ( FAILED( hrRes ))
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]",ERROR_FAILED_TO_INVOKE_GWX_INTERFACE);
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }
        hrRes = m_pGwxDisplay->StopRuntime();
        if ( FAILED( hrRes ))
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]",ERROR_FAILED_TO_INVOKE_GWX_INTERFACE);
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }
        hrRes = m_pGwxDisplay->StartRuntime();
        if ( FAILED( hrRes ))
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]",ERROR_FAILED_TO_INVOKE_GWX_INTERFACE);
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }

       /* CRect rtMarker;
        ::GetWindowRect( hPosMarker, &rtMarker );
        ::ShowWindow( hPosMarker, SW_HIDE );

        POINT ptTopLeft = { rtMarker.left, rtMarker.top };
        if ( NULL != hParent )
        {
            ::ScreenToClient( hParent, &ptTopLeft );
        }
        hrRes = m_pGwxDisplay->SetWindowDimensionsPixels( ptTopLeft.x, ptTopLeft.y, rtMarker.Width(), rtMarker.Height() );*/
		POINT ptTopLeft = { rtPosition.left, rtPosition.top };
        if ( NULL != hParent )
        {
            ::ScreenToClient( hParent, &ptTopLeft );
        }
		
		hrRes = m_pGwxDisplay->SetWindowDimensionsPixels(ptTopLeft.x, ptTopLeft.y , rtPosition.Width(), rtPosition.Height() );
        if ( FAILED( hrRes ))
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]",ERROR_FAILED_TO_INVOKE_GWX_INTERFACE);
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }

        hrRes = m_pGwxDisplay->ViewFitToWindow();
        if ( FAILED( hrRes ))
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]",ERROR_FAILED_TO_INVOKE_GWX_INTERFACE);
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
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]",ERROR_FAILED_TO_INVOKE_GWX_INTERFACE);
            return ERROR_FAILED_TO_INVOKE_GWX_INTERFACE;
        }

        m_bSchematicOpen = true;
    }
    catch ( ... )
    {
		;
    }
    return SUCCESS_NO_ERROR;
}

void GwxDisplayWrapper::closeSchematic()
{
	try
	{
		stopSchematic();
		m_pGwxDisplay->CloseWindow();
	}
	catch (CMemoryException* e)
	{
		;
	}
	catch (CFileException* e)
	{
		;
	}
	catch (CException* e)
	{
		;
	}
	

}

void GwxDisplayWrapper::stopSchematic()
{
    if ( !m_bSchematicOpen )
    {
		/*try
        {
			
            if ( m_pGwxDisplay->IsRuntimeMode() )
            {
				//m_pGwxDisplay->CloseWindow();
            }
        }
        catch (...)
        {
        }*/

        return;
    }

    try
    {
        HWND hSchematic = reinterpret_cast<HWND>(m_pGwxDisplay->GetFrameWindowHandle());
        if ( NULL != hSchematic )
        {
			HRESULT hrRes;
            m_pGwxDisplay->HideWindow();
            if ( NULL != m_hPrevParent )
            {
                ::SetParent( hSchematic, m_hPrevParent );
            }
            m_pGwxDisplay->CloseAllPopupWindows();
		/*	hrRes = m_pGwxDisplay->CancelRuntimeFocus();
			if ( FAILED( hrRes ))
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]",ERROR_FAILED_TO_INVOKE_GWX_INTERFACE);
				return;
			}*/
			hrRes = m_pGwxDisplay->StopRuntime();
			
           
		/*	hrRes =m_pGwxDisplay->CloseWindow();
			if ( FAILED( hrRes ))
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"GWX error:[%d]",ERROR_FAILED_TO_INVOKE_GWX_INTERFACE);
				return;
			}
		*/
        }
    }
    catch (...)
    {
		int e=0;
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
}