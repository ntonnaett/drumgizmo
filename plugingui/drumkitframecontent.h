/* -*- Mode: c++ -*- */
/***************************************************************************
 *            drumkitframecontent.h
 *
 *  Fri Mar 24 21:49:42 CET 2017
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

#include "button.h"
#include "label.h"
#include "lineedit.h"
#include "progressbar.h"
#include "widget.h"
#include "filebrowser.h"

namespace GUI
{

class BrowseFile
	: public Widget
{
public:
	BrowseFile(Widget* parent);

	// From Widget
	virtual void resize(std::size_t width, std::size_t height) override;

	std::size_t getLineEditWidth();
	std::size_t getButtonWidth();

	Button& getBrowseButton();
	LineEdit& getLineEdit();

private:
	HBoxLayout layout{this};

	LineEdit lineedit{this};
	Button browse_button{this};

	std::size_t lineedit_width;
	std::size_t button_width;
};

class DrumkitframeContent
	: public Widget
{
public:
	DrumkitframeContent(Widget* parent);

	// From Widget
	virtual void resize(std::size_t width, std::size_t height) override;

	void kitBrowseClick();
	void midimapBrowseClick();

private:
	void selectKitFile(const std::string& filename);
	void selectMapFile(const std::string& filename);

	VBoxLayout layout{this};

	Label drumkitCaption{this};
	Label midimapCaption{this};
	BrowseFile drumkitFile{this};
	BrowseFile midimapFile{this};
	ProgressBar drumkitFileProgress{this};
	ProgressBar midimapFileProgress{this};

	FileBrowser file_browser{this};
};

} // GUI::