/* -*- Mode: c++ -*- */
/***************************************************************************
 *            humanizerframecontent.cc
 *
 *  Fri Mar 24 21:49:58 CET 2017
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
#include "humanizerframecontent.h"

#include <settings.h>

#include <dggui/painter.h>

namespace GUI
{

HumanizerframeContent::HumanizerframeContent(dggui::Widget* parent,
                                             Settings& settings,
                                             SettingsNotifier& settings_notifier)
	: dggui::Widget(parent)
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	layout.setResizeChildren(false);

	attack.resize(80, 80);
	attack_knob.resize(30, 30);
	attack_knob.showValue(false);
	attack_knob.setDefaultValue(Settings::velocity_modifier_weight_default);
	attack.setControl(&attack_knob);
	layout.addItem(&attack);

	falloff.resize(80, 80);
	falloff_knob.resize(30, 30);
	falloff_knob.showValue(false);
	falloff_knob.setDefaultValue(Settings::velocity_modifier_falloff_default);
	falloff.setControl(&falloff_knob);
	layout.addItem(&falloff);

	stddev.resize(80, 80);
	stddev_knob.resize(30, 30);
	stddev_knob.showValue(false);
	stddev_knob.setDefaultValue(Settings::velocity_stddev_default/stddev_factor);
	stddev.setControl(&stddev_knob);
	layout.addItem(&stddev);

	layout.setPosition(&attack, dggui::GridLayout::GridRange{0, 1, 0, 1});
	layout.setPosition(&falloff, dggui::GridLayout::GridRange{1, 2, 0, 1});
	layout.setPosition(&stddev, dggui::GridLayout::GridRange{2, 3, 0, 1});

	CONNECT(this, settings_notifier.velocity_modifier_weight,
	        &attack_knob, &dggui::Knob::setValue);
	CONNECT(this, settings_notifier.velocity_modifier_falloff,
	        &falloff_knob, &dggui::Knob::setValue);
	CONNECT(this, settings_notifier.velocity_stddev,
	        this, &HumanizerframeContent::stddevSettingsValueChanged);

	CONNECT(&attack_knob, valueChangedNotifier,
	        this, &HumanizerframeContent::attackValueChanged);
	CONNECT(&falloff_knob, valueChangedNotifier,
	        this, &HumanizerframeContent::falloffValueChanged);
	CONNECT(&stddev_knob, valueChangedNotifier,
	        this, &HumanizerframeContent::stddevKnobValueChanged);
}

void HumanizerframeContent::attackValueChanged(float value)
{
	settings.velocity_modifier_weight.store(value);
}

void HumanizerframeContent::falloffValueChanged(float value)
{
	settings.velocity_modifier_falloff.store(value);
}

void HumanizerframeContent::stddevKnobValueChanged(float value)
{
	settings.velocity_stddev.store(value*stddev_factor);
}

void HumanizerframeContent::stddevSettingsValueChanged(float value)
{
	stddev_knob.setValue(value/stddev_factor);
}

} // GUI::
