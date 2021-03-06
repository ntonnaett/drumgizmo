/* -*- Mode: c++ -*- */
/***************************************************************************
 *            ossmidi.h
 *
 *  Sun May 21 10:52:09 CEST 2017
 *  Copyright 2017 Goran Mekić
 *  meka@tilda.center
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
#include "audioinputenginemidi.h"


class OSSInputEngine
	: public AudioInputEngineMidi
{
public:
	OSSInputEngine();
	~OSSInputEngine();

	// based on AudioInputEngine
	bool init(const Instruments& instruments) override;
	void setParm(const std::string& parm, const std::string& value) override;
	bool start() override;
	void stop() override;
	void pre() override;
	void run(size_t pos, size_t len, std::vector<event_t>& events) override;
	void post() override;
	bool isFreewheeling() const override;

private:
	int fd;
	std::string dev;
	std::size_t pos;
	std::vector<event_t> events;
	std::string midimap_file;
};
