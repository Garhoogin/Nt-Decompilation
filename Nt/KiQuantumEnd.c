KPCRB KiProcessorBlock[];

typedef struct _SCHEDULER_ASSIST{
	KPRIORITY Priority;
	ULONG Unknown2;
	ULONG Unknown3;
	ULONG Unknown4;
	BOOLEAN Idle;
	ULONG Unknown5;
	ULONG Unknown6;
} SCHEDULER_ASSIST, *PSCHEDULER_ASSIST;

//Windows 10 Kernel Version 19041 MP
VOID KiQuantumEnd(){
	KPRCB *CurrentPrcb = Kpcr->CurrentPrcb;
	ULONG_PTR var79 = 0;
	BOOLEAN varp67 = FALSE;
	PKTHREAD CurrentThread = CurrentPrcb->CurrentThread;
	if(CurrentThread != CurrentPrcb->IdleThread){
		ULONGLONG CycleTime = CurrentThread->CycleTime;
		if(CycleTime >= CurrentThread->QuantumTarget || CurrentThread->SystemHeteroCpuPolicy){
			ULONG var61 = 0;
			PSCHEDULER_ASSIST SchedulerAssist = CurrentPrcb->SchedulerAssist;
			if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
				SchedulerAssist->Unknown6++;
				if(!SchedulerAssist->Unknown6) {
					KiRemoveSystemWorkPriorityKick(CurrentPrcb);
				}
			}
			while(CurrentThread->ThreadLock & 1){
				SchedulerAssist = CurrentPrcb->SchedulerAssist;
				if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
					SchedulerAssist->Unknown6--;
					if(!SchedulerAssist->Unknown6){
						KiRemoveSystemWorkPriorityKick(CurrentPrcb);
					}
				}
				do{
					KeYieldProcessorEx(&var61);
				} while(CurrentThread->ThreadLock);
				SchedulerAssist = CurrentPrcb->SchedulerAssist;
				if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
					SchedulerAssist->Unknown6++;
					if(!SchedulerAssist->Unknown6){
						KiRemoveSystemWorkPriorityKick(CurrentPrcb);
					}
				}
			}
			CurrentThread->ThreadLock |= 1;
			if(CycleTime >= CurrentThread->QuantumTarget){
				KPRIORITY NewPriority;
				if(CurrentThread->DisableQuantum && CurrentThread->Priority >= 0x10){
					NewPriority = 0x7F;
				} else {
					NewPriority = KiComputeNewPriority(CurrentThread, TRUE);
					ULONG var5D = 0;
					SchedulerAssist = CurrentPrcb->SchedulerAssist;
					if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
						SchedulerAssist->Unknown6++;
						if(!SchedulerAssist->Unknown6) {
							KiRemoveSystemWorkPriorityKick(CurrentPrcb);
						}
					}
					while(CurrentPrcb->PrcbLock & 1){
						SchedulerAssist = CurrentPrcb->SchedulerAssist;
						if(SchedulerAssist && CurrentPrcb->NestingLevel <= 1){
							SchedulerAssist->Unknown6--;
							if(!SchedulerAssist->Unknown6){
								KiRemoveSystemWorkPriorityKick(CurrentPrcb);
							}
						}
						do{
							KeYieldProcessorEx(&var5D);
						} while(CurrentPrcb->PrcbLock);
						SchedulerAssist = CurrentPrcb->SchedulerAssist;
						if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
							SchedulerAssist->Unknown6++;
							if(!SchedulerAssist->Unknown6){
								KiRemoveSystemWorkPriorityKick(CurrentPrcb);
							}
						}
					}
					CurrentPrcb->PrcbLock |= 1;
					if(!KiUpdateVPBackingThreadPriority(CurrentThread, (CurrentPrcb->NextThread == NULL) ? CurrentPrcb : NULL, 0)){
						KiUpdateThreadPriority(CurrentPrcb, CurrentThread, NewPriority, CurrentPrcb->NextThread == NULL);
					}
					CurrentPrcb->PrcbLock = 0;
					SchedulerAssist = CurrentPrcb->SchedulerAssist;
					if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
						SchedulerAssist->Unknown6--;
						if(!SchedulerAssist->Unknown6){
							KiRemoveSystemWorkPriorityKick(CurrentPrcb);
						}
					}
					varp67 = TRUE;
					KiTryScheduleNextForegroundBoost(CurrentThread);
				}
				if(CurrentThread->QuantumDonation){
					CurrentThread->QuantumDonation = FALSE;
				}
				CurrentThread->QuantumTarget = KiCyclesPerClockQuantum * CurrentThread->QuantumReset + CycleTime;
				if(CurrentPrcb->GroupSetMember != CurrentPrcb->CoreProcessorSet){
					if(!CurrentThread->QuantumEndMigrate){
						ULONG_PTR CoreProcessorSet = CurrentPrcb->CoreProcessorSet;
						if((CurrentPrcb->ParentNode->IdleCpuSet | CurrentPrcb->GroupSetMember) & CoreProcessorSet != CoreProcessorSet){
							KNODE *ParentNode = KiProcessorBlock[CurrentThread->IdealProcessor]->ParentNode;
							if(CurrentThread->Affinity.Mask & (ParentNode->NonParkedSet & ParentNode->IdleSmtSet)){
								CurrentThread->QuantumEndMigrate = TRUE;
								CurrentThread->ForceDeferSchedule = TRUE;
							}
						}
					} else {
						CurrentThread->QuantumEndMigrate = FALSE;
					}
				}
			}
			if(KiCheckPreferredHeteroProcessor(CurrentThread, CurrentPrcb, 1)){
				CurrentThread->ForceDeferSchedule = TRUE;
			}
			KiReleaseThreadLockSafe(CurrentThread);
		}
	}
	BOOLEAN bool15 = FALSE;
	ULONG MaxReadyThreads = 16;
	ULONG var35 = *(ULONG *) 0xFFFFF78000000320;
	if(CurrentPrcb->ReadyScanTick - var35 < 0){
		bool15 = TRUE;
		if(KiShouldScanSharedReadyQueue(CurrentPrcb)){
			KSHARED_READY_QUEUE *ReadyQueue = CurrentPrcb->SharedReadyQueue;
			if(ReadyQueue->ReadySummary & 0x7FFE){
				PLIST_ENTRY var9 = NULL;
				varp77 = ReadyQueue->QueueIndex;
				varp6F = CurrentPrcb;
				PSCHEDULER_ASSIST SchedulerAssist = CurrentPrcb->SchedulerAssist;
				ULONG var59 = 0;
				if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
					SchedulerAssist->Unknown6++;
					if(!SchedulerAssist->Unknown6){
						KiRemoveSystemWorkPriorityKick(CurrentPrcb);
					}
				}
				while(ReadyQueue->Lock & 1){
					SchedulerAssist = CurrentPrcb->SchedulerAssist;
					if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
						SchedulerAssist->Unknown6--;
						if(!SchedulerAssist->Unknown6){
							KiRemoveSystemWorkPriorityKick(CurrentPrcb);
						}
					}
					do{
						KeYieldProcessorEx(&var59);
					} while(ReadyQueue->Lock);
					SchdeulerAssist = CurrentPrcb->SchedulerAssist;
					if(SchedulerAssist != NULL && CurrntPrcb->NestingLevel <= 1) {
						SchedulerAssist->Unknown6++;
						if(!SchedulerAssist->Unknown6){
							KiRemoveSystemWorkPriorityKick(CurrentPrcb);
						}
					}
				}
				ReadyQueue->Lock |= 1;
				ULONG ReadySummary = ReadyQueue->ReadySummary & 0x7FFE;
				if(ReadySummary == 0){
					ReadyQueue->Lock = 0;
					SchedulerAssist = CurrentPrcb->SchedulerAssist;
					if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
						SchedulerAssist->Unknown6--;
						if(!SchedulerAssist->Unknown6){
							KiRemoveSystemWorkPriorityKick(CurrentPrcb);
						}
					}
					ReadyQueue->QueueIndex = 1;
				} else {
					ULONG MaxRemoved = 10;
					ULONG QueueIndex = ReadyQueue->QueueIndex;
					LIST_ENTRY *ReadyListHead = ReadyQueue->ReadyListHead;
					ReadySummary = _rotr(ReadySummary, QueueIndex);
					varp6F = (*(ULONG *) 0xFFFFF78000000320) - 300;
					ULONG var41 = 16;
					do{
						ULONG var55 = 0;
						_BitScanForward(&var55, ReadySummary);
						ReadySummary ^= (1 << var55);
						var39 = (var55 + QueueIndex) & 0x1F;
						PLIST_ENTRY ReadyListHead = &ReadyQueue->ReadyListHead[var39];
						PLIST_ENTRY ReadyList = ReadyListHead->Flink;
						do{
							PKTHREAD Thread = (PKTHREAD) (((ULONG_PTR) ReadyList) - offsetof(KTHREAD, WaitListEntry));
							BOOLEAN Remove = FALSE;
							ReadyList = ReadyList->Flink;
							if(Thread->VpBackingThread){
								KPRIORITY AssistPriority = KiReadGuestSchedulerAssistPriority(Thread);
								if(AssistPriority >= 0x10) AssistPriority = 0xF;
								Remove = AssistPriority != Thread->SchedulerAssistPriorityFloor;
							}
							ULONG Time = varp6F - Thread->WaitTime;
							if(Time > 0 || Remove){
								KiRemoveThreadFromSharedReadyQueue(ReadyQueue, Thread, var39);
								KiInsertDeferredReadyList(&var9);
								if(Time > 0){
									MaxRemoved--;
								}
							}
							var41--;
						} while(ReadyList != ReadyListHead && MaxRemoved && var41);
						ReadyListHead = ReadyQueue->ReadyListHead;
						if(ReadySummary == 0 || !MaxRemoved){
							break;
						}
						QueueIndex = ReadyQueue->QueueIndex;
					} while(var41);
					ReadyQueue->Lock = 0;
					SchedulerAssist = CurrentPrcb->SchedulerAssist;
					if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
						SchedulerAssist->Unknown6--;
						if(!SchedulerAssist->Unknown6){
							KiRemoveSystemWorkPriorityKick(CurrentPrcb);
						}
					}
					if(var9 != NULL){
						PLIST_ENTRY NextEntry;
						do{
							PKTHREAD Thread = (PKTHREAD) (((ULONG_PTR) var9) - offsetof(KTHREAD, WaitListEntry));
							varp6F = FALSE;
							var51 = Thread->CycleTime;
							NextEntry = var9->Flink;
							SchedulerAssist = CurrentPrcb->SchedulerAssist;
							ULONG var49 = 0;
							if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
								SchedulerAssist->Unknown6++;
								if(!SchedulerAssist->Unknown6){
									KiRemoveSystemWorkPriorityKick(CurrentPrcb);
								}
							}
							while(Thread->ThreadLock & 1){
								SchedulerAssist = CurrentPrcb->SchedulerAssist;
								if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
									SchedulerAssist->Unknown6--;
									if(!SchedulerAssist->Unknown6){
										KiRemoveSystemWorkPriorityKick(CurrentPrcb);
									}
								}
								do{
									KeYieldProcessorEx(&var49);
								} while(Thread->ThreadLock);
								SchedulerAssist = CurrentPrcb->SchedulerAssist;
								if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
									SchedulerAssist->Unknown6++;
									if(!SchedulerAssist->Unknown6){
										KiRemoveSystemWorkPriorityKick(CurrentPrcb);
									}
								}
							}
							Thread->ThreadLock |= 1;
							ULONG Priority = Thread->Priority;
							if(varp6F - Thread->WaitTime > 0 && Priority < 0xF) {
								varp6F = bool15;
								KiSetPriorityBoost(0, Thread, 0xF, var51);
							}
							KiReleaseThreadLockSafe(Thread);
							if(varp6F && bool15 & PerfGlobalGroupMask[2]){
								EtwTraceAntiStarvationBoost(Thread, Priority);
							}
							PLIST_ENTRY Entry = &Thread->WaitListEntry;
							Entry->Flink = NULL;
							KiReadyDeferredReadyList(CurrentPrcb, &Entry);
						} while(NextEntry != NULL);
					}
					ULONG NextIndex;
					if(!var41 || MaxRemoved == 0){
						NextIndex = var39 + 1;
						if(NextIndex > 0xE){
							NextIndex = 1;
						}
					} else {
						NextIndex = 1;
					}
					ReadyQueue->QueueIndex = NextIndex;
				}
			}
		}
	}
	if(KiGroupSchedulingEnabled){
		KiGroupSchedulingQuantumEnd(CurrentPrcb, CurrentThread, varp67, &var79)
	} else {
		ULONG var31 = 0;
		SchedulerAssist = CurrentPrcb->SchedulerAssist;
		if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
			SchedulerAssist->Unknown6++;
			if(!SchedulerAssist->Unknown6){
				KiRemoveSystemWorkPriorityKick(CurrentPrcb);
			}
		}
		while(CurrentPrcb->PrcbLock & 1){
			SchedulerAssist = CurrentPrcb->SchedulerAssist;
			if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
				SchedulerAssist->Unknown6--;
				if(!SchedulerAssist->Unknown6){
					KiRemoveSystemWorkPriorityKick(CurrentPrcb);
				}
			}
			do{
				KeYieldProcessorEx(&var31);
			} while(CurrentPrcb->PrcbLock);
			SchedulerAssist = CurrentPrcb->SchedulerAssist;
			if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
				SchedulerAssist->Unknown6++;
				if(!SchedulerAssist->Unknown6){
					KiRemoveSystemWorkPriorityKick(CurrentPrcb);
				}
			}
		}
		CurrentPrcb->PrcbLock |= 1;
	}
	ULONG var0 = 10;
	if(bool15){
		ULONG ReadySummary = CurrentPrcb->ReadySummary & 0x7FFE;
		CurrentPrcb->ReadyScanTick = var35 + 75;
		if(ReadySummary){
			PLIST_ENTRY DispatcherReadyListHead = CurrentPrcb->DispatcherReadyListHead;
			ULONG QueueIndex = CurrentPrcb->QueueIndex;
			PLIST_ENTRY WaitList = NULL;
			ReadySummary = _ror(ReadySummary, QueueIndex);
			ULONG varp6F = (*(ULONG *) 0xFFFFF78000000320) - 300;
			ULONG CurrentQueueIndex;
			do{
				ULONG StartPosition;
				_BitScanForward(&StartPosition, ReadySummary);
				ReadySummary ^= (1 << StartPosition);
				CurrentQueueIndex = (StartPosition + QueueIndex) & 0x1F;
				PLIST_ENTRY ReadyListHead = &DispatcherReadyListHead[CurrentQueueIndex];
				PLIST_ENTRY ReadyList = ReadyListHead->Flink;
				do{
					PKTHREAD Thread = (PKTHREAD) (((ULONG_PTR) ReadyList) - offsetof(KTHREAD, WaitListEntry));
					BOOLEAN Remove = FALSE;
					ReadyList = ReadyList->Flink;
					if(Thread->VpBackingThread){
						KPRIORITY AssistPriority = KiReadGuestSchedulerAssistPriority(Thread);
						if(AssistPriority >= 0x10) AssistPriority = 0xF;
						Remove = AssistPriority != Thread->SchedulerAssistPriorityFloor;
					}
					varp77 = varp6F - Thread->WaitTime;
					if(varp77 > 0 || Remove){
						KiRemoveThreadFromReadyQueue(CurrentPrcb, &Thread->WaitListEntry, CurrentQueueIndex);
						KiInsertDeferredReadyList(&ReadyList, Thread);
						if(varp77 > 0){
							var0--;
						}
					}
				} while (ReadyList != ReadyListHead && var0 && --MaxReadyThreads);
			} while (ReadySummary && var0 && MaxReadyThreads);
			if(WaitList != NULL){
				CurrentPrcb->PrcbLock = 0;
				SchedulerAssist = CurrentPrcb->SchedulerAssist;
				if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
					SchedulerAssist->Unknown6--;
					if(!SchedulerAssist->Unknown6){
						KiRemoveSystemWorkPriorityKick(CurrentPrcb);
					}
				}
				do{
					PKTHREAD Thread = (PKTHREAD) (((ULONG_PTR) WaitList) - offsetof(KTHREAD, WaitListEntry));
					varp67 = FALSE;
					WaitList = WaitList->Flink;
					ULONGLONG CycleTime = Thread->CycleTime;
					SchedulerAssist = CurrentPrcb->SchedulerAssist;
					ULONG var29 = 0;
					if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
						SchedulerAssist->Unknown6++;
						if(!SchedulerAssist->Unknown6){
							KiRemoveSystemWorkPriorityKick(CurrentPrcb);
						}
					}
					while(Thread->ThreadLock & 1){
						SchedulerAssist = CurrentPrcb->SchedulerAssist;
						if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
							SchedulerAssist->Unknown6--;
							if(!SchedulerAssist->Unknown6) {
								KiRemoveSystemWorkPriorityKick(CurrentPrcb);
							}
						}
						do{
							KeYieldProcessorEx(&var29);
						} while(Thread->ThreadLock);
						SchedulerAssist = CurrentPrcb->SchedulerAssist;
						if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
							SchedulerAssist->Unknown6++;
							if(!SchedulerAssist->Unknown6){
								KiRemoveSystemWorkPriorityKick(CurrentPrcb);
							}
						}
					}
					Thread->ThreadLock |= 1;
					varp77 = Thread->Priority;
					if(varp6F - Thread->WaitBlock[2].SpareLong > 0 && varp77 < 0xF){
						varp67 = TRUE;
						KiSetPriorityBoost(0, Thread, 0xF, CycleTime);
					}
					KiReleaseThreadLockSafe(Thread);
					if(varp67 && PerfGlobalGroupMask[2] & 1){
						EtwTraceAntiStarvationBoost(Thread, varp77);
					}
					varpF = &Thread->WaitListEntry;
					varpF->Flink = NULL;
					KiReadyDeferredReadyList(CurrentPrcb, &varpF);
				} while(WaitList != NULL);
				ULONG var25 = 0;
				PSCHEDULER_ASSIST SchedulerAssist = CurrentPrcb->SchedulerAssist;
				if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
					SchedulerAssist->Unknown6++;
					if(!SchedulerAssist->Unknown6){
						KiRemoveSystemWorkPriorityKick(CurrentPrcb);
					}
				}
				while(CurrentPrcb->PrcbLock & 1){
					SchedulerAssist = CurrentPrcb->SchedulerAssist;
					if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1) {
						SchedulerAssist->Unknown6--;
						if(!SchedulerAssist->Unknown6){
							KiRemoveSystemWorkPriorityKick(CurrentPrcb);
						}
					}
					do{
						KeYieldProcessorEx(&var25);
					} while(CurrentPrcb->PrcbLock);
					SchedulerAssist = CurrentPrcb->SchedulerAssist;
					if(SchedulerAssist != NULL && CurrentPrcb->NestingLevel <= 1){
						SchedulerAssist->Unknown6++;
						if(!SchedulerAssist->Unknown6){
							KiRemoveSystemWorkPriorityKick(CurrentPrcb);
						}
					}
				}
				CurrentPrcb->PrcbLock |= 1;
			}
			ULONG NextIndex = CurrentQueueIndex;
			if(MaxReadyThreads == 0 || !var0){
				NextIndex++;
				if(NextIndex > 0xE){
					NextIndex = 1;
				}
			} else {
				NextIndex = 1;
			}
			CurrentPrcb->QueueIndex = NextIndex;
		}
	}
	while(1){
		PKTHREAD NextThread = CurrentPrcb->NextThread;
		BOOLEAN FoundNextThread = TRUE;
		if(NextThread != NULL && varp67){
			NextThread->Preempted = FALSE;
		} else {
			if(varp67){
				NextThread = KiSelectReadyThread(CurrentThread->Priority, CurrentPrcb);
			}
			if(!varp67 || NextThread == NULL){
				if(CurrentThread->ForceDeferSchedule){
					if(CurrentThread == CurrentPrcb->IdleThread){
						CurrentThread->ForceDeferSchedule = FALSE;
						FoundNextThread = FALSE;
					} else {
						KiSelectNextThread(CurrentPrcb, &var79);
						NextThread = CurrentPrcb->NextThread;
						if(NextThread == NULL) FoundNextThread = FALSE;
					}
				} else {
					FoundNextThread = FALSE;
				}
			}
		}
		if(FoundNextThread && NextThread != CurrentPrcb->IdleThread){
			if(!KiCheckThreadAffinity(NextThread)){
				KPRIORITY Priority;
				if(!CurrentThread->Header.GroupScheduling){
					Priority = CurrentThread->Priority;
				} else {
					Priority = 1;
					if(!KiIsThreadRankNonZero(CurrntThread, CurrentPrcb)) {
						Priority = CurrentThread->Priority;
					}
				}
				*CurrentPrcb->PriorityState = Priority;
				PSCHEDULER_ASSIST SchedulerAssist = CurrentPrcb->SchedulerAssist;
				if(SchedulerAssist != NULL){
					KPRIORITY AssistPriority = (CurrentThread == CurrentPrcb->IdleThread) ? KiVpThreadSystemWorkPriority : Priority;
					KiSetSchedulerAssistPriority(SchedulerAssist, AssistPriority, 0);
				}
				if(CurrentPrcb->NextThread == NextThread) {
					KiSelectNextThread(CurrentPrcb, &var79);
				}
				KiInsertDeferredReadyList(&var79, NextThread);
				NextThread = NULL;
			}
		}
		if(!var79) break;
		if(NextThread != NULL && NextThread != CurrentPrcb->IdleThread && CurrentPrcb->NextThread != NextThread){
			KPRIORITY Priority;
			if(NextThread->Header.ThreadControlFlags == 4){
				Priority = NextThread->Priority;
			} else {
				Priority = 1;
				if(!KiIsThreadRankNonZero(NextThread, CurrentPrcb)){
					Priority = NextThread->Priority;
				}
			}
			*CurrentPrcb->PriorityState = Priority;
			PVOID SchedulerAssit = CurrentPrcb->SchedulerAssist;
			if(SchedulerAssist != NULL){
				KPRIORITY AssistPriority = (NextThread == CurrentPrcb->IdleThread) ? KiVpThreadSystemWorkPriority : Priority;
				KiSetSchedulerAssistPriority(SchedulerAssist, AssistPriority, 0);
				SchedulerAssist = CurrentPrcb->SchedulerAssist;
			}
			CurrentPrcb->NextThread = NextThread;
			if(SchedulerAssist != NULL){
				SchedulerAssist->Idle = NextThread == CurrentPrcb->IdleThread;
			}
			if(NextThread->State == Ready){
				NextThread->ReadyTime = (*(ULONG *) 0xFFFFF78000000320) + NextThread->ReadyTime - NextThread->WaitTime;
			}
			NextThread->State = Standby
		}
		CurrentPrcb->PrcbLock = 0;
		KiSetVpThreadSpinLockCount(Kpcr->CurrentPrcb, 0);
		KiReadyDeferredReadyList(CurrentPrcb, &var79);
		ULONG var21 = 0;
		var79 = 0;
		KiSetVpThreadSpinLockCount(Kpcr->CurrentPrcb, 1);
		while(CurrentPrcb->PrcbLock & 1){
			do{
				KeYieldProcessorEx(&var21);
			} while(CurrentPrcb->PrcbLock);
			KiSetVpThreadSpinLockCount(Kpcr->CurrentPrcb, 0);
		}
		CurrentPrcb->PrcbLock |= 1;
	}
	PKTHREAD NextThread = CurrentPrcb->NextThread;
	if(NextThread == NULL || CurrentThread == CurrentPrcb->IdleThread) {
		CurrentPrcb->PrcbLock = 0;
		KiSetVpThreadSpinLockCount(Kpcr->CurrentPrcb, 0);
		return;
	}
	CurrentPrcb->NextThread = NULL;
	_disable();
	KiEndThreadCycleAccumulation(CurrentPrcb, CurrentThread, 0);
	_enable();
	KPRIORITY Priority;
	if(NextThread->Header.GroupScheduling){
		Priority = 1;
		if(!KiIsThreadRankNonZero(NextThread, CurrentPrcb)){
			Priority = NextThread->Priority;
		}
	} else {
		Priority = NextThread->Priority;
	}
	*CurrentPrcb->PriorityState = Priority;
	PSCHEDULER_ASSIST SchedulerAssist = CurrentPrcb->SchedulerAssist;
	if(SchedulerAssist != NULL){
		KiSetSchedulerAssistPriority(SchedulerAssist, (NextThread == CurrentPrcb->IdleThread) ? KiVpThreadSystemWorkPriority : Priority, 0);
	}
	CurrentPrcb->CurrentThread = NextThread;
	if(NextThread->State == Ready){
		NextThread->ReadyTime = (*(ULONG *) 0xFFFFF78000000320) + NextThread->ReadyTime - NextThread->WaitTime;
	}
	NextThread->State = Running;
	CurrentThread->WaitReason = WrQuantumEnd;
	KiQueueReadyThread(CurrentPrcb, CurrentThread);
	KiAbProcessContextSwitch(CurrentThread, 1);
	BOOLEAN Allowed = KeIsUserVaAccessAllowed(0);
	if(KeSmapEnabled) _stac();
	KiSwapContext(CurrentThread, NextThread, TRUE);
	if(!Allowed && KeSmapEnabled) _clac();
}
