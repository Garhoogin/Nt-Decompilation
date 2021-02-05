void BgpFwAcquireLock(){
	if(BgInternal[0x1C] & 0xC00 == 0xC00) return
	KIRQL Irql = __readcr8();
	if(Irql <= DISPATCH_LEVEL){
		KSPIN_LOCK SpinLock = ExpWorkerFactoryThreadCreationList[2];
		if(SpinLock){
			YieldProcessor();
		} else {
			Irql = __readcr8()
			__writecr8(DISPATCH_LEVEL);
		}
		if(SpinLock || KiIrqlFlags){
			if(SpinLock & 1 && Irql <= 15){
				ULONG *Assist = (ULONG *) Kpcr->CurrentPrcb->SchedulerAssist;
				Assist[5] |= ~((1 << (Irql + 1)) - 1) & 0x4;
			}
		}
	}
	KeAcquireSpinLockAtDpcLevel(&ExpWorkerFactoryThreadCreationList[2]);
	EtwpLastBranchLookAsideList[0x538] = Irql;
}
