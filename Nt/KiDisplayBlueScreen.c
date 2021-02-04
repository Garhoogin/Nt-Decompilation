void KiDisplayBlueScreen(ULONG BugCheckCode){
	PANSI_STRING var60[2] = {0};
	ANSI_STRING ParamStrings[4] = {0};
	PSTR BugMessageEnd = NULL;
	ULONG var44 = (ULONG) KiBugCheckData[0]; //ULONG_PTR[] I think
	BOOLEAN var40 = TRUE;
	ANSI_STRING var50 = {0};
	UNICODE_STRING BugCheckString;
	UNICODE_STRING ParamStrings[4];
	WCHAR var170[0x100];
	CHAR HexBuffer[11];
	CHAR varD0[36];
	HeadlessDispatch(0xE, 0, 0, 0, 0);
	HeadlessDispatch(1, &var40, 1, 0, 0);
	HeadlessDispatch(0x14, &var44, 4, 0, 0);
	IoSaveBugCheckProgress(0x81);
	PUNICODE_STRING CauseDriver = KiBugCheckDriver;
	ULONG_PTR *Params = (ULONG_PTR) &KiBugCheckData[1];
	if(!CauseDriver){
		KiDumpParameterImages(0, Params, 4, 0);
		CauseDriver = KiBugCheckDriver;
	}
	if(CauseDriver && CauseDriver->Length == 0x1E){
		if(!memcmp(CauseDriver->Buffer, L"VerifierExt.sys", 0x1E)){
			CauseDriver = NULL;
		}
	}
	BugCheckString.Buffer = var170;
	BugCheckString.MaximumLength = 0x100;
	for(int i = 0; i < 4; i++) {
		ParamStrings[i].Buffer = varD0 + i * 9;
		ParamStrings[i].Length = 0x24;
		ParamStrings[i].MaximumLength = 0x26;
	}
	USHORT BugMessageLength;
	PSTR BugMessage;
	if(!KeGetBugMessageText(KiBugCheckData, &var50)){
		RtlStringCbPrintfExA(HexBuffer, 11, &BugMessageEnd, NULL, 0, "0x%08x", KiBugCheckData[0]);
		BugMessageLength = (USHORT) BugMessageEnd - (USHORT) HexBuffer;
		BugMessage = &HexBuffer;
	} else {
		BugMessage = var50.Buffer;
		BugMessageLength = var50.Length;
	}
	ULONG Length = 0;
	USHORT MaximumLength = (BugCheckString.MaximumLength >> 1) - 1;
	if(MaximumLength <= BugMessageLength) BugMessageLength = MaximumLength;
	if(BugMessageLength > 0){
		PWSTR CurrentPosition = BugCheckString.Buffer;
		Length = BugMessageLength;
		for(USHORT i = 0; i < BugMessageLength; i++) {
			*(CurrentPosition++) = (WCHAR) *(BugMessage++);
		}
	}
	BugCheckString.Length = Length * 2;
	BugCheckString.Buffer[(USHORT) Length] = L'\0';
	InbvAcquireDisplayOwnership();
	IoSaveBugCheckProgress(0x82);
	BgpFwAcquireLock();
	if(*(UCHAR *) ((ULONG_PTR) &BgInternal + 0x70)){
		BgpFwDisplayBugCheckScreen(KiBugCheckData[0], Params, CauseDriver, &var60, BugCheckCode);
	}
	BgpFwReleaseLock();
	IoSaveBugCheckProgress(0x80);
	if(var60[0] != NULL && var60[1] != NULL){
		KiHeadlessDisplayString(L"\r\n", 6);
		KiHeadlessDisplayString(var60[0]->Buffer, var60[0]->Length);
		KiHeadlessDisplayString(L"\r\n", 6);
		KiHeadlessDisplayString(var60[1]->Buffer, var60[1]->Length);
	} else {
		for(int i = 0; i < 4; i++) {
			KiBugCheckConvertParameterValueToUnicodeString(Params[i], ParamStrings + i);
		}
	}
	KiHeadlessDisplayString(L"\r\n", 6);
	KiHeadlessDisplayString(BugCheckString.Buffer, BugCheckString.Length);
	if(KiBugCheckDriver != NULL){
		KiHeadlessDisplayString(L"\r\n", 6);
		KiHeadlessDisplayString(KiBugCheckDriver->Buffer, KiBugCheckDriver->Length);
	}
	KiHeadlessDisplayString(L"\r\n", 6);
	KiHeadlessDisplayString(L"\r\n", 6);
	for(int i = 0; i < 4; i++){
		KiHeadlessDisplayString(ParamStrings[i].Buffer, ParamStrings[i].Length);
		KiHeadlessDisplayString(L"\r\n", 6);
	}
	KiHeadlessDisplayString(L"\r\n", 6);
}
