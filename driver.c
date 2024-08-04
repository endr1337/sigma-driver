#include "spoof.h"
#include "shared.h"
#include "smbios.h"
NTSTATUS DriverUnload(PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);

    return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, UNICODE_STRING regPath) {
    UNREFERENCED_PARAMETER(regPath);
    DriverObject->DriverUnload;
    IrpHookDisk();
    ChangeSmbiosSerials();
    return STATUS_SUCCESS;
}
