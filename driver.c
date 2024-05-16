#include <ntifs.h>
#include <wdm.h>
#include <ntstrsafe.h>
#include <acpiioct.h>
#include <ntddstor.h>

#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define diskdriver L"\\Driver\\Disk"
#define MAX_HWID_LENGTH 64


extern POBJECT_TYPE* IoDriverObjectType;
NTSTATUS ObReferenceObjectByName(
    PUNICODE_STRING ObjectName,
    ULONG Attributes,
    PACCESS_STATE AccessState,
    ACCESS_MASK DesiredAccess,
    POBJECT_TYPE ObjectType,
    KPROCESSOR_MODE AccessMode,
    PVOID ParseContext OPTIONAL,
    PVOID* Object
);

NTSTATUS MemoryAllocation() {
    WCHAR diskNumber[] = L"0";
    UNICODE_STRING diskString;
    RtlInitUnicodeString(&diskString, L"\\Driver\\Disk");

    PDRIVER_OBJECT driverObject
    NTSTATUS status = ObReferenceObjectByName(&diskstring, OBJ_CASE_INSENSITIVE, 0, NULL, *IoDriverObjectType, (PVOID)&driverObject);
    if (!NT_SUCCESS(status)) {
        DbgPrintEx(0, 0, "Failed to get driver object pointer: %08X\n", status);
        return status;
    }

    if (driverObject == NULL) {
        DbgPrintEx(0, 0, "DeviceObject is NULL\n");
        return STATUS_UNSUCCESSFUL;
    }

   

    DbgPrintEx(0, 0, "Success!\n");
    return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT pDriverObject) {
    UNREFERENCED_PARAMETER(pDriverObject);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Driver unloaded\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING regPath) {
    UNREFERENCED_PARAMETER(regPath);
    pDriverObject->DriverUnload = DriverUnload;

    NTSTATUS sigma = MemoryAllocation();
    if (!NT_SUCCESS(sigma)) {
        DbgPrintEx(0, 0, "Memory allocation failed\n");
        return sigma;
    }

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Driver initialized successfully\n");
    return STATUS_SUCCESS;
}
