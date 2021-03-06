/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            compareoutputengine.h
 *
 *  Sat May 14 13:27:04 CEST 2016
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
#pragma once

#include <sndfile.h>
#include <audiooutputengine.h>

class CompareOutputEngine
	: public AudioOutputEngine
{
public:
	CompareOutputEngine();
	~CompareOutputEngine();

	// based on AudioOutputEngine
	bool init(const Channels& data) override;
	void setParm(const std::string& parm, const std::string& value) override;
	bool start() override;
	void stop() override;
	void pre(size_t nsamples) override;
	void run(int ch, sample_t* samples, size_t nsamples) override;
	void post(size_t nsamples) override;
	size_t getSamplerate() const override;
	bool isFreewheeling() const override;

private:
	SF_INFO info;
	SNDFILE* handle;
	std::string file;
	sample_t buffer[4096 * 16];
	std::size_t diff_samples{0};
	sample_t ref_buffer[sizeof(buffer) / sizeof(sample_t)];
	size_t pos{0};
};
