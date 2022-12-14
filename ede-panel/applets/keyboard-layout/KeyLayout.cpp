/*
 * $Id: KeyLayout.cpp 3411 2012-09-03 13:00:09Z karijes $
 *
 * Copyright (C) 2011-2012 Sanel Zukan
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "../../Applet.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <FL/Fl_Button.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/x.H>
#include <FL/Fl.H>

#include <stdio.h> /* needed for XKBrules.h */
#include <stdlib.h>

#ifdef HAVE_XKBRULES
# include <X11/XKBlib.h>
# include <X11/extensions/XKBgeom.h>
# include <X11/extensions/XKBrules.h>
#endif

#include <edelib/Resource.h>
#include <edelib/Debug.h>
#include <edelib/List.h>
#include <edelib/Directory.h>
#include <edelib/Nls.h>
#include <edelib/Run.h>
#include <edelib/ForeignCallback.h>
#include <edelib/Functional.h>

/* they should match names in ede-keyboard-conf */
#define PANEL_APPLET_ID "ede-keyboard"
#define CONFIG_NAME     "ede-keyboard"

EDELIB_NS_USING(Resource)
EDELIB_NS_USING(String)
EDELIB_NS_USING(list)
EDELIB_NS_USING(run_async)
EDELIB_NS_USING(foreign_callback_add)
EDELIB_NS_USING(foreign_callback_remove)
EDELIB_NS_USING(for_each)
EDELIB_NS_USING(RES_SYS_ONLY)

static Atom _XA_XKB_RF_NAMES_PROP_ATOM = 0;

class KeyLayout : public Fl_Button {
private:
	bool      should_show_flag;
	String    path, curr_layout;
	Fl_Image *img;

public:
	KeyLayout();
	~KeyLayout();
	void update_flag(bool read_config);
	void do_key_layout();
	int  handle(int e);
};

typedef list<KeyLayout*> KeyLayoutList;
static  KeyLayoutList    keylayout_objects;

#ifdef HAVE_XKBRULES
/* Xkb does not provide this */
static void xkbrf_names_prop_free(XkbRF_VarDefsRec &vd, char *tmp) {
	if(tmp)            free(tmp);
	if(vd.model)       XFree(vd.model);
	if(vd.layout)      XFree(vd.layout);
	if(vd.options)     XFree(vd.options);
	if(vd.variant)     XFree(vd.variant);
	if(vd.extra_names) XFree(vd.extra_names);
}
#endif

static void update_key_layout(KeyLayout *k) {
	k->do_key_layout();
	k->update_flag(false);
}

/* 
 * any layout changes on X will be reported here, executed either via ede-keyboard-conf or
 * another tool, so there are no needs to read layout from configuration file
 */
static int xkb_events(int e) {
	if(fl_xevent->xproperty.atom == _XA_XKB_RF_NAMES_PROP_ATOM) {
		/* TODO: lock this */
		for_each(update_key_layout, keylayout_objects);
	}

	return 0;
}

static void click_cb(Fl_Widget*, void*) {
	run_async("ede-keyboard-conf");
}

static void update_flag_cb(Fl_Window*, void *data) {
	KeyLayout *k = (KeyLayout*)data;
	k->update_flag(true);
	k->redraw();
}

KeyLayout::KeyLayout() : Fl_Button(0, 0, 30, 25) {
	should_show_flag = true;
	curr_layout      = "us"; /* default layout */
	img              = NULL;

	box(FL_FLAT_BOX);
	labelfont(FL_HELVETICA_BOLD);
	labelsize(10);
	label("??");
	align(FL_ALIGN_CLIP);

	tooltip(_("Current keyboard layout"));
	callback(click_cb);
	foreign_callback_add(window(), PANEL_APPLET_ID, update_flag_cb, this);

	path = Resource::find_data("icons/kbflags/21x14", RES_SYS_ONLY, NULL);

	do_key_layout();
	update_flag(true);

	/* TODO: lock this */
	keylayout_objects.push_back(this);

	/* with this, kb layout chages will be catched */
#if HAVE_XKBRULES
	if(!_XA_XKB_RF_NAMES_PROP_ATOM)
		_XA_XKB_RF_NAMES_PROP_ATOM = XInternAtom(fl_display, _XKB_RF_NAMES_PROP_ATOM, False);
#endif

	Fl::add_handler(xkb_events);
}

KeyLayout::~KeyLayout() {
	foreign_callback_remove(update_flag_cb);
}

void KeyLayout::update_flag(bool read_config) {
	if(read_config) {
		Resource r;

		if(r.load(CONFIG_NAME)) 
			r.get("Keyboard", "show_country_flag", should_show_flag, true);
	}

	/* remove previous image if we aren't going to show it */
	if(!should_show_flag)
		img = NULL;

	if(should_show_flag && !path.empty()) {
		/* construct image path that has the same name as layout */
		String full_path = path;

		full_path += E_DIR_SEPARATOR_STR;
		full_path += curr_layout;
		full_path += ".png";

		img = Fl_Shared_Image::get(full_path.c_str());
	}

	image(img);

	if(img)
		label(NULL);
	else
		label(curr_layout.c_str());
	redraw();
}

void KeyLayout::do_key_layout(void) {
#ifdef HAVE_XKBRULES
	char             *tmp = NULL;
	XkbRF_VarDefsRec  vd;

	if(XkbRF_GetNamesProp(fl_display, &tmp, &vd)) {
		/* do nothing if layout do not exists or the same was catched */
		if(!vd.layout || (curr_layout == vd.layout)) {
			xkbrf_names_prop_free(vd, tmp);
			return;
		}

		/* just store it, so update_flag() can do the rest */
		curr_layout = vd.layout;
		xkbrf_names_prop_free(vd, tmp);
	}
#endif
}

int KeyLayout::handle(int e) {
	switch(e) {
		case FL_ENTER:
			box(FL_THIN_UP_BOX);
			redraw();
			break;
		case FL_LEAVE:
			box(FL_FLAT_BOX);
			redraw();
			break;
		default:
			break;
	}

	return Fl_Button::handle(e);
}

EDE_PANEL_APPLET_EXPORT (
 KeyLayout, 
 EDE_PANEL_APPLET_OPTION_ALIGN_RIGHT,
 "Keyboard Layout applet",
 "0.1",
 "empty",
 "Sanel Zukan"
)
