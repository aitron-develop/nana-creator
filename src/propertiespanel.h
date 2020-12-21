/*****************************************************
 *	C++ code generated with Nana Creator (0.28.0)
 *	GitHub repo: https://github.com/besh81/nana-creator
 *
 * PLEASE EDIT ONLY INSIDE THE TAGS:
 *		//<*tag
 *			user code
 *		//*>
*****************************************************/

#ifndef PROPERTIESPANEL_H
#define PROPERTIESPANEL_H

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include "nana_extra/propertygrid.h"

//<*includes
#include "ctrls/property.h"
//*>


class propertiespanel
	: public nana::panel<true>
{
public:
	propertiespanel() = default;

	propertiespanel(nana::window wd, const nana::rectangle& r = {}, bool visible = true)
		: nana::panel<true>(wd, r, visible)
	{
		this->create(wd, r, visible);
	}

	~propertiespanel()
	{
		//<*dtor

		//*>
	}

	bool create(nana::window wd, const nana::rectangle& r = {}, bool visible = true)
	{
		if(!nana::panel<true>::create(wd, r, visible))
			return false;

		init_();

		//<*ctor
		_init_ctrls();
		//*>

		return true;
	}


private:
	void init_()
	{
		_place.bind(*this);
		_place.div("vert <weight=45 margin=3 <weight=42 margin=5 icon><vert <gap=10 arrange=[45,variable] type><gap=10 arrange=[45,variable] name>>><propgrid>");
		bgcolor(nana::color(204,213,240));
		// _pic
		_pic.create(*this);
		_place["icon"] << _pic;
		_pic.bgcolor(this->bgcolor());
		_pic.align(static_cast<nana::align>(1), static_cast<nana::align_v>(1));
		// _type_lbl
		_type_lbl.create(*this);
		_place["type"] << _type_lbl;
		_type_lbl.bgcolor(nana::color(212,208,200));
		_type_lbl.caption("Type");
		_type_lbl.transparent(true);
		_type_lbl.text_align(static_cast<nana::align>(2), static_cast<nana::align_v>(1));
		// _type
		_type.create(*this);
		_place["type"] << _type;
		_type.bgcolor(nana::color(212,208,200));
		_type.caption("");
		_type.transparent(true);
		_type.text_align(static_cast<nana::align>(0), static_cast<nana::align_v>(1));
		// _name_lbl
		_name_lbl.create(*this);
		_place["name"] << _name_lbl;
		_name_lbl.bgcolor(nana::color(212,208,200));
		_name_lbl.caption("Name");
		_name_lbl.transparent(true);
		_name_lbl.text_align(static_cast<nana::align>(2), static_cast<nana::align_v>(1));
		// _name
		_name.create(*this);
		_place["name"] << _name;
		_name.multi_lines(false);
		// _propgrid
		_propgrid.create(*this);
		_place["propgrid"] << _propgrid;

		_place.collocate();
	}


protected:
	nana::place _place;
	nana::picture _pic;
	nana::label _type_lbl;
	nana::label _type;
	nana::label _name_lbl;
	nana::textbox _name;
	nana::propertygrid _propgrid;


	//<*declarations
public:
	void name_changed(std::function<void(const std::string&)> f);
	void property_changed(std::function<void(const std::string&)> f)
	{
		_property_changed_f = f;
	}

	void set(ctrls::properties_collection* properties);


private:
	void _init_ctrls();

	ctrls::properties_collection* _properties{ nullptr };
	nana::paint::image _img;

	bool _grid_setup{ false };

	std::function<void(const std::string&)> _property_changed_f;
	//*>
};

#endif //PROPERTIESPANEL_H

