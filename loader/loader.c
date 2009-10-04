
extern void *usbd_irx;
extern int size_usbd_irx;

extern void *usbhdfsd_irx;
extern int size_usbhdfsd_irx;

extern void *isofs_irx;
extern int size_isofs_irx;
 
#include "loader.h"

char ElfPath[255]; // it should be here to avoid it to be wiped by the clear user mem

void set_ipconfig(void)
{
	memset(g_ipconfig, 0, IPCONFIG_MAX_LEN);
	g_ipconfig_len = 0;
	
	// add ip to g_ipconfig buf
	strncpy(&g_ipconfig[g_ipconfig_len], "192.168.0.10", 15);
	g_ipconfig_len += strlen("192.168.0.10") + 1;

	// add netmask to g_ipconfig buf
	strncpy(&g_ipconfig[g_ipconfig_len], "255.255.255.0", 15);
	g_ipconfig_len += strlen("255.255.255.0") + 1;

	// add gateway to g_ipconfig buf
	strncpy(&g_ipconfig[g_ipconfig_len], "192.168.0.1", 15);
	g_ipconfig_len += strlen("192.168.0.1") + 1;
}

int main(int argc, char **argv){
	int i;
	
	printf("Starting...\n");
	
	SifInitRpc(0);

	set_ipconfig();
	
	sprintf(ElfPath,"cdrom0:\\%s;1",argv[1]);

	GetIrxKernelRAM();
	
	// Clearing user mem, so better not to have anything valuable on stack
	for (i = 0x100000; i < 0x02000000; i += 64) {
		asm (
			"\tsq $0, 0(%0) \n"
			"\tsq $0, 16(%0) \n"
			"\tsq $0, 32(%0) \n"
			"\tsq $0, 48(%0) \n"
			:: "r" (i)
		);
	}

	/* installing kernel hooks */
	Install_Kernel_Hooks();

    SifExitRpc();

    New_Reset_Iop("rom0:UDNL rom0:EELOADCNF", 0);
    
	SifInitRpc(0);

    cdInit(CDVD_INIT_INIT);
	
	GS_BGCOLOUR = 0xff0000; 
	
	printf("Loading Elf...\n");

	LoadExecPS2(ElfPath, 0, NULL);	

	GS_BGCOLOUR = 0x0000ff;
	SleepThread();

	return 0;
}

