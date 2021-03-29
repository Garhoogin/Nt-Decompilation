NTSTATUS NtCreateProcessEx(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ParentProcess, ULONG Flags, HANDLE SectionHandle, HANDLE DebugPort, HANDLE ExceptionPort, ULONG Arg9){
	if(ParentProcess == NULL){
		return STATUS_INVALID_PARAMETER;
	}
	KPROCESSOR_MODE PreviousMode = Kpcr->Prcb.CurrentThread->PreviousMode;
	if(PreviousMode != KernelMode){
		PHANDLE Address = 0x7FFFFFFF0000;
		if((ULONG_PTR) ProcessHandle < 0x7FFFFFFF0000) Address = (PHANDLE) ProcessHandle;
		*Address = *Address;
	}
	return PspCreateProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ParentProcess, PreviousMode, Flags, SectionHandle, DebugPort, ExceptionPort);
}
