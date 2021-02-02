NTSTATUS NtRaiseHardError(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PVOID *Parameters, HARDERROR_RESPONSE_OPTION ResponseOption, PHARDERROR_RESPONSE Response){
	HARDERROR_RESPONSE_OPTION LocalResponse = 0;
	if(NumberOfParameters > 5 || (Parameters != NULL && NumberOfParameters == 0)) return 0xC00000F0;
	PKTHREAD CurrentThread = Kpcr->Prcb.CurrentThread;
	if(CurrentThread->PreviousMode == KernelMode){
		NTSTATUS Status = ExRaiseHardError(ErrorStatus, NumberOfParameters, UnicodeStringParameterMask, Parameters, ResponseOption, &LocalResponse);
		*Response = LocalResponse;
		return Status;
	}
	if(ResponseOption > 8) return 0xC00000F2;
	PHARDERROR_RESPONSE ResponsePointer = Response;
	if(ResponsePointer >= (PHARDERROR_RESPONSE) 0x7FFFFFFF0000) ResponsePointer = (PHARDERROR_RESPONSE) 0x7FFFFFFF0000;
	*ResponsePointer = *ResponsePointer;
	UNICODE_STRING Strings[5];
	PVOID LocalParameters[5], ProcessedParameters[5];
	if(Parameters != NULL){
		ULONG_PTR SizeOfParameters = NumberOfParameters * sizeof(PVOID);
		if(SizeOfParameters != 0){
			if(((ULONG_PTR) Parameters) & 7){
				ExRaiseDatatypeMisalignment();
			}
			PVOID *MaximumParameter = Parameters + NumberOfParameters;
			if(MaximumParameter > (PVOID *) 0x7FFFFFFF0000 || MaximumParameter < Parameters){
				MaximumParameter = NULL;
				*(UCHAR *) 0x7FFFFFFF0000 = 0;
			}
		}
		memcpy(&LocalParameters, Parameters, SizeOfParameters);
		memcpy(&ProcessedParameters, &LocalParameters, SizeOfParameters);
		//UnicodeStringParameter mask looks to mark parameters as being a PUNICODE_STRING
		if(UnicodeStringParameterMask){
			ULONG i;
			for(i = 0; i < NumberOfParameters; i++){
				if(UnicodeStringParameterMask & (1 << i)){
					PVOID Parameter = LocalParameters[i];
					if((ULONG_PTR) Parameter & 7){
						ExRaiseDatatypeMisalignment();
					}
					//Probably to poke each string
					*(UCHAR *) ((Parameter >= (PVOID) 0x7FFFFFFF0000) ? (PVOID) 0x7FFFFFFF0000 : Parameter);
					PUNICODE_STRING String = Strings + i;
					memcpy((PVOID) String, Parameter, sizeof(UNICODE_STRING));
					ULONG MaximumLength = (ULONG) String->MaximumLength;
					if(MaximumLength){
						PWSTR Buffer = String->Buffer;
						PWSTR StrEnd = (PWSTR) (((PUCHAR) String->Buffer) + MaximumLength);
						if(StrEnd > (PWSTR) 0x7FFFFFFF0000 || StrEnd < Buffer){
							*(UCHAR *) 0x7FFFFFFF0000 = 0;
						}
					}
					ProcessedParameters[i] = String;
				}
			}
		}
	}
	NTSTATUS Status = ExpRaiseHardError(ErrorStatus, NumberOfParameters, UnicodeStringParameterMask, &LocalParameters, &ProcessedParameters, ResponseOption, &LocalResponse);
	*Response = LocalResponse;
	return Status;
}
