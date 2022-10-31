/** @file
*
*  Copyright (c) 2011-2014, ARM Limited. All rights reserved.
*  Copyright (c) 2014-2020, Linaro Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <PiPei.h>

#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <libfdt.h>

#include <Guid/EarlyPL011BaseAddress.h>
#include <Guid/FdtHob.h>

#include <configuration.h>

STATIC CONST EFI_PEI_PPI_DESCRIPTOR  mTpm2DiscoveredPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gOvmfTpmDiscoveredPpiGuid,
  NULL
};

STATIC CONST EFI_PEI_PPI_DESCRIPTOR  mTpm2InitializationDonePpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiTpmInitializationDonePpiGuid,
  NULL
};

EFI_STATUS
EFIAPI
PlatformPeim (
  VOID
  )
{
  UINT64        *FdtHobData;
  UINTN         FdtSize;
  UINTN         FdtPages;
  VOID          *NewBase;

  FdtSize = 0x1000;
  FdtPages = EFI_SIZE_TO_PAGES (FdtSize);
  NewBase  = AllocatePages (FdtPages);
  ASSERT (NewBase != NULL);
  fdt_create_empty_tree(NewBase, EFI_PAGES_TO_SIZE (FdtPages));

  FdtHobData = BuildGuidHob (&gFdtHobGuid, sizeof *FdtHobData);
  ASSERT (FdtHobData != NULL);
  *FdtHobData = (UINTN)NewBase;

  UINT64        *UartHobData;
  UartHobData = BuildGuidHob (&gEarlyPL011BaseAddressGuid, sizeof *UartHobData);
  ASSERT (UartHobData != NULL);
  *UartHobData = PL011_BASE;

  BuildFvHob (PcdGet64 (PcdFvBaseAddress), PcdGet32 (PcdFvSize));

  return EFI_SUCCESS;
}
