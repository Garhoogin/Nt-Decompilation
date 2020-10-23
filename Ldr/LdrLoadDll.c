//note that the second parameter is a pointer.
NTSYSAPI NTSTATUS LdrLoadDll(PWCHAR Path, PULONG Flags, PUNICODE_STRING ModuleFileName, PHANDLE ModuleHandle){
	//align stack to 8 byte boundary, reserve 0x174 bytes of locals
	ULONG DllLoadFlags;
	if(Flags == NULL)
		DllLoadFlags = 0;
	else
		DllLoadFlags = LdrpDllCharacteristicsToLoadFlags(*Flags); //__fastcall function
	if(LdrpDebugFlags & 9)
		LdrpLogDbgPrint("minkernel\\ntdll\\ldrapi.c", 0x97, "LdrLoadDll", 3, "DLL name: %wZ\n", ModuleFileName);
	if(LdrpPolicyBits & 4 == 0){
		if(Path & 0x401 == 0x401)
			return STATUS_INVALID_PARAMETER;
	}
	if(DllLoadFlags & 8 && !(LdrpPolicyBits & 8)){
		if(LdrpDebugFlags & 3)
			LdrpLogDbgPrint("minkernel\\ntdll\\ldrapi.c", 0xAC, "LdrLoadDll", 0, "Nonpackaged process attempted to load a packaged DLL\n");
		if(LdrpDebugFlags & 0x10)
			__debugbreak();
		if(LdrpDebugFlags & 9)
			LdrpLogDbgPrint("minkernel\\ntdll\\ldrapi.c", 0xCC, "LdrLoadDll", 4, "Status: 0x%08lx\n", 0xC00001AA);
		return 0xC00001AA; //unknown status value?
	}
	if(NtCurrentTeb()->LoaderWorker){
		if(LdrpDebugFlags & 9)
			LdrpLogDbgPrint("minkernel\\ntdll\\ldrapi.c", 0xCC, "LdrLoadDll", 4, "Status: 0x%08lx\n", STATUS_INVALID_THREAD);
		return STATUS_INVALID_THREAD;
	}
	LdrpInitializeDllPath(ModuleFileName->Buffer, Path, &var128); //__fastcall function
	LdrpLogDllState(0, ModuleFileName, 0x14A8); //__fastcall function
	UNICODE_STRING String;
	String.Length = 0;
	String.MaximumLength = 0x100;
	String.Buffer = StringBuffer;
	WCHAR StringBufferw[128];
	StringBufferw[0] = L'\0';
	PVOID BaseAddress;
	NTSTATUS Status = LdrpPreprocessDllName(ModuleFileName, &String, 0, &DllLoadFlags); //__fastcall function
	if(NT_SUCCESS(Status))
		LdrpLoadDllInternal(&String, &var128, DllLoadFlags, 4, 0, 0, &BaseAddess, &Status); //__fastcall function
	if(String.Buffer != &StringBuffer)
		NtdllpFreeStringRoutine(var24);
	String.Length = 0;
	String.MaximumLength = 0x100;
	String.Buffer = StringBuffer;
	StringBufferw[0] = L'\0';
	LdrpLogDllState(0, ModuleFileName, 0x14A9); //__fastcall function
	if(var174b != 0)
		RtlReleasePath(var128);
	if(NT_SUCCESS(Status)){
		*ModuleHandle = (HMODULE) BaseAddess;
		LdrpDereferenceModule(BaseAddess, ModuleHandle); //__fastcall function
	}
	if(LdrpDebugFlags & 9)
		LdrpLogDbgPrint("minkernel\\ntdll\\ldrapi.c", 0xCC, "LdrLoadDll", 4, "Status: 0x%08lx\n", Status);
	return Status;
}
