#include "app/pa/PAManager/src/stdafx.h"

#include "core/utilities/src/DebugUtil.h"

#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"

CWnd* PaErrorHelper::s_pMessageBoxParentWnd = NULL;

PaErrorHelper::PaErrorHelper()
{
}

PaErrorHelper::~PaErrorHelper()
{
}

void PaErrorHelper::logErrorMsg( const char* strFileName, int nFileLine, int nMsgID, const char* const strAddInfo )
{
    if (( ERROR_NOT_DEFINE >= nMsgID ) || ( ERROR_MSG_COUNT <= nMsgID ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, PA_ERROR_MSG_ID_NOT_EXIST );
        return;
    }

    if ( NULL != strAddInfo )
    {
        std::string strLogContent = PA_ERROR_MSG[nMsgID];
        strLogContent += " : %s";
        LOG_GENERIC( strFileName, nFileLine, TA_Base_Core::DebugUtil::DebugError, strLogContent.c_str(), strAddInfo );
    }
    else
    {
        LOG_GENERIC( strFileName, nFileLine, TA_Base_Core::DebugUtil::DebugError, PA_ERROR_MSG[nMsgID] );
    }
}

void PaErrorHelper::popupErrorMsgBox( int nMsgID )
{
    if ( NULL == s_pMessageBoxParentWnd )
    {
        logErrorMsg( SourceInfo, ERROR_MAIN_WND_FOR_MSG_BOX_NOT_EXIST );
        return;
    }

    if (( ERROR_NOT_DEFINE >= nMsgID ) || ( ERROR_MSG_COUNT <= nMsgID ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, PA_ERROR_MSG_ID_NOT_EXIST );
        return;
    }

    s_pMessageBoxParentWnd->PostMessage( WM_POPUP_MESSAGE_BOX, reinterpret_cast<WPARAM>(PA_ERROR_MSG[nMsgID]), reinterpret_cast<LPARAM>(ERROR_TITLE) );
}

void PaErrorHelper::popupWarningMsgBox( int nMsgID )
{
    if ( NULL == s_pMessageBoxParentWnd )
    {
        logErrorMsg( SourceInfo, ERROR_MAIN_WND_FOR_MSG_BOX_NOT_EXIST );
        return;
    }

    if (( ERROR_NOT_DEFINE >= nMsgID ) || ( ERROR_MSG_COUNT <= nMsgID ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, PA_ERROR_MSG_ID_NOT_EXIST );
        return;
    }

    s_pMessageBoxParentWnd->PostMessage( WM_POPUP_MESSAGE_BOX, reinterpret_cast<WPARAM>(PA_ERROR_MSG[nMsgID]), reinterpret_cast<LPARAM>(WARNING_TITLE) );
}

void PaErrorHelper::registerMsgBoxParentWnd( CWnd* pWnd )
{
    s_pMessageBoxParentWnd = pWnd;
}

void PaErrorHelper::deregisterMsgBoxParentWnd( CWnd* pWnd )
{
    if ( pWnd == s_pMessageBoxParentWnd )
    {
        s_pMessageBoxParentWnd = NULL;
    }
}

void PaErrorHelper::popupErrorMsgBoxImmidate( CWnd* pWnd, int nMsgID )
{
    if ( NULL == pWnd )
    {
        logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return;
    }

    if (( ERROR_NOT_DEFINE >= nMsgID ) || ( ERROR_MSG_COUNT <= nMsgID ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, PA_ERROR_MSG_ID_NOT_EXIST );
        return;
    }

    pWnd->MessageBox( PA_ERROR_MSG[nMsgID], ERROR_TITLE, MB_OK | MB_ICONERROR );
}

void PaErrorHelper::popupWarningMsgBoxImmidate( CWnd* pWnd, int nMsgID )
{
    if ( NULL == pWnd )
    {
        logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return;
    }
    
    if (( ERROR_NOT_DEFINE >= nMsgID ) || ( ERROR_MSG_COUNT <= nMsgID ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, PA_ERROR_MSG_ID_NOT_EXIST );
        return;
    }

    pWnd->MessageBox( PA_ERROR_MSG[nMsgID], WARNING_TITLE, MB_OK | MB_ICONWARNING );
}

bool PaErrorHelper::popupConfirmMsgBoxImmidate( CWnd* pWnd, int nMsgID )
{
    if ( NULL == pWnd )
    {
        logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return false;
    }

    if (( ERROR_NOT_DEFINE >= nMsgID ) || ( ERROR_MSG_COUNT <= nMsgID ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, PA_ERROR_MSG_ID_NOT_EXIST );
        return false;
    }

    int nRet = pWnd->MessageBox( PA_ERROR_MSG[nMsgID], CONFIRM_TITLE, MB_YESNO | MB_ICONQUESTION );
    if ( IDYES == nRet )
    {
        return true;
    }

    return false;
}

void PaErrorHelper::logExceptionCatch( const char* strFileName, int nFileLine, const char* const strExceptionWhat )
{
    if ( NULL == strExceptionWhat )
    {
        return;
    }

    LOG_GENERIC( strFileName, nFileLine, TA_Base_Core::DebugUtil::ExceptionCatch, strExceptionWhat );
}

bool PaErrorHelper::checkError( int nMsgID )
{
    if ( SUCCESS_NO_ERROR == nMsgID )
    {
        return false;
    }

    logErrorMsg( SourceInfo, nMsgID );
    popupErrorMsgBox( nMsgID );
    return true;
}

bool PaErrorHelper::checkErrorImmidate( CWnd* pWnd, int nMsgID )
{
    if ( SUCCESS_NO_ERROR == nMsgID )
    {
        return false;
    }
    
    logErrorMsg( SourceInfo, nMsgID );
    popupErrorMsgBoxImmidate( pWnd, nMsgID );
    return true;
}

void PaErrorHelper::popupSuccessMsgBoxImmidate( CWnd* pWnd, const char* const strMsg )
{
    if ( NULL == pWnd )
    {
        logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return;
    }

    pWnd->MessageBox( strMsg, SUCCESS_MSG_BOX_TITTLE );
}