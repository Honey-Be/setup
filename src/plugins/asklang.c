/*
 *  asklang.c for Frugalware setup
 * 
 *  Copyright (c) 2005, 2007, 2008 by Miklos Vajna <vmiklos@frugalware.org>
 *  Copyright (c) 2005 by Christian Hamar alias krix <krics@linuxforum.hu>
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
 *  USA.
 */

#include <stdio.h>
#ifdef DIALOG
    #include <dialog.h>
#endif
#ifdef GTK
    #include <gtk/gtk.h>
    #include <gutil.h>
#endif
#include <stdlib.h>
#include <string.h>

#include <setup.h>
#include <util.h>
#include "common.h"

#define LANGSNUM 13
char *langs[] =
{
	"pt_BR", "Brazilian Portuguese / Portugu�s do Brasil",
	"cs_CZ", "Czech / Cesky",
	"da_DK", "Danish / Dansk",
	"nl_NL", "Dutch / Nederlands",
	"en_US", "English",
	"fr_FR", "French / Fran�ais",
	"de_DE", "German / Deutsch",
	"hu_HU", "Hungarian / Magyar",
        "id_ID", "Indonesian / Bahasa Indonesia",
        "it_IT", "Italian / Italiano",
	"ro_RO", "Romanian / Rom�n�",
	"sk_SK", "Slovak / Sloven�ina",
	"sv_SE", "Swedish / Svenska"
};

plugin_t plugin =
{
	"asklang",
	desc,
	01,
	run,
	NULL // dlopen handle
};

plugin_t *info()
{
	return &plugin;
}

char *desc()
{
	return _("Selecting language");
}

int setcharset(char *name, GList **config)
{
	char *ptr;

	//TODO: maybe there is a proper system call for this?
	ptr = g_strdup_printf("setfont %s", name);
	fw_end_dialog();
	fw_system(ptr);
	fw_init_dialog();
	FREE(ptr);
	// save the font for later usage
	data_put(config, "font", strdup(name));
	return(0);
}

int run(GList **config)
{
#ifdef GTK
	GtkWidget *label;
	GtkWidget *tv;
	GtkTreeIter iter;
	int i = 0;
#endif

#ifdef DIALOG
	dialog_vars.backtitle=gen_backtitle("Selecting language");
	dlg_put_backtitle();
	dlg_clear();
	dialog_vars.default_item=strdup("en_US");
	if(fw_menu("Please select your language",
		"Please select your language from the list. If your language "
		"is not in the list, you should probably choose English.",
		0, 0, 0, LANGSNUM, langs) == -1)
		return(-1);
	FREE(dialog_vars.default_item);

	LOG("selected language: '%s'", dialog_vars.input_result);
	setenv("LC_ALL", dialog_vars.input_result, 1);
	setenv("LANG",   dialog_vars.input_result, 1);
	setlocale(LC_ALL, dialog_vars.input_result);
	bindtextdomain("setup", "/usr/share/locale");
	
	if(!strcmp("en_US", dialog_vars.input_result))
		setenv("CHARSET", "iso-8859-1", 1);
	else if(!strcmp("es_AR", dialog_vars.input_result))
	{
		setenv("CHARSET", "iso-8859-1", 1);
		setcharset("lat1-16.psfu.gz", config);
	}
	else if(!strcmp("de_DE", dialog_vars.input_result))
		setenv("CHARSET", "iso-8859-15", 1);
	else if(!strcmp("fr_FR", dialog_vars.input_result))
		setenv("CHARSET", "iso-8859-15", 1);
        else if(!strcmp("id_ID", dialog_vars.input_result))
		setenv("CHARSET", "iso-8859-1", 1);
	else if(!strcmp("it_IT", dialog_vars.input_result))
	{
		setenv("CHARSET", "iso-8859-1", 1);
		setcharset("lat9w-16.psfu.gz", config);
	}
	else if(!strcmp("cs_CZ", dialog_vars.input_result))
	{
		setenv("CHARSET", "iso-8859-2", 1);
		setcharset("lat2-16.psfu.gz", config);
	}
	else if(!strcmp("hu_HU", dialog_vars.input_result))
	{
		setenv("CHARSET", "iso-8859-2", 1);
		setcharset("lat2a-16.psfu.gz", config);
	}
	else if(!strcmp("nl_NL", dialog_vars.input_result))
		setenv("CHARSET", "iso-8859-1", 1);
	else if(!strcmp("pl_PL", dialog_vars.input_result))
	{
		setenv("CHARSET", "iso-8859-2", 1);
		setcharset("lat2-16.psfu.gz", config);
	}
	else if(!strcmp("pt_PT", dialog_vars.input_result))
	{
		setenv("CHARSET", "iso-8859-1", 1);
		setcharset("lat1-16.psfu.gz", config);
	}
	else if(!strcmp("sk_SK", dialog_vars.input_result))
	{
		setenv("CHARSET", "iso-8859-2", 1);
		setcharset("lat2-16.psfu.gz", config);
	}
#endif

#ifdef GTK
	tv = create_asklang_view();
	gtk_container_add(GTK_CONTAINER(frame), tv);
	while (i <= LANGSNUM * 2) {
		gtk_list_store_append(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tv))), &iter);
		gtk_list_store_set(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tv))), &iter,
			0, langs[i], 1, langs[i+1], -1);
		i = i+2;
	}
	gtk_widget_show_all(tv);
#endif
	textdomain("setup");
	return(0);
}
