#include <ntddk.h>
#include <wdm.h>
#define SSD_DEVICE_NAME L"\\Device\\Harddisk0\\DR0"
#define SSD_DRIVER_NAME L"\\Driver\\disk"

NTSTATUS GetCpuHandle();
NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject);
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING regPath);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING regPath) {
    UNREFERENCED_PARAMETER(regPath);
    NTSTATUS sigma = CreateFile(L"\\\\.\\Driver\\endr1337", GENERIC_READ | GENERIC_WRITE, 0, NULL);
	if(NT_SUCCESS(sigma)){
	DbgPrintEx(0, 0, "Failed to create file!");
        return status;
        }
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
    UNICODE_STRING DestinationString;
 RtlUnicodeStringPrintf( &DestinationString, L"\\Device\\Harddisk\\DR0%d", raid_port );
 
	PDEVICE_OBJECT DeviceObject = _nullptr;
	PFILE_OBJECT FileObject = _nullptr;
 
	if ( !NT_SUCCESS( IoGetDeviceObjectPointer( &DestinationString, FILE_READ_DATA, &FileObject, &DeviceObject ) ) ){
		return STATUS_UNSUCCESSFUL;
      	}
	if ( DeviceObject == _nullptr ){
		return STATUS_DEVICE_NULL;
	} 
	if ( DeviceObject->DeviceType == FILE_DEVICE_DISK )
	{
		DbgPrintEx(0, 0, "WOOOO IT WORKS", DeviceType);
	}    
}
