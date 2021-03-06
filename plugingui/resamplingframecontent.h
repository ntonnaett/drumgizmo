/* -*- Mode: c++ -*- */
/***************************************************************************
 *            resamplingframecontent.h
 *
 *  Fri May  5 23:43:28 CEST 2017
 *  Copyright 2017 André Nusser
 *  andre.nusser@googlemail.com
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

#include <dggui/widget.h>
#include <dggui/textedit.h>
#include <dggui/knob.h>
#include <dggui/label.h>

class SettingsNotifier;
struct Settings;

namespace GUI
{

class ResamplingframeContent
	: public dggui::Widget
{
public:
	ResamplingframeContent(dggui::Widget* parent,
	                       Settings& settings,
	                       SettingsNotifier& settings_notifier);

	// From Widget
	virtual void resize(std::size_t width, std::size_t height) override;

private:
	void updateContent();

	void updateDrumkitSamplerate(std::size_t drumkit_samplerate);
	void updateSessionSamplerate(double samplerate);
	void updateResamplingRecommended(bool resampling_recommended);
	void updateResamplingQuality(float resampling_quality);
	void valueChangedNotifier(float value);

private:
	dggui::TextEdit text_field{this};
	dggui::Knob quality_knob{this};
	dggui::Label quality_label{this};

	Settings& settings;
	SettingsNotifier& settings_notifier;

	std::string drumkit_samplerate;
	std::string session_samplerate;
	std::string resampling_recommended;
};

} // GUI::
