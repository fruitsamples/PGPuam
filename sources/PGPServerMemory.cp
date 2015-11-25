//	PGPServerMemory.cp -  AppleShare IP memory management// // Apple Macintosh Developer Technical Support// Written by:  Vinne Moscaritolo////  Copyright (work in progress)  Apple Computer, Inc All rights reserved.//// You may incorporate this sample code into your applications without// restriction, though the sample code has been provided "AS IS" and the// responsibility for its operation is 100% yours.  However, what you are// not permitted to do is to redistribute the source as "DSC Sample Code"// after having made changes. If you're going to re-distribute the source,// we require that you make it clear in the source that the code was// descended from Apple Sample Code, but that you've made changes.// #include <OpenTransport.h>  #define FIX_ASLM#include <LibraryManager.h>  #define PGP_MACINTOSH 1#include "pgpErrors.h"#include "pgpKeys.h"#include "pgpMemoryMgr.h"#include "pgpUtilities.h"#include "pgpFeatures.h"#include "pgpHash.h"#include "pgpPublicKey.h"#include "TPGPException.h"#include "TMacException.h"#include "PGPServerMemory.h"#define  OTAssert( _Msg_, _cond_)	 ThrowMsgIfNot( _cond_, _Msg_)	// ---------------------------------------------------------------------------#pragma mark Local Globals?// ---------------------------------------------------------------------------static 	Boolean 					gVirtualMemoryisEnabled = false;static	PGPNewContextStruct 		gContextInfo;static	PGPNewMemoryMgrStruct		gMemMgrInfo;static 	unsigned long				gMemBytesUsed = 0;static 	unsigned long				gMemSecureBytesUsed = 0;// ---------------------------------------------------------------------------#pragma mark Local Prototypes// ---------------------------------------------------------------------------static void *  MemoryAllocationProc ( PGPMemoryMgrRef mgr,										PGPUserValue userValue,										PGPSize requestSize, 										PGPMemoryMgrFlags flags );static	PGPError MemoryReallocationProc ( PGPMemoryMgrRef mgr,										  PGPUserValue userValue,										  void **allocation, 										  PGPSize newAllocationSize,										  PGPMemoryMgrFlags flags, 										  PGPSize existingSize );static	PGPError MemoryDeallocationProc ( PGPMemoryMgrRef mgr,										  PGPUserValue userValue,										  void *allocation, 										  PGPSize allocationSize ); static 	void* 	 MemorySecureAllocationProc ( PGPMemoryMgrRef mgr,											  PGPUserValue userValue,											  PGPSize requestSize, 											  PGPMemoryMgrFlags flags,						       				  PGPBoolean *isNonPageable );						static PGPError	 MemorySecureDeallocationProc ( PGPMemoryMgrRef mgr,												PGPUserValue userValue,												void *allocation, 												PGPSize allocationSize,											    PGPBoolean	wasLocked );											    static OSStatus InitOpenTransportWithMemoryLimit(void);#pragma mark -// ---------------------------------------------------------------------------static void *  MemoryAllocationProc ( PGPMemoryMgrRef mgr,										PGPUserValue userValue,										PGPSize requestSize, 										PGPMemoryMgrFlags flags )// ---------------------------------------------------------------------------//{//	DebugStr("\p Allocation");		void* p =  OTAllocMem(requestSize) ;		if(p) gMemBytesUsed+= requestSize;	 	if(!p) DebugStr("\p Allocation Failed");	 	return p; 	 }// ---------------------------------------------------------------------------static	PGPError MemoryReallocationProc ( PGPMemoryMgrRef mgr,										  PGPUserValue userValue,										  void **allocation, 										  PGPSize newAllocationSize,										  PGPMemoryMgrFlags flags, 										  PGPSize existingSize )// ---------------------------------------------------------------------------//{	void* p;//	DebugStr("\p Reallocation");		p =  OTAllocMem(newAllocationSize); 	if(!p) DebugStr("\p Reallocation Failed");		if(!p) 	{	  	OTFreeMem(p);		return(kPGPError_OutOfMemory);	}	else	{ 		gMemBytesUsed+= (newAllocationSize - existingSize);				OTMemcpy(p,  *allocation, existingSize);	  	OTFreeMem( *allocation );		*allocation = p;	}	  	return noErr;}										  							// ---------------------------------------------------------------------------static	PGPError MemoryDeallocationProc ( PGPMemoryMgrRef mgr,										  PGPUserValue userValue,										  void *allocation, 										  PGPSize allocationSize )// ---------------------------------------------------------------------------//{//	DebugStr("\p Deallocation");		gMemBytesUsed-=allocationSize;	 	OTFreeMem( allocation );	return noErr;} // ---------------------------------------------------------------------------static 	void* 	 MemorySecureAllocationProc ( PGPMemoryMgrRef mgr,											  PGPUserValue userValue,											  PGPSize requestSize, 											  PGPMemoryMgrFlags flags,						       				  PGPBoolean *isNonPageable )// ---------------------------------------------------------------------------//{	void* p;	 //	DebugStr("\p  Secure Allocation");		p =  OTAllocMem(requestSize);  	if(!p) DebugStr("\p Secure Allocation Failed");			if(p)	{ 		if(gVirtualMemoryisEnabled)	HoldMemory(p, requestSize); 		gMemSecureBytesUsed+=requestSize;	} 	return(p);}						// ---------------------------------------------------------------------------static PGPError	 MemorySecureDeallocationProc ( PGPMemoryMgrRef mgr,												PGPUserValue userValue,												void *allocation, 												PGPSize allocationSize,											    PGPBoolean	wasLocked )// ---------------------------------------------------------------------------//{// 	DebugStr("\p  Secure Deallocation");			OTMemzero(allocation, allocationSize); 	if(gVirtualMemoryisEnabled) UnholdMemory(allocation, allocationSize); 	OTFreeMem( allocation );	gMemSecureBytesUsed-=allocationSize;		return noErr;}#pragma mark -// ---------------------------------------------------------------------------OSStatus InitializeServerMemory(  PGPMemoryMgrRef *memMgr )// ---------------------------------------------------------------------------//{	OSStatus err;		long response = 0;// Is virtual memory enabled?	Gestalt(gestaltVMAttr, &response);	gVirtualMemoryisEnabled = (response & (1UL << gestaltVMPresent)) != 0;		// Setup opentransport for memory usage	err = InitOpenTransportWithMemoryLimit();	if(err != noErr) return err; // preallocate memory into heap. 	OTFreeMem(OTAllocMem( 64 * 1024)); // setup custom PGP memory manager structure  	gMemMgrInfo.sizeofStruct =   sizeof( PGPNewMemoryMgrStruct );	gMemMgrInfo.allocProc  			= 	MemoryAllocationProc;	gMemMgrInfo.reallocProc  		= 	MemoryReallocationProc;	gMemMgrInfo.deallocProc  		= 	MemoryDeallocationProc;	gMemMgrInfo.secureAllocProc  	= 	MemorySecureAllocationProc;	gMemMgrInfo.secureDeallocProc  	= 	MemorySecureDeallocationProc;	gMemMgrInfo.customValue  		= 	nil;	gMemMgrInfo.reserved  = 	nil;	gMemMgrInfo.pad[0]  = 	nil;	gMemMgrInfo.pad[1]  = 	nil;	gMemMgrInfo.pad[2]  = 	nil;	gMemMgrInfo.pad[3]  = 	nil;	gMemMgrInfo.pad[4]  = 	nil;	gMemMgrInfo.pad[5]  = 	nil;	gMemMgrInfo.pad[6]  = 	nil;	gMemMgrInfo.pad[7]  = 	nil; 	err = PGPNewMemoryMgrCustom( &gMemMgrInfo, memMgr);		return err;}// ---------------------------------------------------------------------------void FinalizeServerMemory( PGPMemoryMgrRef memMgr )// ---------------------------------------------------------------------------//{	PGPFreeMemoryMgr(memMgr);	CloseOpenTransport();	CleanupLibraryManager();}#pragma mark -// from Quinn's StreamLogWatcher.c// // InitOpenTransportWithMemoryLimit Big Picture// --------------------------------------------//// The LogEngine uses the OT memory allocation routines// (OTAllocMem, OTFreeMem) to allocate space for log entries in// the notifier.  This memory comes from an ASLM memory pool// that OT creates for us when we call InitOpenTransport.  However,// this pool has some bad characteristics://// 	1.	The pool starts off very small, and only grows when we allocate// 		memory from it.  As we do all our allocation from our notifier//		(which is interrupt time with respect to the system Memory Manager)//		the pool can't grow immediately.  So the pool will often run//		be full (ie OTAllocMem will return nil) even though the application//		has plenty of memory.  The pool will later grow, but we've already//		dropped the log entry on the floor.////	2.	Because the pool starts off small and grows by pieces, we get//		an extremely fragmented pool.  While this works, its definitely//		sub-optimal.////	3.	If we're being hammered by strlog (ie people are calling strlog a//		lot), the pool will keep growing and there's nothing to stop//		the pool consuming our entire application heap.  When it does so,//		various toolbox routines (eg QuickDraw) fail ungracefully, ie//		SysError(25).//// There are a number of steps in my solution to this.  The first step// is to call InitLibraryManager myself.  This allows me to specify// the size of the pool. InitOpenTransport notices that I have inited ASLM// myself and doesn't do it itself.  Thus OTAllocMem gets its memory from// whatever pool ASLM created.  This gets around problems 1 and 2.//// The second step is to create a subsidiary zone within my application heap// and specify that ASLM should create its pool in that zone (by supplying// kCurrentZone to InitLibraryManager).  Thus the pool can grow up to the// point where the memory in the subsidiary zone is exhausted.  At that point,// the pool will no longer grow.  So the pool will not steal memory from// the main application heap.  This gets around problem 3.//// There are a number of other ways I could have achieved the same results.// The ASLM memory manager is very flexible.  For example, I could have removed// OT's TPoolNotifier from the pool, which would prevent the pool from growing.// However, this solution does not require me to use any ASLM C++ stuff,// which makes the code more compiler independent.enum {	kBytesReservedForToolboxInApplicationZone = 100L * 1024L,		// This value represents the minimum number of contiguous		// bytes that should remain in the application heap after		// we've created the subsidiary zone.			kBytesReservedForASLMInSubsidaryzone = 2048,		// This value represents the number of bytes in the subsidiary		// zone we should leave lying around for general purpose ASLM		// use.  The remaining bytes in the subsidiary zone are		// dedicated to the ASLM memory pool, ie are passed as the pool		// size to InitLibraryManager.			kMinimumBytesForUsInSubsidiaryZone = 10 * 1024		// This value represents the minimum pool size we pass to		// InitLibraryManager.  If we can't create a pool of at least		// this size, the application doesn't start up.};static OSStatus InitOpenTransportWithMemoryLimit(void)	// See above for an explanation of the big picture here.{	OSStatus err;	SInt32 junkTotalFree;	SInt32 contigFree;	SInt32 zoneSize;	Ptr gSubsidiaryZone;	THz oldZone;	// Debugger();		// First call the system Memory Manager to determine the largest	// contiguous block in the heap.		PurgeSpace(&junkTotalFree, &contigFree);		// If it's too small for our toolbox needs, bail out.		err = noErr;	if (contigFree < kBytesReservedForToolboxInApplicationZone) {		err = memFullErr;	}		// Now calculate the size of the zone we're going to create.	// It's the size of the largest contiguous block, minus	// the size of we reserve for toolbox needs, rounded to the nearest KB.	// If the zone size isn't big enough enough to hold our minimum	// pool size and the amount we reserve for ASLM, bail out.		if (err == noErr) {		zoneSize = contigFree - kBytesReservedForToolboxInApplicationZone;		zoneSize = zoneSize & ~0x003FF;		if (zoneSize < (kBytesReservedForASLMInSubsidaryzone + kMinimumBytesForUsInSubsidiaryZone)) {			err = memFullErr;		}	}		// Allocate the memory for our zone and create a zone in that	// block.  Then init ASLM, telling it to create a pool that	// takes up the entire zone (minus the ASLM overhead factor)	// in the current zone, ie the zone we just created.  Finally,	// initialise OT.  OT will see that we've inited ASLM and use	// the pool that ASLM created (in the zone we created) for	// satisfying OTAllocMem requests.		if (err == noErr) {		gSubsidiaryZone = NewPtr(zoneSize);		OTAssert("InitOpenTransportWithMemoryLimit: Couldn't get the memory but preflight says its there", gSubsidiaryZone != nil);		OTAssert("InitOpenTransportWithMemoryLimit: Just being paranoid", MemError() == noErr);		oldZone = GetZone();		InitZone(nil, 16, gSubsidiaryZone + zoneSize, gSubsidiaryZone);		OTAssert("InitOpenTransportWithMemoryLimit: InitZone failed", MemError() == noErr);		// InitZone sets the current zone to the newly created zone,		// so I don't have to do it myself.						err = InitLibraryManager(zoneSize - kBytesReservedForASLMInSubsidaryzone, kCurrentZone, kNormalMemory);		if (err == noErr) {			err = InitOpenTransport();			if (err != noErr) {				CleanupLibraryManager();			}		}				SetZone(oldZone);	}  	return err;}