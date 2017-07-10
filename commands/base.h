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

define_cmd(ssftpCmdAbor);
define_cmd(ssftpCmdAcct);
define_cmd(ssftpCmdAllo);
define_cmd(ssftpCmdCdup);
define_cmd(ssftpCmdCwd);
define_cmd(ssftpCmdDele);
define_cmd(ssftpCmdHelp);
define_cmd(ssftpCmdList);
define_cmd(ssftpCmdMkd);
define_cmd(ssftpCmdMode);
define_cmd(ssftpCmdNlst);
define_cmd(ssftpCmdNoop);
define_cmd(ssftpCmdPass);
define_cmd(ssftpCmdPasv);
define_cmd(ssftpCmdPort);
define_cmd(ssftpCmdPwd);
define_cmd(ssftpCmdQuit);
define_cmd(ssftpCmdRest);
define_cmd(ssftpCmdRetr);
define_cmd(ssftpCmdRmd);
define_cmd(ssftpCmdRnfr);
define_cmd(ssftpCmdRnto);
define_cmd(ssftpCmdSite);
define_cmd(ssftpCmdStat);
define_cmd(ssftpCmdStor);
define_cmd(ssftpCmdStru);
define_cmd(ssftpCmdSyst);
define_cmd(ssftpCmdType);
define_cmd(ssftpCmdUser);

#ifdef __cplusplus
}
#endif
