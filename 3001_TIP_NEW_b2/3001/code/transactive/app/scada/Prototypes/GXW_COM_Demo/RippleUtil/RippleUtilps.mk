
RippleUtilps.dll: dlldata.obj RippleUtil_p.obj RippleUtil_i.obj
	link /dll /out:RippleUtilps.dll /def:RippleUtilps.def /entry:DllMain dlldata.obj RippleUtil_p.obj RippleUtil_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del RippleUtilps.dll
	@del RippleUtilps.lib
	@del RippleUtilps.exp
	@del dlldata.obj
	@del RippleUtil_p.obj
	@del RippleUtil_i.obj
