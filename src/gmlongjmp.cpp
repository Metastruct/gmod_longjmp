// thread clone and restore memory (or just memory restore?) (what about FDs, threads, etc!)
// vs. CRIU / DMTCP

// TODO: pthread_kill?  https://stackoverflow.com/a/56607278
	// __cxa_throw handling?
	// https://www.corsix.org/content/libunwind-dynamic-code-x86-64
	// https://monkeywritescode.blogspot.com/p/c-exceptions-under-hood.html

	// THROWING IN LUAJIT https://github.com/LuaJIT/LuaJIT/blob/d0e88930ddde28ff662503f9f20facf34f7265aa/src/lj_err.c#L22
	
	
	
extern "C" {
		
	#ifdef __APPLE__
	#include <libkern/OSByteOrder.h>
	#define htobe64(x) OSSwapHostToBigInt64(x)
	#elif __FreeBSD__
	#include <sys/endian.h>
	#elif __linux__
	#include <endian.h>
	#endif
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>

	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "gmlongjmp.hpp"


siginfo_t* crash_info = 0;
void * crash_ucontext = 0;
ucontext_t * ctx;

static sigjmp_buf                   jmpbuf;
static volatile sig_atomic_t        shouldjump = 0;

inline char * demangle_func(const char * funcName) {

	return NULL;
}
void lua_hookhack(lua_State* L, lua_Debug *ar) {
	lua_sethook(L,NULL,0,0);
	lua_pushstring(L,"EPIC FREEZE");
	lua_error(L);
}

int saved_errno=0;
static void ERROR_SIGNAL_HANDLER_FUNC(int sig_nr, siginfo_t* info, void *ucontext) {
	

	bool restored=false;
	struct sigaction isoursih;
	void * caller_address;
	
	char **            messages;
	int                size, i;
	
	
	
	// Ignore current signal
	for(i = 0; signal_handlers[i].type != -1; ++i)
	{
		if(signal_handlers[i].type == sig_nr)
		{		
			
			if (sigaction(sig_nr,/*&signal_handlers[i].sigaction*/NULL,&isoursih) != 0) {
				log("Restoring our signal failed???\n");
			};//.sa_sigaction(code, crash_info, context);
			if (signal_handlers[i].sigaction.sa_sigaction!=isoursih.sa_sigaction) 
			{
				log("WARNING:REGISTERED SIGNAL NOT US\n");
			}
			
			break;
		}
	}

	// SIGUSR1 == watchdog signaled us to break from lua fuckups
	if (sig_nr == SIGUSR1) 
	{
		//log("SIGUSR1:lua_sethook_hack\n");
		//TODO
		//lua_sethook(GLUA,lua_hookhack,LUA_MASKCOUNT,10);
		if (!shouldjump) {
			log("SIGUSR1:nojump\n");
		}
		
		
		shouldjump=0;
		siglongjmp( jmpbuf, -1 );
		log("SHOULD NOT PRINT\n");
		return;
	}
	
	
	
	
}






static int longjmp_dummy(lua_State *L)
{
	lua_pushboolean(L, 1);
	return 1;
}



static int longjmp_sleep2(lua_State *L)
{
	sleep(2);
	lua_pushboolean(L, 1);
	return 1;
}

static bool constructed=false;
class Test
{
   public:
        Test()
       {
              printf("longjmp: TEST: class constructor called! %s\n",constructed?"MEMORY LEAK: NEVER DESTRUCTED!!!!":"");
			  constructed=true;
       }   
 
       ~Test()
          {
              printf("longjmp: TEST: class destructor called!\n");
			  constructed=false;
          }
};


static int longjmp_class_cpp(lua_State *L)
{
	Test t;
	int n=0;
	while (n<2147483641) {
		// testing destructor
		n=n+1;
	};
	printf("%d\n",n);
	lua_pushboolean(L, 1);
	return 1;
}

timer_t killer_timer_id;
static int start_timing(unsigned millis)
{
	
	int status;

	long long nanosecs = 1000000 * millis;


	struct itimerspec timerspec = {
		.it_value = {
			  .tv_sec = nanosecs / 1000000000,
			  .tv_nsec = nanosecs % 1000000000
		}
	};

	status = timer_settime(killer_timer_id, 0, &timerspec, 0);
	if (status == -1)
	return -1;
	return 0;
}

struct itimerspec timerspec_stop = {
	.it_value = {
		  .tv_sec = 0,
		  .tv_nsec = 0
	}
};

static int stop_killer() {
	if (!killer_timer_id) return 0;
	int status;
	status = timer_settime(killer_timer_id, 0, &timerspec_stop, 0);
	if (status==-1) {
		printf("stop_killer %d",errno);
	}
	return status;
}


lua_State old_state;
static int longjmp_timed(lua_State *L)
{
	GLUA=L;
	if (lua_gettop(L) != 1 ||    ! lua_isfunction(L, -1))
		luaL_error(L, "invalid arguments: %d",lua_gettop(L));
	
	// CURSED(how to uncurse, probably not possible: SEH/C++ Exceptions/etc): https://github.com/GaloisInc/galua/blob/c64ae96b78d2d9bda2db1021227f4ef072969dd5/galua/cbits/lua_capi.c#L771
	
	memcpy(&old_state, L, sizeof(lua_State));
	shouldjump=1;
	if (sigsetjmp(jmpbuf,1)) {
		stop_killer();
		shouldjump=0;
		printf("\nlongjmp:longjmp.timed() timed out!\n");
        memcpy(L, &old_state, sizeof(lua_State));
		
		lua_pushboolean(GLUA, 0);
		return 1;
	}
    
		start_timing(80);
		try
		  {
			  
				if (lua_pcall(L, 0, 0, 0)) {
					stop_killer();
					luaL_error(L, "%s", lua_tostring(L, -1));
					return 0;
				}
		  }
		  catch(int64_t x)
		  {
				printf("longjmp: CAUGHT %ld", x);
		  }
		  catch(...)
		  {
				printf("longjmp: CAUGHT SOMETHING");
		  }
				
		stop_killer();
    
	
	shouldjump=0;
	
	lua_pushboolean(L, 1);
	return 1;
}




extern "C" {
		
	int luaopen_longjmp(lua_State *L)
	{
		struct luaL_Reg longjmp[] = {
			/* Database Connection Control Functions */
			{ "class_cpp", longjmp_class_cpp },
			{ "sleep2", longjmp_sleep2 },
			{ "timed", longjmp_timed },
			{ "dummy", longjmp_dummy },
			{ NULL, NULL }
		};
		GLUA=L;




		main_thread = syscall(SYS_gettid);
		void * dummy_trace_array[1];
		unsigned int dummy_trace_size;
		dummy_trace_size = backtrace(dummy_trace_array, 1);

		static char ssp[SIGSTKSZ*2];
		signal_alt_stack.ss_size = SIGSTKSZ*2;
		signal_alt_stack.ss_flags = 0;
		signal_alt_stack.ss_sp = ssp;
		signal_alt_stack.ss_size = SIGSTKSZ*2;
		if (sigaltstack (&signal_alt_stack, 0) < 0) {
			signal_alt_stack.ss_size = 0;
			logf("sigaltstack errno = %d\n", errno);
		}

		static char demanglealloc_mem[DEMANGLE_LEN];
		demanglealloc = demanglealloc_mem;
		demanglealloc[0]=0x00;

		// HANDLERS
		for(unsigned i = 0; signal_handlers[i].type != -1; ++i)
		{
			signal_handlers[i].sigaction.sa_sigaction = ERROR_SIGNAL_HANDLER_FUNC;
			sigemptyset(&signal_handlers[i].sigaction.sa_mask);
			signal_handlers[i].sigaction.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESTART | SA_SIGINFO;

			if(sigaction(signal_handlers[i].type, &signal_handlers[i].sigaction, &signal_handlers[i].old_sigaction) != 0)
			{
				printf("Registering signal handler \"%s\" failed: %s\n", strsignal(signal_handlers[i].type), strerror(errno));
			}
		}

		  struct sigevent event = {
						.sigev_value = {
				.sival_ptr = &killer_timer_id
			},
			.sigev_signo = SIGUSR1,

			.sigev_notify = SIGEV_THREAD_ID,
			
			._sigev_un = {
				._tid = main_thread
			},
			
			
		  };


		  int status;
		 // https://man7.org/linux/man-pages/man2/timer_create.2.html
		 
		  status = timer_create(CLOCK_THREAD_CPUTIME_ID, &event, &killer_timer_id);
		  if (status == -1) {
			  
            lua_pushstring(L, "timer_create");
            lua_error(L);
		  }




	#if LUA_VERSION_NUM >= 502
		luaL_newlib(L, longjmp);
	#else
		luaL_register(L, "longjmp", longjmp);
	#endif






		return 1;
	}

	
	// https://stackoverflow.com/a/24488262
	
	extern void *__cxa_allocate_exception(size_t thrown_size);
	extern void __cxa_throw (void *thrown_exception, void* *tinfo, void (*dest) (void *) );
	extern void * _ZTIl; // typeinfo of long

	void throw_cxa_exception()
	{
	   int64_t * p = (int64_t*)__cxa_allocate_exception(8);
	   *p = 1976;
	   __cxa_throw(p, &_ZTIl, 0);
	  return;
	}
}