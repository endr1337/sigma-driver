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
    UNICODE_STRING deviceName;
    NTSTATUS status;
    HANDLE driver;
    CHAR Buffer[] = "F596D4159F42496AF495"
    PANSI_STRING newS;
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK ioStatusBlock;
    PVOID buffer = ExAllocatePool(NonPagedPool, bufferSize);
  
    PFILE_OBJECT pfileObject;
    PDEVICE_OBJECT pDeviceObject;
    if (buffer == NULL) {
        DbgPrintEx(0, 0, "failed to allocate buffer\n");
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    RtlInitUnicodeString(&deviceName, SSD_DEVICE_NAME);

    // Initialize object attributes
    InitializeObjectAttributes(&objectAttributes, &deviceName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = IoGetDeviceObjectPointer(SSD_DEVICE_NAME, GENERIC_READ | GENERIC_WRITE, pfileObject, pDeviceObject);
    if(!NT_SUCCESS(status)){
       DbgPrintEx(0, 0, "LMAO IT DIDNT WORK YOU FUCKING RETARD XD");
        ExFreePool(buffer);
       return STATUS_POINTER_INVALID:
    }
    else{

   NTSTATUS ansiString;
   ansiString = RtlInitAnsiString(newS, 0x000FFFF);

    RtlInitAnsiString(
    
    ExFreePool(buffer);
    return STATUS_SUCCESS;
}
//PVOID pBuffer = MmAllocateContiguousMemory(bufferSize, 0x1485);
//CHAR serialNumber[NVME_SERIAL_NUMBER] i need to define that
//memcpy() use that thing guys
