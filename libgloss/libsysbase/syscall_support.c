#include <sys/iosupport.h>

//---------------------------------------------------------------------------------
__syscalls_t __syscalls = {
//---------------------------------------------------------------------------------
	NULL,	// sbrk
	NULL,	// exit
	NULL,	// gettod_r
	NULL,	// lock_init
	NULL,	// lock_close
	NULL,	// lock_release
	NULL,	// lock_acquire
	NULL,	// __getreent
};

int __libc_lock_init(_LOCK_T *lock,int recursive) { 

	if ( __syscalls.lock_init ) {
		return __syscalls.lock_init(lock, recursive);
	}
	
	return 0;
}

int __libc_lock_close(_LOCK_T *lock ) {
	
	if ( __syscalls.lock_close) {
		return __syscalls.lock_close(lock);
	}
	
	return 0;
}

int __libc_lock_release(_LOCK_T *lock ) {
	
	if ( __syscalls.lock_release) {
		return __syscalls.lock_release(lock);
	}
	
	return 0;
}

int __libc_lock_acquire(_LOCK_T *lock ) {
	
	if ( __syscalls.lock_acquire) {
		return __syscalls.lock_acquire(lock);
	}
	
	return 0;
}

