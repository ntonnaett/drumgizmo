/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            filebrowser.h
 *
 *  Mon Feb 25 21:09:43 CET 2013
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

#include <notifier.h>
#include <platform.h>
#include <directory.h>

#include <dggui/dialog.h>
#include <dggui/button.h>
#include <dggui/listbox.h>
#include <dggui/lineedit.h>
#include <dggui/label.h>
#include <dggui/image.h>

namespace GUI
{

class FileBrowser
	: public dggui::Dialog
{
public:
	FileBrowser(dggui::Widget* parent);

	void setPath(const std::string& path);

	Notifier<const std::string&> fileSelectNotifier; // (const std::string& path)
	Notifier<> fileSelectCancelNotifier;
	Notifier<const std::string&> defaultPathChangedNotifier; // (const std::string& path)

	// From Widget:
	bool isFocusable() override { return true; }
	virtual void repaintEvent(dggui::RepaintEvent* repaintEvent) override;
	virtual void resize(std::size_t width, std::size_t height) override;

	//! Return the filename selected in the browser.
	std::string getFilename() const;

	//! Returns true if the filebrowser has a selection, false if the window was
	//! closed or the cancel button was clicked.
	bool hasFilename() const;

private:
	void listSelectionChanged();
	void selectButtonClicked();
	void setDefaultPath();
	void cancelButtonClicked();
	void handleKeyEvent();

	Directory dir;
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	bool above_root;
	bool in_root;
#endif

	void cancel();
	void select(const std::string& file);
	void changeDir();

	dggui::Label lbl_path;

	dggui::LineEdit lineedit;
	dggui::ListBox listbox;

	dggui::Button btn_sel;
	dggui::Button btn_def;
	dggui::Button btn_esc;

	dggui::Image back;

	bool has_filename{false};
	std::string filename;
};

} // GUI::
