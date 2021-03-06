/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.cc
 *
 *  Sat Nov 26 14:27:29 CET 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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
#include "mainwindow.h"

#include <string>

#include <config.h>

#include <translation.h>

#include <dggui/painter.h>

namespace GUI
{

MainWindow::MainWindow(Settings& settings, void* native_window)
	: dggui::Window(native_window)
	, settings_notifier(settings)
	, main_tab(this, settings, settings_notifier, config)
	, drumkit_tab(this, settings, settings_notifier)
{
	config.load();

	CONNECT(this, sizeChangeNotifier, this, &MainWindow::sizeChanged);
	CONNECT(eventHandler(), closeNotifier, this, &MainWindow::closeEventHandler);

	setCaption(std::string(_("DrumGizmo v")) + VERSION);

	tabs.setTabWidth(100);
	tabs.move(16, 0); // x-offset to make room for the left side bar.
	tabs.addTab(_("Main"), &main_tab);
	drumkit_tab_id = tabs.addTab(_("Drumkit"), &drumkit_tab);
	changeDrumkitTabVisibility(false); // Hide while no kit is loaded
	tabs.addTab(_("About"), &about_tab);

	CONNECT(&drumkit_tab, imageChangeNotifier,
	        this, &MainWindow::changeDrumkitTabVisibility);
}

MainWindow::~MainWindow()
{
	config.save();
}

bool MainWindow::processEvents()
{
	settings_notifier.evaluate();
	eventHandler()->processEvents();

	if(closing)
	{
		closeNotifier();
		closing = false;
		return false;
	}

	return true;
}

void MainWindow::repaintEvent(dggui::RepaintEvent* repaintEvent)
{
	if(!visible())
	{
		return;
	}

	dggui::Painter painter(*this);

	auto bar_height = tabs.getBarHeight();

	// Grey background
	painter.drawImageStretched(0, 0, back, width(), height());

	// Topbar above the sidebars
	topbar.setSize(16, bar_height);
	painter.drawImage(0, 0, topbar);
	painter.drawImage(width() - 16, 0, topbar);

	// Sidebars
	sidebar.setSize(16, height() - bar_height + 1);
	painter.drawImage(0, bar_height-1, sidebar);
	painter.drawImage(width() - 16, bar_height-1, sidebar);
}

void MainWindow::sizeChanged(std::size_t width, std::size_t height)
{
	tabs.resize(std::max((int)width - 2 * 16, 0), height);
}

void MainWindow::changeDrumkitTabVisibility(bool visible)
{
	// TODO: Check if the currently active tab is the drumkit tab and switch to
	// the main tab if it is.

	// TODO: Add disabled state to the TabButtons and make it disabled instead of
	// hidden here.
	tabs.setVisible(drumkit_tab_id, visible);
}

void MainWindow::closeEventHandler()
{
	closing = true;
}

} // GUI::
