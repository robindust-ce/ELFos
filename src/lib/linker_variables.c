
extern int _kernelspace_start;
extern int _TT2_end;
extern int _kerneltext_start;
extern int _kerneltext_end;
extern int _kerneldata_start;
extern int _kernelrodata_start;
extern int _kernelrodata_end;
extern int _kernelstacks_start;
extern int _kernelstacks_end;

extern int _userspace_start;
extern int _usertext_start;
extern int _usertext_end;
extern int _userdata_start;
extern int _userbss_end;
extern int _userrodata_start;
extern int _userrodata_end;
extern int _userstacks_start;
extern int _userstacks_end;
extern int _userspace_end;

unsigned int kernelspace_start = (unsigned int) &_kernelspace_start;
unsigned int TT2_end = (unsigned int) &_TT2_end;
unsigned int kerneltext_start = (unsigned int) &_kerneltext_start;
unsigned int kerneltext_end= (unsigned int) &_kerneltext_end;
unsigned int kerneldata_start = (unsigned int) &_kerneldata_start;
unsigned int kernelrodata_start = (unsigned int) &_kernelrodata_end;
unsigned int kernelrodata_end = (unsigned int) &_kernelrodata_end;
unsigned int kernelstacks_start = (unsigned int) &_kernelstacks_start;
unsigned int kernelstacks_end = (unsigned int) &_kernelstacks_end;

unsigned int userspace_start = (unsigned int) &_userspace_start;
unsigned int usertext_start = (unsigned int) &_usertext_start;
unsigned int usertext_end = (unsigned int) &_usertext_end;
unsigned int userdata_start = (unsigned int) &_userdata_start;
unsigned int userbss_end = (unsigned int) &_userbss_end;
unsigned int userrodata_start = (unsigned int) &_userrodata_start;
unsigned int userrodata_end = (unsigned int) &_userrodata_end;
unsigned int userstacks_start = (unsigned int) &_userstacks_start;
unsigned int userstacks_end = (unsigned int) &_userstacks_end;
unsigned int userspace_end = (unsigned int) &_userspace_end;
