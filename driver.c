#include <ntddk.h>
#include <intrin.h>
#include <nvme.h>
#define SSD_DEVICE_NAME L"\\Device\\Harddisk0\\DR0"
#define SSD_DRIVER_NAME L"\\Driver\\disk"

NTSTATUS GetCpuHandle();
NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject);
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING regPath);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING regPath) {
    UNREFERENCED_PARAMETER(regPath);

    pDriverObject->DriverUnload = DriverUnload;

    NTSTATUS status = GetCpuHandle();
    if (!NT_SUCCESS(status)) {
        return status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject) {
    UNREFERENCED_PARAMETER(pDriverObject);
    DbgPrintEx(0, 0, "Driver unloaded\n");
    return STATUS_SUCCESS;
}


NTSTATUS GetCpuHandle() {
UNICODE_STRING driverDisk;
    
RtlInitUnicodeString(&driverDisk, L"\\Driver\\disk");
PDRIVER_OBJECT driverObject = nullptr;
auto status = IoGetDeviceObjectPointer(&driverDisk, OBJ_CASE_INSENSITIVE, nullptr, 0, *IoDriverObjectType, KernelMode, nullptr, reinterpret_cast<PVOID*>(&driverObject));
}
//PVOID pBuffer = MmAllocateContiguousMemory(bufferSize, 0x1485);
//CHAR serialNumber[NVME_SERIAL_NUMBER] i need to define that
//memcpy() use that thing guys
