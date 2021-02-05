ULONG BgpGetBitsPerPixel(){
	ULONG Format = ((ULONG *) BgInternal)[4];
	if(Format == 4) return 24;
	if(Format == 5) return 32;
	return 1;
}
