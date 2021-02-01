NTSTATUS KeRaiseUserException(NSTATUS ExceptionCode){
	PKTHREAD CurrentThread = Kpcr->Prcb.CurrentThread;
	KIRQL Irql = __readcr8();
	if(Irql < APC_LEVEL){
		Irql = __readcr8();
		__writecr8(APC_LEVEL);
	}
	PKTRAP_FRAME TrapFrame = CurrentThread->TrapFrame;
	if(TrapFrame != NULL && TrapFrame->SegCs & 1){
		CurrentThread->Teb->ExceptionCode = ExceptionCode;
		TrapFrame->SegCs = 0x33;
		TrapFrame->Rip = KeRaiseUserExceptionDispatcher;
		if(TrapFrame->ExceptionArchive != 2) {
			KiSetupForInstrumentationReturn();
		}
	}
	if(Irql < APC_LEVEL) {
		if(KiIrqlFlags && KiIrqlFlags != 1 && __readcr8() != 0xF){
			PKPRCB CurrentPrcb = Kpcr->CurrentPrcb;
			PVOID SchedulerAssist = CurrentPrcb->SchedulerAssist;
			ULONG var8 = (*(ULONG *) ((ULONG_PTR) SchedulerAssist + 0x14)) & ~(ULONG) (((-1) << (Irql + 1)) & 0xFFFF);
			*(ULONG *) ((ULONG_PTR) SchedulerAssist + 0x14) = var8;
			if(!var8){
				KiRemoveSystemWorkPriorityKick(CurrentPrcb);
			}
		}
		__writecr8(Irql);
	}
	return ExceptionCode;
}
