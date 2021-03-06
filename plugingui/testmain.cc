/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            testmain.cc
 *
 *  Sun Nov 22 20:06:42 CET 2015
 *  Copyright 2015 Bent Bisballe Nyeng
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
#include <chrono>
#include <thread>

#include <hugin.hpp>
#include <settings.h>
#include <config.h>

#include <dggui/window.h>
#include <dggui/resource.h>
#include <dggui/uitranslation.h>

#include "mainwindow.h"

int main()
{
#ifdef WITH_NLS
	dggui::UITranslation translation;
#endif // WITH_NLS

	INFO(example, "We are up and running");

	void* native_window_handle{nullptr};
#ifndef UI_PUGL
	dggui::Window parent{nullptr};
	parent.setCaption("PluginGui Test Application");
	native_window_handle = parent.getNativeWindowHandle();
#endif

	Settings settings;
	GUI::MainWindow main_window(settings, native_window_handle);

#ifndef UI_PUGL
	CONNECT(&parent, eventHandler()->closeNotifier, &main_window,
	        &GUI::MainWindow::closeEventHandler);

	parent.show();
#endif
	main_window.show();

#ifndef UI_PUGL
	parent.resize(GUI::MainWindow::main_width, GUI::MainWindow::main_height);
#else
	main_window.resize(GUI::MainWindow::main_width, GUI::MainWindow::main_height);
#endif

	while(true)
	{
#ifndef UI_PUGL
		parent.eventHandler()->processEvents();
#endif
		if(!main_window.processEvents())
		{
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	return 0;
}
