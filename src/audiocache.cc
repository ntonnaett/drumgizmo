/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiocache.cc
 *
 *  Fri Apr 10 10:39:24 CEST 2015
 *  Copyright 2015 Jonas Suhr Christensen
 *  jsc@umbraculum.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "audiocache.h"

#include <mutex>

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <hugin.hpp>

#include "audiocachefile.h"

#define CHUNKSIZE(x) (x * CHUNK_MULTIPLIER)

AudioCache::~AudioCache()
{

	// TODO: Run through all active cacheids and release them/close their files.

	deinit();
	delete[] nodata;
}

void AudioCache::init(size_t poolsize)
{
	setAsyncMode(true);

	idManager.init(poolsize);
	eventHandler.start();
}

void AudioCache::deinit()
{
	eventHandler.stop();
}

// Invariant: initial_samples_needed < preloaded audio data
sample_t* AudioCache::open(AudioFile* file, size_t initial_samples_needed,
                           int channel, cacheid_t& id)
{
	if(!file->isValid())
	{
		// File preload not yet ready - skip this sample.
		id = CACHE_DUMMYID;
		assert(nodata);
		return nodata;
	}

	// Register a new id for this cache session.
	id = idManager.registerID({});

	// If we are out of available ids we get CACHE_DUMMYID
	if(id == CACHE_DUMMYID)
	{
		// Use nodata buffer instead.
		assert(nodata);
		return nodata;
	}

	// Get the cache_t connected with the registered id.
	cache_t& c = idManager.getCache(id);

	c.afile = &eventHandler.openFile(file->filename);
	c.channel = channel;

	// Next call to 'next()' will read from this point.
	c.localpos = initial_samples_needed;

	c.front = nullptr; // This is allocated when needed.
	c.back = nullptr; // This is allocated when needed.

	// cropped_size is the preload chunk size cropped to sample length.
	size_t cropped_size = file->preloadedsize - c.localpos;
	cropped_size /= framesize;
	cropped_size *= framesize;
	cropped_size += initial_samples_needed;

	if(file->preloadedsize == file->size)
	{
		// We have preloaded the entire file, so use it.
		cropped_size = file->preloadedsize;
	}

	c.preloaded_samples = file->data;
	c.preloaded_samples_size = cropped_size;

	// Next read from disk will read from this point.
	c.pos = cropped_size;//c.preloaded_samples_size;

	// Only load next buffer if there are more data in the file to be loaded...
	if(c.pos < file->size)
	{
		if(c.back == nullptr)
		{
			c.back = new sample_t[CHUNKSIZE(framesize)];
		}

		eventHandler.pushLoadNextEvent(c.afile, c.channel, c.pos, c.back, &c.ready);
	}

	return c.preloaded_samples; // return preloaded data
}

sample_t* AudioCache::next(cacheid_t id, size_t& size)
{
	size = framesize;

	if(id == CACHE_DUMMYID)
	{
		assert(nodata);
		return nodata;
	}

	cache_t& c = idManager.getCache(id);

	if(c.preloaded_samples)
	{

		// We are playing from memory:
		if(c.localpos < c.preloaded_samples_size)
		{
			sample_t* s = c.preloaded_samples + c.localpos;
			c.localpos += framesize;
			return s;
		}

		c.preloaded_samples = nullptr; // Start using samples from disk.

	}
	else
	{

		// We are playing from cache:
		if(c.localpos < CHUNKSIZE(framesize))
		{
			sample_t* s = c.front + c.localpos;
			c.localpos += framesize;
			return s;
		}
	}

	// Check for buffer underrun
	if(!c.ready)
	{
		// Just return silence.
		++numberOfUnderruns;
		return nodata;
	}

	// Swap buffers
	std::swap(c.front, c.back);

	// Next time we go here we have already read the first frame.
	c.localpos = framesize;

	c.pos += CHUNKSIZE(framesize);

	// Does the file have remaining unread samples?
	if(c.pos < c.afile->getSize())
	{
		// Do we have a back buffer to read into?
		if(c.back == nullptr)
		{
			c.back = new sample_t[CHUNKSIZE(framesize)];
		}

		eventHandler.pushLoadNextEvent(c.afile, c.channel, c.pos, c.back, &c.ready);
	}

	// We should always have a front buffer at this point.
	assert(c.front);

	return c.front;
}

bool AudioCache::isReady(cacheid_t id)
{
	if(id == CACHE_DUMMYID)
	{
		return true;
	}

	cache_t& cache = idManager.getCache(id);
	return cache.ready;
}

void AudioCache::close(cacheid_t id)
{
	if(id == CACHE_DUMMYID)
	{
		return;
	}

	eventHandler.pushCloseEvent(id);
}

void AudioCache::setFrameSize(size_t framesize)
{
	// Make sure the event handler thread is stalled while we set the framesize
	// state.
	std::lock_guard<AudioCacheEventHandler> eventHandlerLock(eventHandler);

	// NOTE: Not threaded...
	//std::lock_guard<AudioCacheIDManager> idManagerLock(idManager);

	if(framesize > this->framesize)
	{
		delete[] nodata;
		nodata = new sample_t[framesize];

		for(size_t i = 0; i < framesize; ++i)
		{
			nodata[i] = 0.0f;
		}
	}

	this->framesize = framesize;

	eventHandler.setChunkSize(CHUNKSIZE(framesize));
}

size_t AudioCache::frameSize() const
{
	return framesize;
}

void AudioCache::setAsyncMode(bool async)
{
	// TODO: Clean out read queue.
	// TODO: Block until reader thread is idle, otherwise we might screw up the
	// buffers...?
	eventHandler.setThreaded(async);
}

bool AudioCache::asyncMode() const
{
	return eventHandler.getThreaded();
}

size_t AudioCache::getNumberOfUnderruns() const
{
	return numberOfUnderruns;
}

void AudioCache::resetNumberOfUnderruns()
{
	numberOfUnderruns = 0;
}