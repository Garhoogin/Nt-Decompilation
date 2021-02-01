NTSTATUS MiReadWriteVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T NumberOfBytesToRead, SIZE_T *NumberOfBytesRead, ULONG Mode){
	PEPROCESS Process = NULL;
	PETHREAD CurrentThread = Kpcr->CurrentPrcb.CurrentThread;
	KPROCESSOR_MODE PreviousMode = CurrentThread->PreviousMode;
	if(PreviousMode != KernelMode) {
		ULONG_PTR MaxAddress = NumberOfBytesToRead + (ULONG_PTR) BaseAddress;
		if(MaxAddress < BaseAddress || MaxAddress > 0x7FFFFFFF0000) return STATUS_ACCESS_VIOLATION;
		ULONG_PTR MaxBuffer = (ULONG_PTR) Buffer + NumberOfBytesToRead;
		if(MaxBuffer < Buffer || MaxBuffer > 0x7FFFFFFF0000) return STATUS_ACCESS_VIOLATION;
		if(NumberOfBytesRead != NULL) {
			SIZE_T *BytesReadPointer = NumberOfBytesRead;
			if((ULONG_PTR) BytesReadPointer >= 0x7FFFFFFF0000){
				BytesReadPointer = (SIZE_T *) 0x7FFFFFFF0000;
			}
			*BytesReadPointer = *BytesReadPointer;
		}
	}
	SIZE_T BytesRead = 0;
	NTSTATUS Status = STATUS_SUCCESS;
	if(NumberOfBytesToRead == 0){
		if(NumberOfBytesRead != NULL) *NumberOfBytesRead = 0;
		return STATUS_SUCCESS;
	}
	Status = ObReferenceObjectByHandleWithTag(ProcessHandle, Mode, PsProcessType, PreviousMode, 0x6D566D4D, &Process, 0);
	if(!NT_SUCCESS(Status)){
		if(NumberOfBytesRead != NULL) *NumberOfBytesRead = 0;
		return Status;
	}
	PEPROCESS CurrentProcess = CurrentThread->ApcState.Process;
	if((!(Process->Pcb.SecureState & 1) || (CurrentProcess != Process && Process->DebugPort == 0)) && (Process->Pcb.SecureState & 1)){
		Status = STATUS_ACCESS_VIOLATION;
	} else {
		if(Mode != 0x10){
			Status = MmCopyVirtualMemory(Mode, Buffer, Process, BaseAddress, NumberOfBytesToRead, PreviousMode, &BytesRead);
		} else {
			Status = MmCopyVirtualMemory(Process, BaseAddress, Mode, Buffer, NumberOfBytesToRead, PreviousMode, &BytesRead);
		}
	}
	if(PsIsProcessLoggingEnabled(Process, Mode)){
		EtwTiLogReadWriteVm(Status, Mode, Process, Mode, BaseAddress, BytesRead);
	}
	ObfDereferenceObjectWithTag(Process, 0x6D566D4D);
	if(NumberOfBytesRead != NULL) *NumberOfBytesRead = BytesRead;
	return Status;
}
