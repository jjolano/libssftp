#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

struct FTPClient;

#include "client.h"
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

void string_toupper(char*, const char*);
void string_parsecmd(char**, char**, char*);

#ifdef __cplusplus
}
#endif
