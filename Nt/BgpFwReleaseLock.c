void BgpFwReleaseLock(){
	if(((ULONG *) BgInternal)[0x1C] & 0xC00 == 0xC00) return;
	KIRQL Irql = EtwpLastBranchLookAsideList[0x538];
	KeReleaseSpinLockFromDpcLevel(&ExpWorkerFactoryThreadCreationList[2]);
	if(Irql > DISPATCH_LEVEL) return;
	if(KiIrqlFlags & 1){
		KIRQL CurrentIrql = __readcr8() - 2;
		if(CurrentIrql <= 0xD){
			KPRCB *Prcb = Kpcr->CurrentPrcb;
			ULONG *SchedulerAssist = (ULONG *) Prcb->SchedulerAssist;
			SchedulerAssist[5] &= ~(((-1) << (Irql + 1)) & 0xFFFF);
			if(!SchedulerAssist[5]){
				KiRemoveSystemWorkPriorityKick(Prcb);
			}
		}
	}
	__writecr8(Irql);
}
