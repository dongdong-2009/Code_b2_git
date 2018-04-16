###############################################################################################
##
##
##
##
##
###############################################################################################

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "MFT transActive"
;!define PRODUCT_VERSION "00.00.00.00" this value will be obtained from command line
;!define INSTALL_DIR "c:\transActive" this value will be obtained from command line
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define SETUP_FILE "--SetupFile="
!define TRANSACTIVE_INI_FILE "c:\transActive.ini"
!define PRIMARY_SERVER "PrimaryServer"
!define SECONDARY_SERVER "SecondaryServer"
!define LOCATION_NAME "LocationName"
!define DB_CONNECTION_STRING "DBConnectionString"
!define DEBUG_LEVEL "DebugLevel"

var SetupFileName
var PrimaryServer
var SecondaryServer
var LocationName
var ConnectionStringFile
var DBConnectionString
var DebugLevel
var BackupFlag

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "WordFunc.nsh"
!include "FileFunc.nsh"
!include "Locate.nsh"
!include "EnvVarUpdate.nsh"
!include "StrFunc.nsh"
!include "Library.nsh"

!insertmacro GetParameters

Reservefile MFT_Installer_UI.ini

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
Page custom creator_TransActiveInfoFunction leave_TransActiveInfoFunction ""

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "install-MFT-T${PRODUCT_VERSION}.exe"
InstallDir "c:\transActive"
ShowInstDetails show
ShowUnInstDetails show

;onInit ---------------------------------------------------------------------------
Function .onInit
  LogSet on
  SetDetailsPrint both
  
  ####### Option1: by commandline #######
  /*
  ** if (setupfile == found)
  **     use setupfile info
  **     if (transActive.ini == found)
  **         do nothing
  **     else (tranActive.ini == not found)
  **         copy setupfile to transActive.ini
  ** else (setupfile == not found)
  **      if (transActive.ini == found)
  **          use transActive.ini info
  **      else (transActive.ini == not found)
  **          installation cancelled
  ** Note: if setupfile found and transActive.ini found, will use setupfile info.
  */
  ;get install directory from command line
  StrCpy $INSTDIR ${INSTALL_DIR}
  StrCpy $DebugLevel "INFO"
  
  #### check dependent software installed or not
  call fn_CheckDependentSoftware
  
  ;IfSilent parseCommandLine displayPage
  IfSilent parseCommandLine end
  
  parseCommandLine:
    execWait "echo Installation start..."
    ${GetParameters} $R0

    #### extract parameters
    ${GetOptions} $R0 ${SETUP_FILE} $SetupFileName
    DetailPrint "SetupFile = $SetupFileName"
    StrCmp $SetupFileName "" NoSetupFile SetupFileFound

  NoSetupFile:
    ##>>>>> MessageBox MB_OK "setup file not found... check if transActive.ini file in c:"
    ;check if got transActive.ini file in c:
    IfFileExists ${TRANSACTIVE_INI_FILE} TransActiveIniFileFound TransActiveIniFileNotFound
    
  TransActiveIniFileFound:
    ##>>>>> MessageBox MB_OK "transActive.ini file found"
    push ${TRANSACTIVE_INI_FILE}
    call fn_ExtractIniFile
    goto end
  
  TransActiveIniFileNotFound:
    ##>>>>> MessageBox MB_OK "transActive.ini file not found"
    ;no setup file, no transActive.ini file.. cannot continue.. installation cancelled.
    LogText "No setup file provided... Installation Cancelled"
    Abort "No setup file provided... Installation Cancelled"

  SetupFileFound:
    ##>>>>> MessageBox MB_OK "Setup File option found"
    push $SetupFileName
    call fn_ExtractIniFile
    
    ;check if need to save the information in setupFile to transactive.ini file
    IfFileExists ${TRANSACTIVE_INI_FILE} TransActiveIniFileFound0 TransActiveIniFileNotFound0
    
    TransActiveIniFileFound0:
    ;found transactive file, do nothing
    goto end
    
    TransActiveIniFileNotFound0:
    ;save info in setupFile to transActive.ini file
    CopyFiles $SetupFileName ${TRANSACTIVE_INI_FILE}
    goto end
  goto end

  end:
    LogText "onInit Completed"
FunctionEnd

;Copy Files ----------------------------------------------------
Section "Copy Files" SEC01
  
  ;Note: The install folder must be in staging\mft
  SetOutPath "$INSTDIR"
  
  ;file copy
  File /r "staging\mft\*.*"
SectionEnd

;File Registration---------------------------------------------------------------------------
Section "File Registration"
  ExecWait "regsvr32 /s $INSTDIR\lib\AtsComBridge.dll"
  ExecWait "regsvr32 /s $INSTDIR\lib\opccomn_ps.dll"
  ExecWait "regsvr32 /s $INSTDIR\lib\opcproxy.dll"
  ExecWait "regsvr32 /s $INSTDIR\lib\opc_aeps.dll"
  ExecWait "regsvr32 /s $INSTDIR\lib\opchda_ps.dll"
  ExecWait "regsvr32 /s $INSTDIR\lib\BCGDurationCtrl.ocx"
  ExecWait "regsvr32 /s $INSTDIR\lib\TeeChart7.ocx"
  ExecWait "regsvr32 /s $INSTDIR\lib\craxdrt9.dll"
  ExecWait "regsvr32 /s $INSTDIR\lib\crqe.dll"
  ExecWait "regsvr32 /s $INSTDIR\lib\crtslv.dll"
  ExecWait "regsvr32 /s $INSTDIR\lib\crviewer9.dll"
  ExecWait "regsvr32 /s $INSTDIR\lib\ExportModeller.dll"

/*
  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\AtsComBridge.dll" \
               "$INSTDIR\lib\AtsComBridge.dll" \
               "$INSTDIR\lib"

  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\opccomn_ps.dll" \
               "$INSTDIR\lib\opccomn_ps.dll" \
               "$INSTDIR\lib"

  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\opcproxy.dll" \
               "$INSTDIR\lib\opcproxy.dll" \
               "$INSTDIR\lib"

  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\opc_aeps.dll" \
               "$INSTDIR\lib\opc_aeps.dll" \
               "$INSTDIR\lib"

  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\opchda_ps.dll" \
               "$INSTDIR\lib\opchda_ps.dll" \
               "$INSTDIR\lib"

  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\BCGDurationCtrl.ocx" \
               "$INSTDIR\lib\BCGDurationCtrl.ocx" \
               "$INSTDIR\lib"

  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\TeeChart7.ocx" \
               "$INSTDIR\lib\TeeChart7.ocx" \
               "$INSTDIR\lib"

  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\craxdrt9.dll" \
               "$INSTDIR\lib\craxdrt9.dll" \
               "$INSTDIR\lib"

  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\crqe.dll" \
               "$INSTDIR\lib\crqe.dll" \
               "$INSTDIR\lib"

  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\crtslv.dll" \
               "$INSTDIR\lib\crtslv.dll" \
               "$INSTDIR\lib"

  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\crviewer9.dll" \
               "$INSTDIR\lib\crviewer9.dll" \
               "$INSTDIR\lib"

  !insertmacro InstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "staging\mft\lib\ExportModeller.dll" \
               "$INSTDIR\lib\ExportModeller.dll" \
               "$INSTDIR\lib"
*/
SectionEnd

;Environment - set the environment variable --------------------------------------------------
Section "Environment"
  StrCpy $0 ${ENV_PATH}
  StrCpy $1 1
  
  loop:
  ${WordFind} "$0" ";" "E+$1" $R0
  ;MessageBox MB_OK "AfterWordFind $$R0 = $R0"

  IfErrors 0 code
  StrCmp $1$R0 11 0 error
  StrCpy $R0 $0
    ;check if variable already in PATH
    ReadRegStr $R1 HKLM "SYSTEM\ControlSet001\Control\Session Manager\Environment" "Path"
    
    ${WordFind} "$R1" "$R0" "E+1" $R2

    ifErrors notfound0 found0

    notfound0:
      ${EnvVarUpdate} $R3 "PATH" "A" "HKLM" $R0
      goto end
      
    found0:
    goto end
    
  code:
    ;check if variable already in PATH
    ReadRegStr $R1 HKLM "SYSTEM\ControlSet001\Control\Session Manager\Environment" "Path"

    ${WordFind} "$R1" "$R0" "E-1" $R2

    ifErrors notfound1 found1

    notfound1:
      ${EnvVarUpdate} $R3 "PATH" "A" "HKLM" $R0
      goto next1
      
    found1:
      goto next1
      
    next1:
      IntOp $1 $1 + 1
      goto loop

  error:
    StrCpy $1 ''
    StrCpy $R0 ''

  end:
SectionEnd

;CreateShortCuts - set the shortcuts --------------------------------------------------
Section "CreateShortCuts"
  ;create ControlStation shortcut on desktop
  CreateShortCut "$DESKTOP\ControlStation.lnk" \
                 "$INSTDIR\bin\ControlStation.exe" \
                 "--entity-name=ControlStation --run-param-file=$INSTDIR\start.ini"

  ;create Controlstation shortcut on quick laucnh
  CreateShortCut "$QUICKLAUNCH\ControlStation.lnk" \
                 "$INSTDIR\bin\ControlStation.exe" \
                 "--entity-name=ControlStation --run-param-file=$INSTDIR\start.ini"

  ;create ConfigurationEditor shortcut on quick laucnh
  CreateShortCut "$QUICKLAUNCH\ConfigurationEditor.lnk" \
                 "$INSTDIR\bin\ConfigurationEditor.exe" \
                 "--entity-name=ConfigurationEditor --debug-file=$INSTDIR\logs\Log_ConfigurationEditor.log \
                  --debug-level=$DebugLevel --db-connection-file=$INSTDIR\config\database\ConfigConnectionStrings.csv"

  ;create SystemManager shortcut on quick laucnh
  CreateShortCut "$QUICKLAUNCH\SystemManager.lnk" \
                 "$INSTDIR\bin\SystemManager.exe" \
                 "--entity-name=SystemManager --host-name=$PrimaryServer \
                  --db-connection-file=$INSTDIR\config\database\$ConnectionStringFile"

  ;create EventExtractor shortcut on quick laucnh
  CreateShortCut "$QUICKLAUNCH\EventExtractor.lnk" \
                 "$INSTDIR\bin\EventExtractor.exe" \
                 "--db-connection-file=$INSTDIR\config\database\$ConnectionStringFile" ""
SectionEnd

;Execute Applications --------------------------------------------------
Section "ExecutePrograms"
  ExecWait "$INSTDIR\bin\TA_AlarmStoreService \
             --entity-name=AlarmStore \
             --operation-mode=Control \
             --host-names=$PrimaryServer,$SecondaryServer \
             --db-connection-file=$INSTDIR\config\database\$ConnectionStringFile \
             --debug-level=$DebugLevel \
             --debug-file=$INSTDIR\logs\Log_AlarmStoreService.log -install"
  
  ExecWait "$INSTDIR\bin\TA_OpcBridgeService \
            --entity-name=OPCBridge \
            --host-names=$PrimaryServer,$SecondaryServer \
            --db-connection-file=$INSTDIR\config\database\$ConnectionStringFile \
            --debug-level=$DebugLevel \
            --precreate-opctag=1,3,4,5,14 \
            --debug-file=$INSTDIR\logs\Log_OpcBridge.log -install"
SectionEnd

;Create start.ini file ----------------------------------------------------------
Section "CreateStartINIFile"
  FileOpen $R9 "$INSTDIR\start.ini" w
  FileWrite $R9 "--debug-level=$DebugLevel$\n"
  FileWrite $R9 "--help-file=$INSTDIR\help\Control_Station_Help.chm$\n"
  FileWrite $R9 "--debug-file=$INSTDIR\logs\ControlStation.log$\n"
  FileWrite $R9 "--db-connection=$DBConnectionString$\n"
  FileWrite $R9 "--project-name=4669$\n"
  FileWrite $R9 "--db-connection-file=$INSTDIR\config\database\$ConnectionStringFile$\n"
  FileWrite $R9 "--host-names=$PrimaryServer,$SecondaryServer$\n"
  FileClose $R9
SectionEnd

;Startup services ----------------------------------------------------------
Section "StartServices"
  nsSCM::Start "Alarm Store Service"
  nsSCM::Start "OPC Bridge"
SectionEnd

;Write String to Registry ----------------------------------------------------------
Section "RegistrySettings"
  WriteRegStr "HKLM" "SOFTWARE\Wintertree\SSCE" "MainLexPath" "$INSTDIR\lib"
  WriteRegStr "HKLM" "SOFTWARE\Wintertree\SSCE" "MainLexFiles" "ssceam.tlx,ssceam2.clx,accent.tlx,tech.tlx,userdic.tlx,correct.tlx"
  WriteRegStr "HKLM" "SOFTWARE\Wintertree\SSCE" "HelpFile" "$INSTDIR\help\ssce.hlp"
  WriteRegStr "HKLM" "SOFTWARE\Crystal Decisions\9.0\Crystal Reports" "CommonFiles" "$INSTDIR\lib"
SectionEnd

;---------------------------------------------------------------------------
Section -AdditionalIcons
  ;SetOutPath $INSTDIR
  ;CreateDirectory "$SMPROGRAMS\MFT transActive"
  ;CreateShortCut "$SMPROGRAMS\MFT transActive\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

;---------------------------------------------------------------------------
Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  ;WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  ;WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  ;WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
SectionEnd

;---------------------------------------------------------------------------
Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer." /SD IDOK
  LogSet off
  Rename $INSTDIR\install.log $INSTDIR\uninstall_${PRODUCT_VERSION}.log
FunctionEnd

Function un.onInit
  Rename $INSTDIR\install.log $INSTDIR\install_${PRODUCT_VERSION}.log
  LogSet on
  SetDetailsPrint both
  
  DetailPrint "Uninstallation start..."
  IfSilent end
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
           "Are you sure you want to completely remove $(^Name) and all of its components?" \
           IDYES CheckBackupRequired \
           IDNO CancelledUninstallation
  CancelledUninstallation:
  Abort
  goto end
  
  CheckBackupRequired:
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
           "Do you want to backup $INSTDIR?" \
           IDYES Backup \
           IDNO NoBackup
           
  Backup:
    ;set backup flag to true
    StrCpy $BackupFlag "true"
    DetailPrint "Backup $INSTDIR is required"
  goto end
  
  NoBackup:
    ;set backup flag to false
    StrCpy $BackupFlag "false"
    DetailPrint "Backup $INSTDIR is NOT required"
  goto end
  
  end:
FunctionEnd

;Check if any application is still running --------------------------------------------------
Section un.CheckRunningProcess
  ;check if any application still running, if so.. stop the uninstall and display a message
  ${locate::Open} "$INSTDIR\bin" "/F=1 /D=0" $0
  StrCmp $0 0 0 loop
  DetailPrint "$INSTDIR\bin is empty"
  goto close
  
  loop:
    ${locate::Find} $0 $1 $2 $3 $4 $5 $6
    ;$1        "path\name"
    ;$2        "path"
    ;$3        "name"
    ;$4        "size"        ("" if directory founded)
    ;$5        "time"        (e.g. "29.08.2005 14:27:18")
    ;$6        "attributes"  (e.g. "r-h-", "rahs")

    StrCmp $3 "" close ;no more
    
    FindProcDLL::FindProc $3
    StrCmp $3 "TA_AlarmStoreService.exe" loop ;dont need to check AlarmStoreService as it will be auto stopped.
    StrCmp $3 "TA_OpcBridgeService.exe" loop ;dont need to check AlarmStoreService as it will be auto stopped.
    StrCmp $R0 1 0 loop ;1 mean process found
    MessageBox MB_OK|MB_ICONSTOP "$3 is still running. Please close the application first. Uninstallation is Cancelled" /SD IDOK
    DetailPrint "$3 is still running. Please close the application first. Uninstallation is Cancelled"
    Quit
    
  close:
    ${locate::Close} $0
    ${locate::Unload}
SectionEnd

;Check if any DLL is still running --------------------------------------------------
Section un.CheckRunningDLLProcess
  ;check if any application still running, if so.. stop the uninstall and display a message
  ${locate::Open} "$INSTDIR\lib" "/F=1 /D=0" $0
  StrCmp $0 0 0 loop
  DetailPrint "$INSTDIR\lib is empty"
  goto close
  
  loop:
    ${locate::Find} $0 $1 $2 $3 $4 $5 $6
    ;$1        "path\name"
    ;$2        "path"
    ;$3        "name"
    ;$4        "size"        ("" if directory founded)
    ;$5        "time"        (e.g. "29.08.2005 14:27:18")
    ;$6        "attributes"  (e.g. "r-h-", "rahs")

    StrCmp $3 "" close ;no more

    FindProcDLL::FindProc $3
    StrCmp $3 "TA_AlarmStoreService.exe" loop ;dont need to check AlarmStoreService as it will be auto stopped.
    StrCmp $3 "TA_OpcBridgeService.exe" loop ;dont need to check AlarmStoreService as it will be auto stopped.
    StrCmp $R0 1 0 loop ;1 mean process found
    MessageBox MB_OK|MB_ICONSTOP "$3 is still running. Please close the application first. Uninstallation is Cancelled" /SD IDOK
    DetailPrint "$3 is still running. Please close the application first. Uninstallation is Cancelled"
    Quit
    
  close:
    ${locate::Close} $0
    ${locate::Unload}
SectionEnd

;Environment - set the environment variable --------------------------------------------------
Section un.Environment ;"Environment"
;Function fn_UnEnvironment
  StrCpy $0 ${ENV_PATH}
  StrCpy $1 1

  loop:
  ${WordFind} "$0" ";" "E+$1" $R0
  ;MessageBox MB_OK "AfterWordFind $$R0 = $R0"

  IfErrors 0 code
  StrCmp $1$R0 11 0 error
  StrCpy $R0 $0
    ;check if variable already in PATH
    ReadRegStr $R1 HKLM "SYSTEM\ControlSet001\Control\Session Manager\Environment" "Path"

    ${WordFind} "$R1" "$R0" "E+1" $R2

    ifErrors notfound0 found0

    notfound0:
    goto end
    
    found0:
      DetailPrint "Removing $R0 from PATH"
      ${un.EnvVarUpdate} $R3 "PATH" "R" "HKLM" $R0
    goto end

  code:
    ;check if variable already in PATH
    ReadRegStr $R1 HKLM "SYSTEM\ControlSet001\Control\Session Manager\Environment" "Path"
    ${WordFind} "$R1" "$R0" "E-1" $R2

    ifErrors notfound1 found1

    notfound1:
      goto next1

    found1:
      DetailPrint "Removing $R0 from PATH"
      ${un.EnvVarUpdate} $R3 "PATH" "R" "HKLM" $R0
      goto next1

    next1:
      IntOp $1 $1 + 1
      goto loop

  error:
    StrCpy $1 ''
    StrCpy $R0 ''

  end:
SectionEnd

Section Uninstall
  ;Delete Registry Value
  DetailPrint "Deleting Registry Value..."

  DeleteRegValue "HKLM" "SOFTWARE\Crystal Decisions\9.0\Crystal Reports" "CommonFiles"
  DetailPrint "Delete CommonFiles value from SOFTWARE\Crystal Decisions\9.0\Crystal Reports"
  
  DeleteRegValue "HKLM" "SOFTWARE\Wintertree\SSCE" "HelpFile"
  DetailPrint "Delete HelpFile value from SOFTWARE\Wintertree\SSCE"
  
  DeleteRegValue "HKLM" "SOFTWARE\Wintertree\SSCE" "MainLexFiles"
  DetailPrint "Delete MainLexFiles value from SOFTWARE\Wintertree\SSCE"
  
  DeleteRegValue "HKLM" "SOFTWARE\Wintertree\SSCE" "MainLexPath"
  DetailPrint "Delete MainLexPath value from SOFTWARE\Wintertree\SSCE"

  ;stop and remove all services
  nsSCM::Stop "Alarm Store Service"
  DetailPrint "Stopping Alarm Store Services... Please wait..."

  ;Note: nsSCM::Stop did not wait for the service process to fully terminate, so have to add a wait loop when
  ;before removing the service
  waitloop0:
    FindProcDLL::FindProc "TA_AlarmStoreService.exe"
    StrCmp $R0 1 0 waitdone0
    Sleep 500
  goto waitloop0

  waitdone0:
    DetailPrint "Alarm Store Services is stopped"

  nsSCM::Remove "Alarm Store Service"
  DetailPrint "Alarm Store Services is removed"
 
  nsSCM::Stop "OPC Bridge"
  DetailPrint "Stopping OPC Bridge... Please wait..."

  ;Note: nsSCM::Stop did not wait for the service process to fully terminate, so have to add a wait loop when
  ;before removing the service
  waitloop1:
    FindProcDLL::FindProc "TA_OPCBridgeService.exe"
    StrCmp $R0 1 0 waitdone1
    Sleep 500
  goto waitloop1
    
  waitdone1:
    DetailPrint "OPC Bridge is stopped"
  
  nsSCM::Remove "OPC Bridge"
  DetailPrint "OPC Bridge is removed"
  
  ;stop applications
  ExecWait "$INSTDIR\bin\TA_AlarmStoreService -remove"
  ExecWait "$INSTDIR\bin\TA_OPCBridgeService -remove"

  DetailPrint "Delete shortcuts..."
  Delete "$DESKTOP\ControlStation.lnk"
  Delete "$QUICKLAUNCH\ControlStation.lnk"
  Delete "$QUICKLAUNCH\ConfigurationEditor.lnk"
  Delete "$QUICKLAUNCH\SystemManager.lnk"
  Delete "$QUICKLAUNCH\EventExtractor.lnk"

  ;Have to unregester after backup else the dll cannot be backup
  ;unregister COM server (dll or ocx)
  DetailPrint "Unregister COM server (Dll or ocx)"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\AtsComBridge.dll"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\opccomn_ps.dll"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\opcproxy.dll"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\opc_aeps.dll"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\opchda_ps.dll"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\BCGDurationCtrl.ocx"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\TeeChart7.ocx"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\craxdrt9.dll"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\crqe.dll"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\crtslv.dll"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\crviewer9.dll"
  ExecWait "regsvr32 /u /s $INSTDIR\lib\ExportModeller.dll"

  ;check if need to backup transActive
  StrCmp $BackupFlag "false" DeleteNow BackupNow
  BackupNow:
    ;check if backup directory found
    IfFileExists "$INSTDIR_${PRODUCT_VERSION}\*.*" DeleteBackupDir CreateBackupDir
  
    CreateBackupDir:
      ;directory not yet created, create now
      CreateDirectory $INSTDIR_${PRODUCT_VERSION}
      DetailPrint "Create backup folder $INSTDIR_${PRODUCT_VERSION}"
    goto BackupFiles

    DeleteBackupDir:
      ;backup director already existed, delete all items in this backup folder
      ;Delete $INSTDIR_${PRODUCT_VERSION}\*.*
      RMDir /r $INSTDIR_${PRODUCT_VERSION}
      DetailPrint "$INSTDIR_${PRODUCT_VERSION} is not empty. Delete backup folder first"
      CreateDirectory $INSTDIR_${PRODUCT_VERSION} ;;create a empty one again.
    goto BackupFiles
  
    BackupFiles:
      ;start backup files
      CopyFiles /SILENT $INSTDIR\*.* $INSTDIR_${PRODUCT_VERSION}
      DetailPrint "Start backup $INSTDIR to $INSTDIR_${PRODUCT_VERSION}"
    goto DeleteNow


  /*
  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\AtsComBridge.dll"

  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\opccomn_ps.dll" \

  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\opcproxy.dll" \

  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\opc_aeps.dll" \

  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\opchda_ps.dll" \

  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\BCGDurationCtrl.ocx" \

  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\TeeChart7.ocx" \

  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\craxdrt9.dll" \

  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\crqe.dll" \

  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\crtslv.dll" \

  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\crviewer9.dll" \

  !insertmacro UnInstallLib \
               REGDLL \
               NOTSHARED \
               NOREBOOT_NOTPROTECTED \
               "$INSTDIR\lib\ExportModeller.dll" \
*/
  DeleteNow:
    ;delete transActive files first
    DetailPrint "Start deleting files in $INSTDIR folder"
    ${locate::Open} "$INSTDIR" "" $0
      StrCmp $0 0 0 loop
      DetailPrint "$INSTDIR is empty"
    goto close
  
    loop:
      ${locate::Find} $0 $1 $2 $3 $4 $5 $6
      ;$1        "path\name"
      ;$2        "path"
      ;$3        "name"
      ;$4        "size"        ("" if directory founded)
      ;$5        "time"        (e.g. "29.08.2005 14:27:18")
      ;$6        "attributes"  (e.g. "r-h-", "rahs")

      StrCmp $3 "" close checkIfDirectory ;if path is empty, exit loop

    checkIfDirectory:
      StrCmp $4 "" 0 skip ;if size is empty, mean it is a directory, we delete the whole directory
      RmDir /r $1
      DetailPrint "Delete $1 and all sub-directories and files in this directory"
    goto loop

    skip:
    goto loop
  
    close:
      ${locate::Close} $0
      ${locate::Unload}

    ;RMDIR /r "$INSTDIR"
    Delete "$INSTDIR\uninst.exe"
    Delete "$INSTDIR\start.ini"
  goto end
  
  end:
    ;Delete "$SMPROGRAMS\MFT transActive\Uninstall.lnk"
    ;DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
    SetAutoClose true
SectionEnd

################## Unser Define Functions #########################

;fn_ExtractIniFile ------------------------------------------------------------------
Function fn_ExtractIniFile
  LogText "fn_ExtractIniFile is called"
  Pop $R0 ;the ini file
  ReadINIStr $PrimaryServer $R0 "Info" ${PRIMARY_SERVER}
  ReadINIStr $SecondaryServer $R0 "Info" ${SECONDARY_SERVER}
  ReadINIStr $LocationName $R0 "Info" ${LOCATION_NAME}
  ReadINIStr $DBConnectionString $R0 "Info" ${DB_CONNECTION_STRING}
  ReadINIStr $DebugLevel $R0 "Info" ${DEBUG_LEVEL}
  
  ;MessageBox MB_OK "Primary Server = $PrimaryServer"
  LogText "PrimaryServer=$PrimaryServer"
  LogText "SecondaryServer=$SecondaryServer"
  LogText "LocationName=$LocationName"
  LogText "DBConnectionString=$DBConnectionString"
  LogText "DebugLevel=$DebugLevel"
  ${WordAdd} $LocationName $LocationName "+ConnectionStrings.csv" $ConnectionStringFile
  LogText "ConnectionStringFile=$ConnectionStringFile"
FunctionEnd

;fn_CheckDependentSoftware ------------------------------------------------------------------
Function fn_CheckDependentSoftware
  LogText "fn_CheckDependentSoftware is called"
  
  ;first check if oracle install
  ReadRegStr $0 HKLM SOFTWARE\ORACLE oracle_home
  IfErrors checkFail0 checkSuccess0

  checkFail0:
    LogText "Oracle not installed"
    MessageBox MB_OK "Oracle not installed" /SD IDOK
  Quit
  
  checkSuccess0:
    ;check if graphworx installed
    ReadRegStr $0 HKLM SOFTWARE\ICONICS\Common path
  
  IfErrors checkFail1 checkSuccess1
  
  checkFail1:
    LogText "GraphWorx not installed"
    MessageBox MB_OK "GraphWorx not installed" /SD IDOK
  ;Quit
  
  checkSuccess1:
FunctionEnd

Function creator_TransActiveInfoFunction
  ;check if TransActive.ini already in c:\ folder
  ;dont display if already in c:\ folder
  IfFileExists ${TRANSACTIVE_INI_FILE} TransActiveIniFileFound1 TransActiveIniFileNotFound1

  TransActiveIniFileFound1:
    push ${TRANSACTIVE_INI_FILE}
    call fn_ExtractIniFile
  goto end

  TransActiveIniFileNotFound1:
    ;display the UI
    !insertmacro MUI_INSTALLOPTIONS_EXTRACT "MFT_Installer_UI.ini"
    !insertmacro MUI_HEADER_TEXT "TransActive MFT Installer" "TransActive MFT Installer"
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "MFT_Installer_UI.ini"
  goto end

  end:
FunctionEnd

Function leave_TransActiveInfoFunction
  IfSilent end
    !insertmacro MUI_INSTALLOPTIONS_READ $PrimaryServer "MFT_Installer_UI.ini" "Field 3" "state"
    !insertmacro MUI_INSTALLOPTIONS_READ $SecondaryServer "MFT_Installer_UI.ini" "Field 5" "state"
    !insertmacro MUI_INSTALLOPTIONS_READ $LocationName "MFT_Installer_UI.ini" "Field 8" "state"
    !insertmacro MUI_INSTALLOPTIONS_READ $DBConnectionString "MFT_Installer_UI.ini" "Field 11" "state"
    !insertmacro MUI_INSTALLOPTIONS_READ $DebugLevel "MFT_Installer_UI.ini" "Field 14" "state"
    LogText "leave_TransActiveInfoFunction:PrimaryServer=$PrimaryServer"
    LogText "leave_TransActiveInfoFunction:SecondaryServer=$SecondaryServer"
    LogText "leave_TransActiveInfoFunction:LocationName=$LocationName"
    LogText "leave_TransActiveInfoFunction:DBConnectionString=$DBConnectionString"
    LogText "leave_TransActiveInfoFunction:DebugLevel=$DebugLevel"

    ${WordAdd} $LocationName $LocationName "+ConnectionStrings.csv" $ConnectionStringFile
    LogText "leave_TransActiveInfoFunction:ConnectionStringFile=$ConnectionStringFile"

    ;save info to transctive.ini file
    WriteINIStr ${TRANSACTIVE_INI_FILE} "Info" ${PRIMARY_SERVER} $PrimaryServer
    WriteINIStr ${TRANSACTIVE_INI_FILE} "Info" ${SECONDARY_SERVER} $SecondaryServer
    WriteINIStr ${TRANSACTIVE_INI_FILE} "Info" ${LOCATION_NAME} $LocationName
    WriteINIStr ${TRANSACTIVE_INI_FILE} "Info" ${DB_CONNECTION_STRING} $DBConnectionString
    WriteINIStr ${TRANSACTIVE_INI_FILE} "Info" ${DEBUG_LEVEL} $DebugLevel
  end:
FunctionEnd