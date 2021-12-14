#include <ntddk.h>

UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Devices\\firsthacking");
UNICODE_STRING DeviceLink = RTL_CONSTANT_STRING(L"\\??\\firsthacking");

#define DEVICE_SEND CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_WRITE_DATA)

PDEVICE_OBJECT deviceObject = NULL; // Device Object for device

// Process Create Routine


VOID pLoadImageNotifyRoutine(
	_In_ PUNICODE_STRING,
	_In_ HANDLE,
	_In_ PIMAGE_INFO
);

NTSTATUS DriverUnloadFunc(
	_In_ PDRIVER_OBJECT
);

VOID pCreateProcessRoutine(
	_In_ HANDLE,
	_In_ HANDLE,
	_In_ BOOLEAN
);

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	UNREFERENCED_PARAMETER(DriverObject);
	//NTSTATUS status;
	DriverObject->DriverUnload = DriverUnloadFunc;
	/*
	status = IoCreateDevice(
		DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&deviceObject
	);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Create Device Failed!!\n"));
		return status;
	}
	*/

	/*
	status = IoCreateSymbolicLink(&DeviceLink, &DeviceName);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Create Link Failed!"));
		return status;
	}
	*/

	PsSetLoadImageNotifyRoutine(&pLoadImageNotifyRoutine);
	PsSetCreateProcessNotifyRoutine(&pCreateProcessRoutine, FALSE);
	return STATUS_SUCCESS;
}

VOID pLoadImageNotifyRoutine(
	_In_ PUNICODE_STRING ImageName,
	_In_ HANDLE ProcessID,
	_In_ PIMAGE_INFO ImageInfo
)
{
	UNREFERENCED_PARAMETER(ImageName);
	UNREFERENCED_PARAMETER(ImageInfo);
	UNREFERENCED_PARAMETER(ProcessID);
	KdPrint(("[Image Load] \r\n"));
	KdPrint(("ImageLoad : %ls \r\n", ImageName->Buffer));
	KdPrint(("Process ID : %d \r\n", ProcessID));
	KdPrint(("\r\n"));
}

NTSTATUS DriverUnloadFunc(
	_In_ PDRIVER_OBJECT DriverObject
)
{
	UNREFERENCED_PARAMETER(DriverObject);
	KdPrint(("Good Bye!!"));
	//IoDeleteSymbolicLink(&DeviceLink); // Device Link Delete
	//oDeleteDevice(deviceObject);      // Device Delete
	PsRemoveLoadImageNotifyRoutine(&pLoadImageNotifyRoutine);
	PsSetCreateProcessNotifyRoutine(&pCreateProcessRoutine, TRUE);
	return STATUS_SUCCESS;
}

VOID pCreateProcessRoutine(
	_In_ HANDLE ParentID,
	_In_ HANDLE ProcessID,
	_In_ BOOLEAN Create
)
{
	UNREFERENCED_PARAMETER(ParentID);
	UNREFERENCED_PARAMETER(ProcessID);
	UNREFERENCED_PARAMETER(Create);

	KdPrint(("[Process Created]\r\n"));
	KdPrint(("Parent ID : %d \r\n", ParentID));
	KdPrint(("Process ID : %d \r\n", ProcessID));
	KdPrint(("Status : %ls", (Create) ? L"Create" : L"Remove"));
	KdPrint(("\r\n"));

}