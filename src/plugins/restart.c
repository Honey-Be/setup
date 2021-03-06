/*
 *  restart.c for Frugalware setup
 * 
 *  Copyright (c) 2005 by Miklos Vajna <vmiklos@frugalware.org>
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
#include <sys/mount.h>
#ifdef DIALOG
    #include <dialog.h>
#endif
#ifdef GTK
    #include <gtk/gtk.h>
#endif

#include <setup.h>
#include <util.h>
#include "common.h"

plugin_t plugin =
{
	"restart",
	desc,
	95,
	run,
	NULL // dlopen handle
};

plugin_t *info()
{
	return &plugin;
}

char *desc()
{
	return _("Restarting your computer");
}

static void umount_targets()
{
	FILE *fp;
	char *mp, *ptr;

	if((fp = fopen("/proc/mounts", "r")))
	{
		char line[256];
		while(!feof(fp))
		{
			if(!fgets(line, 255, fp))
				break;
			if(!strstr(line, "root"))
			{
				if(!(mp = strchr(line, ' ')))
					break;
				if((ptr = strchr(++mp, ' ')))
					*ptr='\0';
				umount(mp);
			}
		}
	}
	fclose(fp);
}

int run(GList **config)
{
	char *version = get_version();

	eject((char*)data_get(*config, "srcdev"), SOURCEDIR);

	umount_targets();
	dlg_put_backtitle();
	dlg_clear();
	dialog_msgbox(_("Setup complete"), g_strdup_printf(_("System "
		"installation and configuration is successfully completed. We "
		"hope that you will enjoy %s. Please remove "
		"the media from your drive and press ENTER to reboot."),
		version), 0, 0, 1);
	FREE(version);
	return(0);
}
