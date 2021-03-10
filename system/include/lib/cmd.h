
#ifndef _CMD_H_
#define _CMD_H_

typedef struct __CMD_Entry {
    char *name;
    int	(*func)(int argc, char * const argv[]);
} CMD_Entry;

#define CMD_DEFINE(_cmd, _func) \
	CMD_Entry _CMD_Etnry_##_func_##_cmd __aligned(4)           \
	__attribute__((unused, section(".cmd_entry_2_"#_cmd""))) = { #_cmd, _func }

int CMD_Run(char *command);
#endif
