KPRIORITY KiComputeNewPriority(PKTHREAD Thread, BOOLEAN Arg2){
	KPRIORITY Priority = Thread->Priority;
	if(priority >= 0x10) return Priority;
	if(KiForegrounBoostVelocityFlag){ //yes that's how it's spelled
		if(((PEPROCESS) Thread->Process)->Vm.Instance.Flags.MemoryPriority == 2){
			if(!Thread->DisableBoost){
				UCHAR UnusualBoost = Thread->UnusualBoost;
				UCHAR ForegroundBoost = Thread->ForegroundBoost;
				Thread->UnusualBoost = 0;
				KPRIORITY AdjustedPriority = Priority - (UnusualBoost + Arg2);
				KPRIORITY NewPriority = Thread->BasePriority + ForegroundBoost;
				if(AdjustedPriority >= NewPriority) NewPriority = AdjustedPriority;
				ULONG PriorityFloorSummary = Thread->PriorityFloorSummary;
				if(PriorityFloorSummary){
					KPRIORITY HighestPriorityFloor;
					_BitScanReverse(&HighestPriorityFloor, PriorityFloorSummary);
					NewPriority &= 0xFF;
					if(NewPriority < HighestPriorityFloor) return HighestPriorityFloor;
				}
				return NewPriority;
			}
		}
	}
	KPRIORITY NewPriority = Thread->BasePriority;
	ULONG ForegroundBoost = Thread->ForegroundBoost;
	KPRIORITY AdjustedPriority = Priority - (Thread->UnusualBoost + ForegroundBoost + Arg2);
	if(AdjustedPriority >= NewPriority) NewPriority = AdjustedPriority;
	if(Thread->ForegroundBoost || Thread->UnusualBoost){
		if(ForegroundBoost){
			Thread->ForegroundLossTime = *(ULONG *) 0xFFFFF78000000320;
		}
		Thread->ForegroundBoost = 0;
		Thread->UnusualBoost = 0;
	}
	ULONG PriorityFloorSummary = Thread->PriorityFloorSummary;
	if(PriorityFloorSummary){
		KPRIORITY HighestPriorityFloor;
		_BitScanReverse(&HighestPriorityFloor, PriorityFloorSummary);
		NewPriority &= 0xFF;
		if(NewPriority < HighestPriorityFloor) return HighestPriorityFloor;
	}
	return NewPriority;
}
