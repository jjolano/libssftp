#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct FTPClient;

#include "client.h"
#include "compat/compat.h"
#include "util/pt.h"

typedef void (*cmd_callback)(struct FTPClient*, const char*, const char*);

struct FTPCommand
{
	struct PTTree* commands;
	struct PTTree* secondary_commands;
};

void ftpcmd_create(struct FTPCommand*);
void ftpcmd_register(struct FTPCommand*, bool, const char*, cmd_callback);
bool ftpcmd_call(struct FTPCommand*, bool, struct FTPClient*, const char*, const char*);
void ftpcmd_destroy(struct FTPCommand*);

void string_toupper(char*);
void string_parsecmd(char**, char**, char*);

#ifdef __cplusplus
}
#endif

#define define_cmd(name) void name(struct FTPClient* client, const char* cmd, const char* args)
#define define_cmd2(name) void __attribute__((weak)) name(struct FTPClient* client, const char* cmd, const char* args)
#define register_cmd(commands, name, callback) ftpcmd_register(commands, false, name, callback)
#define register_cmd2(commands, name, callback) ftpcmd_register(commands, true, name, callback)

#define define_data(name) void name(struct FTPClient* client)
#define define_data2(name) void __attribute__((weak)) name(struct FTPClient* client)
