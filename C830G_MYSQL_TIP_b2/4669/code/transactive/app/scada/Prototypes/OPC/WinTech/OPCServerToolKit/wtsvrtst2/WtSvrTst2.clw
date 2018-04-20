; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTagListDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "WtSvrTst2.h"
LastPage=0

ClassCount=7
Class1=CWtSvrTst2App
Class2=CWtSvrTst2Doc
Class3=CWtSvrTst2View
Class4=CMainFrame

ResourceCount=5
Resource1=IDD_DELIMITERDLG
Resource2=IDD_ABOUTBOX
Class5=CAboutDlg
Resource3=IDD_CREATETAGS
Class6=CTagListDlg
Resource4=IDD_WTSVRTST2_FORM
Class7=CDelimitDlg
Resource5=IDR_MAINFRAME

[CLS:CWtSvrTst2App]
Type=0
HeaderFile=WtSvrTst2.h
ImplementationFile=WtSvrTst2.cpp
Filter=N

[CLS:CWtSvrTst2Doc]
Type=0
HeaderFile=WtSvrTst2Doc.h
ImplementationFile=WtSvrTst2Doc.cpp
Filter=N

[CLS:CWtSvrTst2View]
Type=0
HeaderFile=WtSvrTst2View.h
ImplementationFile=WtSvrTst2View.cpp
Filter=D
BaseClass=CFormView
VirtualFilter=VWC
LastObject=ID_SERVER_SETDELIMITER


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
HeaderFile=WtSvrTst2.cpp
ImplementationFile=WtSvrTst2.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

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
CommandCount=14

[DLG:IDD_WTSVRTST2_FORM]
Type=1
Class=CWtSvrTst2View
ControlCount=14
Control1=IDC_TAGTREE,SysTreeView32,1350635527
Control2=IDC_EVENTLIST,listbox,1353777409
Control3=IDC_SUSPEND,button,1476460544
Control4=IDC_NUMBER,static,1342308352
Control5=IDC_FREQUENCY,static,1342308352
Control6=IDC_CREATETIME,static,1342308352
Control7=IDC_CURRENTTIME,static,1342308352
Control8=IDC_STARTTIME,static,1342308352
Control9=IDC_UPDATETIME,static,1342308352
Control10=IDC_STATIC,button,1342177287
Control11=IDC_UPDATES,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_AVGUPDATE,static,1342308352
Control14=IDC_STATIC,static,1342308352

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
CommandCount=8

[DLG:IDD_CREATETAGS]
Type=1
Class=CTagListDlg
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_COUNT,combobox,1344339970
Control5=IDC_STATIC,static,1342308352
Control6=IDC_UPDATE,edit,1350631552
Control7=IDC_RANDOM,button,1342177289
Control8=IDC_INCREMENTAL,button,1342177289
Control9=IDC_STATIC,button,1342177287
Control10=IDC_STATIC,button,1342177287
Control11=IDC_FLOATTYPE,button,1342242819
Control12=IDC_INTTYPE,button,1342242819
Control13=IDC_BOOLTYPE,button,1342242819
Control14=IDC_STRTYPE,button,1342242819
Control15=IDC_LISTUPDATES,button,1342242819
Control16=IDC_STATIC,button,1342177287
Control17=IDC_HASH,button,1342242819

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_CLOSE_APP
Command2=ID_SERVER_REGISTER
Command3=ID_SERVER_UNREGISTER
Command4=ID_SERVER_SETDELIMITER
Command5=ID_SERVER_CREATETAGS
Command6=ID_APP_ABOUT
CommandCount=6

[CLS:CTagListDlg]
Type=0
HeaderFile=TagListDlg.h
ImplementationFile=TagListDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDOK
VirtualFilter=dWC

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

