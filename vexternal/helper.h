#pragma once
#define Log(content) std::wcout << content

typedef enum _KEY_VALUE_INFORMATION_CLASS {
	KeyValueBasicInformation,
	KeyValueFullInformation,
	KeyValuePartialInformation,
	KeyValueFullInformationAlign64,
	KeyValuePartialInformationAlign64,
	KeyValueLayerInformation,
	MaxKeyValueInfoClass  // MaxKeyValueInfoClass should always be the last enum
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_VALUE_FULL_INFORMATION {
	ULONG   TitleIndex;
	ULONG   Type;
	ULONG   DataOffset;
	ULONG   DataLength;
	ULONG   NameLength;
	WCHAR   Name[1];            // Variable size
//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, * PKEY_VALUE_FULL_INFORMATION;


#ifdef __cplusplus
extern "C++"
{
	char _RTL_CONSTANT_STRING_type_check(const char* s);
	char _RTL_CONSTANT_STRING_type_check(const WCHAR* s);
	// __typeof would be desirable here instead of sizeof.
	template <size_t N> class _RTL_CONSTANT_STRING_remove_const_template_class;
template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof(char)> { public: typedef  char T; };
template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof(WCHAR)> { public: typedef WCHAR T; };
#define _RTL_CONSTANT_STRING_remove_const_macro(s) \
    (const_cast<_RTL_CONSTANT_STRING_remove_const_template_class<sizeof((s)[0])>::T*>(s))
}
#else
char _RTL_CONSTANT_STRING_type_check(const void* s);
#define _RTL_CONSTANT_STRING_remove_const_macro(s) (s)
#endif
#define RTL_CONSTANT_STRING(s) \
{ \
    sizeof( s ) - sizeof( (s)[0] ), \
    sizeof( s ) / sizeof(_RTL_CONSTANT_STRING_type_check(s)), \
    _RTL_CONSTANT_STRING_remove_const_macro(s) \
}

extern "C" {
	NTSYSAPI NTSTATUS NTAPI ZwQueryValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_writes_bytes_to_opt_(Length, *ResultLength) PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
		);

	NTSYSAPI NTSTATUS NTAPI ZwClose(
			_In_ HANDLE Handle
		);

	NTSYSAPI NTSTATUS NTAPI ZwOpenKey(
			_Out_ PHANDLE KeyHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes
		);

	NTSYSAPI NTSTATUS NTAPI ZwQueryValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_writes_bytes_to_opt_(Length, *ResultLength) PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
		);

	NTSYSAPI NTSTATUS NTAPI ZwSetValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_opt_ ULONG TitleIndex,
			_In_ ULONG Type,
			_In_reads_bytes_opt_(DataSize) PVOID Data,
			_In_ ULONG DataSize
		);

	NTSYSAPI NTSTATUS ZwCreateKey(
		PHANDLE            KeyHandle,
		ACCESS_MASK        DesiredAccess,
		POBJECT_ATTRIBUTES ObjectAttributes,
		ULONG              TitleIndex,
		PUNICODE_STRING    Class,
		ULONG              CreateOptions,
		PULONG             Disposition
	);
}

namespace registry_helper
{
	__forceinline ULONG get_key_info_size(HANDLE hKey, PUNICODE_STRING Key)
	{
		NTSTATUS Status;
		ULONG KeySize;

		Status = ZwQueryValueKey(hKey, Key, KeyValueFullInformation, 0, 0, &KeySize);

		if (Status == STATUS_BUFFER_TOO_SMALL || Status == STATUS_BUFFER_OVERFLOW)
			return KeySize;

		return 0;
	}

	template <typename type>
	__forceinline type read_registry(UNICODE_STRING RegPath, UNICODE_STRING Key)
	{
		HANDLE hKey;
		OBJECT_ATTRIBUTES ObjAttr;
		NTSTATUS Status = STATUS_UNSUCCESSFUL;

		InitializeObjectAttributes(&ObjAttr, &RegPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

		Status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ObjAttr);

		if (NT_SUCCESS(Status))
		{
			ULONG KeyInfoSize = get_key_info_size(hKey, &Key);
			ULONG KeyInfoSizeNeeded;

			if (KeyInfoSize == NULL)
			{
				ZwClose(hKey);
				return 0;
			}

			PKEY_VALUE_FULL_INFORMATION pKeyInfo = (PKEY_VALUE_FULL_INFORMATION)malloc(KeyInfoSize);
			RtlZeroMemory(pKeyInfo, KeyInfoSize);

			Status = ZwQueryValueKey(hKey, &Key, KeyValueFullInformation, pKeyInfo, KeyInfoSize, &KeyInfoSizeNeeded);

			if (!NT_SUCCESS(Status) || (KeyInfoSize != KeyInfoSizeNeeded))
			{
				ZwClose(hKey);
				free(pKeyInfo);
				return 0;
			}

			ZwClose(hKey);
			free(pKeyInfo);

			return *(type*)((LONG64)pKeyInfo + pKeyInfo->DataOffset);
		}

		return 0;
	}

	__forceinline bool write_registry(UNICODE_STRING RegPath, UNICODE_STRING Key, PVOID Address, ULONG Type, ULONG Size)
	{
		bool Success = false;
		HANDLE hKey;
		OBJECT_ATTRIBUTES ObjAttr;
		NTSTATUS Status = STATUS_UNSUCCESSFUL;

		InitializeObjectAttributes(&ObjAttr, &RegPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

		Status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ObjAttr);

		if (NT_SUCCESS(Status))
		{
			Status = ZwSetValueKey(hKey, &Key, NULL, Type, Address, Size);

			if (NT_SUCCESS(Status))
				Success = true;

			ZwClose(hKey);
		}
		else {
			Status = ZwCreateKey(&hKey, KEY_ALL_ACCESS, &ObjAttr, 0, &RegPath, 0, 0);

			if (NT_SUCCESS(Status))
			{
				Status = ZwSetValueKey(hKey, &Key, NULL, Type, Address, Size);

				if (NT_SUCCESS(Status))
					Success = true;
			}
			ZwClose(hKey);
		}

		return Success;
	}
}

__forceinline bool callbackExample(ULONG64* param1, ULONG64* param2, ULONG64 allocationPtr, ULONG64 allocationSize, ULONG64 mdlptr) {
	UNREFERENCED_PARAMETER(param1);
	UNREFERENCED_PARAMETER(param2);
	UNREFERENCED_PARAMETER(allocationPtr);
	UNREFERENCED_PARAMETER(allocationSize);
	UNREFERENCED_PARAMETER(mdlptr);
	Log("[+] Callback example called" << std::endl);

	return true;
}

__forceinline int paramExists(const int argc, wchar_t** argv, const wchar_t* param) {
	size_t plen = wcslen(param);
	for (int i = 1; i < argc; i++) {
		if (wcslen(argv[i]) == plen + 1ull && _wcsicmp(&argv[i][1], param) == 0 && argv[i][0] == '/') { // with slash
			return i;
		}
		else if (wcslen(argv[i]) == plen + 2ull && _wcsicmp(&argv[i][2], param) == 0 && argv[i][0] == '-' && argv[i][1] == '-') { // with double dash
			return i;
		}
	}
	return -1;
}