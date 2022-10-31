/** @file
  FDT client library for ARM's PL031 RTC driver

  Copyright (c) 2016, Linaro Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/FdtClient.h>

#include <configuration.h>

RETURN_STATUS EFIAPI pl031_lib_constructor(VOID)
{
  UINT64               RegBase;
  RETURN_STATUS        PcdStatus;

  RegBase = PL031_BASE;
  ASSERT (RegBase < MAX_UINT32);

  PcdStatus = PcdSet32S (PcdPL031RtcBase, (UINT32)RegBase);
  ASSERT_RETURN_ERROR (PcdStatus);

  DEBUG ((DEBUG_INFO, "Found PL031 RTC @ 0x%Lx\n", RegBase));

  return EFI_SUCCESS;
}
