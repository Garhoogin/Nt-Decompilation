NTSTATUS NtWaitForAlertByThreadId(PVOID *WaitOnAddressHashTable, PLARGE_INTEGER Timeout){
	LARGE_INTEGER TimeoutCopy = { 0 };
	KPROCESSOR_MODE PreviousMode = Kpcr->Prcb.CurrentThread->PreviousMode;
	if(Timeout != NULL && PreviousMode != KernelMode) {
		Timeout = (ULONG_PTR) Timeout > 0x7FFFFFFF0000 ? (PLARGE_INTEGER) 0x7FFFFFFF0000 : Timeout;
		TimeoutCopy.QuadPart = Timeout->QuadPart;
		Timeout = &TimeoutCopy;
	}
	return KeWaitForAlertByThreadId(PreviousMode, Timeout, WaitOnAddressHashTable);
}
