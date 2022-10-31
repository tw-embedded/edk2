/** @file

 Copyright (c) 2014-2018, Linaro Ltd. All rights reserved.<BR>

 SPDX-License-Identifier: BSD-2-Clause-Patent

 **/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/NorFlashPlatformLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/FdtClient.h>

#include <configuration.h>

#define QEMU_NOR_BLOCK_SIZE  SIZE_256KB

#define MAX_FLASH_BANKS  4

EFI_STATUS
NorFlashPlatformInitialization (
  VOID
  )
{
  return EFI_SUCCESS;
}

NOR_FLASH_DESCRIPTION  mNorFlashDevices[MAX_FLASH_BANKS];

EFI_STATUS
NorFlashPlatformGetDevices (
  OUT NOR_FLASH_DESCRIPTION  **NorFlashDescriptions,
  OUT UINT32                 *Count
  )
{
  UINT32               Num;

  // fake soc configuration
  Num = 0;
  mNorFlashDevices[Num].DeviceBaseAddress = NORFLASH_BASE;
  mNorFlashDevices[Num].RegionBaseAddress = NORFLASH_BASE;
  mNorFlashDevices[Num].Size              = NORFLASH_SIZE;
  mNorFlashDevices[Num].BlockSize         = QEMU_NOR_BLOCK_SIZE;

  *NorFlashDescriptions = mNorFlashDevices;
  *Count                = 1;

  return EFI_SUCCESS;
}
