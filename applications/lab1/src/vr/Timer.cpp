
#include <vr/Timer.h>
#include <iostream>
#include <stdio.h>

#ifdef _WIN32
#pragma comment(lib, "PowrProf.lib") 
#include <windows.h>
#include <PowrProf.h>

#else
#include <time.h>
#endif

using namespace vr;

double Timer::s_clockSpeed = -1;

struct Initializer
{
	Initializer()
	{
		Timer::initClockSpeed();
	}
};

Initializer init;


#ifdef __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#ifdef _WIN32
#include <PowrProf.h>
#else
#include <unistd.h>
#endif



#ifdef _WIN32
typedef struct _PROCESSOR_POWER_INFORMATION {
	ULONG Number;
	ULONG MaxMhz;
	ULONG CurrentMhz;
	ULONG MhzLimit;
	ULONG MaxIdleState;
	ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION;
#endif

namespace
{
	static unsigned int readNumCpu()
	{
#if (defined _WIN32)
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);

		return (unsigned int)sysInfo.dwNumberOfProcessors;

#elif (defined __APPLE__)
		uint64_t tmp = 0;
		size_t s = sizeof(tmp);
		sysctlbyname("hw.activecpu", &tmp, &s, NULL, 0);
		return (UInt)(tmp <= 0 ? 1 : tmp);
#else
		// linux, if failure, assume 1 cpu
		long tmp = sysconf(_SC_NPROCESSORS_ONLN);
		return (UInt)(tmp <= 0 ? 1 : tmp);
#endif
	}



	static unsigned int readCpuFrequency()
	{
#ifdef _MSC_VER
		unsigned long value;
		// structure definition was accidentally not included in WinNT.h
		// see: http://msdn2.microsoft.com/en-us/library/aa373184.aspx


		unsigned int numCpu = readNumCpu();
		PROCESSOR_POWER_INFORMATION *ppi = new PROCESSOR_POWER_INFORMATION[numCpu];
		CallNtPowerInformation(ProcessorInformation, NULL, 0, &(ppi[0]), (ULONG)sizeof(PROCESSOR_POWER_INFORMATION)*ULONG(numCpu));
		value = ppi[0].MaxMhz;
		delete[] ppi;
		return (unsigned int)value;

#elif defined(__APPLE__)
		uint64_t tmp = 0;
		size_t s = sizeof(tmp);
		sysctlbyname("hw.cpufrequency", &tmp, &s, NULL, 0);
		return (UInt)((tmp <= 0 ? 0 : tmp) / 1000000.0);
#else
		float cs = 0;
		int done = EOF;
		FILE *fp = fopen("/proc/cpuinfo", "r");
		char buff[1024];
		if (fp) {
			do {
				char* res = fgets(buff, 1024, fp);
				if (res)
					done = sscanf(buff, "cpu MHz  : %f", &cs);
			} while (done == 0 && done != EOF);
			fclose(fp);
		}

		return done != EOF ? (UInt)cs : 0;
#endif
	}



	unsigned int getCpuFrequency()
	{
		static unsigned int s_cpuFrequency = -1;

		if (s_cpuFrequency == -1)
			s_cpuFrequency = readCpuFrequency();

		return s_cpuFrequency;
	}

}


void Timer::initClockSpeed()
{

#ifdef AGX_APPLE_IOS
	mach_timebase_info_data_t timebase;
	mach_timebase_info(&timebase);

	s_clockSpeed = (double)timebase.numer / (double)timebase.denom / double(1000000.0);

#elif defined( __linux__ )

	uint64_t t1, t2;
	uint32_t ms = 10;
	int sleepOk = -1;

	struct timespec c1, c2;

	while (sleepOk != 0)
	{
		struct timespec ts;

		ts.tv_sec = 0;
		ts.tv_nsec = ms * 1000 * 1000;

		clock_gettime(CLOCK_MONOTONIC, &c1);
		t1 = vr::Timer::getCurrentTick();

		sleepOk = nanosleep(&ts, NULL);

		clock_gettime(CLOCK_MONOTONIC, &c2);
		t2 = vr::Timer::getCurrentTick();
	}

	float ms2 = (float)((double)(c2.tv_nsec - c1.tv_nsec) / 1000000.0);
	if (ms2 < 0)
		ms2 += 1000;

	s_clockSpeed = ms2 / ((float)(t2 - t1));

#elif defined(_MSC_VER)
//	LARGE_INTEGER frequency;
//	QueryPerformanceFrequency(&frequency);
//	s_clockSpeed = double(1000.0) / (double)(frequency.QuadPart);
//#else
	unsigned int cpuFrequency = getCpuFrequency();

	if (cpuFrequency != 0)
	{
		s_clockSpeed = double(1.0) / (double)(1000 * cpuFrequency);
	}
	else
	{
		std::cerr << "Could not determine CPU speed, Timer will not function correctly." << std::endl;
		s_clockSpeed = 1.0;
	}
#endif

}


#if defined(_MSC_VER)
UInt64 Timer::getCurrentTickWindows()
{

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	UInt64 retVal = currentTime.QuadPart;
	return retVal;
}

#endif



/* Implementation */
Timer::Timer(bool startImmediately)
{
	reset();
	if (startImmediately)
		this->start();
}


void Timer::start()
{
	if (m_running)
		return;

	m_running = true;

#if AGX_USE_CPP11_DISABLED
	m_start = std::high_resolution_clock::now();
#else
	m_start = getCurrentTick();
#endif
}


void Timer::stop()
{
	if (m_running) {
		m_running = false;


		UInt64 now = getCurrentTick();
		m_total += (now - m_start);

	}
}


void Timer::reset(bool startAfterReset)
{
	m_start = 0L;
	m_total = 0L;

	m_running = false;
	if (startAfterReset)
		this->start();
}


UInt64 Timer::getNumTicks() const
{
	return m_total;
}


UInt64 Timer::getStartTick() const
{
	return m_start;
}


bool Timer::isRunning() const
{
	return m_running;
}


Real64 Timer::getTime() const
{

	return m_running ? (clockSpeed() * (Real64)(getCurrentTick() - m_start + m_total)) : clockSpeed() * (Real64)m_total;

}


double Timer::clockSpeed()
{
	return s_clockSpeed;
}


UInt64 Timer::getCurrentTick()
{
#if defined(_MSC_VER)

	const UInt64 retVal = __rdtsc();

	return retVal;
#elif defined(AGX_APPLE_IOS)
	return mach_absolute_time();
#elif defined (__GNUC__)  && ! defined(__INTEL_COMPILER) && defined(__ia64__)
	unsigned long long int val;
	__asm__ __volatile__("mov %0=ar.itc" : "=r"(val) :: "memory");
	return(val);
#elif defined(__INTEL_COMPILER) && defined(__ia64__) && !defined(HAVE_TICK_COUNTER)
	return __getReg(_IA64_REG_AR_ITC);
#elif defined(__ECC) && defined(__ia64__) && !defined(HAVE_TICK_COUNTER)
	return __getReg(_IA64_REG_AR_ITC);
#elif (defined(__GNUC__) || defined(__ICC)) && (defined(__i386__) || defined(__x86_64__))  && !defined(HAVE_TICK_COUNTER)

	// "The CPUID instruction serializes the processor pipeline so that all of the preceding 
	// instructions must retire before it begins execution.  ...  This is thought to provide 
	//a more accurate cycle count on the code being measured."  [intel person at forum on intel.com]

	// cpuid writes to eax,ebx,ecx, edx, tell g++ about registers being clobbered
	// in 64bits ebx is 32bits of the larger rbx register so should not require
	// any ifdef's in 64bit
	// would be better to use pushad, cpuid, popad but for some unknow reason gcc
	// doesn't recognize pushad/popad
	// Update: gcc has different name for pushad/popad, l postfix instead of d,
	// but those instructions doesn't work in 64bit

#ifdef __LP64__
	unsigned long long int lowbits, highbits;
	__asm__ __volatile__("rdtsc": "=a" (lowbits), "=d"(highbits) : : "ebx", "ecx");
#else
	unsigned long int lowbits, highbits;
	__asm__ __volatile__("pushal\n\t" "cpuid\n\t" "popal\n\t" "rdtsc": "=a" (lowbits), "=d"(highbits));
	//__asm__ __volatile__("push %%ebx\n" "push %%ecx\n" "cpuid\n" "rdtsc\n" "pop %%ebx\n" "pop %%ecx": "=a" (lowbits), "=d"(highbits));
#endif

	return (((unsigned long long int)highbits << 32) | (unsigned long long int)lowbits);

#else  
	agxAbort1("Timer::getCurrentTick failed!");
	return -1;
#endif

}

Real64 Timer::getTime(UInt64 startTick)
{
	return Timer::convertToMs(Timer::getCurrentTick() - startTick);
}

Real64 Timer::convertToMs(UInt64 ticks)
{
	return clockSpeed() * (Real64)ticks;
}
