/** @file
*  Virtio FDT client protocol driver for virtio,mmio DT node
*
*  Copyright (c) 2014 - 2016, Linaro Ltd. All rights reserved.<BR>
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/VirtioMmioDeviceLib.h>

#include <Guid/VirtioMmioTransport.h>

#include <Protocol/FdtClient.h>

#pragma pack (1)
typedef struct {
  VENDOR_DEVICE_PATH          Vendor;
  UINT64                      PhysBase;
  EFI_DEVICE_PATH_PROTOCOL    End;
} VIRTIO_TRANSPORT_DEVICE_PATH;
#pragma pack ()

EFI_STATUS EFIAPI InitializeVirtioFdtDxe(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    UINT64 RegBase;
    UINT32 RegSize;
    INTN i;
    VIRTIO_TRANSPORT_DEVICE_PATH  *DevicePath;
    EFI_HANDLE Handle;

    RegBase = 0x20002000;
    RegSize = 0x200;
    for (i = 0; i < 8; i++) {
        DevicePath = (VIRTIO_TRANSPORT_DEVICE_PATH *) CreateDeviceNode(HARDWARE_DEVICE_PATH, HW_VENDOR_DP, sizeof(VIRTIO_TRANSPORT_DEVICE_PATH));
        if (DevicePath == NULL) {
            DEBUG((DEBUG_ERROR, "%a: Out of memory\n", __FUNCTION__));
            continue;
        }

        CopyGuid(&DevicePath->Vendor.Guid, &gVirtioMmioTransportGuid);
        DevicePath->PhysBase = RegBase;
        SetDevicePathNodeLength(&DevicePath->Vendor, sizeof (*DevicePath) - sizeof (DevicePath->End));
        SetDevicePathEndNode (&DevicePath->End);

        Handle = NULL;
        Status = gBS->InstallProtocolInterface(&Handle, &gEfiDevicePathProtocolGuid, EFI_NATIVE_INTERFACE, DevicePath);
        if (EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR, "%a: Failed to install the EFI_DEVICE_PATH protocol on a new handle (Status == %r)\n", __FUNCTION__, Status));
            FreePool(DevicePath);
            continue;
        }

        Status = VirtioMmioInstallDevice(RegBase, Handle);
        if (EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR, "%a: Failed to install VirtIO transport @ 0x%Lx on handle %p (Status == %r)\n", __FUNCTION__, RegBase, Handle, Status));
            Status = gBS->UninstallProtocolInterface(Handle, &gEfiDevicePathProtocolGuid, DevicePath);
            ASSERT_EFI_ERROR(Status);
            FreePool(DevicePath);
            continue;
        }
        RegBase += RegSize;
    }

    return EFI_SUCCESS;
}
