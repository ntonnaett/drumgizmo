/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            progressbar.h
 *
 *  Fri Mar 22 22:07:57 CET 2013
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
#pragma once

#include "widget.h"

#include "guievent.h"
#include "painter.h"
#include "texturedbox.h"

namespace dggui
{

enum class ProgressBarState
{
	Red,
	Green,
	Blue,
	Off
};

class ProgressBar
	: public Widget
{
public:
	ProgressBar(Widget* parent);
	virtual ~ProgressBar();

	void setTotal(std::size_t total);
	void setValue(std::size_t value);

	void setState(ProgressBarState state);

protected:
	// From Widget:
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;

private:
	ProgressBarState state{ProgressBarState::Blue};

	TexturedBox bar_bg{getImageCache(), ":resources/progress.png",
			0, 0, // atlas offset (x, y)
			6, 1, 6, // dx1, dx2, dx3
			11, 0, 0}; // dy1, dy2, dy3

	TexturedBox bar_red{getImageCache(), ":resources/progress.png",
			13, 0, // atlas offset (x, y)
			2, 1, 2, // dx1, dx2, dx3
			11, 0, 0}; // dy1, dy2, dy3

	TexturedBox bar_green{getImageCache(), ":resources/progress.png",
			18, 0, // atlas offset (x, y)
			2, 1, 2, // dx1, dx2, dx3
			11, 0, 0}; // dy1, dy2, dy3

	TexturedBox bar_blue{getImageCache(), ":resources/progress.png",
			23, 0, // atlas offset (x, y)
			2, 1, 2, // dx1, dx2, dx3
			11, 0, 0}; // dy1, dy2, dy3

	std::size_t total{0};
	std::size_t value{0};
};

} // dggui::
