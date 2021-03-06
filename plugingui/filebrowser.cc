/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            filebrowser.cc
 *
 *  Mon Feb 25 21:09:44 CET 2013
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
#include "filebrowser.h"

#include <dggui/painter.h>
#include <dggui/button.h>

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <platform.h>
#include <hugin.hpp>

#ifdef __MINGW32__
#include <direct.h>
#endif

#include <translation.h>

namespace GUI
{

FileBrowser::FileBrowser(dggui::Widget* parent)
	: dggui::Dialog(parent, true)
	, dir(Directory::cwd())
	, lbl_path(this)
	, lineedit(this)
	, listbox(this)
	, btn_sel(this)
	, btn_def(this)
	, btn_esc(this)
	, back(":resources/bg.png")
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	above_root = false;
#endif

	setCaption(_("Open file..."));

	lbl_path.setText(_("Path:"));

	//lineedit.setReadOnly(true);
	CONNECT(&lineedit, enterPressedNotifier, this, &FileBrowser::handleKeyEvent);
	CONNECT(&listbox, selectionNotifier,
	        this, &FileBrowser::listSelectionChanged);
	CONNECT(this, fileSelectNotifier, this, &FileBrowser::select);
	CONNECT(eventHandler(), closeNotifier, this, &FileBrowser::cancel);

	btn_sel.setText(_("Select"));
	CONNECT(&btn_sel, clickNotifier, this, &FileBrowser::selectButtonClicked);

	btn_def.setText(_("Set default path"));
	CONNECT(&btn_def, clickNotifier, this, &FileBrowser::setDefaultPath);

	btn_esc.setText(_("Cancel"));
	CONNECT(&btn_esc, clickNotifier, this, &FileBrowser::cancelButtonClicked);

	changeDir();
}

void FileBrowser::setPath(const std::string& path)
{
	INFO(filebrowser, _("Setting path to '%s'\n"), path.c_str());

	if(!path.empty() && Directory::exists(path))
	{
		dir.setPath(Directory::pathDirectory(path));
	}
	else
	{
		dir.setPath(Directory::pathDirectory(Directory::cwd()));
	}

	listbox.clear();

	changeDir();
}

void FileBrowser::resize(std::size_t width, std::size_t height)
{
	dggui::Dialog::resize(width, height);

	int offset = 0;
	int brd = 5; // border
	int btn_h = 30;
	int btn_w = std::max(width * 2 / 7, std::size_t(0));

	offset += brd;

	lbl_path.move(brd, offset);
	lineedit.move(60, offset);

	offset += btn_h;

	lbl_path.resize(60, btn_h);
	lineedit.resize(std::max((int)width - 60 - brd, 0), btn_h);

	offset += brd;

	listbox.move(brd, offset);
	listbox.resize(std::max((int)width - 1 - 2*brd, 0),
	               std::max((int)height - btn_h - 2*brd - offset, 0));

	btn_def.move(brd, height - btn_h - brd);
	btn_def.resize(btn_w, btn_h);

	btn_esc.move(width - (brd + btn_w + brd + btn_w), height - btn_h - brd);
	btn_esc.resize(btn_w, btn_h);

	btn_sel.move(width - (brd + btn_w), height - btn_h - brd);
	btn_sel.resize(btn_w, btn_h);
}

void FileBrowser::repaintEvent(dggui::RepaintEvent* repaintEvent)
{
	dggui::Painter p(*this);
	p.drawImageStretched(0,0, back, width(), height());
}

void FileBrowser::listSelectionChanged()
{
	changeDir();
}

void FileBrowser::selectButtonClicked()
{
	changeDir();
}

void FileBrowser::setDefaultPath()
{
	defaultPathChangedNotifier(dir.path());
}

void FileBrowser::cancelButtonClicked()
{
	cancel();
}

void FileBrowser::handleKeyEvent()
{
	listbox.clearSelectedValue();

	std::string value = lineedit.getText();
	if((value.size() > 1) && (value[0] == '@'))
	{
		DEBUG(filebrowser, _("Selecting ref-file '%s'\n"), value.c_str());
		fileSelectNotifier(value);
		return;
	}

	dir.setPath(lineedit.getText());
	changeDir();
}

void FileBrowser::cancel()
{
	has_filename = false;
	hide();
	fileSelectCancelNotifier();
}

void FileBrowser::select(const std::string& file)
{
	has_filename = true;
	filename = file;
	hide();
}

void FileBrowser::changeDir()
{
	std::string value = listbox.selectedValue();

//  if(!Directory::isDir(dir->path() + dir->seperator()))
//  {
//    return;
//  }

	listbox.clear();

	INFO(filebrowser, _("Changing path to '%s'\n"),
	     (dir.path() + dir.seperator() + value).c_str());

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	if(above_root && !value.empty())
	{
		dir.setPath(value + dir.seperator());
		value.clear();
		above_root = false;
	}
#endif

	if(value.empty() && !dir.isDir() && Directory::exists(dir.path()))
	{
		DEBUG(filebrowser, _("Selecting file '%s'\n"), dir.path().c_str());
		fileSelectNotifier(dir.path());
		return;
	}

	if(!value.empty() && dir.fileExists(value))
	{
	  std::string file = dir.path() + dir.seperator() + value;
	  DEBUG(filebrowser, _("Selecting file '%s'\n"), file.c_str());
	  fileSelectNotifier(file);
	  return;
	}

	std::vector<dggui::ListBoxBasic::Item> items;

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	if(Directory::isRoot(dir.path()) && (value == ".."))
	{
		DEBUG(filebrowser, _("Showing partitions...\n"));
		for(auto drive : dir.drives())
		{
			dggui::ListBoxBasic::Item item;
			item.name = drive.name;
			item.value = drive.name;
			items.push_back(item);
		}
		above_root = true;
	}
	else
#endif
	{
		if(!value.empty() && !dir.cd(value))
		{
			DEBUG(filebrowser, _("Error changing to '%s'\n"),
			      (dir.path() + dir.seperator() + value).c_str());
			return;
		}

		Directory::EntryList entries = dir.entryList();

		if(entries.empty())
		{
			dir.cdUp();
			entries = dir.entryList();
		}

		DEBUG(filebrowser, _("Setting path of lineedit to %s\n"), dir.path().c_str());
		lineedit.setText(dir.path());

		for(auto entry : entries)
		{
			dggui::ListBoxBasic::Item item;
			item.name = entry;
			item.value = entry;
			items.push_back(item);
		}
	}

	listbox.addItems(items);
}

std::string FileBrowser::getFilename() const
{
	return filename;
}

bool FileBrowser::hasFilename() const
{
	return has_filename;
}

} // GUI::
