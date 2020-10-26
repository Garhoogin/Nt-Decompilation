//Windows 10 2004 Build 19041.572
VOID __fastcall LdrpLoadDllInternal(PUNICODE_STRING ModuleFileName, PVOID Arg2, ULONG Flags, int Arg4, int Arg5, LDR_DATA_TABLE_ENTRY *Entry, PVOID *BaseAddress, NTOutStatus *OutOutStatus){
	if(LdrpDebugFlags & 9)
		LdrpLogDbgPrint("minkernel\\ntdll\\ldrapi.c", 0x1A9, "LdrpLoadDllInternal", 3, "Dll name: %wZ\n", ModuleFileName);
	*BaseAddress = NULL;
	LDR_DATA_TABLE_ENTRY *Module = NULL;
	NTSTATUS Status = LdrpFastpthReloadedDll(ModuleFileName, Flags, Entry, BaseAddress);
	if(NT_SUCCESS(Status)){
		*OutStatus = Status;
		if(LdrpDebugFlags & 9)
			LdrpLogDbgPrint("minkernel\\ntdll\\ldrapi.c", 0x28F, "LdrpLoadDllInternal", 4, "OutStatus: 0x%08lx\n", *OutStatus);
		return;
	}
	BOOLEAN LoadOwner;
	if(NtCurrentTeb()->LoadOwner){
		LoadOwner = TRUE;
	} else {
		LoadOwner = FALSE;
		LdrpDrainWorkQueue(0);
	}
	int var30;
	__try{
		var30 = 1;
		if(Entry != NULL && !LoadOwner && Entry->DdagNode->LoadCount == 0){
			*OutStatus = STATUS_DLL_NOT_FOUND;
		} else {
			LdrpDetectDetour();
			Status = LdrpFindOrPrepareLoadingModule(ModuleFileName, Arg2, Flags, Arg4, Arg5, &Module, OutStatus); //__fastcall function
			if(Status != STATUS_DLL_NOT_FOUND) {
				if(Status != STATUS_RETRY && !NT_SUCCESS(Status)) *OutStatus = Status;
			} else {
				LdrpProcessWork(Module->LoadContext, TRUE);
			}
		}
		LdrpDrainWorkQueue(1);
		if(Module == NULL){
			*OutStatus = STATUS_NO_MEMORY;
		} else {
			*BaseAddress = LdrpHandleReplacedModule(Module);
			if(Module != *BaseAddress){
				LdrpFreeReplacedModule(Module); //__fastcall function
				Module = (LDR_DATA_TABLE_ENTRY *) *BaseAddress;
			}
			if(Module->LoadContext != 0)
				LdrpCondenseGraph(Module->DdagNode); //__fastcall function
			if(NT_SUCCESS(*OutStatus)){
				*OutStatus = LdrpPrepareModuleForExecution(Module, OutStatus); //__fastcall function
				if(NT_SUCCESS(*OutStatus)){
					*OutStatus = LdrpBuildForwarderLink(Entry, Module);
					if(NT_SUCCESS(*OutStatus) && LdrInitState == 0){
						LdrpPinModule(Module);
					}
				}
			}
			LdrpFreeLoadContextOfNode(Module->DdagNode, OutStatus) //__fastcall function
			if(!NT_SUCCESS(*OutStatus)){
				*BaseAddress = NULL;
				LdrpDecrementModuleLoadCountEx(Module, 0);
				LdrpDereferenceModule(Module);
			}
		}
		var30 = 0;
	} __finally {
		if(var30){ //an exception was raised
			*OutStatus = STATUS_ACCESS_VIOLATION;
			LdrpLogError(STATUS_ACCESS_VIOLATION, 0x14AB, 0, ModuleFileName);
			LdrpDrainWorkQueue(1);
		}
		if(!LoadOwner) {
			LdrpDropLastInProgressCount();
		}
	}
	if(LdrpDebugFlags & 9){
		LdrpLogDbgPrint("minkernel\\ntdll\\ldrapi.c", 0x28F, "LdrpLoadDllInternal", 4, "OutStatus: 0x%08lx\n", *OutStatus);
	}
}
