/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            eventhandler.h
 *
 *  Sun Oct  9 18:58:29 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#ifndef __DRUMGIZMO_EVENTHANDLER_H__
#define __DRUMGIZMO_EVENTHANDLER_H__

#include "event.h"

class GlobalContext;

class EventHandler {
public:
  EventHandler(GlobalContext *gctx);

  bool hasEvent();
  Event *getNextEvent();

private:
  GlobalContext *gctx;
};

#endif/*__DRUMGIZMO_EVENTHANDLER_H__*/