/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkitparser.cc
 *
 *  Tue Jul 22 16:24:59 CEST 2008
 *  Copyright 2008 Bent Bisballe Nyeng
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
#include "drumkitparser.h"
#include <string.h>
#define DIR_SEPERATOR '/'

DrumKitParser::DrumKitParser(std::string kitfile, bool preload, int min_velocity)
{
  std::string file;
  std::string path;

  char *split = strrchr(kitfile.c_str(), DIR_SEPERATOR);
  if(split) {
    file = split + 1;
    path = kitfile.substr(0, kitfile.length() - file.length());
    // All drum samples are relative to the kitfile, so we must chdir there to be able to open them.
    chdir(path.c_str());
  } else {
    file = kitfile;
  }

  this->preload = preload;
  this->min_velocity = min_velocity;

  dk = NULL;
  fd = fopen(file.c_str(), "r");
  if(!fd) return;
  dk = new DrumKit();
}

DrumKitParser::~DrumKitParser()
{
  if(dk) fclose(fd);
}

void DrumKitParser::startTag(std::string name, std::map< std::string, std::string> attributes)
{
  if(name == "drumkit") {
    dk->name = attributes["name"];
    dk->description = attributes["description"];
  }

  if(name == "channels") {}

  if(name == "channel") {
    Channel *c = new Channel(attributes["name"]);
    dk->channels[attributes["name"]] = c;
  }

  if(name == "samples") {}

  if(name == "sample") {
    Sample *s = new Sample(attributes["name"]);
    dk->samples[attributes["name"]] = s;
    lastsample = s;
  }

  if(name == "audiofile") {
    AudioFile *af = new AudioFile(attributes["name"], preload, min_velocity);
    af->channel = attributes["channel"];
    lastsample->audiofiles[attributes["name"]] = af;
  }

  if(name == "instruments") {}

  if(name == "instrument") {
    midi_note_t m = atoi(attributes["midimap"].c_str());
    Instrument *i = new Instrument(attributes["name"], m);
    dk->instruments[m] = i;
    lastinstrument = i;
  }
  
  if(name == "velocity") {
    Velocity *v = new Velocity(atoi(attributes["lower"].c_str()),
                               atoi(attributes["upper"].c_str()));
    lastinstrument->addVelocity(v);
    lastvelocity = v;
  }

  if(name == "sampleref") {
    lastvelocity->addSample(dk->samples[attributes["name"]],
                            atof(attributes["probability"].c_str()));
  }
}

void DrumKitParser::endTag(std::string name)
{
}

DrumKit *DrumKitParser::getDrumkit()
{
  return dk;
}

int DrumKitParser::readData(char *data, size_t size)
{
  if(!fd) return 0;
  return fread(data, 1, size, fd);
}
