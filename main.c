#include <ntddk.h>
NTSTATUS GetGpuHandle() {
    UNICODE_STRING deviceName;
    PDEVICE_OBJECT deviceObject = NULL;
    PFILE_OBJECT fileObject = NULL;
    NTSTATUS status;

    RtlInitUnicodeString(&deviceName, L"\\Device\\Video0");

    status = IoGetDeviceObjectPointer(&deviceName, FILE_READ_DATA, &fileObject, &deviceObject);

    if (!NT_SUCCESS(status)) {
        DbgPrintEx(0, 0, "Failed to get device object pointer (status: 0x%X)\n", status);
        return status;
    }

    DbgPrintEx(0, 0, "Successfully obtained handle to gpu!\n");

    if (fileObject != NULL) {
        ObDereferenceObject(fileObject);
    }

    return STATUS_SUCCESS;
}

NTSTATUS GetCpuHandle() {
    UNICODE_STRING deviceName;
    PDEVICE_OBJECT deviceObject = NULL;
    PFILE_OBJECT fileObject = NULL;
    NTSTATUS status;

    RtlInitUnicodeString(&deviceName, L"\\Device\\Harddisk0\\DR0");

    status = IoGetDeviceObjectPointer(&deviceName, FILE_READ_DATA, &fileObject, &deviceObject);

    if (!NT_SUCCESS(status)) {
        DbgPrintEx(0, 0, "Failed to get device object pointer (status: 0x%X)\n", status);
        return status;
    }

    DbgPrintEx(0, 0, "Successfully obtained handle to cpu!\n");

    if (fileObject != NULL) {
        ObDereferenceObject(fileObject);
    }
    
    NTSTATUS sigma = GetGpuHandle();
    if (!NT_SUCCESS(sigma)) {
        return sigma;
    }

    return STATUS_SUCCESS;
}

NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject) {
    UNREFERENCED_PARAMETER(pDriverObject);
    DbgPrintEx(0, 0,"Unloaded!\n");
    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING regPath) {
    UNREFERENCED_PARAMETER(regPath);

    DbgPrintEx(0, 0,"Loading!\n");
    pDriverObject->DriverUnload = DriverUnload;

    DbgPrintEx(0, 0,"Loaded!\n");


    NTSTATUS status = GetCpuHandle();
    if (!NT_SUCCESS(status)) {
        return status;
    }

    return STATUS_SUCCESS;
}
