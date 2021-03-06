/* -*- Mode: c++ -*- */
/***************************************************************************
 *            dgxmlparser.h
 *
 *  Fri Jun  8 22:04:32 CEST 2018
 *  Copyright 2018 Jonas Suhr Christensen
 *  jsc@umbraculum.org
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

#include <string>
#include <functional>

#include "DGDOM.h"
#include "logger.h"

bool probeDrumkitFile(const std::string& filename, LogFunction logger = nullptr);
bool probeInstrumentFile(const std::string& filename, LogFunction logger = nullptr);

bool parseDrumkitFile(const std::string& filename, DrumkitDOM& dom, LogFunction logger = nullptr);
bool parseInstrumentFile(const std::string& filename, InstrumentDOM& dom, LogFunction logger = nullptr);
