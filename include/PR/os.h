#ifndef _OS_H_
#define	_OS_H_

#include "os_thread.h"
#include "os_message.h"
#include "os_exception.h"
#include "os_pi.h"
#include "os_system.h"
#include "os_convert.h"

#ifdef _LANGUAGE_C_PLUS_PLUS
extern "C" {
#endif

#include "ultratypes.h"

/*
 * Stack size for I/O device managers: PIM (PI Manager), VIM (VI Manager),
 *	SIM (SI Manager)
 *
 */
#define OS_PIM_STACKSIZE	4096
#define OS_VIM_STACKSIZE	4096
#define OS_SIM_STACKSIZE	4096

#define	OS_MIN_STACKSIZE	72

/* 
 * Leo Disk 
 */

/* transfer mode */

#define LEO_BLOCK_MODE	1
#define LEO_TRACK_MODE	2
#define LEO_SECTOR_MODE	3

/*
 * Boot addresses
 */
#define	BOOT_ADDRESS_ULTRA	0x80000400
#define	BOOT_ADDRESS_COSIM	0x80002000
#define	BOOT_ADDRESS_EMU	0x20010000
#define	BOOT_ADDRESS_INDY 	0x88100000


#ifdef _LANGUAGE_C_PLUS_PLUS
}
#endif

#endif /* !_OS_H */
