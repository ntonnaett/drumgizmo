/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            slider.cc
 *
 *  Sat Nov 26 18:10:22 CET 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#include "slider.h"

#include "painter.h"

#include <hugin.hpp>
#include <stdio.h>

namespace dggui
{

Slider::Slider(Widget* parent) : Widget(parent)
{
	state = State::up;

	current_value = 0.0;
	maximum = 1.0;
	minimum = 0.0;
}

void Slider::setValue(float new_value)
{
	current_value = new_value;
	if (current_value < 0.)
	{
		current_value = 0.;
	}
	else if (current_value > 1.0) {
		current_value = 1.0;
	}

	redraw();
	clickNotifier();
	valueChangedNotifier(current_value);
}

float Slider::value() const
{
	return current_value;
}

void Slider::setColour(Colour colour)
{
	switch (colour) {
	case Colour::Green:
		active_inner_bar = &inner_bar_green;
		break;
	case Colour::Red:
		active_inner_bar = &inner_bar_red;
		break;
	case Colour::Blue:
		active_inner_bar = &inner_bar_blue;
		break;
	case Colour::Yellow:
		active_inner_bar = &inner_bar_yellow;
		break;
	case Colour::Purple:
		active_inner_bar = &inner_bar_purple;
		break;
	case Colour::Grey:
		active_inner_bar = &inner_bar_grey;
		break;
	}

	if (enabled) { inner_bar = active_inner_bar; }
}

void Slider::setEnabled(bool enabled)
{
	this->enabled = enabled;

	if (enabled) {
		inner_bar = active_inner_bar;
	}
	else {
		active_inner_bar = inner_bar;
		inner_bar = &inner_bar_light_grey;
	}

	redraw();
}

void Slider::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	auto inner_offset = (current_value / maximum) * getControlWidth();
	auto button_x = button_offset + inner_offset - (button.width() / 2);
	auto button_y = (height() - button.height()) / 2;

	// draw bar
	bar.setSize(width(), height());
	p.drawImage(0, 0, bar);

	// draw inner bar
	inner_bar->setSize(button_x - bar_boundary, height() - 2 * bar_boundary);
	p.drawImage(bar_boundary, bar_boundary, *inner_bar);

	// draw button
	p.drawImage(button_x, button_y, button);
}

void Slider::buttonEvent(ButtonEvent* buttonEvent)
{
	// Ignore everything except left clicks.
	if(!enabled || buttonEvent->button != MouseButton::left)
	{
		return;
	}

	if(buttonEvent->direction == Direction::down)
	{
		state = State::down;
		recomputeCurrentValue(buttonEvent->x);

		redraw();
		clickNotifier();
		valueChangedNotifier(current_value);
	}

	if(buttonEvent->direction == Direction::up)
	{
		state = State::up;
		recomputeCurrentValue(buttonEvent->x);

		redraw();
		clickNotifier();
		valueChangedNotifier(current_value);
	}
}

void Slider::mouseMoveEvent(MouseMoveEvent* mouseMoveEvent)
{
	if(state == State::down)
	{
		recomputeCurrentValue(mouseMoveEvent->x);

		redraw();
		clickNotifier();
		valueChangedNotifier(current_value);
	}
}

void Slider::scrollEvent(ScrollEvent* scrollEvent)
{
	if (!enabled) { return; }

	current_value -= scrollEvent->delta/(float)getControlWidth();
	if (current_value < 0.)
	{
		current_value = 0.;
	}
	else if (current_value > 1.0) {
		current_value = 1.0;
	}

	redraw();
	clickNotifier();
	valueChangedNotifier(current_value);
}

std::size_t Slider::getControlWidth() const
{
	if(width() < 2 * button_offset)
	{
		return 0;
	}

	return width() - 2 * button_offset;
}

void Slider::recomputeCurrentValue(float x)
{
	if(x < button_offset)
	{
		current_value = 0;
	}
	else
	{
		current_value = (x - button_offset) / getControlWidth();
	}

	if (current_value < 0.)
	{
		current_value = 0.;
	}
	else if (current_value > 1.0) {
		current_value = 1.0;
	}
}

} // dggui::
