/*
 *  partdisk.c for Frugalware setup
 *
 *  Copyright (c) 2005, 2006, 2007, 2008 by Miklos Vajna <vmiklos@frugalware.org>
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
#include <limits.h>
#include <string.h>
#ifdef DIALOG
    #include <dialog.h>
#endif
#ifdef GTK
    #include <gtk/gtk.h>
#endif
#include <parted/parted.h>

#include <setup.h>
#include <util.h>
#include "common.h"

plugin_t plugin =
{
	"partdisk",
	desc,
	35,
	run,
	NULL // dlopen handle
};

plugin_t *info()
{
	return &plugin;
}

char *desc()
{
	return _("Partitioning the disk drives");
}

int buggy_md0()
{
	FILE *fp;
	char line[256];

	fp = fopen("/proc/mdstat", "r");
	if(!fp)
		return(1);
	while(!feof(fp))
	{
		if(fgets(line, 255, fp) == NULL)
			break;
		if(!strncmp(line, "md", 2))
		{
			fclose(fp);
			return(0);
		}
	}
	return(1);
}

PedExceptionOption peh(PedException* ex)
{
	return(PED_EXCEPTION_IGNORE);
}

GList *listparts(void)
{
	GList *devs=NULL;
	PedDevice *dev=NULL;
	char *ptr;

	// silly raid autodetect, md0 always created
	if(buggy_md0())
		unlink("/dev/md0");

	ped_exception_set_handler(peh);
	ped_device_probe_all();

	if(ped_device_get_next(NULL)==NULL)
		return(NULL);

	for(dev=ped_device_get_next(NULL);dev!=NULL;dev=dev->next)
	{
		if(dev->read_only)
			// we don't want to partition cds ;-)
			continue;
		devs = g_list_append(devs, dev->path);
		ptr = fsize(dev->length);
		devs = g_list_append(devs, g_strdup_printf("%s\t%s", ptr, dev->model));
		FREE(ptr);
	}

	return(devs);
}

char *selpartsw()
{
#ifndef ARCH_PPC
	char *sws[] =
	{
		"fdisk", _("The traditional partitioning program for Linux"),
		"gdisk", _("Works like fdisk, but uses GPT instead of MBR."),
		"parted", _("A partition manipulation program")
	};
#else
	char *sws[] =
	{
		"mac-fdisk", _("Apple disk partitioning utility"),
		"parted", _("A partition manipulation program")
	};
#endif
	/*
		Start with the size of the whole array of pointers.
		Divide by the size of each pointer to get the number of pointers.
		Divide by two to get the actual number of partition programs.
	*/
	int swnum = sizeof(sws) / sizeof(*sws) / 2;

	dialog_vars.backtitle=gen_backtitle(_("Creating partitions"));
	dlg_put_backtitle();
	dlg_clear();
	if(fw_menu(_("Select partitioning program"),
		_("Select the program you want to use for partitioning:"),
		0, 0, 0, swnum, sws) == -1)
		return(NULL);

	return(dialog_vars.input_result);
}

int raid_confirm(void)
{
#ifdef DIALOG
	int ret;
	dialog_vars.defaultno=1;
	dialog_vars.default_button=DLG_EXIT_CANCEL;
	ret = dialog_yesno(_("Want to create RAID partitions?"),
		_("Do you want to create software RAID partitions during "
		"partitioning?"), 0, 0);
	dialog_vars.default_button=DLG_EXIT_OK;
	dialog_vars.defaultno=0;
	if(ret==DLG_EXIT_OK)
	{
		LOG("selected 'enable' for raid support");
		return(1);
	}
	else
	{
		LOG("selected 'disable' for raid support");
		return(0);
	}
#endif
}

int run(GList **config)
{
	GList *lp;
	char **array;
	char path[PATH_MAX];
	char partsw[PATH_MAX];
	int ret;
	char my_buffer[MAX_LEN + 1] = "";
	int wantraid;
	char *ptr;

	if((lp = listparts())==NULL)
	{
		dialog_msgbox(_("Disk drive not found"), _("Sorry, no hard disk drives were found in this computer. Press ENTER to reboot."), 0, 0, 1);
		return(-1);
	}

	array = glist2dialog(lp);

	dialog_vars.backtitle=gen_backtitle(_("Creating partitions"));
	dlg_put_backtitle();
	wantraid = raid_confirm();

	while(1)
	{
		if(wantraid)
		{
			dialog_vars.extra_button=1;
			dialog_vars.extra_label=strdup(_("Create RAID"));
		}

		dlg_put_backtitle();
		dlg_clear();
		dialog_vars.cancel_label = strdup(_("Continue"));
		dialog_vars.input_result = my_buffer;
		dialog_vars.input_result[0]='\0';
		ret = dialog_menu(_("Select a hard disk to partition"),
		_("Please select a hard disk to partition. Once you are "
		"done, or in case you don't want to partition "
		"your disks at all, select 'Continue'. The "
		"following ones are available:"),
		0, 0, 0, g_list_length(lp)/2, array);
		dialog_vars.cancel_label = '\0';
		if(wantraid)
		{
			dialog_vars.extra_button=0;
			dialog_vars.extra_label[0]='\0';
		}
		if (ret == DLG_EXIT_OK)
		{
			strcpy(path, dialog_vars.input_result);
			dialog_vars.input_result[0]='\0';
			ptr = selpartsw();
			if(ptr == NULL)
				return(-1);
			strcpy(partsw, ptr);
			fw_end_dialog();
			fw_system_interactive(g_strdup_printf("%s %s", partsw, path));
			fw_init_dialog();
		}
		else if (ret == DLG_EXIT_EXTRA)
		{
			fw_end_dialog();
			fw_system_interactive(RAIDCONFIGSCRIPT);
			fw_init_dialog();
		}
		else
			break;
	}

	FREE(array);
	return(0);
}
