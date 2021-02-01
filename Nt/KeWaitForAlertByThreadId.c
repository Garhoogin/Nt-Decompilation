NTSTATUS KeWaitForAlertByThreadId(KPROCESSOR_MODE WaitMode, PLARGE_INTEGER Timeout, PVOID *WaitOnAddressHashTable){
	PKTHREAD CurrentThread = Kpcr->Prcb.CurrentThread;
	ULONG_PTR var30 = 0, var20;
	ULONG var78 = 0;
	BOOLEAN WasAlerted = CurrentThread->AlertedByThreadId;
	CurrentThread->AlertedByThreadId = FALSE;
	if(WasAlerted) return STATUS_ALERTED;
	if(Timeout != NULL && Timeout->QuadPart == 0){
		return STATUS_TIMEOUT;
	}
	PKWAIT_BLOCK WaitBlock = &CurrentThread->WaitBlock[0];
	UCHAR var0 = KiCheckWaitNext(CurrentThread, Timeout, 0, &var30, &var78);
	while(1){
		NTSTATUS Status = KiBeginThreadWait(CurrentThread, WaitMode, 0x25, 0);
		if(Status != STATUS_SUCCESS) return Status;
		WasAlerted = CurrentThread->AlertedByThreadId;
		CurrentThread->AlertedByThreadId = FALSE;
		if(WasAlerted){
			CurrentThread->WaitReason = 0;
			KiFastExitThreadWait(Kpcr->CurrentPrcb, CurrentThread, var0);
			return STATUS_ALERTED;
		}
		Status = KiCheckDueTimeExpired(CurrentThread, var78, var30, var30);
		if(Status != STATUS_SUCCESS){
			CurrentThread->WaitReason = 0;
			KiFastExitThreadWait(Kpcr->CurrentPrcb, CurrentThread, var0);
			return STATUS_TIMEOUT;
		}
		var20 = 0;
		WaitBlock->Object = WaitOnAddressHashTable;
		WaitBlock->BlockState = 5;
		CurrentThread->WaitBlockCount = 1;
		Status = KiCommitThreadWait(CurrentThread, WaitBlock, var78);
		CurrentThread->WaitReason = 0;
		if(Status != 0x100) return Status;
		var0 = 0;
		KeRaiseIrql(DISPATCH_LEVEL, &CurrentThread->WaitIrql);
	}
}
