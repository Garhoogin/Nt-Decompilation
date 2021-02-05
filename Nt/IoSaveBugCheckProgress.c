void IoSaveBugCheckProgress(ULONG Progress){
	if(CrashdmpDumpBlock == NULL) return;
	ULONG var0 = ((ULONG *) CrashdmpDumpBlock)[0x15F];
	if(!(var0 & 0x60000)){
		((ULONG *) CrashdmpDumpBlock)[0x15F] ^= ((var0 ^ Progress) & 0x1FF)
		IoUpdateBugCheckProgressEnvVariable();
	}
	if(Progress == 4){
		struct{
			ULONG Tag;
			ULONG u2;
			ULONG Size;
			ULONG u4;
			ULONG u5; //+10
			NTSTATUS Status;
			ULONG u7;
			ULONG u8;
			ULONG u9; //+20
			ULONG u10;
		} var20;
		var20.Tag = 'WhLg';
		var20.u2 = 1;
		var20.Size = sizeof(var20);
		var20.u4 = 0;
		var20.u5 = ((*(ULONG *) CrashdmpDumpBlock)[0x14E] == 4) ? 0x2301 : 0x2300;
		var20.Status = STATUS_REDIRECTOR_HAS_OPEN_HANDLES;
		var29.u7 = 0x18;
		var29.u8 = 8;
		var20.u10 = [CrashdmpDumpBlock + 0x57C]
		var20.u9 = ((ULONG *) CrashdmpDumpBlock)[6];
		WheaLogInternalEvent(&var20);
	}
}
