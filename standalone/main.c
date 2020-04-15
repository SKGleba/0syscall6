/* 
	0syscall6 by SKGleba
	All Rights Reserved
*/

#include <stdio.h>
#include <string.h>
#include <psp2kern/kernel/modulemgr.h>
#include <vitasdkkern.h>
#include "../../psp2renga/Include/nmprunner.h"
#include "../sk_rvk_patch/sk_rvk_patch.h"

static int zss_sysevent_handler(int resume, int eventid, void *args, void *opt) {
	if ((resume) && (eventid == 0x100000))
		NMPrun_default(&sk_rvk_patch_nmp, sk_rvk_patch_nmp_len);
	return 0;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
	// Enable update_sm caching
	NMPcache_ussm("os0:sm/update_service_sm.self", 1);
	
	// Use venezia SPRAM
	NMPcorridor_paddr = 0x1f850000;
	NMPcorridor_size = 0x10000;
	
	// Patch syscall6 to return 0
	NMPrun_default(&sk_rvk_patch_nmp, sk_rvk_patch_nmp_len);
	
	// Add sehandler to rerun the patcher at every resume (crypto reset)
	ksceKernelRegisterSysEventHandler("zss_sysevent", zss_sysevent_handler, NULL);
	
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	return SCE_KERNEL_STOP_SUCCESS;
}
