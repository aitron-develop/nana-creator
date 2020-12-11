/*
 *      Nana C++ Library - Creator
 *      Author: besh81
 */
#include "info.h"
#include "config.h"
#include "version.h"


void info::_init_ctrls()
{
	_ver.caption(CREATOR_VERSION);

	_thanks_txt.caption(
		"Jinhao:\t\tNana C++ Library and help\n"
		"kassane:\t\tCMAKE build\n"
		"ppetraki:\t\tCMAKE build\n"
		"szega83:\t\tbug fixing\n"
		"JamesBremner:\t\tpropertygrid improvements\n");


	//------------------
	// events
	//------------------

	// ok button
	_ok_btn.events().click([this]()
	{
		nana::API::close_window(handle());
	});


	// set focus
	_ok_btn.focus();
}
