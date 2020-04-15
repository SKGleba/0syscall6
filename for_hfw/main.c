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
#include "rvkp/sk_rvk_patch.h"
#include "smlh/sm_load_patch.h"

// flag if patched load_sm should patch rvk checks ASAP
static int should_patch = 0;

// "Cache" the custom kprx_auth_sm.elf
static void *cached_kprxauthsm = NULL;
static volatile uint32_t cached_kprxauthsm_sz = 0;
static int cache_kprxauthsm(char *smsrc) {
	SceIoStat stat;
	int stat_ret = ksceIoGetstat(smsrc, &stat);
	if(stat_ret < 0 || stat.st_size > 0xA000)
		return 2;
	int mbuid = ksceKernelAllocMemBlock("cached_kprxsm", 0x1020D006, 0xA000, NULL);
	ksceKernelGetMemBlockBase(mbuid, (void**)&cached_kprxauthsm);
	int fd = ksceIoOpen(smsrc, SCE_O_RDONLY, 0);
	ksceIoRead(fd, cached_kprxauthsm, stat.st_size);
	ksceIoClose(fd);
	cached_kprxauthsm_sz = stat.st_size;
	return 0;
}

// A mod of NMPrun_default
static int patch_rvkc(uint32_t fw) {
	int ret = 0;
	NMPctx = -1;
	ret = NMPexploit_init(fw);
	if (ret != 0)
		return ret;
	ret = NMPconfigure_stage2(fw);
	if (ret != 0)
		return (0x60 + ret);
	ret = NMPreserve_commem(0, 1);
	if (ret != 0)
		return (0x10 + ret);
	char bkp[0x80];
	memcpy(&bkp, NMPcorridor, 0x80); // save first 0x80 bytes of SPRAM
	memset(NMPcorridor, 0, 0x300);
	ret = NMPcopy(&NMPstage2_payload, 0, sizeof(NMPstage2_payload), 0);
	if (ret != 0)
		return (0x20 + ret);
	ret = NMPcopy(&sk_rvk_patch_nmp, 0x100, sk_rvk_patch_nmp_len, 0);
	if (ret != 0)
		return (0x30 + ret);
	ret = NMPcopy(&sm_load_patch_nmp, 0x220, sm_load_patch_nmp_len, 0);
	if (ret != 0)
		return (0x30 + ret);
	if (cached_kprxauthsm_sz > 0) { // add custom kprx_auth_sm if present
		memcpy((NMPcorridor + 0x300), cached_kprxauthsm, cached_kprxauthsm_sz);
		*(uint32_t *)(NMPcorridor + 0x200) = 0xDEAFFADE; // magic
		*(uint32_t *)(NMPcorridor + 0x204) = (0x1f850300 + 0x1000); // .elf \ header
		*(uint32_t *)(NMPcorridor + 0x208) = ((uint32_t)cached_kprxauthsm_sz - 0x1000); // size of .elf \ header
	}
	ret = NMPf00d_jump((uint32_t)NMPcorridor_paddr, fw);
	if (ret != 0)
		return (0x40 + ret);
	memcpy(NMPcorridor, &bkp, 0x80); // restore first 0x80 bytes of SPRAM (WHY? something dies)
	ret = NMPfree_commem(0);
	if (ret != 0)
		return (0x50 + ret);
	ksceSblSmCommStopSm(NMPctx, &NMPstop_res);
	return 0;
}

// If the flag is set - run the rvk_patch payload
static tai_hook_ref_t load_sm_data_ref;
static int load_sm_data_hook(int a0, int a1, int a2, int a3, int a4, int a5) {
	if (should_patch == 1) {
		should_patch = 0;
		patch_rvkc(0x03650000); // lv0 offsets are the same for 3.60-3.65
	}
	return TAI_CONTINUE(int, load_sm_data_ref, a0, a1, a2, a3, a4, a5);
}

// 0x10000 is the first sysevent that is passed, make load_sm hook patch rvk ASAP
static int zss_sysevent_handler(int resume, int eventid, void *args, void *opt) {
	if ((resume) && (eventid == 0x10000))
		should_patch = 1;
	return 0;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
	// Enable origin fw update_sm caching
	NMPcache_ussm("os0:zss_ussm.self", 1);
	
	// Use venezia SPRAM
	NMPcorridor_paddr = 0x1f850000;
	NMPcorridor_size = 0x10000;
	
	ksceKernelRegisterSysEventHandler("zss_sysevent", zss_sysevent_handler, NULL);
	
	// Add custom (signed) kprxauth if present
	cache_kprxauthsm("os0:zss_ka.elf");
	
	taiHookFunctionExportForKernel(0x10005, &load_sm_data_ref, "SceSblSsSmComm", 0xCD3C89B6, 0x039C73B1, load_sm_data_hook);
	
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	return SCE_KERNEL_STOP_SUCCESS;
}
