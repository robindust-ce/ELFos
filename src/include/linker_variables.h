#ifndef _LINKER_VARIABLES_H_
#define _LINKER_VARIABLES_H_

extern unsigned int kernelspace_start;
extern unsigned int TT2_end;
extern unsigned int kerneltext_start;
extern unsigned int kerneltext_end;
extern unsigned int kerneldata_start;
extern unsigned int kernelrodata_start;
extern unsigned int kernelrodata_end;
extern unsigned int kernelstacks_start;
extern unsigned int kernelstacks_end;

extern unsigned int userspace_start;
extern unsigned int usertext_start;
extern unsigned int usertext_end;
extern unsigned int userdata_start;
extern unsigned int userbss_end;
extern unsigned int userrodata_start;
extern unsigned int userrodata_end;
extern unsigned int userstacks_start;
extern unsigned int userstacks_end;
extern unsigned int userspace_end;

#endif//_LINKER_VARIABLES_H_
