# 0syscall6
A tool that patches Playstation Vita's lv0's secure_kernel's syscall 6 (SELF/RVK checks)

## What does that syscall 6 patch do?
	- Removes firmware/sdk version checks from SELFs
		- You can i.e play higher firmware games on lower firmwares, assuming that keys didnt change.
	- Removes RVK-list checks from SELFs
		- You can i.e use the revoked PSM-dev app
	- Some other basic SELFs checks are removed

## Usage
	1) Put the plugin somewhere, preferably in ur0:tai/
	2) Add a line to the currently active tai/config.txt (ux0 or, preferably, ur0) under "*KERNEL"
		- ur0:tai/0syscall6.skprx
	3) Reboot
	
## Notes
	- If the SELF uses new key revisions please use the _renga version with a decrypted kprx_auth_sm (.bin) in ur0:tai/
		- psp2renga framework is required
	- If you are using the psp2renga framework, please use the _renga version to prevent framework overwrite
	
## Credits
	- Team Molecule / xyz for the update_sm 0x50002 exploit and help over discord
	- Team Molecule for HenKaku and TaiHen
	- CelesteBlue for StorageMgr (the ssresume hook)