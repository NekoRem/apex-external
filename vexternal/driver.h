#pragma once
#include <Windows.h>
#include <stdio.h>
#include <cstdint>
#include <string>
#include <tlhelp32.h>

#include "globals.h"

// removed real driver and replaced it with this ~rea

static std::mutex isuse;
static HWND ValidHwnd;
static UINT MsgKey;

NTSTATUS(*NtUserMessageCall)(HWND hWnd, UINT msg, PVOID wParam, PVOID lParam, ULONG_PTR ResultInfo, DWORD dwType, BOOLEAN bAnsi) = nullptr;
#define KEY 0x1337ADEADL

enum RequestType
{
	TEST,
	GETBASE,
	WRITE,
	READ
};

typedef struct CommsStruct
{
	RequestType ReqType;
	uintptr_t Key;
	int PID;
	void* SourceAddress;
	void* TargetAddress;
	SIZE_T Size;
	uintptr_t ReturnValue;
} Communication, * PCommunication;

static bool driver_init() {
	LoadLibraryA("user32.dll");
	LoadLibraryA("win32u.dll");
	LoadLibraryA("ntdll.dll");

	*(PVOID*)&NtUserMessageCall = GetProcAddress(
		GetModuleHandleA("win32u.dll"),
		"NtUserMessageCall"
	);
	if (!NtUserMessageCall)
		return false;

	srand(GetTickCount64() * GetCurrentProcessId() * GetCurrentThreadId());
	MsgKey = 0xbd4 + (rand() % 0x1ffff);


	ValidHwnd = FindWindowA("WorkerW", 0);
	if (INVALID_HANDLE_VALUE != ValidHwnd) {
		Communication Request = {};
		SecureZeroMemory(&Request, sizeof(Request));
		Request.ReqType = RequestType::TEST;
		Request.Key = KEY;
		Request.PID = g_pid;
		NtUserMessageCall(ValidHwnd, MsgKey, &Request, 0, 0xDEAD, 16, 0);
		return true;
	}
	else
		return false;
}

static DWORD64 get_base_address()
{
	Communication Request = {};
	SecureZeroMemory(&Request, sizeof(Communication));

	Request.ReqType = RequestType::GETBASE;
	Request.Key = KEY;
	Request.PID = g_pid;

	NtUserMessageCall(ValidHwnd, MsgKey, &Request, 0, 0xDEAD, 16, 0);
	return Request.ReturnValue;
}

template <typename T>
T read(uintptr_t address)
{
	Communication Request = {};
	SecureZeroMemory(&Request, sizeof(Request));

	T Buffer;

	Request.ReqType = RequestType::READ;
	Request.Key = KEY;
	Request.SourceAddress = (void*)address;
	Request.TargetAddress = &Buffer;
	Request.Size = sizeof(Buffer);
	Request.PID = g_pid;

	NtUserMessageCall(ValidHwnd, MsgKey, &Request, 0, 0xDEAD, 16, 0);
	return Buffer;
}

template <typename T>
T read(uintptr_t address, size_t size)
{
	Communication Request = {};
	SecureZeroMemory(&Request, sizeof(Request));

	T Buffer;

	Request.ReqType = RequestType::READ;
	Request.Key = KEY;
	Request.SourceAddress = (void*)address;
	Request.TargetAddress = &Buffer;
	Request.Size = size;
	Request.PID = g_pid;

	NtUserMessageCall(ValidHwnd, MsgKey, &Request, 0, 0xDEAD, 16, 0);
	return Buffer;
}

static NTSTATUS ReadProcessMemory(uint64_t src, void* dest, uint32_t size)
{
	Communication Request = {};
	SecureZeroMemory(&Request, sizeof(Request));

	Request.ReqType = RequestType::READ;
	Request.Key = KEY;
	Request.SourceAddress = (void*)src;
	Request.TargetAddress = dest;
	Request.Size = size;
	Request.PID = g_pid;

	return NtUserMessageCall(ValidHwnd, MsgKey, &Request, 0, 0xDEAD, 16, 0);
}

static NTSTATUS WriteProcessMemory(uint64_t src, void* dest, uint32_t size)
{
	Communication Request = {};
	SecureZeroMemory(&Request, sizeof(Request));

	Request.ReqType = RequestType::WRITE;
	Request.Key = KEY;
	Request.SourceAddress = (void*)src;
	Request.TargetAddress = dest;
	Request.Size = size;
	Request.PID = g_pid;

	return NtUserMessageCall(ValidHwnd, MsgKey, &Request, 0, 0xDEAD, 16, 0);
}

template <typename T>
T write(uintptr_t address, T what)
{
	T Buffer{};
	WriteProcessMemory(address, &what, sizeof(T));
	return Buffer;
}

static std::string readwtf(uintptr_t Address, void* Buffer, SIZE_T Size)
{
	ReadProcessMemory(Address, Buffer, Size);

	char name[255] = { 0 };
	memcpy(&name, Buffer, Size);

	return std::string(name);
}