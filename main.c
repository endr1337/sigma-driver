#include <ntddk.h>
#include <intrin.h>
#include <nvme.h>

#define SSD_DEVICE_NAME L"\\Device\\Harddisk0\\DR0"
#define SSD_DRIVER_NAME L"\\Driver\\Disk"

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
    HANDLE ssdHandle = NULL;
    HANDLE driver;
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK ioStatusBlock;
    ULONG bufferSize = sizeof(NVME_IDENTIFY_CONTROLLER_DATA);
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

    if (!NT_SUCCESS(status)) {
        DbgPrintEx(0, 0, "ZwCreateFile failed with status 0x%X\n", status);
        ExFreePool(buffer);
        return status;
    }NVME_IDENTIFY_CONTROLLER_DATA* controllerData = (NVME_IDENTIFY_CONTROLLER_DATA*)buffer;
    

    if (!NT_SUCCESS(status)) {
        DbgPrintEx(0, 0, "Failed to send Identify Controller command\n");
        ZwClose(ssdHandle);
        ExFreePool(buffer);
        return status;
    }

    NT_STATUS = sigmaPart;
    sigmaPart = (PVOID) MmAllocateContigousMemory(28,  0x0000000000FFFFFF);
    
  if(!NT_SUCCESS(sigmaPart)){
      DbgPrintEx(0, 0, "you fucked up idk why but something happened");
      return STATUS_ADDRESS_INVALID;
  }
    ZwClose(ssdHandle);
    ExFreePool(buffer);
    return STATUS_SUCCESS;
}
//PVOID pBuffer = MmAllocateContiguousMemory(bufferSize, 0x1485);
//CHAR serialNumber[NVME_SERIAL_NUMBER] i need to define that
//memcpy() use that thing guys
