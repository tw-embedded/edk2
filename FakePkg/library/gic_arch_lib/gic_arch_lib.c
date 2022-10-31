/** @file
  ArmGicArchLib library class implementation for DT based virt platforms

  Copyright (c) 2015 - 2016, Linaro Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi.h>

#include <Library/ArmGicLib.h>
#include <Library/ArmGicArchLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/FdtClient.h>

#include <configuration.h>

STATIC ARM_GIC_ARCH_REVISION  mGicArchRevision;

RETURN_STATUS
EFIAPI
gic_arch_lib_constructor (
  VOID
  )
{
  UINT32               IccSre;
  UINT64               DistBase, RedistBase;
  RETURN_STATUS        PcdStatus;

  //
  // The GIC v3 DT binding describes a series of at least 3 physical (base
  // addresses, size) pairs: the distributor interface (GICD), at least one
  // redistributor region (GICR) containing dedicated redistributor
  // interfaces for all individual CPUs, and the CPU interface (GICC).
  // Under virtualization, we assume that the first redistributor region
  // listed covers the boot CPU. Also, our GICv3 driver only supports the
  // system register CPU interface, so we can safely ignore the MMIO version
  // which is listed after the sequence of redistributor interfaces.
  // This means we are only interested in the first two memory regions
  // supplied, and ignore everything else.
  //

  // RegProp[0..1] == { GICD base, GICD size }
  DistBase = GIC_V3_DIST_BASE;
  ASSERT (DistBase < MAX_UINTN);

  // RegProp[2..3] == { GICR base, GICR size }
  RedistBase =  GIC_V3_REDIST_BASE;
  ASSERT (RedistBase < MAX_UINTN);

  PcdStatus = PcdSet64S (PcdGicDistributorBase, DistBase);
  ASSERT_RETURN_ERROR (PcdStatus);
  PcdStatus = PcdSet64S (PcdGicRedistributorsBase, RedistBase);
  ASSERT_RETURN_ERROR (PcdStatus);

  DEBUG ((
    DEBUG_INFO,
    "Found GIC v3 (re)distributor @ 0x%Lx (0x%Lx)\n",
    DistBase,
    RedistBase
    ));

  //
  // The default implementation of ArmGicArchLib is responsible for enabling
  // the system register interface on the GICv3 if one is found. So let's do
  // the same here.
  //
  IccSre = ArmGicV3GetControlSystemRegisterEnable ();
  if (!(IccSre & ICC_SRE_EL2_SRE)) {
    ArmGicV3SetControlSystemRegisterEnable (IccSre | ICC_SRE_EL2_SRE);
    IccSre = ArmGicV3GetControlSystemRegisterEnable ();
  }

  //
  // Unlike the default implementation, there is no fall through to GICv2
  // mode if this GICv3 cannot be driven in native mode due to the fact
  // that the System Register interface is unavailable.
  //
  ASSERT (IccSre & ICC_SRE_EL2_SRE);

  mGicArchRevision = ARM_GIC_ARCH_REVISION_3;

  return RETURN_SUCCESS;
}

ARM_GIC_ARCH_REVISION
EFIAPI
ArmGicGetSupportedArchRevision (
  VOID
  )
{
  return mGicArchRevision;
}
