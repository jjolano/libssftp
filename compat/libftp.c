// Compatibility layer implementation for libftp
#include <stdint.h>
#include <stdbool.h>
#include <libftp.h>
#include <sys/ppu_thread.h>

#include "server/server.h"
#include "server/client.h"

#ifdef __CELLOS_PRX__

#else
#include <cell/sysmodule.h>
#endif

FTPServer libftp_server;

bool libftp_running = false;
sys_ppu_thread_t libftp_tid = 0;
CellFtpServiceEventHandler libftp_handler = NULL;

void _cellFtpServiceEventClientConnected(FTPClient* client)
{
	if(libftp_handler != NULL)
	{
		(*libftp_handler)(CELL_FTP_SERVICE_EVENT_CLIENT_CONNECTED, NULL, 0);
	}
}

void _cellFtpServiceEventClientDisconnected(FTPClient* client)
{
	if(libftp_handler != NULL)
	{
		(*libftp_handler)(CELL_FTP_SERVICE_EVENT_CLIENT_DISCONNECTED, NULL, 0);
	}
}

void _cellFtpServiceStart(uint64_t arg)
{
	ftpserv_create(&libftp_server, 21);

	ftpserv_event_connect_register(&libftp_server, _cellFtpServiceEventClientConnected);
	ftpserv_event_disconnect_register(&libftp_server, _cellFtpServiceEventClientDisconnected);

	libftp_running = true;

	if(libftp_handler != NULL)
	{
		// send event
		(*libftp_handler)(CELL_FTP_SERVICE_EVENT_STARTED, NULL, 0);
	}

	int ret = ftpserv_run(&libftp_server);

	libftp_running = false;

	if(libftp_handler != NULL)
	{
		// send event
		CellFtpServiceEvent event = CELL_FTP_SERVICE_EVENT_STOPPED;

		if(ret != 0)
		{
			event = CELL_FTP_SERVICE_EVENT_FATAL;
		}

		(*libftp_handler)(event, NULL, 0);
	}

	ftpserv_destroy(&libftp_server);

	sys_ppu_thread_exit((uint64_t) ret);
}

int cellFtpServiceStart(void)
{
	if(libftp_running)
	{
		return CELL_FTP_SERVICE_ERROR_ALREADY_RUNNING;
	}

	#ifndef __CELLOS_PRX__
	if(cellSysmoduleIsLoaded(CELL_SYSMODULE_NET) != CELL_SYSMODULE_LOADED)
	{
		return CELL_FTP_SERVICE_ERROR_NETWORK_NOT_INITIALIZED;
	}

	if(cellSysmoduleIsLoaded(CELL_SYSMODULE_RTC) != CELL_SYSMODULE_LOADED)
	{
		return CELL_FTP_SERVICE_ERROR_RTCLOCK_NOT_INITIALIZED;
	}

	if(cellSysmoduleIsLoaded(CELL_SYSMODULE_FS) != CELL_SYSMODULE_LOADED)
	{
		return CELL_FTP_SERVICE_ERROR_STORAGE_NOT_INITIALIZED;
	}
	#endif

	int ret = sys_ppu_thread_create(&libftp_tid, _cellFtpServiceStart, 0, 1000, 0x2000, SYS_PPU_THREAD_CREATE_JOINABLE, "libftp");

	return ret == 0 ? CELL_FTP_SERVICE_OK : CELL_FTP_SERVICE_ERROR_INTERNAL;
}

int cellFtpServiceStop(uint64_t* result)
{
	if(!libftp_running)
	{
		return CELL_FTP_SERVICE_ERROR_NOT_RUNNING;
	}

	ftpserv_stop(&libftp_server);

	int ret = sys_ppu_thread_join(libftp_tid, result);

	libftp_tid = 0;

	switch(*result)
	{
		case 1: return CELL_FTP_SERVICE_ERROR_NULL_POINTER;
		case 2: return CELL_FTP_SERVICE_ERROR_NETWORK_ERROR;
	}

	return ret == 0 ? CELL_FTP_SERVICE_OK : CELL_FTP_SERVICE_ERROR_INTERNAL;
}

int cellFtpServiceRegisterHandler(CellFtpServiceEventHandler handler)
{
	libftp_handler = handler;
	return libftp_handler != NULL ? CELL_FTP_SERVICE_OK : CELL_FTP_SERVICE_ERROR_NULL_POINTER;
}

void cellFtpServiceUnregisterHandler(void)
{
	libftp_handler = NULL;
}
