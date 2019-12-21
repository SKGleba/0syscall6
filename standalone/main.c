/* 
	0syscall6 by SKGleba
	All Rights Reserved
*/

#include <stdio.h>
#include <string.h>
#include <taihen.h>
#include <psp2kern/kernel/modulemgr.h>
#include <vitasdkkern.h>
#include "../../psp2renga/Include/nmprunner.h"
#include "../sk_rvk_patch/sk_rvk_patch.h"

// Thanks CelesteBlue for RE on this (source: StorageMgr)
SceUID sub_81000000_patched_hook = -1;
static tai_hook_ref_t sub_81000000_patched_ref;
SceUID sub_81000000_patched(int resume, int eventid, void *args, void *opt) {
	int ret = TAI_CONTINUE(SceUID, sub_81000000_patched_ref, resume, eventid, args, opt);
	if (eventid == 0x100000)
		NMPrun_default(&sk_rvk_patch_nmp, sk_rvk_patch_nmp_len);
	return ret;
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
	
	// Hook ssmgr to rerun the patcher at every resume (crypto reset)
	tai_module_info_t scesblssmgr_modinfo;
	taiGetModuleInfoForKernel(KERNEL_PID, "SceSblSsMgr", &scesblssmgr_modinfo);
	sub_81000000_patched_hook = taiHookFunctionOffsetForKernel(KERNEL_PID, &sub_81000000_patched_ref, scesblssmgr_modinfo.modid, 0, 0, 1, sub_81000000_patched);
	
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	// Free the cached sm memblock
	NMPcache_ussm(NULL, 0);
	return SCE_KERNEL_STOP_SUCCESS;
}
