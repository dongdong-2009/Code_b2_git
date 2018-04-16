#pragma once

class PaErrorHelper
{
public:
    PaErrorHelper();
    virtual ~PaErrorHelper();

    static void registerMsgBoxParentWnd( CWnd* pWnd );
    static void deregisterMsgBoxParentWnd( CWnd* pWnd );

    // Should be used in other thread beside GUI thread, return false if no error
    static bool checkError( int nMsgID );

    // Can be used in GUI thread, return false if no error
    static bool checkErrorImmidate( CWnd* pWnd, int nMsgID );

    static void logErrorMsg( const char* strFileName, int nFileLine, int nMsgID, const char* const strAddInfo = NULL );

    static void logExceptionCatch( const char* strFileName, int nFileLine, const char* const strExceptionWhat );
    
    // Should be used in other thread beside GUI thread
    static void popupErrorMsgBox( int nMsgID );

    // Can be used in GUI thread
    static void popupErrorMsgBoxImmidate( CWnd* pWnd, int nMsgID );

    // Should be used in other thread beside GUI thread
    static void popupWarningMsgBox( int nMsgID );

    // Can be used in GUI thread
    static void popupWarningMsgBoxImmidate( CWnd* pWnd, int nMsgID );
    
    // Can be used in GUI thread, return true if user confirm
    static bool popupConfirmMsgBoxImmidate( CWnd* pWnd, int nMsgID );

    static void popupSuccessMsgBoxImmidate( CWnd* pWnd, const char* const strMsg );

private:
    static CWnd* s_pMessageBoxParentWnd;
};
