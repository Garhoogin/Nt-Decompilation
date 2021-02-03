void KeBugCheckEx(ULONG BugCheckCode, ULONG_PTR BugCheckParameter1, ULONG_PTR BugCheckParameter2, ULONG_PTR BugCheckParameter3, ULONG_PTR BugCheckParameter4){
	ULONG_PTR Flags = __readflags();
	_disable();
	RtlCaptureContext(Kpcr->CurrentPrcb->Context);
	KiSaveProcessorControlState(&Kpcr->CurrentPrcb.ProcessorState);
	CONTEXT *Context = Kpcr->CurrentPrcb->Context;
	Context->Rcx = BugCheckCode;
	Context->EFlags = Flags;
	if((ULONG_PTR) KiBugCheckReturn + 5 == (ULONG_PTR) _ReturnAddress()){
		Context->Rsp = (ULONG_PTR) _AddressOfReturnAddress() + 0x30;
		Context->Rip = (ULONG_PTR) KeBugCheck;
	} else {
		Context->Rsp = (ULONG_PTR) _AddressOfReturnAddress();
		Context->Rip = (ULONG_PTR) KeBugCheckEx;
	}
	KIRQL Irql = __readcr8();
	Kpcr->Prcb.DebuggerSavedIRQL = Irql;
	if(Irql < DISPATCH_LEVEL){
		__writecr8(DISPATCH_LEVEL);
	}
	if(Flags & 0x200){
		_enable();
	}
	KiHardwareTrigger++;
	if((ULONG_PTR) KiBugCheckReturn + 5 != (ULONG_PTR) _ReturnAddress()){
		KeBugCheck2(BugCheckCode, BugCheckParameter1, BugCheckParameter2, BugCheckParameter3, BugCheckParameter4, 0);
	}
	KeBugCheck2(BugCheckCode, 0, 0, 0, 0, 0);
}
