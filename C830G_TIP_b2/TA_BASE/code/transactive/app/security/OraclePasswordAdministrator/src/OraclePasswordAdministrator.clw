; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CUpdateDatabaseDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "OraclePasswordAdministrator.h"

ClassCount=4
Class1=COraclePasswordAdministratorApp
Class2=COraclePasswordAdministratorDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_ORACLEPASSWORDADMINISTRATOR_DIALOG
Class4=CUpdateDatabaseDlg
Resource4=IDD_UPDATE_DIALOG

[CLS:COraclePasswordAdministratorApp]
Type=0
HeaderFile=OraclePasswordAdministrator.h
ImplementationFile=OraclePasswordAdministrator.cpp
Filter=N
LastObject=COraclePasswordAdministratorApp
BaseClass=CWinApp
VirtualFilter=AC

[CLS:COraclePasswordAdministratorDlg]
Type=0
HeaderFile=OraclePasswordAdministratorDlg.h
ImplementationFile=OraclePasswordAdministratorDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_EDIT_SERVER_NAME

[CLS:CAboutDlg]
Type=0
HeaderFile=OraclePasswordAdministratorDlg.h
ImplementationFile=OraclePasswordAdministratorDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_ORACLEPASSWORDADMINISTRATOR_DIALOG]
Type=1
Class=COraclePasswordAdministratorDlg
ControlCount=11
Control1=IDOK,button,1476460545
Control2=IDCANCEL,button,1342242816
Control3=IDC_BTN_CLEAR,button,1342242816
Control4=IDC_STATIC_USERNAME,static,1342308352
Control5=IDC_EDIT_USERNAME,edit,1350631552
Control6=IDC_STATIC_PASSWORD,static,1342308352
Control7=IDC_EDIT_PASSWORD,edit,1350631584
Control8=IDC_STATIC_CONNECT,button,1342177287
Control9=IDC_STATIC_LOGO,static,1342180366
Control10=IDC_STATIC_SERVER_NAME,static,1342308352
Control11=IDC_EDIT_SERVER_NAME,edit,1350631552

[DLG:IDD_UPDATE_DIALOG]
Type=1
Class=CUpdateDatabaseDlg
ControlCount=8
Control1=IDC_BTN_GENERATE,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_BTN_PERFORM,button,1342242816
Control4=IDC_EDIT_UPDATESQL_STATEMENT,edit,1353779396
Control5=IDC_BTN_COPY,button,1476460544
Control6=IDC_BTN_EXPORT,button,1476460544
Control7=IDC_BTN_REVERT,button,1342242816
Control8=IDC_STATIC_CONNECTiNFO,static,1342308364

[CLS:CUpdateDatabaseDlg]
Type=0
HeaderFile=UpdateDatabaseDlg.h
ImplementationFile=UpdateDatabaseDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CUpdateDatabaseDlg

