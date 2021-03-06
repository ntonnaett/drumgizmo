/* -*- Mode: c++ -*- */
/***************************************************************************
 *            configparsertest.cc
 *
 *  Wed Jul 25 19:08:40 CEST 2018
 *  Copyright 2018 Bent Bisballe Nyeng
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
#include <uunit.h>

#include <configparser.h>

class ConfigParserTest
	: public uUnit
{
public:
	ConfigParserTest()
	{
		uUNIT_TEST(ConfigParserTest::test);
		uUNIT_TEST(ConfigParserTest::invalid);
	}

	void test()
	{
		std::string xml =
			"<?xml version='1.0' encoding='UTF-8'?>\n" \
			"<config>\n" \
			"  <value name=\"foo\">42</value>\n" \
			"  <value name=\"bar\">true</value>\n" \
			"  <value name=\"bas\">&quot;&lt;</value>\n" \
			"</config>";


		ConfigParser parser;
		uUNIT_ASSERT(parser.parseString(xml));

		uUNIT_ASSERT_EQUAL(std::string("42"), parser.value("foo", "-"));
		uUNIT_ASSERT_EQUAL(std::string("true"), parser.value("bar", "-"));
		uUNIT_ASSERT_EQUAL(std::string("\"<"), parser.value("bas", "-"));

		// Non-existing value
		uUNIT_ASSERT_EQUAL(std::string("-"), parser.value("bas2", "-"));
	}

	void invalid()
	{
		std::string xml =
			"<?xml version='1.0' encoding='UTF-8'?>\n" \
			"<config\n" \
			"  <value name=\"foo\">42</value>\n" \
			"  <value name=\"bar\">true</value>\n" \
			"  <value name=\"bas\">&quot;&lt;</value>\n" \
			"</config>";


		ConfigParser parser;
		// Epxect parser error (missing '>' in line 2)
		uUNIT_ASSERT(!parser.parseString(xml));
	}
};

// Registers the fixture into the 'registry'
static ConfigParserTest test;
