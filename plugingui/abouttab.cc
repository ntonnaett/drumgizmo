/* -*- Mode: c++ -*- */
/***************************************************************************
 *            abouttab.cc
 *
 *  Fri Apr 21 18:51:13 CEST 2017
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
#include "abouttab.h"

#include <version.h>

#include "utf8.h"

namespace GUI
{

AboutTab::AboutTab(Widget* parent)
	: Widget(parent)
{
	text_edit.setText(getAboutText());
	text_edit.setReadOnly(true);
	text_edit.resize(width() - 2*margin, height() - 2*margin);
	text_edit.move(margin, margin);
}

void AboutTab::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);
	text_edit.resize(width - 2*margin, height - 2*margin);
}

std::string AboutTab::getAboutText()
{
	std::string about_text;

	// About
	about_text.append(
	"=============\n"
	"             About\n"
	"=============\n"
	"\n");
	about_text.append(about.data());

	// Version
	about_text.append(
	"\n"
	"=============\n"
	"            Version\n"
	"=============\n"
	"\n");
	about_text.append(std::string(VERSION) + "\n");

	// Authors
	about_text.append(
	"\n"
	"=============\n"
	"            Authors\n"
	"=============\n"
	"\n");
	about_text.append(UTF8().toLatin1(authors.data()));

	// GPL
	about_text.append(
	"\n"
	"=============\n"
	"            License\n"
	"=============\n"
	"\n");
	about_text.append(gpl.data());

	return about_text;
}

} // GUI::