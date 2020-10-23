//Windows 10 2004 Build 19041.572
VOID __fastcall LdrpInitializeDllPath(PWCHAR Buffer, PWCHAR Path, PVOID *Block){
	memset(Block, 0, 0x50);
	if(Path & 1 == 0 && Path){
		Block[0] = Path;
		if(LdrpDebugFlags & 5){
			LdrpLogDbgPrint("minkernel\\ntdll\\ldrutil.c", 0x5A5, "LdrpInitializeDllPath", 2, "DLL search path passed in exterally: %ws", Path);
		}
		LdrpLogDllStateEx2(Buffer, Buffer, Block[0], 0x14C0); //__fastcall function
	} else {
		Block[4] = Buffer;
		Block[3] = Path & ~1;
	}
}
