#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>

#include "server/cmd.h"
#include "server/response.h"

#ifdef __CELLOS_LV2__
#include "compat/cellos_lv2.h"
#endif

#ifdef __CELLOS_PRX__
#include "compat/cellos_prx/vsh/include/allocator.h"
#include "compat/cellos_prx/vsh/include/stdc.h"
#include "compat/cellos_prx/vsh/include/sys_net.h"
#endif

define_data(ssftpDataList);
define_data(ssftpDataNlst);
define_data(ssftpDataRetr);
define_data(ssftpDataStor);

#ifdef __cplusplus
}
#endif
