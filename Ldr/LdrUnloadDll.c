NTSYSAPI NTSTATUS LdrUnloadDll(HMODULE ModuleHandle){
	if(PebLdr->ShutdownInProgress) return STATUS_SUCCESS;
	LDR_DATA_TABLE_ENTRY *DllDataTableEntry = NULL;
	if(ModuleHandle == NULL) return STATUS_DLL_NOT_FOUND;
	if(ModuleHandle == (HMODULE) LdrpSystemDllBase){
		DllDataTableEntry = LdrpNtDllDataTableEntry;
	} else {
		RtlAcquireSRWLockExclusive(&LdrpModuleDatatableLock);
		RTL_BALANCED_NODE *Min = LdrpModuleBaseAddressIndex.Min;
		RTL_BALANCED_NODE *CurrentNode = LdrpModuleBaseAddressIndex.Root;
		if(Min & 1 && CurrentNode != NULL)
			CurrentNode ^= &LdrpModuleBaseAddressIndex;
		BOOLEAN Xor = Min & 1;
		if(CurrentNode != NULL){
			while(1){
				LDR_DATA_TABLE_ENTRY *Entry = (LDR_DATA_TABLE_ENTRY *) (((ULONG_PTR) CurrentNode) - 0x68);
				if(ModuleHandle < Entry->DllBase){
					RTL_BALANCED_NODE *Left = CurrentNode->Left;
					if(Xor && Left != NULL)
						CurrentNode ^= Left;
					else 
						CurrentNode = Left;
					if(Currentnode == NULL) break;
				} else if(ModuleHandle > Entry->DllBase){
					RTL_BALANCED_NODE *Right = CurrentNode->Right;
					if(Xor && Right != NULL)
						CurrentNode ^= Right;
					else
						CurrentNode = Right;
					if(CurrentNode == NULL) break;
				} else if(CurrentNode != NULL){
					DllDataTableEntry = Entry;
					if(Entry->DdagNode->LoadCount != -1){
						if(*(UCHAR *) (((ULONG_PTR) Entry->DdagNode.Modules.Flink) - 0x20) & 0x20 == 0)
							Entry->ReferenceCount++;
					}
					break;
				} else break;
			}
		}
		RtlReleaseSRWLockExclusive(&LdrpModuleDatatableLock);
	}
	if(DllDataTableEntry == NULL) return STATUS_DLL_NOT_FOUND;
	if(DllDataTableEntry->DdagNode->LoadCount != 1){
		RtlAcquireSRWLockExclusive(&LdrpModuleDatatableLock);
		LDR_DDAG_NODE *Node = DllDataTableEntry->DdagNode;
		BOOLEAN varC;
		NTSTATUS Status = LdrpDecrementNodeLoadCountLockHeld(Node, 1, &varC); //__fastcall function
		RtlReleaseSRWLockExclusive(&LdrpModuleDatatableLock);
		if(varC){
			LdrpAcquireLoaderLock();
			LdrpUnloadNode(Node); //__fastcall function
			LdrpReleaseLoaderLock(Node, 8, 0, Node); //__fastcall function
		}
		if(Status != STATUS_WMI_NOT_SUPPORTED){
			LdrpDereferenceModule(DllDataTableEntry); //__fastcall function
			return Status;
		}
	}
	BOOLEAN LoadOwner = NtCurrentTeb()->LoadOwner;
	if(!LoadOwner)
		LdrpDrainWorkQueue(0); //__fastcall function
	LdrpDecrementModuleLoadCountEx(DllDataTableEntry, 0); //__fastcall function
	if(!LoadOwner)
		LdrpDropLastInProgressCount();
	LdrpDereferenceModule(DllDataTableEntry); //__fastcall function
	return STATUS_SUCCESS;
}
