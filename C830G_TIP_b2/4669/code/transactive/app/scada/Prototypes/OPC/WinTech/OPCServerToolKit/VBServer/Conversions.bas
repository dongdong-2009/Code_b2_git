Attribute VB_Name = "Conversions"
Sub GUIDToByte(strGUID As String, GUID() As Byte)
    ' Copy a GUID in string form from GUIDGEN.EXE into a VB byte array for WinTech OPC server.
    ' Required format, including brackets. "{BB352C70-0BB4-11d4-80Cr0-00C04F790F3B}"
    GUID(0) = Val("&H" & Mid$(strGUID, 8, 2))
    GUID(1) = Val("&H" & Mid$(strGUID, 6, 2))
    GUID(2) = Val("&H" & Mid$(strGUID, 4, 2))
    GUID(3) = Val("&H" & Mid$(strGUID, 2, 2))
    GUID(4) = Val("&H" & Mid$(strGUID, 13, 2))
    GUID(5) = Val("&H" & Mid$(strGUID, 11, 2))
    GUID(6) = Val("&H" & Mid$(strGUID, 18, 2))
    GUID(7) = Val("&H" & Mid$(strGUID, 16, 2))
    GUID(8) = Val("&H" & Mid$(strGUID, 21, 2)) 'Swapped due to Intel "big endian" format
    GUID(9) = Val("&H" & Mid$(strGUID, 23, 2)) '
    GUID(10) = Val("&H" & Mid$(strGUID, 26, 2))
    GUID(11) = Val("&H" & Mid$(strGUID, 28, 2))
    GUID(12) = Val("&H" & Mid$(strGUID, 30, 2))
    GUID(13) = Val("&H" & Mid$(strGUID, 32, 2))
    GUID(14) = Val("&H" & Mid$(strGUID, 34, 2))
    GUID(15) = Val("&H" & Mid$(strGUID, 36, 2))
End Sub
Sub StringToByte(strString As String, ByteArray() As Byte)
    'Copy a string to a null terminalted VB byte array.
    Dim i As Long
    For i = 1 To Len(strString)
        ByteArray(i - 1) = Asc(Mid$(strString, i, 1))
    Next i
    ByteArray(i - 1) = 0
End Sub

