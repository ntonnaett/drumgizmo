/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            message.h
 *
 *  Wed Mar 20 15:50:57 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#ifndef __DRUMGIZMO_MESSAGE_H__
#define __DRUMGIZMO_MESSAGE_H__

class Message {
public:
  typedef enum {
    // Engine -> GUI Messages:
    LoadStatus, // Signal GUI the current load status.

    // GUI -> Engine, Engine -> Engine Messages:
    LoadDrumKit, // Signal engine to load drumkit.
    LoadMidimap, // Signal engine to load midimap.
    EngineSettingsMessage, // Request or receive engine settings.
    ChangeSettingMessage, // Update named setting in engine.
  } type_t;

  virtual ~Message() {}
  virtual type_t type() = 0;
};

class LoadStatusMessage : public Message {
public:
  type_t type() { return Message::LoadStatus; }
  unsigned int number_of_files;
  unsigned int numer_of_files_loaded;
  std::string current_file;
};

class LoadDrumKitMessage : public Message {
public:
  type_t type() { return Message::LoadDrumKit; }
  std::string drumkitfile;
};

class LoadMidimapMessage : public Message {
public:
  type_t type() { return Message::LoadMidimap; }
  std::string midimapfile;
};

class EngineSettingsMessage : public Message {
public:
  type_t type() { return Message::EngineSettingsMessage; }
  std::string midimapfile;
  bool midimap_loaded;

  std::string drumkitfile;
  bool drumkit_loaded;

  float enable_velocity_modifier;
  float velocity_modifier_falloff;
  float velocity_modifier_weight;
  float enable_velocity_randomiser;
  float velocity_randomiser_weight;
};

class ChangeSettingMessage : public Message {
public:
  typedef enum {
    enable_velocity_modifier,
    velocity_modifier_weight,
    velocity_modifier_falloff,
  } setting_name_t;

  ChangeSettingMessage(setting_name_t n, float v) {
    name = n;
    value = v;
  }

  type_t type() { return Message::ChangeSettingMessage; }

  setting_name_t name;
  float value;
};

#endif/*__DRUMGIZMO_MESSAGE_H__*/