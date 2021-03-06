/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            combobox.cc
 *
 *  Sun Mar 10 19:04:50 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "combobox.h"

#include "painter.h"
#include "font.h"

#include <stdio.h>

#define BORDER 10

namespace dggui
{

void ComboBox::listboxSelectHandler()
{
	ButtonEvent buttonEvent;
	buttonEvent.direction = Direction::down;
	this->buttonEvent(&buttonEvent);
}

ComboBox::ComboBox(Widget* parent)
	: Widget(parent)
	, listbox(parent)
{
	CONNECT(&listbox, selectionNotifier, this, &ComboBox::listboxSelectHandler);
	CONNECT(&listbox, clickNotifier, this, &ComboBox::listboxSelectHandler);

	listbox.hide();
}

ComboBox::~ComboBox()
{
}

void ComboBox::addItem(std::string name, std::string value)
{
	listbox.addItem(name, value);
}

void ComboBox::clear()
{
	listbox.clear();
	redraw();
}

bool ComboBox::selectItem(int index)
{
	listbox.selectItem(index);
	redraw();
	return true;
}

std::string ComboBox::selectedName()
{
	return listbox.selectedName();
}

std::string ComboBox::selectedValue()
{
	return listbox.selectedValue();
}

static void drawArrow(Painter &p, int x, int y, int w, int h)
{
	p.drawLine(x, y, x+(w/2), y+h);
	p.drawLine(x+(w/2), y+h, x+w, y);

	y++;
	p.drawLine(x, y, x+(w/2), y+h);
	p.drawLine(x+(w/2), y+h, x+w, y);
}

void ComboBox::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	std::string _text = selectedName();

	int w = width();
	int h = height();
	if(w == 0 || h == 0)
	{
		return;
	}

	box.setSize(w, h);
	p.drawImage(0, 0, box);

	p.setColour(Colour(183.0f/255.0f, 219.0f/255.0f, 255.0/255.0f, 1.0f));
	p.drawText(BORDER - 4 + 3, height()/2+5 + 1 + 1, font, _text);

	//  p.setColour(Colour(1, 1, 1));
	//  p.drawText(BORDER - 4, (height()+font.textHeight()) / 2 + 1, font, _text);

	//int n = height() / 2;

	//  p.drawLine(width() - n - 6, 1 + 6, width() - 1 - 6, 1 + 6);
	{
		int w = 10;
		int h = 6;
		drawArrow(p, width() - 6 - 4 - w, (height() - h) / 2, w, h);
		p.drawLine(width() - 6 - 4 - w - 4, 7,
		           width() - 6 - 4 - w - 4, height() - 8);
	}
}

void ComboBox::scrollEvent(ScrollEvent* scrollEvent)
{
	/*
	scroll_offset += e->delta;
	if(scroll_offset < 0)
	{
		scroll_offset = 0;
	}
	if(scroll_offset > (items.size() - 1))
	{
		scroll_offset = (items.size() - 1);
	}
	redraw();
	*/
}

void ComboBox::keyEvent(KeyEvent* keyEvent)
{
	if(keyEvent->direction != Direction::up)
	{
		return;
	}

	/*
	switch(keyEvent->keycode) {
	case Key::up:
		{
			selected--;
			if(selected < 0)
			{
				selected = 0;
			}
			if(selected < scroll_offset)
			{
				scroll_offset = selected;
				if(scroll_offset < 0)
				{
					scroll_offset = 0;
				}
			}
		}
		break;
	case Key::down:
		{
			// Number of items that can be displayed at a time.
			int numitems = height() / (font.textHeight() + padding);

			selected++;
			if(selected > (items.size() - 1))
			{
				selected = (items.size() - 1);
			}
			if(selected > (scroll_offset + numitems - 1))
			{
				scroll_offset = selected - numitems + 1;
				if(scroll_offset > (items.size() - 1))
				{
					scroll_offset = (items.size() - 1);
				}
			}
		}
		break;
	case Key::home:
		selected = 0;
		break;
	case Key::end:
		selected = items.size() - 1;
		break;
	default:
		break;
	}

	redraw();
	*/
}

void ComboBox::buttonEvent(ButtonEvent* buttonEvent)
{
	// Ignore everything except left clicks.
	if(buttonEvent->button != MouseButton::left)
	{
		return;
	}

	if(buttonEvent->direction != Direction::down)
	{
		return;
	}

	if(!listbox.visible())
	{
		listbox.resize(width() - 10, 100);
		listbox.move(x() + 5, y() + height() - 7);
	}
	else
	{
		valueChangedNotifier(listbox.selectedName(), listbox.selectedValue());
	}

	listbox.setVisible(!listbox.visible());
}

} // dggui::
