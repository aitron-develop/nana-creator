/*
 *      Nana C++ Library - Creator
 *      Author: besh81
 */
#include "creator.h"
#include "version.h"
#include <nana/gui/filebox.hpp>
#include <iostream>
#include "pugixml/pugixml.hpp"
#include "guimanager.h"
#include "filemanager.h"
#include "updater.h"
#include "inifile.h"
#include "codegenerator.h"
#include "scrollablecanvas.h"
#include "propertiespanel.h"
#include "assetspanel.h"
#include "objectspanel.h"
#include "itemseditorpanel.h"
#include "new_project.h"
#include "info.h"


 // toolbar
#define TB_NEW					0
#define TB_LOAD					1
#define TB_SAVE					2
#define TB_SAVE_AS				3
 //
#define TB_UNDO					5
#define TB_REDO					6
 //
#define TB_GENERATE				8
 //
#define TB_DELETE				10
 //
#define TB_UP					12
#define TB_DOWN					13
#define TB_MOVE_INTO			14
	#define TB_MOVE_INTO_FIELD			0
	#define TB_MOVE_INTO_GRID			1
	#define TB_MOVE_INTO_PANEL			2
 //
#define TB_CUT					16
#define TB_COPY					17
#define TB_PASTE				18
// >> go-right >>
#define TB_SETTINGS				19
#define TB_INFO					20


guimanager*					p_gui_mgr;	// manage all the gui elements
nana::filebox::path_type	prj_name;

extern inifile				g_inifile;



void creator::enable_gui(bool state, bool new_load, bool undo, bool redo)
{
	_tb.enable(TB_NEW, state ? true : new_load);
	_tb.enable(TB_LOAD, state ? true : new_load);
	_tb.enable(TB_SAVE, state);
	_tb.enable(TB_SAVE_AS, state);
	_tb.enable(TB_UNDO, state ? undo : false);
	_tb.enable(TB_REDO, state ? redo : false);
	_tb.enable(TB_GENERATE, state);
	_tb.enable(TB_DELETE, state);
	_tb.enable(TB_UP, state);
	_tb.enable(TB_DOWN, state);
	_tb.enable(TB_MOVE_INTO, state);
	_tb.enable(TB_CUT, state);
	_tb.enable(TB_COPY, state);
	_tb.enable(TB_PASTE, state);
	_tb.enable(TB_SETTINGS, state ? true : new_load);
	_tb.enable(TB_INFO, state ? true : new_load);
}


bool creator::load_xml(const std::string& filename)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());

	if(!result)
	{
		std::cout << "XML [" << filename << "] parsed with errors\n";
		std::cout << "Error description: " << result.description() << "\n";
		std::cout << "Error offset: " << result.offset << " (error at [..." << result.offset << "]\n\n";
		return false;
	}

	// read root node
	pugi::xml_node root = doc.child(NODE_ROOT);
	if(root.empty())
	{
		std::cout << "Error missing root node: " << NODE_ROOT << "\n";
		return false;
	}

	// update project file if necessary
	updater(&root);

	// deserialize the XML structure
	return p_gui_mgr->deserialize(&root);
}


bool creator::save_xml(const std::string& filename)
{
	pugi::xml_document doc;

	// append root node
	pugi::xml_node root = doc.append_child(NODE_ROOT);
	root.append_attribute("version") = CREATOR_VERSION;

	p_gui_mgr->serialize(&root);

	//doc.print(std::cout); //TEMP debug
	return doc.save_file(filename.c_str());
}


bool creator::generate_cpp()
{
	codegenerator cpp;
	cpp.generate(handle(), p_gui_mgr->get_root()->child, prj_name.parent_path().string());
	cpp.print(std::cout);
	return true;
}


void creator::sb_clear()
{
	_sb.caption("");
}


void creator::sb_set(const std::string& str)
{
	_sb.caption(str);
}


void creator::_init_ctrls()
{
	caption(CREATOR_TITLE);

#ifdef __RELEASE
	nana::API::zoom_window(*this, true);
#endif

	// toolbar
	_tb.events().selected([this](const nana::arg_toolbar & arg)
	{
		if(arg.button == TB_NEW) // new project
		{
			// confirmation message
			if(prj_name != "" && p_gui_mgr->modified())
			{
				nana::msgbox m(*this, "Save project", nana::msgbox::yes_no_cancel);
				m.icon(m.icon_warning);
				m << "Do you want to save the current project?";
				auto ret = m();
				if(ret == nana::msgbox::pick_cancel)
					return;
				else if(ret == nana::msgbox::pick_yes)
					save_xml(prj_name.string());
			}

			// update window caption
			caption(CREATOR_TITLE);

			p_gui_mgr->clear();
			prj_name = "";

			new_project dlg(*this);
			dlg.modality();

			if(dlg.return_val() == nana::msgbox::pick_ok)
			{
				prj_name = dlg.get_filename();

				// update window caption
				std::string title = CREATOR_TITLE;
				if(!prj_name.string().empty())
				{
					title.append(" - ");
					title.append(prj_name.filename().string());
				}
				caption(title);

				p_gui_mgr->new_project(dlg.get_ctrl_type(), dlg.get_projectname());
				
				// crea file di progetto
				save_xml(prj_name.string());
			}
		}
		else if(arg.button == TB_LOAD) // load project
		{
			// confirmation message
			if(prj_name != "" && p_gui_mgr->modified())
			{
				nana::msgbox m(*this, "Save project", nana::msgbox::yes_no_cancel);
				m.icon(m.icon_warning);
				m << "Do you want to save the current project?";
				auto ret = m();
				if(ret == nana::msgbox::pick_cancel)
					return;
				else if(ret == nana::msgbox::pick_yes)
					save_xml(prj_name.string());
			}

			// update window caption
			caption(CREATOR_TITLE);

			p_gui_mgr->clear();
			prj_name = "";

			nana::filebox fb(*this, true);
			fb.add_filter("Nana Creator Project (*." PROJECT_EXT ")", "*." PROJECT_EXT);
			fb.add_filter("All Files (*.*)", "*.*");

		#if defined(NANA_WINDOWS)
			// solve the problem with lpstrinitialdir
			auto p = equalize_path(g_inifile.load_project_dir(), '/', '\\');
			fb.init_file(p.empty() ? "./" : p + "\\.");
		#else
			fb.init_path(equalize_path(g_inifile.load_project_dir()));
		#endif

			auto paths = fb.show();
			if(!paths.empty())
			{
				// save load_project folder
				prj_name = paths[0];
				if(prj_name.parent_path().string() != g_inifile.load_project_dir())
					g_inifile.load_project_dir(prj_name.parent_path().string(), true);

				if(!load_xml(prj_name.string()))
					prj_name = "";

				// update window caption
				std::string title = CREATOR_TITLE;
				if(!prj_name.string().empty())
				{
					title.append(" - ");
					title.append(prj_name.filename().string());
				}
				caption(title);
			}
		}
		else if(arg.button == TB_SAVE) // save project
		{
			if(prj_name == "")
				return;

			save_xml(prj_name.string());
		}
		else if(arg.button == TB_SAVE_AS) // save project as
		{
			if(prj_name == "")
				return;

			nana::filebox fb(*this, false);
			fb.add_filter("Nana Creator Project (*." PROJECT_EXT ")", "*." PROJECT_EXT);
			fb.init_file(prj_name.string());

			auto paths = fb.show();
			if(!paths.empty())
			{
				prj_name = paths[0];
				save_xml(prj_name.string());

				// update window caption
				std::string title = CREATOR_TITLE;
				title.append(" - ");
				title.append(prj_name.filename().string());
				caption(title);
			}
		}
		else if(arg.button == TB_UNDO) // undo
		{
			p_gui_mgr->undo();
		}
		else if(arg.button == TB_REDO) // redo
		{
			p_gui_mgr->redo();
		}
		else if(arg.button == TB_GENERATE) // generate code
		{
			if(prj_name == "")
				return;

			generate_cpp();
		}
		else if(arg.button == TB_DELETE) // delete current selection
		{
			p_gui_mgr->deleteselected();
		}
		else if(arg.button == TB_UP) // move up current selection
		{
			p_gui_mgr->moveupselected();
		}
		else if(arg.button == TB_DOWN) // move down current selection
		{
			p_gui_mgr->movedownselected();
		}
		else if(arg.button == TB_CUT) // cut current selection
		{
			p_gui_mgr->cutselected();
		}
		else if(arg.button == TB_COPY) // copy current selection
		{
			p_gui_mgr->copyselected();
		}
		else if(arg.button == TB_PASTE) // paste into/after current selection
		{
			p_gui_mgr->pasteselected();
		}
		else if(arg.button == TB_INFO) // info
		{
			info dlg(*this);
			dlg.modality();
		}
	});

	// toolbar - move into dropdown
	auto ip = _tb.at(TB_MOVE_INTO);
	ip.dropdown_answerer(TB_MOVE_INTO_FIELD, [this](const nana::toolbar::item_proxy& ip)
		{
			p_gui_mgr->moveintofield();
		});
	ip.dropdown_answerer(TB_MOVE_INTO_GRID, [this](const nana::toolbar::item_proxy& ip)
		{
			p_gui_mgr->moveintogrid();
		});
	ip.dropdown_answerer(TB_MOVE_INTO_PANEL, [this](const nana::toolbar::item_proxy& ip)
		{
			p_gui_mgr->moveintopanel();
		});


	// adi
	_adi_place.bind(_adi_panel);
	//
	_adi_place.scheme().caption_bg = nana::color(64, 86, 141);
	_adi_place.scheme().caption_highlight = nana::color(200, 92, 92);

	nana::paneinfo pinfo;

	auto assets = new assetspanel(_adi_panel);
	pinfo = _adi_place.add_pane("A", assets, "Assets");
	pinfo.show_close(false);
	_adi_place.update_pane(pinfo);

	auto objects = new objectspanel(_adi_panel);
	pinfo = _adi_place.add_pane("O", objects, nana::dockposition::down, "Objects");
	pinfo.show_close(false);
	_adi_place.update_pane(pinfo);

	auto canvas = new scrollablecanvas(_adi_panel);
	nana::paneinfo piC("C");
	piC.show_caption(false);
	piC.center(true);
	piC.weight(55);
	_adi_place.add_pane(piC, canvas, nana::dockposition::right);

	auto properties = new propertiespanel(_adi_panel);
	pinfo = _adi_place.add_pane("P", properties, nana::dockposition::right, "Properties");
	pinfo.show_close(false);
	_adi_place.update_pane(pinfo);

	auto itemseditor = new itemseditorpanel(_adi_panel);
	pinfo = _adi_place.add_pane("I", itemseditor, "P", nana::dockposition::down, "Items editor");
	pinfo.show_close(false);
	_adi_place.update_pane(pinfo);

	_adi_place.collocate();


	p_gui_mgr = new guimanager(*this);
	p_gui_mgr->registerEnableGUI([this](bool state, bool new_load, bool undo, bool redo)
		{
			enable_gui(state, new_load, undo, redo);
		});
	p_gui_mgr->registerSetStatusbar([this](const std::string& str)
		{
			sb_set(str);
		});
	p_gui_mgr->init(properties, assets, objects, itemseditor, canvas);
	

	// statusbar
	sb_set("Ready");


	_place.collocate();
}


void creator::_destroy_ctrls()
{
	p_gui_mgr->clear();
	delete p_gui_mgr;
}
