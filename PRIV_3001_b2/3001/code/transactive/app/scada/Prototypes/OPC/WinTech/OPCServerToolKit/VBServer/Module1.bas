Attribute VB_Name = "Module1"
Public Type FileTime
    dwLowDateTime As Long
    dwHighDateTime As Long
   End Type
   
'
'  Initialization and Registration functions
'
'  WTOPCsvrRevision()
'      Simply returns a version identifier for the DLL
Declare Function WTOPCsvrRevision Lib "WtOPCSvr" Alias "_WTOPCsvrRevision@0" () As Integer
'
'  InitWTOPCsvrB()
'      Initializes DCOM, Security, and creates the Server ClassFactory
Declare Function InitWTOPCsvr Lib "WtOPCSvr" Alias "_InitWTOPCsvr@8" (ByRef CLSID_Svr As Byte, ByVal ServerRate As Long) As Long
'
'  UninitWTOPCsvr()
'      Uninitializes COM.
Declare Function UninitWTOPCsvr Lib "WtOPCSvr" Alias "_UninitWTOPCsvr@0" () As Long
'
'  ResetServerRate()
'      The specified ServerRate is the fastest rate at which the data can
'      be updated to a client.  The minimum server rate that may be selected is
'      10 ms.
Declare Function ResetServerRate Lib "WtOPCSvr" Alias "_ResetServerRate@4" (ByVal ServerRate As Long) As Long
'
'  SetVendorInfoA()
'      Allows the application to specify a vendor specifc string to identify
'      the server.  This string will appear as part of the OPCSERVERSTATUS structure
'      returned by the GetStatus Interface.
Declare Function SetVendorInfo Lib "WtOPCSvr" Alias "_SetVendorInfo@4" (ByRef VendorInfo As Byte) As Long
'
'  UpdateRegistryB()
'      Makes the appropriate entries to the Windows Registry to identify the Server.
'      the argument types.
Declare Function UpdateRegistry Lib "WtOPCSvr" Alias "_UpdateRegistry@16" (ByRef CLSID_Svr As Byte, ByRef Name As Byte, ByRef Descr As Byte, ByRef ExePath As Byte) As Long
'
'  AddLocalServiceKeysToRegistry()
'      Makes additional Registry Entries required if the Server is to be installed
'      as an NT Service.
Declare Function AddLocalServiceKeysToRegistry Lib "WtOPCSvr" Alias "_AddLocalServiceKeysToRegistry@4" (ByRef Name As Byte) As Long
'
'  UnregisterServerB()
'      Removes the Registry Entries
Declare Function UnregisterServer Lib "WtOPCSvr" Alias "_UnregisterServer@8" (ByRef CLSID_Svr As Byte, ByRef Name As Byte) As Long


'
'  OPC Item Functions
'


'  CreateTagA()
'      Add an OPC Item to the WtOPCsvr.DLL local tag list.  The dll takes care of
'      all client references to the tag and provides a callback to the application
'      if the tag is updated by a client.  Once a tag gets created, it's name will
'      automatically be presented in the browse list to any interested OPC Client.
Declare Function CreateTag Lib "WtOPCSvr" Alias "_CreateTag@28" (ByRef Name As Byte, ByVal Value As Variant, ByVal InitialQuality As Integer, ByVal IsWritable As Long) As Long
'
'  UpdateTag()
'  UpdateTagWithTimeStamp()
'  UpdateTagByNameA()
'      Allows the Server Application to change the value, quality and timestamp of
'      a tag.  All updates are automatically provided to subscribing clients as defined
'      by the particular connection.
Declare Function UpdateTag Lib "WtOPCSvr" Alias "_UpdateTag@24" (ByVal TagHandle As Long, ByVal Value As Variant, ByVal Quality As Integer) As Long
Declare Function UpdateTagWithTimeStamp Lib "WtOPCSvr" Alias "_UpdateTagWithTimeStamp@32" (ByVal TagHandle As Long, ByVal Value As Variant, ByVal Quality As Integer, ByVal Timestamp As Double) As Long
Declare Function UpdateTagByName Lib "WtOPCSvr" Alias "_UpdateTagByName@24" (ByRef Name As Byte, ByVal Value As Variant, ByVal Quality As Integer) As Long
'
'''''''''''''''''''
'These three functions must be used together, and provide a more efficient means
'of updating multiple tags.
'UpdateTagToList replaces UpdateTag call, requires prior StartUpdateTags else fails
'and EndUpdateTags after all tags are updated.
'  StartUpdateTags()
Declare Function StartUpdateTags Lib "WtOPCSvr" Alias "_StartUpdateTags@0" () As Long
'  UpdateTagToList()
Declare Function UpdateTagToList Lib "WtOPCSvr" Alias "_UpdateTagToList@24" (ByVal TagHandle As Long, ByVal Value As Variant, ByVal Quality As Integer) As Long
'  EndUpdateTags()
Declare Function EndUpdateTags Lib "WtOPCSvr" Alias "_EndUpdateTags@0" () As Long
''''''''''''''''''
'
'  SetTagProperties()
'      Tag Properties are values associated with an OPC Item other than its Value,
'      Quality and TimeStamp.  Any property value may be assigned by the server
'      for a defined tag.
Declare Function SetTagProperties Lib "WtOPCSvr" Alias "_SetTagProperties@28" (ByVal TagHandle As Long, ByVal PropertyID As Long, ByRef Description As Byte, ByVal Value As Variant) As Long
'
'  ReadTag()
'  ReadTagWithTimeStamp()
'      Allows the Application to read each tag value from the WtOPCsvr.dll local Tag List.
Declare Function ReadTag Lib "WtOPCSvr" Alias "_ReadTag@8" (ByVal TagHandle As Long, ByRef Value As Variant) As Long
Declare Function ReadTagWithTimeStamp Lib "WtOPCSvr" Alias "_ReadTagWithTimeStamp&16" (ByVal TagHandle As Long, ByRef Value As Variant, ByRef Quality As Integer, ByRef Timestamp As FileTime) As Long
'
'  SuspendTagUpdates()
'      When a Tag is created by the Server Application, it is automatically enabled for
'      client subscription.  The tag name will automatically be included in the server
'      browse list and any client may connect and read it's current value.  In certain
'      applications, it is desirable to only create the tag whenever a client requests it.
'      The UNKNOWNITEMPROC callback may be used for dynamic tag allocation, but until the
'      tag is created, the tag name will not show up in the browse list.  Creation of the
'      tag followed by a call to SuspendTagUpdates() will allow the tag name to be browsed
'      by a client, but not subscribed to.  In this scenario, the WtOPCsvr.dll will issue
'      the UNKNOWNITEMPROC callback to allow the Application to enable the tag and begin
'      updating it's value only when being actively subscribed by an OPC Client.
Declare Function SuspendTagUpdates Lib "WtOPCSvr" Alias "_SuspendTagUpdates@8" (ByVal TagHandle As Long, ByVal OnOff As Long) As Long
'
'  RemoveTag()
'      Deletes a tag from the WtOPCsvr Tag List.
Declare Function RemoveTag Lib "WtOPCSvr" Alias "_RemoveTag@4" (ByVal TagHandle As Long) As Long


'
'  Auxilary Functions
'

'  NumbrClientConnections()
'      Allows the Server Application to determine the number of OPC Clients
'      currently connected.
Declare Function NumbrClientConnections Lib "WtOPCSvr" Alias "_NumbrClientConnections@0" () As Long
'
'  RequestDisconnect()
'      Provides the ability for the Server Application to request that all
'      Clients gracefully disconnect.  There is no guarantee that any client
'      will honor the request.
Declare Sub RequestDisconnect Lib "WtOPCSvr" Alias "_RequestDisconnect@0" ()
'
'  RefreshAllClients()
'      Forces the WtOPCsvr.DLL to issue data updates to all connected Clients.
'      (Primarily used for debugging.  Normal client updates are issued automatically
'      by the WtOPCsvr.DLL based on an iternal clock tick.)
Declare Function RefreshAllClients Lib "WtOPCSvr" Alias "_RefreshAllClients@0" () As Long
'
'  ConvertVBDateToFileTime1()
'  ConvertFileTimeToVBDate1()
'       Allows VB applications to convert standard Variant Date to OPC Specified Filetime
Declare Function ConvertVBDateToFileTime1 Lib "WtOPCSvr" Alias "_ConvertVBDateToFileTime1@8" (ByRef VBDate As Date, ByRef Timestamp As Double) As Long
Declare Function ConvertFileTimeToVBDate1 Lib "WtOPCSvr" Alias "_ConvertFileTimeToVBDate1@8" (ByRef Timestamp As Double, ByRef VBDate As Date) As Long

'
'   Deactivate30MinTimerA
Declare Function Deactivate30MinTimer Lib "WtOPCSvr" Alias "_Deactivate30MinTimer@4" (ByRef Authorization As Byte) As Long


'
' Support for Alarms & Events
'
' Literal Definitions for LevelID
'
'   ID_LOLO_LIMIT   1
'   ID_LO_LIMIT     2
'   ID_HI_LIMIT     3
'   ID_HIHI_LIMIT   4
'

'
'  UserAEMessage()
Declare Function UserAEMessage Lib "WtOPCSvr" Alias "_UserAEMessage@8" (ByRef Msg As Byte, ByVal Severity As Long) As Long
'
'   SetItemLevelAlarm()
Declare Function SetItemLevelAlarm Lib "WtOPCSvr" Alias "_SetItemLevelAlarm@20" (ByVal TagHandle As Long, ByVal LevelID As Long, ByVal Limit As Single, ByVal Severity As Long, ByVal Enabled As Long) As Long
'
'   GetItemLevelAlarm()
Declare Function GetItemLevelAlarm Lib "WtOPCSvr" Alias "_GetItemLevelAlarm@20" (ByVal TagHandle As Long, ByVal LevelID As Long, ByRef Limit As Single, ByRef Severity As Long, ByRef Enabled As Long) As Long



'
'  Callback Definitions
'
'  Used by the DLL to pass control back to the Server Application
'  Each callback must be explicitly enabled by calling one of the following
'  exported API functions:
'                  EnableNotification (NOTIFYPROC lpCallback);
Declare Function EnableWriteNotification Lib "WtOPCSvr" Alias "_EnableWriteNotification@8" (ByVal Callback As Long, ByVal ConvertToNative As Long) As Long
'                  EnableUnknownItemNotification (UNKNOWNITEMPROC lpCallback);
Declare Function EnableUnknownItemNotification Lib "WtOPCSvr" Alias "_EnableUnknownItemNotification@4" (ByVal Callback As Long) As Long
'                  EnableItemRemovalNotification (ITEMREMOVEDPROC lpCallback);
Declare Function EnableItemRemovalNotification Lib "WtOPCSvr" Alias "_EnableItemRemovalNotification@4" (ByVal Callback As Long) As Long
'                  EnableDisconnectNotification (DISCONNECTPROC lpCallback);
Declare Function EnableDisconnectNotification Lib "WtOPCSvr" Alias "_EnableDisconnectNotification@4" (ByVal Callback As Long) As Long
'                  EnableEventMsgs (EVENTMSGPROC lpCallback);
Declare Function EnableEventMsgs Lib "WtOPCSvr" Alias "_EnableEventMsgs@4" (ByVal Callback As Long) As Long
'                  EnableDeviceRead (DEVICEREADPROC lpCallback);
Declare Function EnableDeviceRead Lib "WtOPCSvr" Alias "_EnableDeviceRead@4" (ByVal Callback As Long) As Long
'
'
'  NOTIFYPROC
'      Signals the Application that an OPC Client has written data to a
'      tag.  HANDLE value represents the tag item that was written
'       NewValue is teh value written and DeviceError allows the application to
'       provide an error indication back to the client if the write to the device fails
Sub NotificationCallback(ByVal TagHandle As Long, ByRef newValue As Variant, ByRef DeviceError As Long)

End Sub
'  UNKNOWNITEMPROC
'      Signals the Application that a Client has requested a tag that has
'      not been previously defined, (or has been suspended).  The two character
'      pointer arguments represent the OPC Path Name and Item Name of the requested
'      tag.  This callback allows the Server Application to operate with dynamic
'      tags that are created and maintained only when subscribed to by a connected
'      Client.
Sub UnknownItemCallback(ByRef PathName As Byte, ByRef ItemName As Byte)

End Sub
'  ITEMREMOVEDPROC
'      Works in conjuction with the above UNKNOWNITEMPROC.  This callback signals the
'      Application that the last client subscription for a given item has ended.  The
'      HANDLE of the tag item is returned as well as the Path and Item Name.  The
'      Server Application may choose to remove or suspend the tag when no clients are
'      subscribing to the data.
Sub ItemremovedCallback(ByVal Itemhandle As Long, ByRef PathName As Byte, ByRef ItemName As Byte)

End Sub
'  DISCONNECTPROC
'      Notifies the Server Application whenever a client disconnects.  The DWORD argument
'      defines the number of client connections remaining.  This callback may be used
'      to shutdown the server when the last client disconnects.
Sub DisconnectCallback(ByVal nConnects As Long)

End Sub
'  EVENTMSGPROC
'      Allows the Application to receive event messages from the WtOPCsvr.dll for
'      tracing OPC Client Interface calls.  Primarily used for debugging purposes.
'
Sub EventMsgCallback(ByRef Msg As Byte)

End Sub
'  DEVICEREADPROC
'      Callback into the application whenever a client requests a SyncIO Read
'      with dwSource set to OPC_DS_DEVICE.
'
Sub DeviceReadCallback(ByVal TagHandle As Long, ByRef newValue As Variant, ByRef Quality As Integer, ByRef Timestamp As Date)


End Sub


