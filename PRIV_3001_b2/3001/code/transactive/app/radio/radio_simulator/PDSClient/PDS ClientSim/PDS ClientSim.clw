; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CConfigDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "PDS ClientSim.h"

ClassCount=7
Class1=CPDSClientSimApp
Class2=CPDSClientSimDlg
Class3=CAboutDlg

ResourceCount=7
Resource1=IDD_DLG_REPEAT
Resource2=IDR_MAINFRAME
Resource3=IDD_DLG_DATAVALIDATOR
Class4=CFrameRepeatDlg
Resource4=IDD_ABOUTBOX
Class5=CDlgDataValidator
Resource5=IDD_DLG_FRAME_RAND
Class6=CRandFrameDlg
Resource6=IDD_PDSCLIENTSIM_DIALOG
Class7=CConfigDlg
Resource7=IDD_DLG_CONFIG

[CLS:CPDSClientSimApp]
Type=0
HeaderFile=PDS ClientSim.h
ImplementationFile=PDS ClientSim.cpp
Filter=N

[CLS:CPDSClientSimDlg]
Type=0
HeaderFile=PDS ClientSimDlg.h
ImplementationFile=PDS ClientSimDlg.cpp
Filter=D
LastObject=CPDSClientSimDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=PDS ClientSimDlg.h
ImplementationFile=PDS ClientSimDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_PDSCLIENTSIM_DIALOG]
Type=1
Class=CPDSClientSimDlg
ControlCount=18
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,button,1342177287
Control5=IDC_EDT_IP,edit,1484849280
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDT_PORT,edit,1484849280
Control8=IDC_STATIC,button,1342177287
Control9=IDC_LST_SENDEVENT,listbox,1352728833
Control10=IDC_STATIC,button,1342177287
Control11=IDC_STATIC,static,1342308352
Control12=IDC_CMBO_TrainID,combobox,1344340226
Control13=IDC_STATIC,button,1342177287
Control14=IDC_BUTTON1,button,1342242816
Control15=IDC_BTNREPEAT,button,1342242816
Control16=IDC_STATIC,button,1342177287
Control17=IDC_LST_RECV_DATA,listbox,1352728833
Control18=IDC_BTN_CONF,button,1342242816

[DLG:IDD_DLG_REPEAT]
Type=1
Class=CFrameRepeatDlg
ControlCount=11
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDT_FrameNum,edit,1350631552
Control5=IDC_LST_FRAMES,listbox,1352728841
Control6=IDC_BTN_ADD,button,1342242816
Control7=IDC_BTN_REMOVE,button,1342242816
Control8=IDC_LSTFRAMEREPEAT,listbox,1352728833
Control9=IDC_BTN_ADDALL,button,1342242816
Control10=IDC_BTN_RMVALL,button,1342242816
Control11=IDC_BUTTON1,button,1342242816

[CLS:CFrameRepeatDlg]
Type=0
HeaderFile=FrameRepeatDlg.h
ImplementationFile=FrameRepeatDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CFrameRepeatDlg

[DLG:IDD_DLG_DATAVALIDATOR]
Type=1
Class=CDlgDataValidator
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDT_DATA,edit,1350631552

[CLS:CDlgDataValidator]
Type=0
HeaderFile=DlgDataValidator.h
ImplementationFile=DlgDataValidator.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgDataValidator

[DLG:IDD_DLG_FRAME_RAND]
Type=1
Class=CRandFrameDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDT_RAND_ID,edit,1350631552
Control4=IDC_STATIC,static,1342308352

[CLS:CRandFrameDlg]
Type=0
HeaderFile=RandFrameDlg.h
ImplementationFile=RandFrameDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CRandFrameDlg
VirtualFilter=dWC

[CLS:CConfigDlg]
Type=0
HeaderFile=ConfigDlg.h
ImplementationFile=ConfigDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CConfigDlg
VirtualFilter=dWC

[DLG:IDD_DLG_CONFIG]
Type=1
Class=CConfigDlg
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDC_SERVER_STAT,button,1342177287
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDT_IP,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDT_PORT,edit,1350631552
Control7=IDC_BTN_UPDATE,button,1342242816
Control8=IDC_STATIC,button,1342177287
Control9=IDC_CHK_CHKSUMOVERIDE,button,1342242819

