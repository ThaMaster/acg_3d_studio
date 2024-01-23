
#pragma once

#include <stdint.h>

#if defined(_MSC_VER) 


#include <intrin.h> 
#pragma intrinsic(__rdtsc) 

#endif



#ifdef __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

// Still supporting compilers without full support for C++11, so cannot use chrono yet.


#ifdef _WIN32

#else
#include <unistd.h>
#endif

namespace vr {

	typedef double Real64;
	#ifdef _WIN32
	typedef unsigned __int64 UInt64;
	#else
	typedef uint64_t UInt64;
	typedef unsigned int UInt;

	#endif
	/**
	The Timer class permits timing execution speed with the same
	refinement as the built in hardware clock of the CPU.

	Profiling tools typically have sampling rates which are too coarse for
	detailed timing analysis. The Timer class uses the hardware clock to
	record beginning and end times of given code sections.
	(For Windows, QueryPerformanceFrequency/QueryPerformanceTimer is used if VR_USE_HPET is set).
	*/
	class Timer
	{
	public:

		/**
		Creates a timer.
		\param startImmediately Should the timer start immediately or start in stopped mode
		(now, but also after consequent calls to reset())?
		*/
		Timer(bool startImmediately = false);

		/**
		Starts the timer.
		Calling start on a already started timer will have no effect.
		*/
		void start();

		/**
		Stops the timer.
		Calling stop on a already stopped timer will have no effect.
		Upon stopping, the amount of cycles and time ran will be updated.
		*/
		void stop();

		/**
		Clear all data.
		*/
		void reset(bool startAfterReset = false);

		/**
		Report total elapsed time since start in milliseconds, excluding
		intervals when the timer was suspended.
		*/
		Real64 getTime() const;

		/// Report execution time in CPU cycles. 
		UInt64 getNumTicks() const;

		/// \return the start tick.
		UInt64 getStartTick() const;

		/// \return true if running.
		bool isRunning() const;

		/// \return raw value of the TSC register (in Windows: QueryPerformanceTimer if VR_USE_HPET is set). 
		static UInt64 getCurrentTick();

		/// \return time in ms relative to a start tick
		static Real64 getTime(UInt64 startTick);

		/// \return the given number of ticks in ms.
		static Real64 convertToMs(UInt64 ticks);

		/// For internal use only.
		static void initClockSpeed();

	private:
		static double clockSpeed();
#if defined(_MSC_VER)
		/// Report execution time in CPU cycles. 
		static UInt64 getCurrentTickWindows();
#endif

	private:


		UInt64 m_start;
		UInt64 m_total;


		bool m_running;

		static double s_clockSpeed;
	};




} // namespace vr 

