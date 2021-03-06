/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            cachemanagertest.cc
 *
 *  Sun Apr 19 10:15:59 CEST 2015
 *  Copyright 2015 Bent Bisballe Nyeng
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
#include <uunit.h>

#include <thread>
#include <chrono>

#include <audiofile.h>
#include <audiocache.h>
#include <settings.h>

#include "drumkit_creator.h"

#define FRAMESIZE 64

class AudioCacheTest
	: public uUnit
{
public:
	AudioCacheTest()
	{
		uUNIT_TEST(AudioCacheTest::singleChannelNonThreaded);
		uUNIT_TEST(AudioCacheTest::singleChannelThreaded);
		uUNIT_TEST(AudioCacheTest::multiChannelNonThreaded);
		uUNIT_TEST(AudioCacheTest::multiChannelThreaded);
	}

	DrumkitCreator drumkit_creator;

	//! Test runner.
	//! \param filename The name of the file to read.
	//! \param channel The channel number to do comparison on.
	//! \param thread Control if this test is running in threaded mode or not.
	//! \param framesize The initial framesize to use.
	void testHelper(const char* filename, int channel, bool threaded,
	                int framesize, int num_channels)
	{
		// Reference file:
		AudioFile audio_file_ref(filename, channel);
		printf("audio_file_ref.load\n");
		audio_file_ref.load(nullptr);

		// Input file:
		AudioFile audio_file(filename, channel);
		printf("audio_file.load\n");
		audio_file.load(nullptr, 4096);

		Settings settings;
		AudioCache audio_cache(settings);
		printf("audio_cache.init\n");
		audio_cache.init(100);
		audio_cache.setAsyncMode(threaded);

		// Set initial (upper limit) framesize
		audio_cache.setFrameSize(framesize);
		audio_cache.updateChunkSize(num_channels);

		cacheid_t id;

		for(size_t initial_samples_needed = 0;
		    initial_samples_needed < (size_t)(framesize + 1);
		    ++initial_samples_needed)
		{

			printf("open: initial_samples_needed: %d\n", (int)initial_samples_needed);
			sample_t *samples =
				audio_cache.open(audio_file, initial_samples_needed, channel, id);
			size_t size = initial_samples_needed;
			size_t offset = 0;

			// Test pre cache:
			for(size_t i = 0; i < size; ++i)
			{
				uUNIT_ASSERT_EQUAL(audio_file_ref.data[offset], samples[i]);
				++offset;
			}

			// Test the rest
			while(offset < audio_file_ref.size)
			{
				if(threaded)
				{
					// Wait until we are finished reading
					int timeout = 1000;
					while(!audio_cache.isReady(id))
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
						if(--timeout == 0)
						{
							uUNIT_ASSERT(false); // timeout
						}
					}
				}

				size = framesize;
				samples = audio_cache.next(id, size);

				uUNIT_ASSERT_EQUAL(std::size_t(0), settings.number_of_underruns.load());

				for(size_t i = 0; (i < size) && (offset < audio_file_ref.size); ++i)
				{
					if(audio_file_ref.data[offset] != samples[i])
					{
						printf("-----> offset: %d, size: %d, diff: %d,"
						       " i: %d, size: %d, block-diff: %d\n",
						       (int)offset, (int)audio_file_ref.size,
						       (int)(audio_file_ref.size - offset),
						       (int)i, (int)size, (int)(size - i));
					}
					uUNIT_ASSERT_EQUAL(audio_file_ref.data[offset], samples[i]);
					++offset;
				}
			}

			audio_cache.close(id);
		}

		printf("done\n");
	}

	void singleChannelNonThreaded()
	{
		printf("\nsinglechannel_nonthreaded()\n");

		auto filename = drumkit_creator.createSingleChannelWav("single_channel.wav");

		// Conduct test
		int channel = 0;
		bool threaded = false;
		int num_channels = 1;
		testHelper(filename.c_str(), channel, threaded, FRAMESIZE, num_channels);
	}

	void singleChannelThreaded()
	{
		printf("\nsinglechannel_threaded()\n");

		auto filename = drumkit_creator.createSingleChannelWav("single_channel.wav");

		// Conduct test
		int channel = 0;
		bool threaded = true;
		int num_channels = 1;
		testHelper(filename.c_str(), channel, threaded, FRAMESIZE, num_channels);
	}

	void multiChannelNonThreaded()
	{
		printf("\nmultichannel_nonthreaded()\n");

		auto filename = drumkit_creator.createMultiChannelWav("multi_channel.wav");

		// Conduct test
		int channel = 0;
		bool threaded = false;
		int num_channels = 13;
		testHelper(filename.c_str(), channel, threaded, FRAMESIZE, num_channels);
		++channel;
		testHelper(filename.c_str(), channel, threaded, FRAMESIZE, num_channels);
	}

	void multiChannelThreaded()
	{
		printf("\nmultichannel_threaded()\n");

		auto filename = drumkit_creator.createMultiChannelWav("multi_channel.wav");

		// Conduct test
		int channel = 0;
		bool threaded = true;
		int num_channels = 13;
		testHelper(filename.c_str(), channel, threaded, FRAMESIZE, num_channels);
		++channel;
		testHelper(filename.c_str(), channel, threaded, FRAMESIZE, num_channels);
	}

};

// Registers the fixture into the 'registry'
static AudioCacheTest test;
