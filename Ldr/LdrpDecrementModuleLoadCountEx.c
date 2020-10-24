//Windows 10 2004 Build 19041.572
NTSTATUS LdrpDecrementModuleLoadCountEx(LDR_DATA_TABLE_ENTRY *Entry, BOOLEAN Arg2){
	if(Arg2 && Entry->DdagNode->LoadCount == 1)
		return STATUS_RETRY;
	RtlAcquireSRWLockExclusive(&LdrpModuleDatatableLock);
	NTSTATUS Status = LdrpDecrementNodeLoadCountLockHeld(Entry->DdagNode, Arg2, &var4); //__fastcall function
	RtlReleaseSRWLockExclusive(&LdrpModuleDatatableLock);
	if(var4 != 0){
		LdrpAcquireLoaderLock();
		LdrpUnloadNode(Entry->DdagNode); //__fastcall function
		LdrpReleaseLoaderLock(Entry->DdagNode, 8, 0, Entry->DdagNode);
	}
	return Status;
}
