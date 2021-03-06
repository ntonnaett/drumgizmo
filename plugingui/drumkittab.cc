/* -*- Mode: c++ -*- */
/***************************************************************************
 *            drumkittab.cc
 *
 *  Fri Jun  8 21:50:03 CEST 2018
 *  Copyright 2018 André Nusser
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
#include "drumkittab.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "cpp11fix.h" // required for c++11

#include <dggui/painter.h>

#include "settings.h"

#include <dgxmlparser.h>
#include <path.h>

namespace GUI
{

DrumkitTab::DrumkitTab(dggui::Widget* parent,
                       Settings& settings,
                       SettingsNotifier& settings_notifier)
	: dggui::Widget(parent)
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	velocity_label.move(10, height()-velocity_label.height()-5);
	updateVelocityLabel();
	velocity_label.resizeToText();

	instrument_name_label.move(velocity_label.width()+30,
	                           height()-instrument_name_label.height()-5);
	updateInstrumentLabel(-1);

	pos_to_colour_index.setDefaultValue(-1);

	CONNECT(this, settings_notifier.drumkit_file,
	        this, &DrumkitTab::drumkitFileChanged);
}

void DrumkitTab::resize(std::size_t width, std::size_t height)
{
	dggui::Widget::resize(width, height);

	if(drumkit_image)
	{
		dggui::Painter painter(*this);
		painter.clear();

		drumkit_image_x = (this->width()-drumkit_image->width())/2;
		drumkit_image_y = (this->height()-drumkit_image->height())/2;
		painter.drawImage(drumkit_image_x, drumkit_image_y, *drumkit_image);
	}

	velocity_label.move(10, height-velocity_label.height()-5);
	instrument_name_label.move(velocity_label.width()+30,
	                           height-instrument_name_label.height()-5);
}

void DrumkitTab::buttonEvent(dggui::ButtonEvent* buttonEvent)
{
	if(map_image)
	{
		if(buttonEvent->button == dggui::MouseButton::right)
		{
			if(buttonEvent->direction == dggui::Direction::down)
			{
				dggui::Painter painter(*this);
				painter.drawImage(drumkit_image_x, drumkit_image_y, *map_image);
				shows_overlay = true;
				redraw();
				return;
			}

			if(buttonEvent->direction == dggui::Direction::up)
			{
				dggui::Painter painter(*this);
				painter.clear();
				painter.drawImage(drumkit_image_x, drumkit_image_y, *drumkit_image);

				highlightInstrument(current_index);

				shows_overlay = false;
				redraw();
				return;
			}
		}
	}

	if(buttonEvent->button == dggui::MouseButton::left)
	{
		if(buttonEvent->direction == dggui::Direction::down)
		{
			triggerAudition(buttonEvent->x, buttonEvent->y);
			highlightInstrument(current_index);
			redraw();
		}

		if(buttonEvent->direction == dggui::Direction::up)
		{
			if(shows_instrument_overlay)
			{
				dggui::Painter painter(*this);
				painter.clear();
				painter.drawImage(drumkit_image_x, drumkit_image_y, *drumkit_image);
				if(shows_overlay)
				{
					painter.drawImage(drumkit_image_x, drumkit_image_y, *map_image);
				}
				highlightInstrument(current_index);
				redraw();
			}
			shows_instrument_overlay = false;
		}
	}
}

void DrumkitTab::scrollEvent(dggui::ScrollEvent* scrollEvent)
{
	current_velocity -=  0.01 * scrollEvent->delta;
	current_velocity = std::max(std::min(current_velocity, 1.0f), 0.0f);
	updateVelocityLabel();
	velocity_label.resizeToText();

	triggerAudition(scrollEvent->x, scrollEvent->y);
}

void DrumkitTab::mouseMoveEvent(dggui::MouseMoveEvent* mouseMoveEvent)
{
	// change to image coordinates
	const auto x = mouseMoveEvent->x - drumkit_image_x;
	const auto y = mouseMoveEvent->y - drumkit_image_y;

	auto index = pos_to_colour_index(x, y);

	if(index == current_index)
	{
		return;
	}

	current_index = index;

	dggui::Painter painter(*this);
	painter.clear();
	painter.drawImage(drumkit_image_x, drumkit_image_y, *drumkit_image);
	if(shows_overlay)
	{
		painter.drawImage(drumkit_image_x, drumkit_image_y, *map_image);
	}

	highlightInstrument(index);
	updateInstrumentLabel(index);
	redraw();
}

void DrumkitTab::mouseLeaveEvent()
{
	if(map_image && (shows_overlay || shows_instrument_overlay))
	{
		dggui::Painter painter(*this);
		painter.clear();
		painter.drawImage(drumkit_image_x, drumkit_image_y, *drumkit_image);

		shows_overlay = false;
		redraw();
	}
}

void DrumkitTab::triggerAudition(int x, int y)
{
	// change to image coordinates
	x -= drumkit_image_x;
	y -= drumkit_image_y;

	auto index = pos_to_colour_index(x, y);
	if(index == -1)
	{
		return;
	}

	auto const& instrument = to_instrument_name[index];
	if(!instrument.empty())
	{
		++settings.audition_counter;
		settings.audition_instrument = instrument;
		settings.audition_velocity = current_velocity;
	}
}

void DrumkitTab::highlightInstrument(int index)
{
	if(index != -1)
	{
		dggui::Painter painter(*this);
		const auto& colour = colours[index];
		//Colour colour(1.0f, 1.0f, 0.0f);
		auto const& positions = colour_index_to_positions[index];
		painter.draw(positions.begin(), positions.end(),
		             drumkit_image_x, drumkit_image_y, colour);
		shows_instrument_overlay = true;
	}
	else
	{
		shows_instrument_overlay = false;
	}
}

void DrumkitTab::updateVelocityLabel()
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << current_velocity;
	velocity_label.setText("Velocity: " + stream.str());
}

void DrumkitTab::updateInstrumentLabel(int index)
{
	current_instrument = (index == -1 ? "" : to_instrument_name[index]);
	instrument_name_label.setText("Instrument: " + current_instrument);
	instrument_name_label.resizeToText();
}

void DrumkitTab::init(const std::string& image_file,
                      const std::string& map_file)
{
	drumkit_image = std::make_unique<dggui::Image>(image_file);
	map_image = std::make_unique<dggui::Image>(map_file);

	// collect all colours and build lookup table
	auto const height = map_image->height();
	auto const width = map_image->width();

	colours.clear();
	pos_to_colour_index.assign(width, height, -1);
	colour_index_to_positions.clear();
	to_instrument_name.clear();

	for(std::size_t y = 0; y < map_image->height(); ++y)
	{
		for(std::size_t x = 0; x < map_image->width(); ++x)
		{
			auto colour = map_image->getPixel(x, y);

			if(colour.alpha() == 0.)
			{
				continue;
			}

			auto it = std::find(colours.begin(), colours.end(), colour);
			int index = std::distance(colours.begin(), it);

			if(it == colours.end())
			{
				// XXX: avoids low alpha values due to feathering of edges
				colours.emplace_back(colour.red(), colour.green(), colour.blue(), 178);
				colour_index_to_positions.emplace_back();
			}

			pos_to_colour_index(x, y) = index;
			colour_index_to_positions[index].emplace_back(x, y);
		}
	}

	// set instrument names
	to_instrument_name.resize(colours.size());
	for(std::size_t i = 0; i < colours.size(); ++i)
	{
		for(auto const& pair: colour_instrument_pairs)
		{
			if(pair.colour == colours[i])
			{
				to_instrument_name[i] = pair.instrument;
			}
		}
	}

	imageChangeNotifier(drumkit_image->isValid());
}

void DrumkitTab::drumkitFileChanged(const std::string& drumkit_file)
{
	if(drumkit_file.empty())
	{
		return;
	}

	DrumkitDOM dom;
	if(parseDrumkitFile(drumkit_file, dom))
	{
		colour_instrument_pairs.clear();
		for(const auto& clickmap : dom.metadata.clickmaps)
		{
			if(clickmap.colour.size() != 6)
			{
				continue;
			}

			try
			{
				auto hex_colour = std::stoul(clickmap.colour, nullptr, 16);
				float red   = (hex_colour >> 16 & 0xff) / 255.0f;
				float green = (hex_colour >>  8 & 0xff) / 255.0f;
				float blue  = (hex_colour >>  0 & 0xff) / 255.0f;
				dggui::Colour colour(red, green, blue);
				colour_instrument_pairs.push_back({colour, clickmap.instrument});
			}
			catch(...)
			{
				// Not valid hex number
			}
		}

		std::string path = getPath(drumkit_file);
		init(path + "/" + dom.metadata.image, path + "/" + dom.metadata.image_map);
	}
}

} // GUI::
