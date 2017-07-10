#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "cmd.h"
#include "util/pt.h"

#ifdef __CELLOS_PRX__
#include "compat/cellos_prx/vsh_exports.h"
#endif

void ftpcmd_create(struct FTPCommand* command)
{
	command->commands = pttree_create();
	command->secondary_commands = pttree_create();
}

void ftpcmd_register(struct FTPCommand* command, bool secondary, const char* name, cmd_callback callback)
{
	struct PTTree* commands = command->commands;

	if(secondary)
	{
		commands = command->secondary_commands;
	}

	if(commands != NULL)
	{
		pttree_insert(commands, name, callback);
	}
}

bool ftpcmd_call(struct FTPCommand* command, bool secondary, struct FTPClient* client, const char* name, const char* args)
{
	struct PTTree* commands = command->commands;

	if(secondary)
	{
		commands = command->secondary_commands;
	}

	if(commands == NULL)
	{
		return false;
	}

	struct PTNode* n = pttree_search(commands, name);
	cmd_callback callback = NULL;

	if(n != NULL)
	{
		callback = n->data_ptr;
	}
	else
	{
		return false;
	}

	(*callback)(client, name, args);
	return true;
}

void ftpcmd_destroy(struct FTPCommand* command)
{
	pttree_destroy(command->commands);
	command->commands = NULL;

	pttree_destroy(command->secondary_commands);
	command->secondary_commands = NULL;
}

void string_toupper(char* dst)
{
	char c;

	while((c = *dst) != '\0')
	{
		if(c >= 'a' && c <= 'z')
		{
			*dst -= 32;
		}

		++dst;
	}
}

void string_parsecmd(char** name, char** args, char* src)
{
	*args = strchr(src, ' ');

	if(*args != NULL)
	{
		**args = '\0';
		++*args;
	}

	*name = src;
}
