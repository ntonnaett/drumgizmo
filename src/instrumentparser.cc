/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            instrumentparser.cc
 *
 *  Wed Mar  9 13:22:24 CET 2011
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
#include "instrumentparser.h"

#include <string.h>
#include <stdio.h>

#include "path.h"

InstrumentParser::InstrumentParser(const std::string &file, Instrument &i)
  : instrument(i)
{
  s = NULL;
  printf("Parsing instrument in %s\n", file.c_str());
  path = getPath(file);
  fd = fopen(file.c_str(), "r");
  if(!fd) return;
}

InstrumentParser::~InstrumentParser()
{
  if(fd) fclose(fd);
}

void InstrumentParser::startTag(std::string name,
                                std::map< std::string, std::string> attr)
{
  if(name == "instrument") {
    if(attr.find("name") != attr.end())
      instrument._name = attr["name"];

    if(attr.find("description") != attr.end())
      instrument._description = attr["description"];
  }

  if(name == "samples") {
  }

  if(name == "sample") {
    if(attr.find("name") == attr.end()) {
      printf("Missing required attribute 'name'.\n");
      return;
    }
    s = new Sample(attr["name"]);
  }

  if(name == "audiofile") {
    if(s == NULL) {
      printf("Missing Sample!\n");
      return;
    }
    
    if(attr.find("file") == attr.end()) {
      printf("Missing required attribute 'file'.\n");
      return;
    }

    if(attr.find("channel") == attr.end()) {
      printf("Missing required attribute 'channel'.\n");
      return;
    }

    AudioFile *af = new AudioFile(path + "/" + attr["file"]);
    InstrumentChannel *ch = new InstrumentChannel(attr["channel"]);
    channellist.push_back(ch);
    s->addAudioFile(ch, af);
    instrument.audiofiles.push_back(af);
  }

  if(name == "velocities") {
  }

  if(name == "velocity") {
    if(attr.find("lower") == attr.end()) {
      printf("Missing required attribute 'lower'.\n");
      return;
    }

    if(attr.find("upper") == attr.end()) {
      printf("Missing required attribute 'upper'.\n");
      return;
    }

    lower = atof(attr["lower"].c_str());
    upper = atof(attr["upper"].c_str());
  }

  if(name == "sampleref") {
    if(attr.find("name") == attr.end()) {
      printf("Missing required attribute 'name'.\n");
      return;
    }

    Sample *sample = NULL;
    std::vector<Sample *>::iterator i = instrument.samplelist.begin();
    while(i != instrument.samplelist.end()) {
      if((*i)->name == attr["name"]) {
        sample = *i;
        break;
      }
      i++;
    }

    if(sample == NULL) {
      printf("Samplref pointed at non-existing sample.\n");
      return;
    }

    instrument.addSample(lower, upper, sample);
  }
}

void InstrumentParser::endTag(std::string name)
{
  if(name == "sample") {
    if(s == NULL) {
      printf("Missing Sample.\n");
      return;
    }

    instrument.samplelist.push_back(s);
    s = NULL;
  }
}

int InstrumentParser::readData(char *data, size_t size)
{
  if(!fd) return -1;
  return fread(data, 1, size, fd);
}


#ifdef TEST_INSTRUMENTPARSER
//deps: saxparser.cc instrument.cc sample.cc audiofile.cc channel.cc
//cflags: $(EXPAT_CFLAGS) $(SNDFILE_CFLAGS)
//libs: $(EXPAT_LIBS) $(SNDFILE_LIBS)
#include "test.h"

const char xml[] = 
"<?xml version='1.0' encoding='UTF-8'?>\n"
"<instrument name=\"kick-r\">\n"
" <samples>\n"
"  <sample name=\"kick-r-1\">\n"
"   <audiofile channel=\"Alesis\" file=\"samples/1-kick-r-Alesis-3.wav\"/>\n"
"   <audiofile channel=\"Amb L\" file=\"samples/1-kick-r-Amb L-3.wav\"/>\n"
"   <audiofile channel=\"Amb R\" file=\"samples/1-kick-r-Amb R-3.wav\"/>\n"
"   <audiofile channel=\"Kick L\" file=\"samples/1-kick-r-Kick L-3.wav\"/>\n"
"   <audiofile channel=\"Kick R\" file=\"samples/1-kick-r-Kick R-3.wav\"/>\n"
"  </sample>\n"
"  <sample name=\"kick-r-2\">\n"
"   <audiofile channel=\"Alesis\" file=\"samples/2-kick-r-Alesis-3.wav\"/>\n"
"   <audiofile channel=\"Amb L\" file=\"samples/2-kick-r-Amb L-3.wav\"/>\n"
"   <audiofile channel=\"Amb R\" file=\"samples/2-kick-r-Amb R-3.wav\"/>\n"
"   <audiofile channel=\"Kick L\" file=\"samples/2-kick-r-Kick L-3.wav\"/>\n"
"   <audiofile channel=\"Kick R\" file=\"samples/2-kick-r-Kick R-3.wav\"/>\n"
"  </sample>\n"
"  <sample name=\"kick-r-3\">\n"
"   <audiofile channel=\"Alesis\" file=\"samples/3-kick-r-Alesis-3.wav\"/>\n"
"   <audiofile channel=\"Amb L\" file=\"samples/3-kick-r-Amb L-3.wav\"/>\n"
"   <audiofile channel=\"Amb R\" file=\"samples/3-kick-r-Amb R-3.wav\"/>\n"
"   <audiofile channel=\"Kick L\" file=\"samples/3-kick-r-Kick L-3.wav\"/>\n"
"   <audiofile channel=\"Kick R\" file=\"samples/3-kick-r-Kick R-3.wav\"/>\n"
"  </sample>\n"
" </samples>\n"
" <velocities>\n"
"  <velocity lower=\"0\" upper=\"0.7\">\n"
"   <sampleref name=\"kick-r-1\"/>\n"
"   <sampleref name=\"kick-r-2\"/>\n"
"  </velocity>\n"
"  <velocity lower=\"0.7\" upper=\"1.0\">\n"
"   <sampleref name=\"kick-r-3\"/>\n"
"  </velocity>\n"
" </velocities>\n"
"</instrument>\n"
  ;

#define FNAME "/tmp/instrtest.xml"

TEST_BEGIN;

FILE *fp = fopen(FNAME, "w");
fprintf(fp, "%s", xml);
fclose(fp);

Instrument instr;
InstrumentParser p(FNAME, instr);
TEST_EQUAL_INT(p.parse(), 0, "Parsing went well?");

TEST_EQUAL_STR(instr.name(), "kick-r", "Compare name");

unlink(FNAME);

TEST_END;

#endif/*TEST_INSTRUMENTPARSER*/