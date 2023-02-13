#ifndef __GMLONGJMP_H__
#define __GMLONGJMP_H__


//#define malloc this_isnt_how

// NOTES:
// 		http://www.ibm.com/developerworks/library/l-reent/
//		https://www.securecoding.cert.org/confluence/display/seccode/SIG30-C.+Call+only+asynchronous-safe+functions+within+signal+handlers
//		
// CREDITS:
// 		Way too many to release this, I had 300 browser tabs open at one point


extern "C" {
			
	#include <pthread.h>
	#include <dlfcn.h>
	#include <sys/mman.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <time.h>  
	#include <string.h>
	#include <signal.h>
	#include <ucontext.h>
	#include <errno.h>
	#include <bfd.h>
	#include <dlfcn.h>
	#include <link.h>
	#include <fcntl.h>
	#include <setjmp.h>
	#include <cxxabi.h>
	#include <sys/types.h>
	 
	#include <sys/syscall.h>

	#include <sys/stat.h>
	//#include <asm/sigcontext.h>
	#include <execinfo.h>
		//#undef backtrace_symbols;
	//#include "bridge.h"


	// ???
	#define UNW_LOCAL_ONLY
	#include <libunwind.h>

	#include <fenv.h>
}


#define DEMANGLE_LEN 1024
char *demanglealloc;



/* This structure mirrors the one found in /usr/include/asm/ucontext.h */
typedef struct _sig_ucontext {
 unsigned long     uc_flags;
 struct ucontext   *uc_link;
 stack_t           uc_stack;
 struct sigcontext uc_mcontext;
 sigset_t          uc_sigmask;
} sig_ucontext_t;

#include "lua.hpp"
extern "C" {
	
	extern void *__cxa_allocate_exception(size_t thrown_size);
	extern void __cxa_throw (void *thrown_exception, void* *tinfo, void (*dest) (void *) );
	extern void * _ZTIl; // typeinfo of long

}

lua_State* GLUA;

#ifdef CRASH_DEBUG
FILE * outstream = stderr;
#endif

int logfile = 0;
static char __log_arr__[255]; 
int __log_len__ = 0;
int ___foo_ret___ = 0;

// not safe but what can you do...

void log(const char * str) {
	char * s = (char *)"<NULLSTR>";
	if (str!=NULL) {
		s=(char *)str;
	}
	int len = strlen(s);
	if (logfile) {
		if(write (logfile, s, len)){}
	}
	if(write (2, s, strlen(s))) {}
};
void log(const char * str, int len) {
	char * s = (char *)"<NULLSTR>";
	if (str!=NULL) {
		s=(char *)str;
	}
	if (logfile) {
		if (write (logfile, s, len)) {}
	}
	if(write (2, s, len)){}
};

//#define log(s) if (logfile) write (logfile, (s==NULL)?"NULL":s, strlen ((s==NULL)?"NULL":s)); write (2, (s==NULL)?"NULL":s, strlen ((s==NULL)?"NULL":s))
#define logf(a,...) { __log_len__ = snprintf (__log_arr__, 255,a, ##__VA_ARGS__);\
					if (logfile) ___foo_ret___ = write (logfile,	__log_arr__, __log_len__ ); \
					___foo_ret___ = write (2, 		__log_arr__, __log_len__ ); }

inline bool starts_with(const char *str,const char *pre)
{
    size_t lenpre = strlen(pre);
    size_t lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

struct Handler
{	int type;
	struct sigaction sigaction;
	struct sigaction old_sigaction;
};

Handler signal_handlers[] =
{
	{SIGUSR1},
	{-1}
};

#define STACKSIZE 4096

static ucontext_t thread_context;
static ucontext_t scheduler_context;
//char thread_stack[STACKSIZE];
struct sigaction sih;
struct sigaction old_sih;
	
pid_t main_thread;

static bfd* abfd = 0;
static asymbol **syms = 0;
static asection *text = 0;

stack_t signal_alt_stack;	
static bool sig_loaded = false;



static void ms2ts(struct timespec *ts, unsigned long ms)
{
    ts->tv_sec = ms / 1000;
    ts->tv_nsec = (ms % 1000) * 1000000;
}


struct lua_State
{
#if ( defined( _WIN32 ) || defined( __linux__ ) || defined( __APPLE__ ) ) && \
    !defined( __x86_64__ ) && !defined( _M_X64 )
    // Win32, Linux32 and macOS32
    unsigned char _ignore_this_common_lua_header_[48 + 22];
#elif ( defined( _WIN32 ) || defined( __linux__ ) || defined( __APPLE__ ) ) && \
    ( defined( __x86_64__ ) || defined( _M_X64 ) )
    // Win64, Linux64 and macOS64 (not tested)
    unsigned char _ignore_this_common_lua_header_[92 + 22];
#else
    #error Unsupported platform
#endif


	// fuck the rest
    //GarrysMod::Lua::ILuaBase* luabase;
};


#endif /* __GMLONGJMP_H__ */
