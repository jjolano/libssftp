#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>

#include "server/cmd.h"
#include "server/response.h"
#include "compat/compat.h"

define_data(ssftpDataList);
define_data(ssftpDataNlst);
define_data(ssftpDataRetr);
define_data(ssftpDataStor);

#ifdef __cplusplus
}
#endif
