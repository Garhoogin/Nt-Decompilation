NTSTATUS NtCreateProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ParentProcess, BOOLEAN InheritObjectTable, HANDLE SectionHandle, HANDLE DebugPort, HANDLE ExceptionPort){
	ULONG Flags = 0;
	Flags = ((ULONG) SectionHandle) & 1;
	if(!((ULONG) DebugPort & 1)) Flags |= 2;
	if(InheritObjectTable) Flags |= 4;
	return NtCreateProcessEx(ProcessHandle, DesiredAccess, ObjectAttributes, ParentProcess, Flags, SectionHandle, DebugPort, ExceptionPort, 0);
}
