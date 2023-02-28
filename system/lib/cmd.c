#include <string.h>
#include <cmsis_device.h>
#include <config.h>

#ifdef CD_ENABLED

#ifndef CD_BUFFER_SIZE
#define CD_BUFFER_SIZE		128
#endif
#ifndef CD_AXARGS
#define CD_AXARGS		16
#endif

#ifdef __DEBUG__
#include <console.h>
#define dbg(m...)		Printf(m)
#else
#define dbg(m...)		do {} while(0)
#endif
#define msg(m...)		Printf(m)

#if !defined(__ARCC_VERSION)
#define CD_ENTRY_START \
({ \
        static char _cmd_start[0] __attribute__((unused, section(".cmd_entry_1"))); \
        (CD_Entry *)&_cmd_start; \
})

#define CD_ENTRY_END \
({ \
        static char _cmd_end[0] __attribute__((unused, section(".cmd_entry_3"))); \
        (CD_Entry *)&_cmd_end; \
})

static CD_Entry *CD_FindEntry(const char *cmd)
{
	CD_Entry *start = CD_ENTRY_START;
	CD_Entry *end = CD_ENTRY_END;

	dbg("Entry: cmd:%s, 0x%x-0x%x, %dEa\n", cmd, start, end, (end - start));
	if (!cmd || (unsigned int)(end - start) == 0)
		return NULL;

	for (CD_Entry *p = start; p < end; p++) {
		if (strcmp(cmd, p->name) == 0) {
			if (strlen(cmd) == strlen(p->name)) {
				dbg("Found [%s:%s]\n", cmd, p->name);
				return p;
			}
		}
	}

	return NULL;
}

#else
static LIST_HEAD(cmd_entry_list);

void CD_Register(CD_Entry *entry)
{
	list_add(&entry->list, &cmd_entry_list);
}

CD_Entry *CD_FindEntry(const char *cmd)
{
	struct list_head *pos;

	if (!cmd || list_empty(&cmd_entry_list))
		return NULL;

	list_for_each(pos, &cmd_entry_list) {
		CD_Entry *p = list_entry(pos, CD_Entry, list);
		if (strcmp(cmd, p->name) == 0) {
			if (strlen(cmd) == strlen(p->name)) {
				dbg("Found [%s:%s]\n", cmd, p->name);
				return p;
			}
		}
	}

	return NULL;
}
#endif

#define isblank(c)      (c == ' ' || c == '\t')

static int LineToArgs(char *line, char *argv[])
{
	int nargs = 0;

	while (nargs < CD_AXARGS) {
		/* skip any white space */
		while (isblank(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
			return nargs;
		}

		argv[nargs++] = line;	/* begin of argument string	*/

		/* find end of string */
		while (*line && !isblank(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
			return nargs;
		}

		*line++ = '\0';		/* terminate current arg	 */
	}

	return nargs;
}

static int CD_RunEntry(int argc, char * const argv[])
{
	CD_Entry *entry = CD_FindEntry(argv[0]);

	if (!entry)
		return -1;

	return (entry->func)(argc, argv);
}

int CD_Run(char *cmd)
{
	char cmdbuf[CD_BUFFER_SIZE]; /* working copy of cmd */
	char *str = cmdbuf;
	char *argv[CD_AXARGS + 1];	/* NULL terminated	*/
	char *token;			/* start of token in cmdbuf	*/
	char *sep;			/* end of token (separator) in cmdbuf */
	int argc, inquotes;
	int rc = 0;

	if (!cmd || !*cmd)
		return -1;

	if (strlen(cmd) >= CD_BUFFER_SIZE)
		return -1;

	strcpy(cmdbuf, cmd);

	while (*str) {
		/*
		 * Find separator, or string end
		 * Allow simple escape of ';' by writing "\;"
		 */
		for (inquotes = 0, sep = str; *sep; sep++) {
			if (!inquotes &&
			    (*sep == ';') &&	/* separator		*/
			    (sep != str) &&	/* past string start	*/
			    (*(sep - 1) != '\\'))	/* and NOT escaped */
				break;
		}

		/*
		 * Limit the token to data between separators
		 */
		token = str;
		if (*sep) {
			str = sep + 1;	/* start of command for next pass */
			*sep = '\0';
		} else {
			str = sep;	/* no more commands for next pass */
		}

		argc = LineToArgs(token, argv);
		if (argc == 0) {
			rc = -1;	/* no command at all */
			continue;
		}

		rc = CD_RunEntry(argc, argv);
	}

	return rc;
}

#ifdef CMD_SUPPORT_HELP
static int do_cmd_help(int argc, char * argv[])
{
	CD_Entry *start = CD_ENTRY_START;
	CD_Entry *end = CD_ENTRY_END;
	char *cmd = NULL;

	if ((unsigned int)(end - start) == 0)
		return -1;

	if (argc > 1)
		cmd = argv[1];

	for (CD_Entry *p = start; p < end; p++) {
		if (!cmd) {
			msg("\r%s\n", p->name);
			if (p->help == NULL)
				continue;
			puts("\t");
			puts(p->help);
			puts("\n");
			continue;
		}

		if (!strcmp(cmd, p->name) && strlen(cmd) == strlen(p->name)) {
			msg("\r%s\n", p->name);
			if (p->help == NULL)
				continue;
			puts("\t");
			puts(p->help);
			puts("\n");
			return 0;
		}
	}

	return 0;
}
CD_DEFINE(help, do_cmd_help, "help <cmd>");
#endif
#endif
