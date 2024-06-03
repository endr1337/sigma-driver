#include <ntifs.h>
#include <wdm.h>
#include <ntstrsafe.h>
#include <acpiioct.h>
#include <ntdddisk.h>
#include <ntddk.h>
#include <ntddscsi.h>
#include <ntddstor.h>

#define IOCTL_STORAGE_QUERY_PROPERTY CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define DEVICE_NAME L"\\Device\\skibidi"
#define SYMBOLIC_NAME L"\\DosDevices\\skibidi"

extern POBJECT_TYPE* IoDriverObjectType;

typedef struct _DEVICE_EXTENSION {
    PDEVICE_OBJECT DeviceObject;
    PDRIVER_DISPATCH OriginalDispatchFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;

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

IO_STATUS_BLOCK ioStatusBlock;

NTSTATUS DispatchPassThrough(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PDEVICE_EXTENSION deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    // Check if the IRP has already been completed
    if (Irp->Tail.Overlay.DriverContext[0] != NULL) {
        // Already completed, avoid double completion
        return STATUS_SUCCESS;
    }

    // Mark IRP as in-progress to avoid re-completion
    Irp->Tail.Overlay.DriverContext[0] = (PVOID)1;

    // Check if the IRP is an IOCTL_STORAGE_QUERY_PROPERTY request
    if (irpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL &&
        irpStack->Parameters.DeviceIoControl.IoControlCode == IOCTL_STORAGE_QUERY_PROPERTY) {

        DbgPrintEx(0, 0, "Intercepted IOCTL_STORAGE_QUERY_PROPERTY\n");

        // Handle the query here
        // Call the original function to get the data
        NTSTATUS status = deviceExtension->OriginalDispatchFunction[irpStack->MajorFunction](DeviceObject, Irp);

        // Wait for the lower driver to complete the IRP
        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&Irp->UserEvent, Executive, KernelMode, FALSE, NULL);
            status = Irp->IoStatus.Status;
        }

        DbgPrintEx(0, 0, "Original function returned status: 0x%08X\n", status);

        if (NT_SUCCESS(status)) {
            PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)Irp->AssociatedIrp.SystemBuffer;
            if (query->PropertyId == StorageDeviceProperty && query->QueryType == PropertyStandardQuery) {
                PSTORAGE_DEVICE_DESCRIPTOR deviceDescriptor = (PSTORAGE_DEVICE_DESCRIPTOR)Irp->AssociatedIrp.SystemBuffer;
                if (deviceDescriptor->SerialNumberOffset != 0) {
                    char* serialNumber = (char*)deviceDescriptor + deviceDescriptor->SerialNumberOffset;
                    DbgPrintEx(0, 0, "Original Serial Number: %s\n", serialNumber);

                    WCHAR newSerialNum[] = L"5125167ADF";

                    swprintf((wchar_t*)serialNumber, L"%ws", newSerialNum);
                    DbgPrintEx(0, 0, "Updated Serial Number: %ws\n", (wchar_t*)serialNumber);
                }
            }
        }
        else {
            DbgPrintEx(0, 0, "Failed to get device properties: 0x%08X\n", status);
        }

        if (Irp->PendingReturned) {
            IoMarkIrpPending(Irp);
        }
        else {
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
        }
        return status;
    }

    // Pass through to the original dispatch function
    return deviceExtension->OriginalDispatchFunction[irpStack->MajorFunction](DeviceObject, Irp);
}


NTSTATUS CreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
    PDEVICE_EXTENSION deviceExtension = (PDEVICE_EXTENSION)DriverObject->DeviceObject->DeviceExtension;

    UNICODE_STRING symLink;
    RtlInitUnicodeString(&symLink, SYMBOLIC_NAME);
    IoDeleteSymbolicLink(&symLink);

    for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
        DriverObject->MajorFunction[i] = deviceExtension->OriginalDispatchFunction[i];
    }

    IoDeleteDevice(deviceExtension->DeviceObject);
    DbgPrintEx(0, 0, "Driver unloaded\n");
}

NTSTATUS HookDriverDispatch(PUNICODE_STRING DriverName) {
    PDRIVER_OBJECT targetDriverObject = NULL;
    NTSTATUS status = ObReferenceObjectByName(
        DriverName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        0,
        *IoDriverObjectType,
        KernelMode,
        NULL,
        (PVOID*)&targetDriverObject
    );

    if (!NT_SUCCESS(status)) {
        DbgPrintEx(0, 0, "Failed to reference driver object: 0x%08X\n", status);
        return status;
    }

    PDEVICE_EXTENSION deviceExtension = (PDEVICE_EXTENSION)targetDriverObject->DeviceObject->DeviceExtension;

    for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
        deviceExtension->OriginalDispatchFunction[i] = targetDriverObject->MajorFunction[i];
        targetDriverObject->MajorFunction[i] = DispatchPassThrough;
    }

    ObDereferenceObject(targetDriverObject);
    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    UNICODE_STRING devName, symLink;
    PDEVICE_OBJECT deviceObject = NULL;
    NTSTATUS status;

    RtlInitUnicodeString(&devName, DEVICE_NAME);
    status = IoCreateDevice(
        DriverObject,
        sizeof(DEVICE_EXTENSION),
        &devName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &deviceObject
    );

    if (!NT_SUCCESS(status)) {
        DbgPrintEx(0, 0, "Failed to create device: 0x%08X\n", status);
        return status;
    }

    PDEVICE_EXTENSION deviceExtension = (PDEVICE_EXTENSION)deviceObject->DeviceExtension;
    deviceExtension->DeviceObject = deviceObject;

    for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
        deviceExtension->OriginalDispatchFunction[i] = DriverObject->MajorFunction[i];
        DriverObject->MajorFunction[i] = DispatchPassThrough;
    }

    RtlInitUnicodeString(&symLink, SYMBOLIC_NAME);
    status = IoCreateSymbolicLink(&symLink, &devName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        DbgPrintEx(0, 0, "Failed to create symbolic link: 0x%08X\n", status);
        return status;
    }

    DriverObject->DriverUnload = DriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;

    DbgPrintEx(0, 0, "Driver initialized successfully\n");

    UNICODE_STRING targetDriverName;
    RtlInitUnicodeString(&targetDriverName, L"\\Driver\\Disk");
    status = HookDriverDispatch(&targetDriverName);
    if (!NT_SUCCESS(status)) {
        DbgPrintEx(0, 0, "Failed to hook driver dispatch: 0x%08X\n", status);
    }

    return STATUS_SUCCESS;
}
