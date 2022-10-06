
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS EFIAPI fake_driver_entry(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    DEBUG((DEBUG_ERROR, "fake driver entry!!!\n"));
    return EFI_SUCCESS;
}
