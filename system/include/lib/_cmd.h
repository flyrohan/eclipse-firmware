#ifndef _CMD_ENTRY_H_
#define _CMD_ENTRY_H_

#include <_list.h>

typedef struct __CMD_Entry {
    char *name;
    int	(*func)(int argc, char * const argv[]);
#ifdef CMD_SUPPORT_HELP
	const char *help;
#endif
#if defined(__ARMCC_VERSION)
    struct list_head list;
#endif
} CMD_Entry;

int  CMD_Run(char *command);

#if !defined(__ARMCC_VERSION)
#define CMD_TYPE	static
#ifdef CMD_SUPPORT_HELP
#define CMD_DEFINE(_cmd, _func, _help) \
	CMD_Entry _c_ent_##_func_##_cmd		\
	__attribute__((unused, section(".cmd_entry_2_"#_cmd""))) = { \
		.name = #_cmd, .func = _func, .help = _help }
#else
#define CMD_DEFINE(_cmd, _func) \
	CMD_Entry _c_ent_##_func_##_cmd		\
	__attribute__((unused, section(".cmd_entry_2_"#_cmd""))) = { #_cmd, _func }
#endif
#else
void CMD_Register(CMD_Entry *entry);
#define CMD_TYPE
#ifdef CMD_SUPPORT_HELP
#define CMD_DEFINE(_cmd, _func, _help) \
static CMD_Entry _c_ent_##_cmd	= { #_cmd, _func, _help, LIST_HEAD_INIT(_c_ent_##_cmd.list) }; \
void do_cmd_##_cmd(void) __attribute__((constructor(10)));	\
void do_cmd_##_cmd(void) { \
	CMD_Register(&_c_ent_##_cmd); \
}
#else
#define CMD_DEFINE(_cmd, _func) \
static CMD_Entry _c_ent_##_cmd	= { #_cmd, _func, LIST_HEAD_INIT(_c_ent_##_cmd.list) }; \
void do_cmd_##_cmd(void) __attribute__((constructor(10)));	\
void do_cmd_##_cmd(void) { \
	CMD_Register(&_c_ent_##_cmd); \
}
#endif
#endif

#endif
