/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            wavfileoutputengine.cc
 *
 *  Sat May 14 13:26:51 CEST 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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
#include "wavfileoutputengine.h"

#include <iostream>

WavfileOutputEngine::WavfileOutputEngine()
	: AudioOutputEngine{}
	, info{}
	, file{"output"}
{
	info.samplerate = 44100;
	info.channels = 1;
	info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
}

WavfileOutputEngine::~WavfileOutputEngine()
{
	sf_close(handle);
}

bool WavfileOutputEngine::init(const Channels& data)
{
	info.channels = data.size();

	handle = sf_open(file.c_str(), SFM_WRITE, &info);
	if(handle == nullptr)
	{
		std::cerr << "[WavfileOutputEngine] Failed to open "
		          << file << " for writing.\n";
		return false;
	}

	return true;
}

void WavfileOutputEngine::setParm(const std::string& parm, const std::string& value)
{
	if(parm == "file")
	{
		// apply output filename prefix
		file = value;
	}
	else if(parm == "srate")
	{
		// try to apply samplerate
		try
		{
			info.samplerate = std::stoi(value);
		}
		catch(...)
		{
			std::cerr << "[WavfileOutputEngine] Invalid samplerate " << value
			          << "\n";
		}
	}
	else
	{
		std::cerr << "[WavfileOutputEngine] Unsupported parameter '" << parm
		          << "'\n";
	}
}

bool WavfileOutputEngine::start()
{
	return true;
}

void WavfileOutputEngine::stop()
{
}

void WavfileOutputEngine::pre(size_t nsamples)
{
}

void WavfileOutputEngine::run(int ch, sample_t* samples, size_t nsamples)
{
	if(ch >= info.channels)
	{
		std::cerr << "[WavfileOutputEngine] cannot access channel #" << ch
		          << " (" << info.channels << " channels available)\n";
		return;
	}

	for(std::size_t i = 0; i < nsamples; ++i)
	{
		buffer[i * info.channels + ch] = samples[i];
	}
}

void WavfileOutputEngine::post(size_t nsamples)
{

	sf_writef_float(handle, buffer, nsamples);
}

size_t WavfileOutputEngine::getSamplerate() const
{
	return info.samplerate;
}

bool WavfileOutputEngine::isFreewheeling() const
{
	return true;
}
