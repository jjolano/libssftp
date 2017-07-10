#include "feat.h"

define_cmd2(ssftpCmdFeat)
{
	ftpclient_send_message(client, 211, true, "Features:");

	ftpclient_send_message(client, 0, true, "REST STREAM");
	ftpclient_send_message(client, 0, true, "SIZE");
	ftpclient_send_message(client, 0, true, "MDTM");
	ftpclient_send_message(client, 0, true, "TVFS");

	ftpclient_send_message(client, 211, false, "End");
}
