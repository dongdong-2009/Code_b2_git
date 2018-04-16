Attribute VB_Name = "Module2"
Public TagHandle(4) As Long

Dim error As Long

Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" (ByRef dest As Any, ByRef source As Any, ByVal numbytes As Long)


Sub MyWriteCallback(ByVal Itemhandle As Long, ByRef newValue As Variant, ByRef DeviceError As Long)
    DeviceError = 0
    If (Itemhandle = TagHandle(0)) Then
        Form1.TAG1.Text = newValue
        error = UpdateTag(TagHandle(0), newValue, 192)
        End If
    If (Itemhandle = TagHandle(1)) Then
        Form1.Tag2.Text = newValue
        error = UpdateTag(TagHandle(1), newValue, 192)
        End If
    If (Itemhandle = TagHandle(2)) Then
        Form1.Tag3.Text = newValue
        error = UpdateTag(TagHandle(2), newValue, 192)
        End If
    If (Itemhandle = TagHandle(3)) Then
        Form1.Tag4.Text = newValue
        error = UpdateTag(TagHandle(3), newValue, 192)
        End If
End Sub


Sub MyUnknownItemCallback(ByRef Path As Byte, ByRef Item As Byte)
Dim ItemName(100) As Byte
Dim PathName(100) As Byte
Dim TagName As String

CopyMemory PathName(0), Path, 100
CopyMemory ItemName(0), Item, 100

TagName = "Client Requested Unknown Item: "
For i = 0 To 99
    If PathName(i) <> 0 Then
        TagName = TagName + Chr(PathName(i))
        Else
            Exit For
        End If
    Next i
    
If PathName(0) <> 0 Then
    TagName = TagName + "."
    End If
    
For i = 0 To 99
    If ItemName(i) <> 0 Then
        TagName = TagName + Chr(ItemName(i))
        Else
            Exit For
        End If
    Next i
  
Form1.Label5 = TagName
 
End Sub

