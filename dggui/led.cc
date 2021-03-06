/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            led.cc
 *
 *  Sat Oct 15 19:12:33 CEST 2011
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
#include "led.h"

#include "painter.h"

namespace dggui
{

LED::LED(Widget *parent)
	: Widget(parent)
	, state(Off)
{
}

void LED::setState(state_t state)
{
	if(this->state != state)
	{
		this->state = state;
		redraw();
	}
}

void LED::repaintEvent(RepaintEvent* repaintEvent)
{
	size_t h = height() - 1;
	size_t w = width() - 1;

	Painter p(*this);
	float alpha = 0.9;
	switch(state) {
	case Red:
		p.setColour(Colour(1, 0, 0,alpha));
		break;
	case Green:
		p.setColour(Colour(0, 1, 0, alpha));
		break;
	case Blue:
		p.setColour(Colour(0, 0, 1, alpha));
		break;
	case Off:
		p.setColour(Colour(0.2, 0.2, 0.2, alpha));
		break;
	}

	size_t size = w / 2;
	if((h / 2) < size)
	{
		size = h / 2;
	}
	p.drawFilledCircle(w / 2, h / 2, size);

	switch(state) {
	case Red:
		p.setColour(Colour(0.4, 0, 0, alpha));
		break;
	case Green:
		p.setColour(Colour(0, 0.4, 0, alpha));
		break;
	case Blue:
		p.setColour(Colour(0, 0, 0.4, alpha));
		break;
	case Off:
		p.setColour(Colour(0.1, 0.1, 0.1, alpha));
		break;
	}
	p.drawCircle(w / 2, h / 2, size);

	p.setColour(Colour(1, alpha));
	p.drawFilledCircle(w / 3, h / 3, size / 6);
}

} // dggui::
