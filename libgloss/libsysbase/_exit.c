/* Stub version of _exit.  */

#include <limits.h>
#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/iosupport.h>

_VOID
_DEFUN (_exit, (rc),
	int rc)
{

	if ( __syscalls.exit ) {
		__syscalls.exit(rc);
	}

	while(1);
}
