//Windows 10 2004 Build 19041.572
VOID LdrpAcquireLoaderLock(){
	PUCHAR Flag;
	if(RtlGetCurrentServiceSessionId() != 0)
		Flag = NtCurrentTeb()->ProcessEnvironmentBlock->SharedData + 0x22A;
	else
		Flag = SharedUserData + 0x384;
	PUCHAR Flag2 = SharedUserData + 0x385;
	if(byte [eax] != 0){
		if(NtCurrentTeb()->ProcessEnvironmentBlock->LibLoaderTracingEnabled){
			if(RtlGetCurrentServiceSessionId() != 0)
				Flag = NtCurrentTeb()->ProcessEnvironmentBlock->SharedData + 0x22B;
			else
				Flag = Flag2;
			if(*Flag & 0x20)
				LdrpLogEtwEvent(0x1480, -1, -1, -1, 0, 0);

		}
	}
	RtlEnterCriticalSection(&LdrpLoaderLock);
	if(RtlGetCurrentServiceSessionId() == 0)
		if(SharedUserData[0x384] == 0) return;
	else
		if(NtCurrentTeb()->ProcessEnvironmentBlock->SharedData[0x22A] == 0) return;
	if(NtCurrentTeb()->ProcessEnvironmentBlock->LibLoaderTracingEnabled){
		if(RtlGetCurrentServiceSessionId() != 0)
			Flag2 = NtCurrentTeb()->ProcessEnvironmentBlock->SharedData + 0x22B;
		if(*Flag2 & 0x20)
			LdrpLogEtwEvent(0x1481, -1, -1, -1, 0, 0);
	}
}
