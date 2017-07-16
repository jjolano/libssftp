#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>

#include "server/cmd.h"
#include "server/response.h"
#include "compat/compat.h"

define_cmd(ssftpCmdChmod);
define_cmd(ssftpCmdStop);
define_cmd(ssftpCmdWrite);

#ifdef __cplusplus
}
#endif
