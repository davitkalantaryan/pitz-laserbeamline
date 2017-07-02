/*
 *	File: pitz_rpi_tools_serial.cpp
 *
 *	Created on: 11 Mar, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */

#include "pitz_rpi_tools_serial.hpp"


pitz::rpi::tools::Serial::Serial()
	:
	m_handle(INVALID_COM_HANDLE)
{
}

pitz::rpi::tools::Serial::~Serial()
{
	CloseCom();
}


int pitz::rpi::tools::Serial::Write(const void* a_data, int a_data_len)
{
	DWORD dwWritten;
	BOOL bRet = WriteFile(m_handle, a_data, a_data_len, &dwWritten, NULL);
	if (bRet){return dwWritten;}
	return -((int)GetLastError());
}


int pitz::rpi::tools::Serial::Read(void* a_buffer, int a_buf_len)
{
	DWORD dwReaded;
	BOOL bRet = ReadFile(m_handle, a_buffer, a_buf_len, &dwReaded, NULL);
	if (bRet) { return dwReaded; }
	return -((int)GetLastError());
}


int pitz::rpi::tools::Serial::OpenSerial(const char* a_entry_name)
{
#ifdef WIN32
	CloseCom(); // First close any open handle

	m_handle = CreateFileA(
		a_entry_name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);

	return m_handle != INVALID_HANDLE_VALUE ? 0 : -((int)GetLastError());
#else  // #ifdef WIN32
	return -1;
#endif // #ifdef WIN32
}


int pitz::rpi::tools::Serial::OpenSerial(const wchar_t* a_entry_name)
{
#ifdef WIN32
	CloseCom(); // First close any open handle

	m_handle = CreateFileW(
		a_entry_name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);

	return m_handle != INVALID_HANDLE_VALUE ? 0 : -((int)GetLastError());
#else  // #ifdef WIN32
	return -1;
#endif // #ifdef WIN32
}


void pitz::rpi::tools::Serial::CloseCom()
{
	//if (hComm && (hComm != INVALID_HANDLE_VALUE)){CloseHandle(hComm);}
#ifdef WIN32
	if (m_handle && (m_handle != INVALID_HANDLE_VALUE)){CloseHandle(m_handle);}
#else  // #ifdef WIN32
	//return FALSE;
#endif // #ifdef WIN32
	m_handle = INVALID_COM_HANDLE;
}


int pitz::rpi::tools::Serial::SetupCommState(const DCB* a_DcbPtr, const COMMTIMEOUTS* a_timeouts,
	int a_inQueue, int a_outQueue)
{
	if (!SetupComm(m_handle, a_inQueue, a_outQueue)) {return GetLastError();}
	if (!SetCommState(m_handle, const_cast<DCB*>(a_DcbPtr))) {return GetLastError();}

	if (a_timeouts)
	{
		if (!SetCommTimeouts(m_handle, const_cast<COMMTIMEOUTS*>(a_timeouts))) { return GetLastError(); }
	}

	return 0;
}


int pitz::rpi::tools::Serial::SetupCommState(
	DWORD a_BaudSet, BYTE a_Parity, BYTE a_ByteSize,
	BYTE a_StopBits, DWORD a_fInX,
	DWORD a_fOutX, DWORD a_fOutxDsrFlow,
	DWORD a_fOutxCtsFlow,
	DWORD a_fDtrControl,
	DWORD a_fRtsControl,
	DWORD a_ReadIntervalTimeout,
	int a_inQueue, int a_outQueue)
{
	int nReturn(0);
	DCB	aDcb;
	COMMTIMEOUTS aTimeouts;

	if ((nReturn = this->GetCommStates(&aDcb, &aTimeouts))) { return nReturn; }

	aDcb.BaudRate = a_BaudSet;
	aDcb.Parity = a_Parity;
	// ...
	aTimeouts.ReadIntervalTimeout = a_ReadIntervalTimeout;
	return SetupCommState(&aDcb, &aTimeouts, a_inQueue, a_outQueue);
}


int pitz::rpi::tools::Serial::GetCommStates(DCB* a_DcbPtr, COMMTIMEOUTS* a_timeouts)
{
	if (!(::GetCommState(m_handle, a_DcbPtr))) { return GetLastError(); }
	if (!(::GetCommTimeouts(m_handle, a_timeouts))) { return GetLastError(); }
	return 0;
}


pitz::rpi::tools::Serial::operator COM_HANDLE&()
{
	return m_handle;
}