#include <ntddk.h>

UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\testdevice123");
UNICODE_STRING SymLinkName = RTL_CONSTANT_STRING(L"\\DosDevices\\testdevice123");

PDEVICE_OBJECT DeviceObject = NULL;

VOID Unload(PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);

	IoDeleteSymbolicLink(&SymLinkName);
	IoDeleteDevice(DeviceObject);
	KdPrint(("Drive Unload!\r\n"));
}

NTSTATUS DispatchPassThru(
	_In_ PDEVICE_OBJECT deviceObject,
	_In_ PIRP Irp
)
{
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION IrpLocation;
	UNREFERENCED_PARAMETER(deviceObject);
	UNREFERENCED_PARAMETER(Irp);

	IrpLocation = IoGetCurrentIrpStackLocation(Irp);

	switch (IrpLocation->MajorFunction) {
	case IRP_MJ_CREATE:
		KdPrint(("ShinDriver: Create Request! \r\n"));
		break;
	case IRP_MJ_CLOSE:
		KdPrint(("ShinDriver: Close Request! \r\n"));
		break;
	case IRP_MJ_READ:
		KdPrint(("ShinDriver: Read Request! \r\n"));
		break;
	default:
		break;
	}

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);
	NTSTATUS status;
	DriverObject->DriverUnload = Unload;

	//================= 디바이스 등록 ==================
	status = IoCreateDevice(DriverObject,
							0,
							&DeviceName,
							FILE_DEVICE_UNKNOWN,
							FILE_DEVICE_SECURE_OPEN,
							FALSE,
							&DeviceObject);
	//===================================================

	//================ 디바이스 등록 실패 ===============
	if (!NT_SUCCESS(status)) {
		KdPrint(("Device Create Failed\r\n")); // 메시지 띄우고
		return status;                         // 종료
	}
	//===================================================

	//================ 심볼릭 링크 생성 =================
	status = IoCreateSymbolicLink(&SymLinkName, &DeviceName);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Symbloic Link Create Failed!\r\n"));
		IoDeleteDevice(DeviceObject);                  // 등록 실패시 디바이스 삭제
		return status;
	}
	//===================================================
	
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		DriverObject->MajorFunction[i] = DispatchPassThru;
	}

	KdPrint(("Driver Load Complete!\r\n"));
	return status;
}