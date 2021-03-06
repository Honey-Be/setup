/*
 *  formatdisk.c for Frugalware setup
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
#include <dialog.h>
#include <parted/parted.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <blkid.h>

#include <setup.h>
#include <util.h>
#include "common.h"

#define BLKGETSIZE64 _IOR(0x12,114,size_t)

plugin_t plugin =
{
	"formatdisk",
	desc,
	40,
	run,
	NULL // dlopen handle
};

plugin_t *info()
{
	return &plugin;
}

char *desc()
{
	return _("Formatting the selected partitions");
}

GList *parts=NULL;
GList *partschk=NULL;
char *findmount(char *dev, int mode);
int detect_parts(int noswap);

int partdetails(PedPartition *part, int noswap)
{
	char *pname, *ptype, *ptr;
	PedFileSystemType *type;

	if ((type = ped_file_system_probe (&part->geom)) == NULL)
		// prevent a nice segfault
		ptype = strdup("unknown");
	else
		ptype = (char*)type->name;

	if(noswap && !strncmp("linux-swap", ptype, 10))
		return(0);

	pname = ped_partition_get_path(part);

	// remove the unnecessary "p-1" suffix from sw raid device names
	if((ptr = strstr(pname, "p-1"))!=NULL)
		*ptr='\0';

	// for dialog menus
	parts = g_list_append(parts, pname);
	ptr = fsize(part->geom.length);
	parts = g_list_append(parts, g_strdup_printf("%s\t%s", ptr, ptype));
	// for dialog checklists
	partschk = g_list_append(partschk, pname);
	partschk = g_list_append(partschk, g_strdup_printf("%s %s", ptr, ptype));
	FREE(ptr);
	partschk = g_list_append(partschk, strdup("Off"));

	return(0);
}

int listparts(PedDisk *disk, int noswap)
{
	PedPartition *part = NULL;
	PedPartition *extpart = NULL;

	if(ped_disk_next_partition(disk, NULL)==NULL)
		// no partition detected
		return(1);
	for(part=ped_disk_next_partition(disk, NULL);
		part!=NULL;part=part->next)
	{
		if((part->num>0) && (part->type != PED_PARTITION_EXTENDED) && !ped_partition_get_flag(part, PED_PARTITION_RAID))
			partdetails(part, noswap);
		if(part->type == PED_PARTITION_EXTENDED)
			for(extpart=part->part_list;
				extpart!=NULL;extpart=extpart->next)
				if(extpart->num>0 && !ped_partition_get_flag(extpart, PED_PARTITION_RAID))
					partdetails(extpart, noswap);
	}
	return(0);
}

int detect_raids()
{
	FILE *fp;
	char *line, *ptr;
	PedDevice *dev = NULL;
	PedDisk *disk = NULL;
	PedPartition *part = NULL;

	if ((fp = fopen("/proc/mdstat", "r"))== NULL)
	{
		//perror("Could not open output file for writing");
		return(-1);
	}
	MALLOC(line, PATH_MAX);
	while(!feof(fp))
	{
		if(fgets(line, PATH_MAX, fp) == NULL)
			break;
		if(strstr(line, "md")==line)
		{
			ptr = line;
			while(*ptr!=' ')
				ptr++;
			*ptr='\0';
			dev = ped_device_get(g_strdup_printf("/dev/%s", line));
			disk = ped_disk_new_fresh(dev, ped_disk_type_get ("loop"));
			if(disk)
			{
				part=ped_disk_next_partition(disk, NULL);
				partdetails(part, 0);
			}
		}
	}
	FREE(line);
	fclose(fp);
	return(0);
}

GList *selswap(void)
{
	char **arraychk;
	GList *partlist;

	arraychk = glist2dialog(partschk);

	dialog_vars.backtitle=gen_backtitle(_("Setting up swap space"));
	dlg_put_backtitle();
	dlg_clear();
	partlist = fw_checklist(_("Setting up swap partitions"),
		_("Please select which swap partitions you want Frugalware "
		"to use:"), 0, 0, 0, g_list_length(partschk)/3, arraychk,
		FLAG_CHECK);
	return(partlist);
}

char *selmkswapmode(char *dev)
{
	int modenum=2;
	char *ret=NULL;
	char *title=NULL;
	char *msg=NULL;
	char *modes[] =
	{
		"format", _("Quick format with no bad block checking"),
		"check", _("Slow format that checks for bad blocks")
	};

	dialog_vars.backtitle=gen_backtitle(_("Formatting partitions"));
	dlg_put_backtitle();
	dlg_clear();
	title = g_strdup_printf(_("Format %s"), dev);
	msg = g_strdup_printf(_("If %s has not been formatted, you should "
		"format it.\n"
		"NOTE: This will erase all data on %s. Would you like to "
		"format this partition?"), dev, dev);
	if (fw_menu(title,msg,0,0,0,modenum,modes) != -1)
		ret = dialog_vars.input_result;
	FREE(title);
	FREE(msg);

	return ret;
}

char *selformatmode(char *dev)
{
	int modenum=3;
	char *ret=NULL;
	char *title=NULL;
	char *msg=NULL;
	char *modes[] =
	{
		"format", _("Quick format with no bad block checking"),
		"check", _("Slow format that checks for bad blocks"),
		"noformat", _("Do not format, just mount the partition")
	};

	dialog_vars.backtitle=gen_backtitle(_("Formatting partitions"));
	dlg_put_backtitle();
	dlg_clear();
	title = g_strdup_printf(_("Format %s"), dev);
	msg = g_strdup_printf(_("If %s has not been formatted, you should "
		"format it.\n"
		"NOTE: This will erase all data on %s. Would you like to "
		"format this partition?"), dev, dev);
	if (fw_menu(title,msg,0,0,0,modenum,modes) != -1)
		ret = dialog_vars.input_result;
	FREE(title);
	FREE(msg);

	return ret;
}

char *selfs(char *dev)
{
#ifndef ARCH_PPC
	int fsnum=7;
#else
	int fsnum=4;
#endif
	char *fss[] =
	{
		"ext2", _("Standard Linux ext2fs filesystem"),
		"ext3", _("Journaling version of the ext2 filesystem"),
		"ext4", _("The evolution of the ext3 filesystem"),
#ifndef ARCH_PPC
		"reiserfs", _("Hans Reiser's journaling filesystem"),
#endif
		"jfs", _("Journaled File System"),
		"xfs", _("SGI's journaling filesystem"),
		"btrfs", _("B-tree file system (EXPERIMENTAL)")
	};

	dialog_vars.backtitle=gen_backtitle(_("Formatting partitions"));
	dlg_put_backtitle();
	dlg_clear();
	dialog_vars.default_item=strdup("ext4");
	if(fw_menu(g_strdup_printf(_("Selecting filesystem for %s"), dev),
		g_strdup_printf(_("Please select the type of filesystem to "
		"use for %s."), dev), 0, 0, 0, fsnum, fss) == -1)
		return(NULL);
	FREE(dialog_vars.default_item);

	return(strdup(dialog_vars.input_result));
}

static char* get_uuid(char *device)
{
	char path[PATH_MAX];
	int fd;
	blkid_probe pr = NULL;
	uint64_t size;
	const char *uuid;
	char *ret;

	if(!device || !strlen(device))
		return NULL;

	fd = open(device, O_RDONLY);

	if (fd < 0)
		return NULL;

	pr = blkid_new_probe();
	blkid_probe_set_request (pr, BLKID_PROBREQ_UUID);
	ioctl(fd, BLKGETSIZE64, &size);
	blkid_probe_set_device(pr, fd, 0, size);
	blkid_do_safeprobe(pr);
	blkid_probe_lookup_value(pr, "UUID", &uuid, NULL);
	snprintf(path, PATH_MAX, "/dev/disk/by-uuid/%s", uuid);
	ret = strdup(path);
	blkid_free_probe(pr);
	close(fd);

	return ret;
}

int doswap(GList *partlist, GList **config)
{
	char *fn, *item, *ptr;
	FILE* fp;
	int i;

	// create an initial fstab
	fn = strdup("/tmp/setup_XXXXXX");
	mkstemp(fn);
	if ((fp = fopen(fn, "wb")) == NULL)
	{
		perror(_("Could not open output file for writing"));
		FREE(fn);
		return(-1);
	}
	fprintf(fp, "%-16s %-16s %-11s %-16s %-3s %s\n",
		"none", "/proc", "proc", "defaults", "0", "0");
	fprintf(fp, "%-16s %-16s %-11s %-16s %-3s %s\n",
		"none", "/sys", "sysfs", "defaults", "0", "0");
	fprintf(fp, "%-16s %-16s %-11s %-16s %-3s %s\n",
		"devpts", "/dev/pts", "devpts", "gid=5,mode=620", "0", "0");
	fprintf(fp, "%-16s %-16s %-11s %-16s %-3s %s\n",
		"usbfs", "/proc/bus/usb", "usbfs", "devgid=23,devmode=664", "0", "0");
	fprintf(fp, "%-16s %-16s %-11s %-16s %-3s %s\n",
		"tmpfs", "/dev/shm", "tmpfs", "defaults", "0", "0");

	// format the partitions
	for (i=0; i<g_list_length(partlist); i++)
	{
		char *tmp;
		dialog_vars.input_result[0]='\0';
		item = strdup((char*)g_list_nth_data(partlist, i));
		ptr = selmkswapmode(item);
		if(ptr == NULL)
			return(-1);
		if(!strcmp("format", ptr))
		{
			tmp = g_strdup_printf(_("Formatting %s as a swap "
						"partition"), item);
			fw_info(_("Formatting swap partition"), tmp);
			FREE(tmp);
			tmp = g_strdup_printf("%s %s", MKSWAP, item);
			fw_system(tmp);
			FREE(tmp);
		}
		else if (!strcmp("check", ptr))
		{
			tmp = g_strdup_printf(_("Formatting %s as a swap "
						"partition and checking for bad blocks"), item);
			fw_info(_("Formatting swap partition"), tmp);
			FREE(tmp);
			tmp = g_strdup_printf("%s -c %s", MKSWAP, item);
			fw_system(tmp);
			FREE(tmp);
		}
		ptr = g_strdup_printf("%s %s", SWAPON, item);
		fw_system(ptr);
		FREE(ptr);
		char *uuid = get_uuid(item);
		fprintf(fp, "%-16s %-16s %-11s %-16s %-3s %s\n",
			uuid, "swap", "swap", "defaults", "0", "0");
		free(uuid);
	}

	fclose(fp);

	// save fstab location for later
	data_put(config, "fstab", fn);
	// re-detect parts so that new swap partitions will recognized as swap
	// partitions
	detect_parts(0);
	return(0);
}

char *selrootdev()
{
	char **array;

	array = glist2dialog(parts);
	dialog_vars.backtitle=gen_backtitle(_("Setting up the root partition"));
	dlg_put_backtitle();
	if(fw_menu(_("Select the Linux installation partition"),
		_("Please select a partition from the following list to use "
		"for your root (/) partition. The following ones "
		"are available:"), 0, 0, 0, g_list_length(parts)/2, array) == -1)
		return(NULL);
	return(strdup(dialog_vars.input_result));
}

int mkfss(char *dev, char *fs, int check)
{
	char *opts=NULL;
	char *cmd=NULL;
	int ret=1;

	opts = strdup(check ? "-c" : "");

	fw_info(_("Formatting"), g_strdup_printf(check ?
		_("Creating %s filesystem on %s and checking for bad blocks") :
		_("Creating %s filesystem on %s"),
		fs, dev));
	umount(findmount(dev, 1));
	if(!strcmp(fs, "ext2"))
	{
		cmd = g_strdup_printf("mke2fs %s %s", opts, dev);
		ret = fw_system(cmd);
		goto cleanup;
	}
	else if(!strcmp(fs, "ext3"))
	{
		cmd = g_strdup_printf("mke2fs -j %s %s", opts, dev);
		ret = fw_system(cmd);
		goto cleanup;
	}
	else if(!strcmp(fs, "ext4"))
	{
		cmd = g_strdup_printf("mkfs.ext4 %s %s", opts, dev);
		ret = fw_system(cmd);
		goto cleanup;
	}
	else if(!strcmp(fs, "reiserfs"))
	{
		cmd = g_strdup_printf("echo y |mkreiserfs %s", dev);
		ret = fw_system(cmd);
		goto cleanup;
	}
	else if(!strcmp(fs, "jfs"))
	{
		cmd = g_strdup_printf("mkfs.jfs -q %s %s", opts, dev);
		ret = fw_system(cmd);
		goto cleanup;
	}
	else if(!strcmp(fs, "xfs"))
	{
		cmd = g_strdup_printf("mkfs.xfs -f %s", dev);
		ret = fw_system(cmd);
		goto cleanup;
	}
	else if(!strcmp(fs, "btrfs"))
	{
		cmd = g_strdup_printf("mkfs.btrfs %s", dev);
		ret = fw_system(cmd);
		goto cleanup;
	}
	// never reached
	cleanup:
	FREE(opts);
	if (cmd)
		FREE(cmd);
	return(ret);
}

int formatdev(char *dev)
{
	char *mode, *fs;
	int check=0;
	int ret=-1;

	dialog_vars.input_result[0]='\0';
	mode = selformatmode(dev);
	if(mode == NULL)
		return(-1);

	// if don't have to format, nothing to do
	if(!strcmp("noformat", mode))
		return(0);
	if(!strcmp("check", mode))
		check=1;

	fs = selfs(dev);
	if(fs != NULL)
	{
		ret = mkfss(dev, fs, check);
		FREE(fs);
	}

	return(ret);
}

// mode=0: fs, mode=1: mountpoint
char *findmount(char *dev, int mode)
{
	FILE *fp;
	char line[PATH_MAX], *ptr;
	int i;

	if ((fp = fopen("/proc/mounts", "r"))== NULL)
	{
		perror("Could not open output file for reading");
		return(NULL);
	}
	while(!feof(fp))
	{
		if(fgets(line, PATH_MAX, fp) == NULL)
			break;
		if(strstr(line, dev)==line)
		{
			ptr = strstr(line, " ")+1;
			if(!mode)
				ptr = strstr(ptr, " ")+1;
			for(i=0;*(ptr+i)!='\0';i++)
				if(*(ptr+i)==' ')
					*(ptr+i)='\0';
			fclose(fp);
			return(strdup(ptr));
		}
	}
	fclose(fp);
	return(NULL);
}

int mountdev(char *dev, char *mountpoint, GList **config)
{
	char *type=NULL;
	char *tmp=NULL;
	FILE* fp;

	// open fstab
	if ((fp = fopen((char*)data_get(*config, "fstab"), "a")) == NULL)
	{
		perror(_("Could not open output file for writing"));
		return(-1);
	}

	// mount
	tmp = g_strdup_printf("%s/%s", TARGETDIR, mountpoint);
	makepath(tmp);
	FREE(tmp);

	umount_if_needed(mountpoint);
	tmp = g_strdup_printf("mount %s %s/%s",
			dev, TARGETDIR, mountpoint);
	fw_system(tmp);
	FREE(tmp);
	// unlink a possible stale lockfile
	tmp = g_strdup_printf("%s/%s/tmp/pacman-g2.lck", TARGETDIR, mountpoint);
	unlink(tmp);
	FREE(tmp);

	// make fstab entry
	type = findmount(dev, 0);
	char *uuid = get_uuid(dev);
	fprintf(fp, "%-16s %-16s %-11s %-16s %-3s %s\n", uuid, mountpoint,
		type, "defaults", "1", "1");
	free(uuid);
	free(type);
	fclose(fp);
	return(0);
}

char *asktowhere(char *dev)
{
	while(1)
	{
		if(fw_inputbox(g_strdup_printf(_("Select mount point for %s"), dev),
		"You need to specify where you want the new partition mounted. "
		"For example, if you want to put it under /usr/local, then "
		"respond: /usr/local\n\nWhere would you like to mount this "
		"partition?", 0, 0, "", 0) == -1)
		return(NULL);
		if(!findmount(dialog_vars.input_result, 1))
			break;
	}
	return(strdup(dialog_vars.input_result));
}

char **parts2dialog(GList *list)
{
	int i;
	char **array, *ptr;

	MALLOC(array, g_list_length(list)*sizeof(char*));
	for (i=0; i<g_list_length(list); i++)
	{
		if(!(i%2))
		{
			ptr = findmount((char*)g_list_nth_data(list, i), 1);
			if(ptr!=NULL)
				array[i] = strdup(_("(in use)"));
			else
				array[i] = (char*)g_list_nth_data(list, i);
		}
		else
		{
			ptr = findmount((char*)g_list_nth_data(list, i-1), 1);
			if(ptr && (strlen(TARGETDIR) <= strlen(ptr)))
				array[i] = g_strdup_printf(_("%s on %s/\t%s"),
					(char*)g_list_nth_data(list, i-1), ptr+strlen(TARGETDIR),
					(char*)g_list_nth_data(list, i));
			else
				array[i] = g_strdup_printf("%s",
					(char*)g_list_nth_data(list, i));
		}
	}
	return(array);
}

PedExceptionOption peh(PedException* ex)
{
	return(PED_EXCEPTION_IGNORE);
}

int detect_parts(int noswap)
{
	PedDevice *dev = NULL;
	PedDisk *disk = NULL;

	if(parts)
	{
		g_list_free(parts);
		parts = NULL;
	}
	if(partschk)
	{
		g_list_free(partschk);
		partschk = NULL;
	}
	ped_device_free_all();
	chdir("/");

	ped_exception_set_handler(peh);
	ped_device_probe_all();

	if(ped_device_get_next(NULL)==NULL)
		// no disk detected already handled before, no need to inform
		// the user about this
		return(1);

	for(dev=ped_device_get_next(NULL);dev!=NULL;dev=dev->next)
	{
		if(dev->read_only)
			// we don't want to partition cds ;-)
			continue;
		disk = ped_disk_new(dev);
		if(disk)
			listparts(disk, noswap);
	}

	// software raids
	detect_raids();
	return(0);
}

int run(GList **config)
{
	GList *partlist;
	char **nrdevs, *ptr, *op, *np, *dest;
	int ret;
	char my_buffer[MAX_LEN + 1] = "";

	detect_parts(0);

	// select swap partitions to use
	partlist = selswap();

	// format swap partitions
	if(doswap(partlist, config) == -1)
		return(-1);

	// root partition
	ptr = selrootdev();
	if(ptr == NULL)
		return(-1);
	if(formatdev(ptr) == -1)
		return(-1);
	mountdev(ptr, "/", config);

	// move temporarily stuff to the final location
	chdir(TARGETDIR);
	np = g_strdup_printf("%s/%s", TARGETDIR, "/etc/profile.d");
	makepath(np);
	FREE(np);
	op = (char*)data_get(*config, "fstab");
	np = g_strdup_printf("%s/%s", TARGETDIR, "/etc/fstab");
	copyfile(op, np);
	unlink(op);
	chmod (np, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	data_put(config, "fstab", strdup(np));
	FREE(np);

	np = g_strdup_printf("%s/%s", TARGETDIR, "/etc/sysconfig");
	makepath(np);
	FREE(np);

	// so that 1) the user can't mount a partition as /dev because
	// it'll be used 2) install scriptlets will be able to do
	// >/dev/null
	makepath(TARGETDIR "/dev");
	makepath(TARGETDIR "/proc");
	makepath(TARGETDIR "/sys");
	fw_system("mount none -t devtmpfs " TARGETDIR "/dev");
	fw_system("mount none -t proc " TARGETDIR "/proc");
	fw_system("mount none -t sysfs " TARGETDIR "/sys");

	// non-root partitions
	dialog_vars.backtitle=gen_backtitle(_("Selecting other partitions"));
	while(1)
	{
		dialog_vars.input_result = my_buffer;
		nrdevs = parts2dialog(parts);
		dlg_put_backtitle();
		dlg_clear();
		dialog_vars.cancel_label = _("Continue");
		dialog_vars.input_result = my_buffer;
		dialog_vars.input_result[0]='\0';
		ret = dialog_menu(
		_("Select other Linux partitions for /etc/fstab"),
		_("You may use your other partitions to distribute your Linux "
		"system across more than one partition. Currently, you have "
		"only mounted your / partition. You might want to mount "
		"directories such as /boot, /home or /usr/local on separate "
		"partitions. You should not try to mount /usr, /etc, /sbin or "
		"/bin on their own partitions since they contain utilities "
		"needed to bring the system up and mount partitions. Also, "
		"do not reuse a partition that you've already entered before. "
		"Please select one of the partitions listed below, or if "
		"you're done, hit Continue."),
		0, 0, 0, g_list_length(parts)/2, nrdevs);
		dialog_vars.cancel_label = '\0';
		FREE(nrdevs);
		if (ret != DLG_EXIT_CANCEL)
		{
			if(!strcmp(_("(in use)"), dialog_vars.input_result))
				continue;
			ptr = strdup(dialog_vars.input_result);
			if(formatdev(ptr) == -1)
				return(-1);
			dest = asktowhere(ptr);
			if(dest == NULL)
				return(-1);
			mountdev(ptr, dest, config);
			FREE(dest);
			FREE(ptr);
		}
		else
			break;
	}

	makepath(g_strdup_printf("%s/%s", TARGETDIR, "/var/log"));
	np = g_strdup_printf("%s/%s", TARGETDIR, LOGFILE);
	copyfile(LOGFILE, np);
	unlink(LOGFILE);
	chmod (np, S_IRUSR|S_IWUSR);
	FREE(np);

	// disable caching for cds
	// this is needed here since when the cds is loaded we had no
	// formatted root partition
	if((char*)data_get(*config, "netinstall")==NULL)
	{
		char *pacbindir = g_strdup_printf("%s/frugalware-%s", SOURCEDIR, ARCH);
		char *ptr;

		ptr = g_strdup_printf("%s/var/cache/pacman-g2/pkg", TARGETDIR);
		makepath(ptr);
		FREE(ptr);
		disable_cache(pacbindir);
		FREE(pacbindir);
	}
	return(0);
}
