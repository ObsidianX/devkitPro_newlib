#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <sys/iosupport.h>

#define MAX_HANDLES 1024

static __handle __stdin_handle  = {0, 1, NULL};
static __handle __stdout_handle = {1, 1, NULL};
static __handle __stderr_handle = {2, 1, NULL};

static __handle* handles[MAX_HANDLES] = {
	&__stdin_handle,
	&__stdout_handle,
	&__stderr_handle
};

__LOCK_INIT_RECURSIVE(static, __hndl_lock);

void __release_handle(int fd) {

	if ( fd <0 || fd >= MAX_HANDLES ) return;

	__lock_acquire_recursive (__hndl_lock);

	__handle* handle = handles[fd];
	if ( NULL != handle ) {

		if (handle != &__stdin_handle && handle != &__stdout_handle && handle != &__stderr_handle)
			free(handle);

		handles[fd] = NULL;
	}

	__lock_release_recursive (__hndl_lock);

}

int  __alloc_handle(size_t size) {

	int i, ret = -1;

	__lock_acquire_recursive (__hndl_lock);

	for ( i = 0; i < MAX_HANDLES; i++ ) {
		if ( handles[i] == NULL ) break;
	}

	if ( i < MAX_HANDLES ) {
		handles[i] = malloc(size);
		if ( NULL != handles[i] ) {
			handles[i]->refcount = 1;
			ret =  i;
		}
	}

	__lock_release_recursive (__hndl_lock);

	return ret;
}

__handle *__get_handle(int fd) {

	if ( fd < 0 || fd >= MAX_HANDLES ) return NULL;

	return handles[fd];

}

static int __dup(int oldfd) {

	int i;

	if (handles[oldfd]==NULL) {
		errno = EBADF;
		return -1;
	}

	for ( i = 0; i < MAX_HANDLES; i++ ) {
		if ( handles[i] == NULL ) break;
	}

	if (i<MAX_HANDLES) {
		handles[i] = handles[oldfd];
		handles[oldfd]->refcount++;
		return i;
	}

	return -1;
}

int dup(int oldfd) {

	__lock_acquire_recursive (__hndl_lock);

	int ret = __dup(oldfd);

	__lock_release_recursive (__hndl_lock);

	return ret;

}

static int __dup2(int oldfd, int newfd) {

	if ( newfd < 0 || newfd >= MAX_HANDLES ||
		 oldfd < 0 || oldfd >= MAX_HANDLES ||
		 handles[oldfd] == NULL ) {

		errno = EBADF;
		return -1;
	}

	if ( newfd == oldfd ) return newfd;

	if ( handles[newfd] != NULL) {

		handles[newfd]->refcount--;

		if (handles[newfd]->refcount == 0 ) {

			if( devoptab_list[handles[newfd]->device]->close_r != NULL) {
				devoptab_list[handles[newfd]->device]->close_r(_REENT,(unsigned int)handles[newfd]->fileStruct);
			}
			__release_handle(newfd);
		}
	}

	handles[newfd] = handles[oldfd];
	handles[newfd]->refcount++;

	return newfd;

}

int dup2(int oldfd, int newfd) {

	__lock_acquire_recursive (__hndl_lock);

	newfd = __dup2(oldfd,newfd);

	__lock_release_recursive (__hndl_lock);

	return newfd;

}