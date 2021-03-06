/*
 *  select.c for Frugalware setup
 *
 *  Copyright (c) 2005 by Miklos Vajna <vmiklos@frugalware.org>
 *  Copyright (c) 2006 by Alex Smith <alex@alex-smith.me.uk>
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
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <pacman.h>

#include <setup.h>
#include <util.h>
#include "common.h"

plugin_t plugin =
{
	"select",
	desc,
	45,
	run,
	NULL // dlopen handle
};

plugin_t *info()
{
	return &plugin;
}

char *desc()
{
	return _("Selecting packages");
}

PM_DB *mydatabase;

void cb_db_register(const char *section, PM_DB *db)
{
	mydatabase = db;
}

int g_list_is_strin(char *needle, GList *haystack)
{
	int i;

	for(i=0; i<g_list_length(haystack); i++)
		if(g_list_nth_data(haystack, i) && !strcmp(g_list_nth_data(haystack, i), needle))
			return(1);
	return(0);
}

// 1: add pkgdesc and On for dialog; 0: don't add
GList* group2pkgs(GList *syncs, char *group, int dialog)
{
	PM_GRP *grp;
	PM_LIST *pmpkgs, *lp, *junk;
	GList *pkgs=NULL;
	GList *list=NULL;
	int i, optional=0, addpkg=1;
	char *ptr, *pkgname, *pkgfullname, *lang;

	// add the core group to the start of the base list
	if(!strcmp(group, "base"))
		list = group2pkgs(syncs, "core", dialog);

	// get language suffix
	lang = strdup(getenv("LANG"));
	ptr = rindex(lang, '_');
	*ptr = '\0';

	if(strlen(group) >= strlen(EXGRPSUFFIX) && !strcmp(group + strlen(group) - strlen(EXGRPSUFFIX), EXGRPSUFFIX))
		optional=1;

	for (i=0; i<g_list_length(syncs); i++)
	{
		grp = pacman_db_readgrp(g_list_nth_data(syncs, i), group);
		if(grp)
		{
			pmpkgs = pacman_grp_getinfo(grp, PM_GRP_PKGNAMES);
			for(lp = pacman_list_first(pmpkgs); lp; lp = pacman_list_next(lp))
				pkgs = g_list_append(pkgs, pacman_list_getdata(lp));
			break;
		}
	}
	if(pacman_trans_init(PM_TRANS_TYPE_SYNC, PM_TRANS_FLAG_NODEPS|PM_TRANS_FLAG_NOCONFLICTS, NULL, NULL, NULL) == -1)
	{
		fprintf(stderr, "failed to init transaction (%s)\n",
			pacman_strerror(pm_errno));
		return(NULL);
	}
	for (i=0; i<g_list_length(pkgs); i++)
		if(pacman_trans_addtarget(g_list_nth_data(pkgs, i)))
		{
			fprintf(stderr, "failed to add target '%s' (%s)\n",
				(char*)g_list_nth_data(pkgs, i), pacman_strerror(pm_errno));
			return(NULL);
		}

	if(pacman_trans_prepare(&junk) == -1)
	{
		fprintf(stderr, "failed to prepare transaction (%s)\n",
			pacman_strerror(pm_errno));
		return(NULL);
	}
	pmpkgs = pacman_trans_getinfo(PM_TRANS_PACKAGES);
	for(lp = pacman_list_first(pmpkgs); lp; lp = pacman_list_next(lp))
	{
		PM_SYNCPKG *sync = pacman_list_getdata(lp);
		PM_PKG *pkg = pacman_sync_getinfo(sync, PM_SYNC_PKG);
		//printf("%s\n", pacman_pkg_getinfo(pkg, PM_PKG_NAME));
		pkgname = pacman_pkg_getinfo(pkg, PM_PKG_NAME);
		pkgfullname = g_strdup_printf("%s-%s", (char*)pacman_pkg_getinfo(pkg, PM_PKG_NAME),
			(char*)pacman_pkg_getinfo(pkg, PM_PKG_VERSION));
		// enable by default the packages in the
		// frugalware repo + enable the
		// language-specific parts from
		// locale-extra
		addpkg = ((strcmp(getenv("LANG"), "en_US") &&
			!strcmp(group, "locale-extra") &&
			strlen(pkgname) >= strlen(lang) &&
			!strcmp(pkgname + strlen(pkgname) -
			strlen(lang), lang)) || !optional);
		if(!dialog && addpkg && !g_list_is_strin(pkgfullname, list))
			list = g_list_append(list, strdup(pkgfullname));
		if(dialog && !g_list_is_strin(pkgfullname, list))
		{
			list = g_list_append(list, strdup(pkgfullname));
			// TODO: PM_PKG_SIZE
			list = g_list_append(list,
				strdup(pacman_pkg_getinfo(pkg, PM_PKG_DESC)));
			if(addpkg)
				list = g_list_append(list,
					strdup("On"));
			else
				list = g_list_append(list,
					strdup("Off"));
		}
		FREE(pkgfullname);
	}
	pacman_trans_release();
	return(list);
}

char* categorysize(GList *syncs, char *category)
{
	int i;
	double size=0;
	PM_GRP *grp;
	PM_LIST *pmpkgs, *lp, *junk;
	GList *pkgs=NULL;

	for (i=0; i<g_list_length(syncs); i++)
	{
		grp = pacman_db_readgrp(g_list_nth_data(syncs, i), category);
		if(grp)
		{
			pmpkgs = pacman_grp_getinfo(grp, PM_GRP_PKGNAMES);
			for(lp = pacman_list_first(pmpkgs); lp; lp = pacman_list_next(lp))
				pkgs = g_list_append(pkgs, pacman_list_getdata(lp));
			break;
		}
	}
	if(pacman_trans_init(PM_TRANS_TYPE_SYNC, PM_TRANS_FLAG_NODEPS|PM_TRANS_FLAG_NOCONFLICTS, NULL, NULL, NULL) == -1)
	{
		fprintf(stderr, "failed to init transaction (%s)\n",
			pacman_strerror(pm_errno));
		return(NULL);
	}
	for (i=0; i<g_list_length(pkgs); i++)
		if(pacman_trans_addtarget(g_list_nth_data(pkgs, i)))
		{
			fprintf(stderr, "failed to add target '%s' (%s)\n",
				(char*)g_list_nth_data(pkgs, i), pacman_strerror(pm_errno));
			return(NULL);
		}

	if(pacman_trans_prepare(&junk) == -1)
	{
		fprintf(stderr, "failed to prepare transaction (%s)\n",
			pacman_strerror(pm_errno));
		return(NULL);
	}
	pmpkgs = pacman_trans_getinfo(PM_TRANS_PACKAGES);
	for(lp = pacman_list_first(pmpkgs); lp; lp = pacman_list_next(lp))
	{
		PM_SYNCPKG *sync = pacman_list_getdata(lp);
		PM_PKG *pkg = pacman_sync_getinfo(sync, PM_SYNC_PKG);
		size += (long)pacman_pkg_getinfo(pkg, PM_PKG_SIZE);
	}
	pacman_trans_release();

	size = (double)(size/1048576.0);
	if(size < 0.1)
		size=0.1;
	return(g_strdup_printf("%6.1f MB", size));
}

GList *selcat(PM_DB *db, GList *syncs)
{
	char *ptr;
	GList *catlist=NULL;
	char **arraychk;
	GList *ret;
	PM_LIST *lp;

	for(lp = pacman_db_getgrpcache(db); lp; lp = pacman_list_next(lp))
	{
		PM_GRP *grp = pacman_list_getdata(lp);

		ptr = (char *)pacman_grp_getinfo(grp, PM_GRP_NAME);

			if(!index(ptr, '-') && strcmp(ptr, "core"))
			{
				catlist = g_list_append(catlist, strdup(ptr));
				catlist = g_list_append(catlist,
					categorysize(syncs, ptr));
				catlist = g_list_append(catlist, strdup("On"));
			}
			else if(strstr(ptr, EXGRPSUFFIX))
			{
				catlist = g_list_append(catlist, strdup(ptr));
				catlist = g_list_append(catlist,
					categorysize(syncs, ptr));
				if(strcmp(ptr, "locale-extra"))
					catlist = g_list_append(catlist,
						strdup("Off"));
				else
					catlist = g_list_append(catlist,
						strdup("On"));
			}
	}

	// now display the list
	arraychk = glist2dialog(catlist);

	dlg_put_backtitle();
	dlg_clear();
	ret = fw_checklist(_("Selecting categories"),
		_("Please select which categories to install:"),
		0, 0, 0, g_list_length(catlist)/3, arraychk,
		FLAG_CHECK);
	return(ret);
}

GList *selpkg(char *category, GList *syncs)
{
	char **arraychk;
	GList *pkglist;
	GList *ret;
	char *tmp;

	pkglist = group2pkgs(syncs, category, 1);
	arraychk = glist2dialog(pkglist);

	dlg_put_backtitle();
	dlg_clear();
	tmp = g_strdup_printf(("Please select packages to install from the %s section:"), category);
	ret = fw_checklist(_("Selecting packages"),
		tmp,
		0, 0, 0, g_list_length(pkglist)/3, arraychk,
		FLAG_CHECK);
	FREE(tmp);
	return(ret);
}

int selpkg_confirm(void)
{
	int ret;
	dialog_vars.defaultno=1;
	dialog_vars.default_button=DLG_EXIT_CANCEL;
	dlg_put_backtitle();
	dlg_clear();
	ret = dialog_yesno(_("Use expert menus?"),
		_("If you like, you may select your packages from expert "
		"menus. Where the normal menu shows a choice like 'C compiler "
		"system', the expert menus show you 'C libs', 'C compiler', "
		"'C include files', etc - each individual package. Obviously, "
		"you should know what you're doing if you use the expert menus "
		"since it's possible to skip packages that are crucial to the "
		"functioning of your system. Choose 'no' for using normal "
		"menus that select groups of packages, or choose 'yes' for "
		"using expert menus with a switch for each package."), 0, 0);
	dialog_vars.default_button=DLG_EXIT_OK;
	dialog_vars.defaultno=0;
	if(ret==DLG_EXIT_OK)
		return(1);
	else
		return(0);
}

int prepare_pkgdb(char *repo, GList **config, GList **syncs)
{
	char *pacbindir, *pkgdb;
	struct stat sbuf;
	int ret;
	PM_DB *i;
	FILE *fp;

	pacbindir = g_strdup_printf("%s/frugalware-%s",
		SOURCEDIR, ARCH);
	pkgdb = g_strdup_printf("%s/var/lib/pacman-g2/%s.fdb", TARGETDIR, repo);

	// prepare pkgdb if necessary
	if(stat(pkgdb, &sbuf))
	{
		// pacman can't lock & log without these
		char *temp = NULL;

		temp = g_strdup_printf("%s/tmp", TARGETDIR);
		makepath(temp);
		g_free(temp);
		temp = g_strdup_printf("%s/var/log", TARGETDIR);
		makepath(temp);
		g_free(temp);
		if((char*)data_get(*config, "netinstall")==NULL)
		{
			// TODO: use libpacman for this
			temp = g_strdup_printf("%s/%s.fdb", pacbindir, repo);
			copyfile(temp, pkgdb);
			g_free(temp);
			if ((fp = fopen("/etc/pacman-g2.conf", "w")) == NULL)
			{
				LOG("could not open output file '/etc/pacman-g2.conf' for writing: %s", strerror(errno));
				return(1);
			}
			fprintf(fp, "[options]\n");
			fprintf(fp, "LogFile     = /var/log/pacman-g2.log\n");
			fprintf(fp, "MaxTries = 5\n");
			fprintf(fp, "[%s]\n", repo);
			fprintf(fp, "Server = file://%s\n\n", pacbindir);
			fclose(fp);
		}
		else
		{
			LOG("parsing the pacman-g2 configuration file");
			if (pacman_parse_config("/etc/pacman-g2.conf", cb_db_register, "") == -1) {
				dlg_put_backtitle();
				dialog_msgbox(_("Error"), g_strdup_printf(_("Failed to parse pacman-g2 configuration file (%s)"), pacman_strerror(pm_errno)), 0, 0, 1);
				return(-1);
			}

			LOG("getting the database");
			if (mydatabase == NULL)
			{
				dlg_put_backtitle();
				dialog_msgbox(_("Error"), g_strdup_printf(_("Could not register '%s' database (%s)"), PACCONF, pacman_strerror(pm_errno)), 0, 0, 1);
				return(-1);
			}
			else
			{
				LOG("updating the database");
				ret = pacman_db_update(1, mydatabase);
				if(ret == 0) {
					LOG("database update done");
				}
				if (ret == -1) {
					LOG("database update failed");
					if(pm_errno == PM_ERR_DB_SYNC) {
						dlg_put_backtitle();
						dialog_msgbox(_("Error"), g_strdup_printf(_("Failed to synchronize %s"), PACCONF), 0, 0, 1);
						return(-1);
					} else {
						dlg_put_backtitle();
						dialog_msgbox(_("Error"), g_strdup_printf(_("Failed to update %s (%s)"), PACCONF, pacman_strerror(pm_errno)), 0, 0, 1);
						return(-1);
					}
				}
			}

			LOG("cleaning up the database");
			pacman_db_unregister(mydatabase);
			mydatabase = NULL;
		}
	}

	// register the database
	i = pacman_db_register(PACCONF);
	if(i==NULL)
	{
		LOG("could not register '%s' database (%s)\n",
			PACCONF, pacman_strerror(pm_errno));
		return(1);
	}
	else
		*syncs = g_list_append(*syncs, i);
	return(0);
}

int fw_select(GList **config, int selpkgc, GList *syncs)
{
	int i, j;
	GList *cats=NULL;
	GList *allpkgs=NULL;
	PM_LIST *x, *lp, *junk, *sorted;
	char *ptr;

	dialog_vars.backtitle=gen_backtitle(_("Selecting packages"));
	dlg_put_backtitle();
	dlg_clear();
	dialog_msgbox(_("Please wait"), _("Searching for categories..."),
		0, 0, 0);
	if(prepare_pkgdb(PACCONF, config, &syncs) == -1)
	{
		LOG("preparing the package database failed!");
		return(-1);
	}
	cats = selcat(g_list_nth_data(syncs, 1), syncs);
	if(cats == NULL)
	{
		LOG("no categories are found!");
		return(-1);
	}
	if(!selpkgc)
	{
		dlg_put_backtitle();
		dialog_msgbox(_("Please wait"), _("Searching for packages..."),
		0, 0, 0);
	}
	for (i=0; i<g_list_length(cats); i++)
	{
		GList *pkgs=NULL;
		if(selpkgc)
		{
			pkgs = selpkg(strdup((char*)g_list_nth_data(cats, i)), syncs);
			if(pkgs == NULL)
			{
				LOG("failed to select packages from category '%s'", (char*)g_list_nth_data(cats, i));
				return(-1);
			}
		}
		else
			pkgs = group2pkgs(syncs, strdup((char*)g_list_nth_data(cats, i)), 0);
		for(j=0;j<g_list_length(pkgs);j++)
			allpkgs = g_list_append(allpkgs, g_list_nth_data(pkgs, j));
	}

	if(pacman_trans_init(PM_TRANS_TYPE_SYNC, PM_TRANS_FLAG_NOCONFLICTS, NULL, NULL, NULL) == -1)
	{
		LOG("failed to initialize the transaction!");
		return(1);
	}
	for(i=0;i<g_list_length(allpkgs);i++)
	{
		ptr = strdup((char*)g_list_nth_data(allpkgs, i));
		if(pacman_trans_addtarget(drop_version(ptr)))
		{
			LOG("failed to add package '%s' to the transaction!", (char*)g_list_nth_data(allpkgs, i));
			return(1);
		}
		FREE(ptr);
	}
	g_list_free(allpkgs);
	allpkgs=NULL;
	dlg_put_backtitle();
	dialog_msgbox(_("Please wait"), _("Searching for missing dependencies..."),
	0, 0, 0);
	if(pacman_trans_prepare(&junk) == -1) {
		LOG("pacman-g2 error: %s", pacman_strerror(pm_errno));

		/* Well well well, LOG pacman deps error at tty4 */
		for(x = pacman_list_first(junk); x; x = pacman_list_next(x))
		    {
			PM_DEPMISS *miss = pacman_list_getdata(x);
			LOG(":: %s: %s %s",
			    (char*)pacman_dep_getinfo(miss, PM_DEP_TARGET),
			    (long)pacman_dep_getinfo(miss, PM_DEP_TYPE) == PM_DEP_TYPE_DEPEND ? "requires" : "is required by",
			    (char*)pacman_dep_getinfo(miss, PM_DEP_NAME));
		    }
		pacman_list_free(junk);
		pacman_trans_release();
		return(-1);
	}
	sorted = pacman_trans_getinfo(PM_TRANS_PACKAGES);
	for(lp = pacman_list_first(sorted); lp; lp = pacman_list_next(lp))
	{
		PM_SYNCPKG *sync = pacman_list_getdata(lp);
		PM_PKG *pkg = pacman_sync_getinfo(sync, PM_SYNC_PKG);
		ptr = g_strdup_printf("%s-%s", (char*)pacman_pkg_getinfo(pkg, PM_PKG_NAME),
			(char*)pacman_pkg_getinfo(pkg, PM_PKG_VERSION));
		allpkgs = g_list_append(allpkgs, ptr);
	}
	pacman_trans_release();
	data_put(config, "packages", allpkgs);
	return(0);
}
int run(GList **config)
{
	int selpkgc, ret = 0;
	PM_DB *i;
	GList *syncs=NULL;

	if(pacman_initialize("/mnt/target") == -1)
	{
		LOG("failed to initialize pacman library (%s)",
			pacman_strerror(pm_errno));
		return(1);
	}
	//pacman_set_option(PM_OPT_LOGMASK, -1);
	pacman_set_option(PM_OPT_LOGCB, (long)cb_log);
	if(pacman_set_option(PM_OPT_DBPATH, (long)PM_DBPATH) == -1)
	{
		LOG("failed to set option DBPATH (%s)",
				pacman_strerror(pm_errno));
		return(1);
	}
	i = pacman_db_register("local");
	if(i==NULL)
	{
		LOG("could not register 'local' database (%s)",
			pacman_strerror(pm_errno));
		return(1);
	}
	else
		syncs = g_list_append(syncs, i);

	selpkgc = selpkg_confirm();
	chdir(TARGETDIR);
	if(fw_select(config, selpkgc, syncs) == -1)
	{
		LOG("failed to select packages!");
		ret = -1;
	}
	pacman_release();
	return(ret);
}
