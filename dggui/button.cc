/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            button.cc
 *
 *  Sun Oct  9 13:01:56 CEST 2011
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
#include "button.h"

#include "painter.h"

#include <hugin.hpp>
#include <stdio.h>

namespace dggui
{

Button::Button(Widget* parent)
	: ButtonBase(parent)
{
}

Button::~Button()
{
}

void Button::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);
	p.clear();

	int padTop = 3;
	int padLeft = 0;
	int padTextTop = 3;

	int w = width();
	int h = height();
	if(w == 0 || h == 0)
	{
		return;
	}

	if (enabled) {
		switch(draw_state)
		{
		case State::Up:
			box_up.setSize(w - padLeft, h - padTop);
			p.drawImage(padLeft, padTop, box_up);
			break;

		case State::Down:
			box_down.setSize(w - padLeft, h - padTop);
			p.drawImage(padLeft, padTop, box_down);
			break;
		}
	}
	else {
		box_grey.setSize(w - padLeft, h - padTop);
		p.drawImage(padLeft, padTop, box_grey);

		p.setColour(Colour(0.55));
	}

	auto x = padLeft + (width() - font.textWidth(text)) / 2;
	auto y = padTop + padTextTop + font.textHeight(text);
	p.drawText(x, y, font, text, enabled);
}

} // dggui::
