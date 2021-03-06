/* -*- Mode: c++ -*- */
/***************************************************************************
 *            bleedcontrolframecontent.cc
 *
 *  Wed May 24 14:40:16 CEST 2017
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
#include "bleedcontrolframecontent.h"

#include <settings.h>

#include <limits>

#include <translation.h>

namespace GUI
{

BleedcontrolframeContent::BleedcontrolframeContent(dggui::Widget* parent,
                                                   Settings& settings,
                                                   SettingsNotifier& settings_notifier)
	: dggui::Widget(parent)
	, slider_width{250}
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	label_text.setText(_("Master Bleed Volume:"));
	label_text.setAlignment(dggui::TextAlignment::center);

	label_value.setText(_("0 %"));
	label_value.setAlignment(dggui::TextAlignment::center);

	CONNECT(this, settings_notifier.master_bleed,
	        this, &BleedcontrolframeContent::bleedSettingsValueChanged);
	CONNECT(&slider, valueChangedNotifier,
	        this, &BleedcontrolframeContent::bleedValueChanged);
}

void BleedcontrolframeContent::resize(std::size_t width, std::size_t height)
{
	dggui::Widget::resize(width, height);

	slider_width = 0.8 * width;
	auto x_start = 0.1 * width;

	label_text.move(x_start, 0);
	slider.move(x_start, 20);
	label_value.move(x_start, 40);

	label_text.resize(slider_width, 15);
	slider.resize(slider_width, 15);
	label_value.resize(slider_width, 15);
}

void BleedcontrolframeContent::setEnabled(bool enabled)
{
	this->enabled = enabled;

	if(enabled)
	{
		label_text.resetColour();
		label_value.resetColour();
		slider.setEnabled(true);
	}
	else
	{
		label_text.setColour(0.7);
		label_value.setColour(0.7);
		slider.setEnabled(false);
	}

	redraw();
}

void BleedcontrolframeContent::bleedSettingsValueChanged(float value)
{
	slider.setValue(value);

	int percentage = 100 * value;
	label_value.setText(std::to_string(percentage) + " %");
	slider.setColour(dggui::Slider::Colour::Blue);
}

void BleedcontrolframeContent::bleedValueChanged(float value)
{
	settings.master_bleed.store(value);
}

} // GUI::
