ULONG LdrpDllCharacteristicsToLoadFlags(ULONG Characteristics){
	ULONG Flags = (Characteristics & 4) << 1;
	if(Characteristics & 2)
		Flags |= 0x40;
	if(Characteristics & 0x800000)
		Flags |= 0x80;
	if(Characteristics & 0x1000)
		Flags |= 0x100;
	if(Characteristics & 0x80000000)
		Flags |= 0x400000;
	return Flags;
}
