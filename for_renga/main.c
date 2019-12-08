/* 
	0syscall6 by SKGleba
	All Rights Reserved
*/

#include <stdio.h>
#include <string.h>
#include <taihen.h>
#include <psp2kern/kernel/modulemgr.h>
#include <vitasdkkern.h>
#include "../../psp2renga/Include/renga-defs.h"
#include "../../psp2renga/Include/renga-funcs.h"
#include "../sk_rvk_patch/sk_rvk_patch.h"

static int hook_kprx_reset_entry = 0;
static int sm_entry = 0;

static void hook_kprx() {
	// Path syscall6
	renga_exec_code(&sk_rvk_patch_nmp, NULL, sk_rvk_patch_nmp_len, 0);
	
	// Add custom kprxauth if present
	if (sm_entry == 0)
		sm_entry = renga_add_entry(NULL, "ur0:tai/kprx_auth_sm.bin", 0, RENGA_TYPE_SM, RENGA_MAGIC_KPRX);
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
	// Patch syscall6 & add custom kprxauth if present
	hook_kprx();
	
	// Add the function vaddr to renga's reset entries
	// The function will be executed everytime crypto is reset
	hook_kprx_reset_entry = renga_add_reset_entry(hook_kprx, 0);
	
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	renga_add_reset_entry(NULL, hook_kprx_reset_entry);
	return SCE_KERNEL_STOP_SUCCESS;
}
