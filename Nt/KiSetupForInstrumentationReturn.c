VOID KiSetupForInstrumentationReturn(PKTRAP_FRAME TrapFrame){
	PVOID InstrumentationCallback = Kpcr->Prcb.CurrentThread->ApcState.Process->InstrumentationCallback;
	if(InstrumentationCallback != NULL && TrapFrame->SegCs == 0x33){
		TrapFrame->R10 = TrapFrame->Rip;
		TrapFrame->Rip = InstrumentationCallback;
	}
}
