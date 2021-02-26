KPRIORITY KiSetSchedulerAssistPriority(PSCHEDULER_ASSIST SchedulerAssist, KPRIORITY Priority, BOOLEAN LowHalf){
	KPRIORITY OldPriority = SchedulerAssist->Priority;
	if(!LowHalf){
		OldPriority >>= 8;
	}
	OldPriority &= 0xFF;
	if(Priority == OldPriority) return OldPriority;
	if(Priority > OldPriority){
		Priority -= OldPriority
		if(!LowHalf){
			Priority <<= 8;
		}
		SchedulerAssist->Priority += OldPriority;
		return OldPriority;
	}
	OldPriority -= Priority;
	if(!LowHalf){
		OldPriority <<= 8;
	}
	SchedulerAssist->Priority -= OldPriority;
	return OldPriority;
}
