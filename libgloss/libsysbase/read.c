#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <stdio.h>
#include <errno.h>

#include <sys/iosupport.h>

#ifdef REENTRANT_SYSCALLS_PROVIDED
//---------------------------------------------------------------------------------
_ssize_t _DEFUN(_read_r,(r, fileDesc, ptr, len),
			struct _reent * r _AND
			int   fileDesc  _AND
			void *ptr   _AND
			size_t   len) {
//---------------------------------------------------------------------------------
#else
//---------------------------------------------------------------------------------
_ssize_t _DEFUN(_read,(fileDesc, ptr, len),
           int   fileDesc  _AND
           char *ptr   _AND
           size_t   len) {
//---------------------------------------------------------------------------------
	struct _reent *r = _REENT;
#endif
	int ret = -1;
	unsigned int dev = 0;
	int fd = -1;

	__handle * handle = NULL;

	if(fileDesc!=-1) {
		handle = __get_handle(fileDesc);

		if ( NULL == handle ) return ret;

		dev = handle->device;
		fd = (int)handle->fileStruct;

		if(devoptab_list[dev]->read_r)
			ret = devoptab_list[dev]->read_r(r,fd,ptr,len);
		else
			r->_errno=ENOSYS;
	}
	return ret;
}

