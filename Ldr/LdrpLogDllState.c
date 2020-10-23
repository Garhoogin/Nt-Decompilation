//Windows 10 2004 Build 19041.572
VOID __fastcall LdrpLogDllState(LONG arg1, PUNICODE_STRING ModuleName, LONG State){
	PULONG SharedData = NtCurrentTeb()->ProcessEnvironmentBlock->SharedData;
	PUCHAR Var1;
	if(SharedData != NULL && *SharedData != 0)
		Var1 = NtCurrentTeb()->ProcessEnvironmentBlock->SharedData + 0x22A;
	else
		Var1 = SharedUserData + 0x384;
	if(*Var1 != 0){
		if(Var1[0x240] & 4){
			if(RtlGetCurrentServiceSessionId() == 0)
				Var1 = SharedUserData + 0x385;
			else
				Var1 = NtCurrentTeb()->ProcessEnvironmentBlock->SharedData + 0x22B;
			if(*Var1 & 0x20)
				LdrpLogEtwEvent(State, arg1, 0, 0, ModuleName, 0);
		}
	}
}
