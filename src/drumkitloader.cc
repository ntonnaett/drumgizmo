/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkitloader.cc
 *
 *  Thu Jan 17 20:54:14 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "drumkitloader.h"

#include <iostream>

#include <hugin.hpp>

#include "drumgizmo.h"
#include "audioinputenginemidi.h"
#include "DGDOM.h"
#include "dgxmlparser.h"
#include "path.h"
#include "domloader.h"
#include "directory.h"

#define REFSFILE "refs.conf"

DrumKitLoader::DrumKitLoader(Settings& settings, DrumKit& kit,
                             AudioInputEngine& ie,
                             Random& rand,
                             AudioCache& audio_cache)
	: settings(settings)
	, getter(settings)
	, kit(kit)
	, ie(ie)
	, rand(rand)
	, audio_cache(audio_cache)
{
	logger =
		[&](LogLevel level, const std::string& msg)
		{
			std::string message;
			switch(level)
			{
			case LogLevel::Info:
				//message = "[Info]";
				//break;
				return; // Ignore info level messages
			case LogLevel::Warning:
				message = "[Warning]";
				break;
			case LogLevel::Error:
				message = "[Error]";
				break;
			}
			message += " " + msg + "\n";
			std::string status = settings.load_status_text.load();
			status += message;
			settings.load_status_text.store(status);
		};
}

DrumKitLoader::~DrumKitLoader()
{
	assert(!running);
}

void DrumKitLoader::init()
{
	run();
	run_semaphore.wait(); // Wait for the thread to actually start.
}

void DrumKitLoader::deinit()
{
	if(running)
	{
		framesize_semaphore.post();

		{
			std::lock_guard<std::mutex> guard(mutex);
			load_queue.clear();
		}

		running = false;
		semaphore.post();
		wait_stop();
	}
}

bool DrumKitLoader::loadkit(const std::string& file)
{
	settings.drumkit_load_status.store(LoadStatus::Idle);

	settings.number_of_files_loaded.store(0);

	if(file == "")
	{
		if (getter.reload_counter.getValue() != 0)
		{
			settings.drumkit_load_status.store(LoadStatus::Error);

			// Show a full bar
			settings.number_of_files.store(1);
			settings.number_of_files_loaded.store(1);
		}

		return false;
	}

	DEBUG(drumgizmo, "loadkit(%s)\n", file.c_str());

	// Remove all queue AudioFiles from loader before we actually delete them.
	skip();

	// Delete all Channels, Instruments, Samples and AudioFiles.
	kit.clear();

	// Clear metadata from previously loaded drumkit
	settings.drumkit_name.store("");
	settings.drumkit_description.store("");
	settings.drumkit_version.store("");
	settings.drumkit_samplerate.store(44100);
	settings.load_status_text.store("");

	settings.drumkit_load_status.store(LoadStatus::Parsing);

	// Parse drumkit and instrument xml

	settings.has_bleed_control.store(false);

	ConfigFile refs(REFSFILE);

	auto edited_filename(file);

	if(refs.load())
	{
		if((file.size() > 1) && (file[0] == '@'))
		{
			edited_filename = refs.getValue(file.substr(1));
		}
	}
	else
	{
		WARN(drumkitparser, "Error reading refs.conf");
	}

	DrumkitDOM drumkitdom;
	std::vector<InstrumentDOM> instrumentdoms;
	std::string path = getPath(edited_filename);
	bool parseerror = false;
	bool ret = parseDrumkitFile(edited_filename, drumkitdom, logger);
	if(!ret)
	{
		WARN(drumkitloader, "Drumkit file parser error: '%s'",
		     edited_filename.data());
	}

	parseerror |= !ret;

	for(const auto& ref : drumkitdom.instruments)
	{
		instrumentdoms.emplace_back();
		bool ret = parseInstrumentFile(path + "/" + ref.file, instrumentdoms.back(),
		                               logger);
		if(!ret)
		{
			WARN(drumkitloader, "Instrument file parser error: '%s'",
			     edited_filename.data());
		}

		parseerror |= !ret;
	}

	DOMLoader domloader(settings, rand);
	ret = domloader.loadDom(path, drumkitdom, instrumentdoms, kit, logger);
	if(!ret)
	{
		WARN(drumkitloader, "DOMLoader error");
	}
	parseerror |= !ret;
	if(parseerror)
	{
		ERR(drumgizmo, "Drumkit parser failed: %s\n", file.c_str());
		settings.drumkit_load_status.store(LoadStatus::Error);

		// Show a full bar
		settings.number_of_files.store(1);
		settings.number_of_files_loaded.store(1);

		return false;
	}

	// Check if there is enough free RAM to load the drumkit.
	//if(!memchecker.enoughFreeMemory(kit))
	//{
	//	printf("WARNING: "
	//	       "There doesn't seem to be enough RAM available to load the kit.\n"
	//	       "Trying to load it anyway...\n");
	//}

	// Put some information about the kit into the settings
	settings.drumkit_name = kit.getName();
	settings.drumkit_description = kit.getDescription();
	settings.drumkit_version = kit.getVersion();
	settings.drumkit_samplerate = kit.getSamplerate();
	// only load the default midi map if there is one and no midimap is selected yet
	if (drumkitdom.metadata.default_midimap_file != "" && settings.midimap_file == "")
	{
		const std::string drumkit_path = Directory::pathDirectory(settings.drumkit_file);
		settings.midimap_file = drumkit_path + "/" + drumkitdom.metadata.default_midimap_file;
	}

	// Done parsing. Now start loading the audio
	settings.midimap_load_status.store(LoadStatus::Loading);

	loadKitAudio(kit);

	DEBUG(loadkit, "loadkit: Success\n");

	return true;
}

void DrumKitLoader::loadKitAudio(const DrumKit& kit)
{
//	std::lock_guard<std::mutex> guard(mutex);

	DEBUG(loader, "Create AudioFile queue from DrumKit\n");

	auto cache_limit = settings.disk_cache_upper_limit.load();
	auto cache_enable = settings.disk_cache_enable.load();

	DEBUG(loader, "cache_enable: %s\n", cache_enable?"true":"false");

	auto number_of_files = kit.getNumberOfFiles();
	if(cache_enable && number_of_files > 0)
	{
		auto cache_limit_per_file = cache_limit / number_of_files;

		assert(framesize != 0);

		preload_samples = cache_limit_per_file / sizeof(sample_t);

		if(preload_samples < 4096)
		{
			preload_samples = 4096;
		}

		DEBUG(loader, "cache_limit: %lu, number_of_files: %lu,"
		      " cache_limit_per_file: %lu, preload_samples: %lu\n",
		      (unsigned long)cache_limit,
		      (unsigned long)number_of_files,
		      (unsigned long)cache_limit_per_file,
		      (unsigned long)preload_samples);
	}
	else
	{
		preload_samples = std::numeric_limits<std::size_t>::max();
	}

	settings.number_of_files_loaded.store(0);

	// Count total number of files that need loading:
	settings.number_of_files.store(0);
	for(const auto& instr_ptr: kit.instruments)
	{
		settings.number_of_files.fetch_add(instr_ptr->audiofiles.size());
	}

	// Now actually queue them for loading:
	for(const auto& instr_ptr: kit.instruments)
	{
		for(auto& audiofile: instr_ptr->audiofiles)
		{
			load_queue.push_back(audiofile.get());
		}
	}

	DEBUG(loader, "Queued %d (size: %d) AudioFiles for loading.\n",
	      (int)settings.number_of_files.load(), (int)load_queue.size());

	audio_cache.updateChunkSize(kit.channels.size());

	semaphore.post(); // Start loader loop.
}

void DrumKitLoader::skip()
{
	std::lock_guard<std::mutex> guard(mutex);
	load_queue.clear();
}

void DrumKitLoader::setFrameSize(std::size_t framesize)
{
	std::lock_guard<std::mutex> guard(mutex);
	this->framesize = framesize;
	framesize_semaphore.post(); // Signal that the framesize has been set.
}

void DrumKitLoader::thread_main()
{
	running = true;

	run_semaphore.post(); // Signal that the thread has been started.

	framesize_semaphore.wait(); // Wait until the framesize has been set.

	while(running)
	{
		std::size_t size;
		{
			std::lock_guard<std::mutex> guard(mutex);
			size = load_queue.size();
		}

		// Only sleep if queue is empty.
		if(size == 0)
		{
			semaphore.wait(std::chrono::milliseconds(10));
		}

		bool newKit = false;
		if(getter.drumkit_file.hasChanged() ||
		   getter.reload_counter.hasChanged())
		{
			loadkit(getter.drumkit_file.getValue());
			newKit = true;
		}

		if(getter.midimap_file.hasChanged() || newKit)
		{
			auto ie_midi = dynamic_cast<AudioInputEngineMidi*>(&ie);

			// if there's a midi engine and this is not just the default midimap
			// name which is set.
			if(ie_midi && (getter.midimap_file.getValue() != "" ||
						   getter.reload_counter.getValue() != 0))
			{
				settings.midimap_load_status.store(LoadStatus::Loading);
				bool ret = ie_midi->loadMidiMap(getter.midimap_file.getValue(),
				                                kit.instruments);
				if(ret)
				{
					settings.midimap_load_status.store(LoadStatus::Done);
				}
				else
				{
					settings.midimap_load_status.store(LoadStatus::Error);
				}
			}
		}

		std::string filename;
		{
			std::lock_guard<std::mutex> guard(mutex);

			if(load_queue.size() == 0)
			{
				continue;
			}

			AudioFile *audiofile = load_queue.front();
			load_queue.pop_front();
			filename = audiofile->filename;
			try
			{
				audiofile->load(logger, preload_samples);
			}
			catch(std::bad_alloc&)
			{
				settings.drumkit_load_status.store(LoadStatus::Error);
				load_queue.clear();
			}
		}

		settings.number_of_files_loaded.fetch_add(1);

		if(settings.number_of_files.load() == settings.number_of_files_loaded.load())
		{
			settings.drumkit_load_status.store(LoadStatus::Done);
		}
	}

	DEBUG(loader, "Loader thread finished.");
}
