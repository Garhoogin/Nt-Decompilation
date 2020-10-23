//Windows 10 2004 Build 19041.572
NTSTATUS NTAPI LdrGetProcedureAddress(HMODULE ModuleHandle, PANSI_STRING FunctionName, WORD Ordinal, PVOID *FunctionAddress){
	return LdrGetProcedureAddressForCaller(ModuleHandle, FunctionName, Ordinal, FunctionAddress, 0, _ReturnAddress());
}
