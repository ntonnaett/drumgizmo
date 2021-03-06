/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            textedit.h
 *
 *  Tue Oct 21 11:23:58 CEST 2014
 *  Copyright 2014 Jonas Suhr Christensen
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
#include <vector>

#include "font.h"
#include "notifier.h"
#include "scrollbar.h"
#include "texturedbox.h"
#include "widget.h"

namespace dggui
{

class TextEdit
	: public Widget
{
public:
	TextEdit(Widget* parent);
	virtual ~TextEdit();

	// From Widget
	bool isFocusable() override
	{
		return true;
	}
	void resize(std::size_t width, std::size_t height) override;

	std::string getText();
	void setText(const std::string& text);

	void setReadOnly(bool readonly);
	bool readOnly();

	void preprocessText();

	Notifier<> textChangedNotifier;

protected:
	// From Widget
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;
	void scrollEvent(ScrollEvent* scrollEvent) override;

private:
	void scrolled(int value);

	TexturedBox box{getImageCache(), ":resources/widget.png", 0,
	    0,         // atlas offset (x, y)
	    7, 1, 7,   // dx1, dx2, dx3
	    7, 63, 7}; // dy1, dy2, dy3

	static constexpr int x_border{10};
	static constexpr int y_border{8};

	ScrollBar scroll;
	Font font;

	std::string text;

	bool readonly{true};
	bool needs_preprocessing{false};

	std::vector<std::string> preprocessed_text;
};

} // dggui::
