//Windows 10 2004 Build 19041.572
HANDLE LdrGetDllHandle(PWCHAR Path, PVOID Unused, PUNICODE_STRING ModuleFileName, PHANDLE Module){
	return LdrGetDllHandleEx(1, Path, Unused, ModuleFileName, Module);
}
