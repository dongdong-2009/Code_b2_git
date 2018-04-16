; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTagDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "WTSvrTest.h"
LastPage=0

ClassCount=10
Class1=CWTSvrTestApp
Class2=CWTSvrTestDoc
Class3=CWTSvrTestView
Class4=CMainFrame

ResourceCount=7
Resource1=IDD_ABOUTBOX
Class5=CAboutDlg
Resource2=IDD_ADDTAG
Class6=CTagDlg
Resource3=IDD_AE_LIMITS
Class7=CAEMsgDlg
Resource4=IDD_USERAE
Class8=CAELimsDlg
Resource5=IDD_NEWUSER
Class9=CDelimitDlg
Resource6=IDD_DELIMITERDLG
Class10=CAddUserDlg
Resource7=IDR_MAINFRAME

[CLS:CWTSvrTestApp]
Type=0
HeaderFile=WTSvrTest.h
ImplementationFile=WTSvrTest.cpp
Filter=N
LastObject=CWTSvrTestApp
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CWTSvrTestDoc]
Type=0
HeaderFile=WTSvrTestDoc.h
ImplementationFile=WTSvrTestDoc.cpp
Filter=N
LastObject=CWTSvrTestDoc

[CLS:CWTSvrTestView]
Type=0
HeaderFile=WTSvrTestView.h
ImplementationFile=WTSvrTestView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CWTSvrTestView

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CFrameWnd
VirtualFilter=fWC



[CLS:CAboutDlg]
Type=0
HeaderFile=WTSvrTest.cpp
ImplementationFile=WTSvrTest.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_SAVETAGLIST
Command2=ID_FILE_RESTORETAGLIST
Command3=ID_CLOSE_APP
Command4=ID_EDIT_REGISTER
Command5=ID_EDIT_UNREGISTER
Command6=ID_EDIT_TAGDELIMITER
Command7=ID_EDIT_ADDTAG
Command8=ID_EDIT_DELETETAG
Command9=ID_EDIT_UPDATETAG
Command10=ID_EDIT_DYNAMICTAGS
Command11=ID_EDIT_FORCECLIENTREFRESH
Command12=ID_EDIT_USERAEMSG
Command13=ID_NTSECURITY_ENABLE
Command14=ID_NTSECURITY_DISABLE
Command15=ID_NTSECURITY_ADDUSER
Command16=ID_VIEW_TOOLBAR
Command17=ID_VIEW_STATUS_BAR
Command18=ID_VIEW_DLLREVISION
Command19=ID_HELP_FINDER
Command20=ID_APP_ABOUT
CommandCount=20

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
Command15=ID_CONTEXT_HELP
Command16=ID_HELP
CommandCount=16

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
Command9=ID_CONTEXT_HELP
CommandCount=9

[DLG:IDD_ADDTAG]
Type=1
Class=CTagDlg
ControlCount=25
Control1=IDC_NAME,edit,1350631552
Control2=IDC_VALUE,edit,1350631552
Control3=IDC_DESCR,edit,1350631552
Control4=IDC_UNITS,edit,1350631552
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_ALARMS,button,1342242816
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_FLOAT,button,1342177289
Control13=IDC_INTEGER,button,1342177289
Control14=IDC_BOOLEAN,button,1342177289
Control15=IDC_STRING,button,1342177289
Control16=IDC_STATIC,button,1342177287
Control17=IDC_READSECURITY,combobox,1344339970
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,button,1342177287
Control20=IDC_WRITESECURITY,combobox,1344339970
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,button,1342177287
Control23=IDC_ARRAY,button,1342242819
Control24=IDC_LENTEXT,static,1342308352
Control25=IDC_LENGTH,edit,1350631552

[CLS:CTagDlg]
Type=0
HeaderFile=TagDlg.h
ImplementationFile=TagDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_ARRAY
VirtualFilter=dWC

[DLG:IDD_USERAE]
Type=1
Class=CAEMsgDlg
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_MESSAGE,edit,1350631552
Control6=IDC_SEVERITY,edit,1350631552

[CLS:CAEMsgDlg]
Type=0
HeaderFile=AEMsgDlg.h
ImplementationFile=AEMsgDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CAEMsgDlg

[DLG:IDD_AE_LIMITS]
Type=1
Class=CAELimsDlg
ControlCount=20
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_HIHICHK,button,1342242819
Control4=IDC_HIHILEVEL,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_HIHISEV,edit,1350631552
Control8=IDC_STATIC,button,1342177287
Control9=IDC_HICHK,button,1342242819
Control10=IDC_HILEVEL,edit,1350631552
Control11=IDC_HISEV,edit,1350631552
Control12=IDC_STATIC,button,1342177287
Control13=IDC_LOCHK,button,1342242819
Control14=IDC_LOLEVEL,edit,1350631552
Control15=IDC_LOSEV,edit,1350631552
Control16=IDC_STATIC,button,1342177287
Control17=IDC_LOLOCHK,button,1342242819
Control18=IDC_LOLOLEVEL,edit,1350631552
Control19=IDC_LOLOSEV,edit,1350631552
Control20=IDC_STATIC,button,1342177287

[CLS:CAELimsDlg]
Type=0
HeaderFile=AELimsDlg.h
ImplementationFile=AELimsDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CAELimsDlg

[DLG:IDD_DELIMITERDLG]
Type=1
Class=CDelimitDlg
ControlCount=5
Control1=IDC_EDIT1,edit,1350631553
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352

[CLS:CDelimitDlg]
Type=0
HeaderFile=DelimitDlg.h
ImplementationFile=DelimitDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDelimitDlg

[DLG:IDD_NEWUSER]
Type=1
Class=CAddUserDlg
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_USER,edit,1350631552
Control4=IDC_GROUP,edit,1350631552
Control5=IDC_RIGHTS,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352

[CLS:CAddUserDlg]
Type=0
HeaderFile=AddUserDlg.h
ImplementationFile=AddUserDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CAddUserDlg
VirtualFilter=dWC

