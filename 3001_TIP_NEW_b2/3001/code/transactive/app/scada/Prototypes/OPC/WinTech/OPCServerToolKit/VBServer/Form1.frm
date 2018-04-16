VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   4545
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6795
   LinkTopic       =   "Form1"
   ScaleHeight     =   4545
   ScaleWidth      =   6795
   StartUpPosition =   3  'Windows Default
   Begin VB.Timer Timer2 
      Enabled         =   0   'False
      Interval        =   2000
      Left            =   720
      Top             =   3360
   End
   Begin VB.Timer Timer1 
      Enabled         =   0   'False
      Interval        =   1000
      Left            =   120
      Top             =   3360
   End
   Begin VB.CommandButton TimerButton 
      Caption         =   "Enable Timer"
      Height          =   375
      Left            =   240
      TabIndex        =   14
      Top             =   2160
      Width           =   1815
   End
   Begin VB.TextBox Tag4 
      Height          =   285
      Left            =   4320
      TabIndex        =   13
      Text            =   "4.00"
      Top             =   3000
      Width           =   975
   End
   Begin VB.TextBox Tag3 
      Height          =   285
      Left            =   4320
      TabIndex        =   12
      Text            =   "3.00"
      Top             =   2400
      Width           =   975
   End
   Begin VB.TextBox Tag2 
      Height          =   285
      Left            =   4320
      TabIndex        =   11
      Text            =   "2.00"
      Top             =   1680
      Width           =   975
   End
   Begin VB.Frame Frame1 
      Caption         =   "Tag List"
      Height          =   3495
      Left            =   2520
      TabIndex        =   5
      Top             =   480
      Width           =   3855
      Begin VB.TextBox TAG1 
         Height          =   285
         Left            =   1800
         TabIndex        =   10
         Text            =   "1.00"
         Top             =   600
         Width           =   975
      End
      Begin VB.Label Label4 
         Caption         =   "FOUR"
         Height          =   375
         Left            =   480
         TabIndex        =   9
         Top             =   2640
         Width           =   615
      End
      Begin VB.Label Label3 
         Caption         =   "THREE"
         Height          =   375
         Left            =   480
         TabIndex        =   8
         Top             =   1960
         Width           =   615
      End
      Begin VB.Label Label2 
         Caption         =   "TWO"
         Height          =   375
         Left            =   480
         TabIndex        =   7
         Top             =   1280
         Width           =   615
      End
      Begin VB.Label Label1 
         Caption         =   "ONE"
         Height          =   375
         Left            =   480
         TabIndex        =   6
         Top             =   600
         Width           =   615
      End
   End
   Begin VB.CommandButton UnregButton 
      Caption         =   "Unregister Server"
      Height          =   375
      Left            =   240
      TabIndex        =   4
      Top             =   2640
      Width           =   1815
   End
   Begin VB.CommandButton UpdateButtton 
      Caption         =   "Update Tags"
      Height          =   375
      Left            =   240
      TabIndex        =   3
      Top             =   1680
      Width           =   1815
   End
   Begin VB.CommandButton AddButton 
      Caption         =   "Add Tags"
      Height          =   375
      Left            =   240
      TabIndex        =   2
      Top             =   1200
      Width           =   1815
   End
   Begin VB.CommandButton InitButton 
      Caption         =   "Initialize Server"
      Height          =   375
      Left            =   240
      TabIndex        =   1
      Top             =   720
      Width           =   1815
   End
   Begin VB.CommandButton RegButton 
      Caption         =   "Register Server"
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   240
      Width           =   1815
   End
   Begin VB.Label Label5 
      Caption         =   " "
      Height          =   255
      Left            =   120
      TabIndex        =   15
      Top             =   4080
      Width           =   6255
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim GUID(16) As Byte
Dim SvrName(100) As Byte
Dim SvrDesc(100) As Byte
Dim ExePath(100) As Byte
Dim TagName(100) As Byte
Dim VendorInfo(100) As Byte
Dim CurrentTime    As Date
Dim MyTimeStamp As Double

Dim error As Boolean ' I think Boolean in VB is a short int, and I think this should a 32 bit int,i.e Vb data type Long

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
   Dim numberclients As Long
   
   numberclients = NumbrClientConnections()
   
   ' If still connected
   If (numberclients <> 0) Then
        ' disconnect and delay a little
        RequestDisconnect
        Timer1.Enabled = False
        ' close form when Timer2 expires
        Cancel = True
        Timer2.Enabled = True
        End If
            
End Sub

Private Sub Timer2_Timer()
' Delayed form close
 Unload Form1
 
End Sub

Private Sub Form_Load()


TagHandle(0) = -1   'initialize TagHandles to -1
TagHandle(1) = -1
TagHandle(2) = -1
TagHandle(3) = -1

'initialize GUID, SvrName, SvrDesc & Exe Path

' GUID from GUIDGEN
' {BB352C70-0BB4-11d4-80C0-00C04F790F3B}
'{ 0xbb352c70, 0xbb4, 0x11d4, { 0x80, 0xc0, 0x0, 0xc0, 0x4f, 0x79, 0xf, 0x3b } };
GUIDToByte "{BB352C70-0BB4-11d4-80C0-00C04F790F3B}", GUID()
StringToByte "VBServer", SvrName()
StringToByte "VB Example OPC Server", SvrDesc()
StringToByte App.Path & "\" & App.EXEName & ".exe", ExePath()
StringToByte "ABC Corp.", VendorInfo()

'UnComment the following three lines to automatically
' initialize the server dll, create the tags and start the timer
'(Useful for testing VB app installed as NT Service)
'InitButton_Click
'AddButton_Click
'TimerButton_Click

End Sub

Private Sub Form_Unload(Cancel As Integer)

error = RemoveTag(ByVal TagHandle(0))
error = RemoveTag(ByVal TagHandle(1))
error = RemoveTag(ByVal TagHandle(2))
error = RemoveTag(ByVal TagHandle(3))

End Sub

Private Sub RegButton_Click()

' make the necessary additions to the Windows Registry
error = UpdateRegistry(GUID(0), SvrName(0), SvrDesc(0), ExePath(0))

End Sub


Private Sub InitButton_Click()

' Initialize WtOPCSvr
error = SetVendorInfo(VendorInfo(0))
error = InitWTOPCsvr(GUID(0), 1000)
error = EnableWriteNotification(AddressOf MyWriteCallback, True)
error = EnableUnknownItemNotification(AddressOf MyUnknownItemCallback)


End Sub


Private Sub AddButton_Click()

' Add four OPC items
' OPC_QUALITY_GOOD literalizes to 0xc0 (192)
StringToByte "ONE", TagName()
TagHandle(0) = CreateTag(TagName(0), TAG1.Text, 192, True)

StringToByte "TWO", TagName()
TagHandle(1) = CreateTag(TagName(0), Tag2.Text, 192, True)

StringToByte "THREE", TagName()
TagHandle(2) = CreateTag(TagName(0), Tag3.Text, 192, True)

StringToByte "FOUR", TagName()
TagHandle(3) = CreateTag(TagName(0), Tag4.Text, 192, True)

End Sub

Private Sub Timer1_Timer()
TAG1.Text = TAG1.Text + 1
Tag2.Text = Tag2.Text + 1
Tag3.Text = Tag3.Text + 1
Tag4.Text = Tag4.Text + 1
error = UpdateTag(TagHandle(0), TAG1.Text, 192)
error = UpdateTag(TagHandle(1), Tag2.Text, 192)
error = UpdateTag(TagHandle(2), Tag3.Text, 192)
error = UpdateTag(TagHandle(3), Tag4.Text, 192)

End Sub

Private Sub TimerButton_Click()
Timer1.Enabled = True
End Sub

Private Sub UpdateButtton_Click()

' Update the OPC Item Values
' The application can either provide it's own timestamp
' or use the default current time stamped by the dll

CurrentTime = Now
error = ConvertVBDateToFileTime1(CurrentTime, MyTimeStamp)

error = UpdateTagWithTimeStamp(TagHandle(0), TAG1.Text, 192, MyTimeStamp)
'error = UpdateTag(TagHandle(0), Tag1.Text, 192)
error = UpdateTag(TagHandle(1), Tag2.Text, 192)
error = UpdateTag(TagHandle(2), Tag3.Text, 192)
error = UpdateTag(TagHandle(3), Tag4.Text, 192)
End Sub

Private Sub UnregButton_Click()

' Remove the Registry Entries
error = UnregisterServer(GUID(0), SvrName(0))
End Sub

