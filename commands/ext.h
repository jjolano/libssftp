#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>

#include "server/cmd.h"
#include "server/response.h"
#include "compat/compat.h"

define_cmd(ssftpCmdSize);
define_cmd(ssftpCmdMdtm);

#ifdef __cplusplus
}
#endif
