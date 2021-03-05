//had to make up intrinsics to try to describe what this does

VOID KiSystemStartup(PLOADER_PARAMETER_BLOCK LoaderBlock){
	KeLoaderBlock = LoaderBlock;
	if(KeLoaderBlock->Prcb->Number == 0){
		KdInitSystem(-1, KeLoaderBlock);
	}
	KPRCB *Prcb = KeLoaderBlock->Prcb;
	KPCR *Pcr = ((KPCR *) Prcb) - 1;
	Pcr->Self = Pcr;
	Pcr->CurrentPrcb = Prcb;
	Pcr->Prcb.ProcessorState.SpecialRegisters.Cr0 = __readcr0();
	Pcr->Prcb.ProcessorState.SpecialRegisters.Cr2 = __readcr2();
	Pcr->Prcb.ProcessorState.SpecialRegisters.Cr3 = __readcr3();
	Pcr->Prcb.ProcessorState.SpecialRegisters.Cr4 = __readcr4();
	sgdt tbyte Kpcr->Prcb.ProcessorState.SpecialRegisters.Gdtr.Limit
	KGDTENTRY64 *GdtEntry = Kpcr->Prcb.ProcessorState.SpecialRegisters.Gdtr.Base;
	Pcr->GdtBase = GdtEntry;
	__sidt(&Pcr->Prcb.ProcessorState.SpecialRegisters.Idtr.Limit);
	Pcr->IdtBase = Pcr->Prcb.ProcessorState.SpecialRegisters.Idtr.Base;
	__str(&Pcr->Prcb.ProcessorState.SpecialRegisters.Tr);
	__sldt(&Pcr->Prcb.ProcessorState.SpecialRegisters.Ldtr);
	Pcr->Prcb.MxCsr = 0x1F80;
	ldmxcsr Pcr->Prcb.MxCsr
	if(Prcb->Number == 0){
		GdtEntry[5].LimitLow = 0x3C00;
	}
	__writeds(0x2B); //not actual intrinsics
	__writees(0x2B);
	__writefs(0x53);
	if(!VslVsmEnabled){
		__loadldt(0); //made up this intrinsic
	}
	Pcr->TssBase = GdtEntry[4].BaseLow | (GdtEntry[4].Bytes[0] << 16) | (GdtEntry[4].Bytes[3] << 24) | (GdtEntry[4].BaseUpper << 32);
	__writemsr(0xC0000101, (ULONG_PTR) Pcr);
	__writemsr(0xC0000102, (ULONG_PTR) Pcr);
	if(Prcb->Number == 0){
		_guard_dispatch_icall_fptr = &guard_dispatch_icall;
		_guard_check_icall_fptr = &guard_check_icall;
	}
	KiInitializeBootStructures(KeLoaderBlock);
	if(KeLoaderBlock->Prcb->Number == 0){
		KdInitSystem(0, KeLoaderBlock);
	}
	KiInitializeXSave(KeLoaderBlock, KeLoaderBlock->Prcb->Number);
	__writecr8(HIGH_IRQL);
	PVOID __declspec(align(64)) XSaveArea = alloca(KiXSaveAreaLength);
	alloca(0x1B0);
	PKPROCESS Process = KeLoaderBlock->Process;
	PKTHREAD Thread = KeLoaderBlock->Thread;
	PVOID RspBase;
	if(KiKvaShadow & 1){
		RspBase = Kpcr->IdtBase[263].OffsetHigh;
		Kpcr->Prcb.RspBaseShadow = RspBase;
	} else {
		RspBase = Kpcr->TssBase->Rsp0;
	}
	Kpcr->Prcb.RspBase = RspBase;
	KeInitializeKernel(Process, Thread, RspBase, KeLoaderBlock->Prcb, KeLoaderBlock->Prcb->Number, KeLoaderBlock);
	if(Kpcr->Prcb.Number == 0){
		ULONG_PTR Clock __rdtsc();
		ULONG_PTR Random = ((Clock ^ _rotr64(Clock, 49)) ^ ExpSecurityCookieRandomData) & 0xFFFFFFFFFFFF;
		_security_cookie = Random;
		_security_cookie_complement = ~Random;
	}
	__setrsp(Kpcr->Prcb.RspBase);
	PVOID __declspec(align(64)) XSaveArea2 = alloca(KiXSaveAreaLength);
	alloca(0x30);
	PKTHREAD CurrentThread = Kpcr->Prcb.CurrentThread;
	CurrentThread->WaitIrql = DISPATCH_LEVEL;
	do{
		YieldProcesor();
	while (KiBarrierWait);
	KiIdleLoop();
}
