void HalReturnToFirmware(FIRMWARE_REENTRY Routine){
	if(Routine){
		if(Routine == 1){
			KdPowerTransitionEx(4, 0);
			HalpShutdown();
		}
		if(Routine <= 1 || Routine > 3){
			DbgPrint("HalReturnToFirmware called\n");
			__debugbreak();
			return;
		}
	}
	KdPowerTransitionEx(4, 0);
	if(HalpRebootHandler){
		(void (*) ()) (HalPrivateDispatchTable[0x78])();
	}
	_disable();
	if(HalpHvCpuManager == 0 || HalpEnlightenment != 0){
		HalpTimerStopAllTimers();
	}
	HalpAcquireCmosSpinLock();
	if(HalpResetParkDisposition != 0){
		HalpInterruptResetAllProcessors();
	} else {
		HalpPowerWriteResetCommand(0, 0);
	}
	__debugbreak();
}
