NTSTATUS NtClose(HANDLE Handle){
	KPROCESSOR_MODE PreviousMode = Kpcr->Prcb.CurrentThread->PreviousMode;
	if(MmVerifierData & 0x100 && PreviousMode == KernelMode && !ObpIsKernelHandle(Handle, KernelMode)){
		VfCheckUserHandle(Handle);
	}
	PKTHREAD CurrentThread = Kpcr->Prcb.CurrentThread;
	HANDLE_TABLE *HandleTable;
	PKPROCESS HandleProcess;
	BOOLEAN InApc = FALSE;
	PEPROCESS CurrentProcess = (PEPROCESS) CurrentThread->ApcState.Process;
	if(ObpIsKernelHandle(Handle, PreviousMode)){
		HandleTable = ObpKernelHandleTable;
		Handle ^= 0xFFFFFFFF80000000;
		HandleProcess = PsInitialSystemProcess;
	} else {
		HandleProcess = CurrentProcess;
		if(Kpcr->Prcb.CurrentThread->ApcStateIndex == 1){
			HandleTable = ObReferenceProcessHandleTable(CurrentProcess);
			if(HandleTable == NULL) {
				return STATUS_INVALID_HANDLE;
			}
			InApc = TRUE;
		} else {
			HandleTable = CurrentProcess->ObjectTable;
			if(HandleTable == ObpKernelHandleTable) {
				return STATUS_INVALID_HANDLE;
			}
		}
	}
	CurrentThread->KernelApcDisable--;
	if((ULONG_PTR) Handle & 0x3FC){
		PLIST_ENTRY Entry = ExpLookupHandleTableEntry(HandleTable, Handle);
		if(Entry != NULL && ExLockHandleTableEntry(HandleTable, Entry)){
			NTSTATUS Status = ObCloseHandleTableEntry(HandleTable, Entry, HandleProcess, Handle, PreviousMode, FALSE);
			if(InApc){
				ExReleaseRundownProtection(&HandleProcess->RundownProtect);
			}
			return Status;
		}
	}
	KeLeaveCriticalRegionThread(CurrentThread);
	if(Handle + 6 > 6) {
		BOOLEAN var70 = FALSE;
		ULONG_PTR var78;
		ExQueryHandleExceptionsPermanency(HandleTable, &var78, &var70);
		if(HandleTable->RaiseUMExceptionOnInvalidHandleClose && var70){
			ExHandleLogBadReference(HandleTable, Handle, PreviousMode);
		}
		if(PreviousMode == KernelMode){
			if(!CurrentThread->Terminated && CurrentProcess->Peb != NULL && KdDebuggerEnabled){
				KeBugCheckEx(INVALID_KERNEL_HANDLE, 0, 1, 0, 0);
			}
		} else if((NtGlobalFlag & 0x400000) || CurrentProcess->DebugPort != NULL || HandleTable->DebugInfo != NULL){
			NTSTATUS Status;
			if(Kpcr->Prcb.CurrentThread->ApcStateIndex != 1){
				Status = KeRaiseUserException(STATUS_INVALID_HANDLE);
			} else {
				Status = STATUS_INVALID_HANDLE;
			}
			if(InApc){
				ExReleaseRundownProtection(&HandleProcess->RundownProtect);
			}
			return Status;
		}
	}
	if(InApc){
		ExReleaseRundownProtection(&HandleProcess->RundownProtect);
	}
	if((ULONG_PTR) Handle >= 0xFFFFFFFFFFFFFFFA) return STATUS_SUCCESS;
	return STATUS_INVALID_HANDLE;
}
