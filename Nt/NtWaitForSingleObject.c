NTSTATUS NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout){
	LARGE_INTEGER TimeoutCopy = { 0 };
	KPROCESSOR_MODE PreviousMode = Kpcr->Prcb.CurrentThread->PreviousMode;
	if(Timeout != NULL && PreviousMode == UserMode){
		if((ULONG_PTR) Timeout >= 0x7FFFFFFF0000){
			Timeout = (PLARGE_INTEGER) 0x7FFFFFFF0000;
		}
		TimeoutCopy.QuadPart = Timeout->QuadPart;
		Timeout = &TimeoutCopy;
	}
	return ObWaitForSingleObject(Handle, PreviousMode, PreviousMode, Alertable, Timeout);
}
