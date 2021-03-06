/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            sem.cc
 *
 *  Sat Oct  8 17:44:13 CEST 2005
 *  Copyright  2005 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "sem.h"

#include <hugin.hpp>
#include <limits>
#include <assert.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "platform.h"

#if DG_PLATFORM != DG_PLATFORM_WINDOWS
#include <semaphore.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#endif

#if DG_PLATFORM == DG_PLATFORM_OSX
//#include <Multiprocessing.h>
#include <CoreServices/CoreServices.h>
#endif

struct semaphore_private_t {
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	HANDLE semaphore;
#elif DG_PLATFORM == DG_PLATFORM_OSX
	MPSemaphoreID semaphore;
#else
	sem_t semaphore;
#endif
};

Semaphore::Semaphore(std::size_t initial_count)
{
	prv = new struct semaphore_private_t();

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	prv->semaphore = CreateSemaphore(nullptr,  // default security attributes
	                                 initial_count,
	                                 std::numeric_limits<LONG>::max(),
	                                 nullptr); // unnamed semaphore
#elif DG_PLATFORM == DG_PLATFORM_OSX
	MPCreateSemaphore(std::numeric_limits<std::uint32_t>::max(),
	                  initial_count,
	                  &prv->semaphore);
#else
	const int pshared = 0;
	memset(&prv->semaphore, 0, sizeof(sem_t));
	sem_init(&prv->semaphore, pshared, initial_count);
#endif
}

Semaphore::~Semaphore()
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	CloseHandle(prv->semaphore);
#elif DG_PLATFORM == DG_PLATFORM_OSX
	MPDeleteSemaphore(prv->semaphore);
#else
	sem_destroy(&prv->semaphore);
#endif

	delete prv;
}

void Semaphore::post()
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	ReleaseSemaphore(prv->semaphore, 1, nullptr);
#elif DG_PLATFORM == DG_PLATFORM_OSX
	MPSignalSemaphore(prv->semaphore);
#else
	sem_post(&prv->semaphore);
#endif
}

bool Semaphore::wait(const std::chrono::milliseconds& timeout)
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	DWORD ret = WaitForSingleObject(prv->semaphore, timeout.count());
	if(ret == WAIT_TIMEOUT)
	{
		return false;
	}

	assert(ret == WAIT_OBJECT_0);
#elif DG_PLATFORM == DG_PLATFORM_OSX
	auto ret = MPWaitOnSemaphore(prv->semaphore,
	                             kDurationMillisecond * timeout.count());
	if(ret == kMPTimeoutErr)
	{
		return false;
	}
#else
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	// Add timeout
	time_t seconds = (time_t)(timeout.count() / 1000);
	ts.tv_sec += seconds;
	ts.tv_nsec += (long)((timeout.count() % 1000) * 1000000);

	// Make sure we don't overflow the nanoseconds.
	constexpr long nsec = 1000000000LL;
	if(ts.tv_nsec >= nsec)
	{
		ts.tv_nsec -= nsec;
		ts.tv_sec += 1;
	}

again:
	int ret = sem_timedwait(&prv->semaphore, &ts);
	if(ret < 0)
	{
		if(errno == EINTR)
		{
			// The timed wait were interrupted prematurely so we need to wait some
			// more. To prevent an infinite loop-like behaviour we make a short sleep.
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			goto again;
		}

		if(errno == ETIMEDOUT)
		{
			return false;
		}

		perror("sem_timedwait()");
		assert(false);
	}
#endif

	return true;
}

void Semaphore::wait()
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	WaitForSingleObject(prv->semaphore, INFINITE);
#elif DG_PLATFORM == DG_PLATFORM_OSX
	MPWaitOnSemaphore(prv->semaphore, kDurationForever);
#else
again:
	int ret = sem_wait(&prv->semaphore);
	if(ret < 0)
	{
		if(errno == EINTR)
		{
			// The wait were interrupted prematurely so we need to wait again
			// To prevent an infinite loop-like behaviour we make a short sleep.
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			goto again;
		}

		perror("sem_wait()");
		assert(false);
	}
#endif
}
