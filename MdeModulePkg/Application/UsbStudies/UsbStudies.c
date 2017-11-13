/** @file
  This sample application list all USB devices

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/UsbIo.h>
#include <Protocol/SerialIo.h>

#include <Library/UefiUsbLib.h>


//
// String token ID of help message text.
// Shell supports to find help message in the resource section of an application image if
// .MAN file is not found. This global variable is added to make build tool recognizes
// that the help string is consumed by user and then build tool will add the string into
// the resource section. Thus the application can use '-?' option to show help message in
// Shell.
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING_ID mStringHelpTokenId = STRING_TOKEN (STR_HELLO_WORLD_HELP_INFORMATION);

typedef struct {
  UINT8 Led1;
  UINT8 Led2;
  UINT8 Led3;
  UINT8 Led4;
} MY_LED_MAP;

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS Status;
    UINTN Index;
    EFI_USB_IO_PROTOCOL *UsbIo;

    EFI_HANDLE* HandleBuffer = NULL;   
    UINTN HandleCount = 0;

    EFI_USB_DEVICE_DESCRIPTOR  DeviceDescriptor;

    MY_LED_MAP LedMap;

    Status = gBS->LocateHandleBuffer(ByProtocol, 
        &gEfiUsbIoProtocolGuid, 
        NULL, 
        &HandleCount, 
        &HandleBuffer);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    Print (L"Handle count %x\n", HandleCount);

    for(Index = 0; Index < HandleCount; Index++) {
        Status = gBS->HandleProtocol( HandleBuffer[Index],
            &gEfiUsbIoProtocolGuid,
            (VOID*) &UsbIo);
	
        if(EFI_ERROR(Status)) {
            return Status;
        }	
	
        //
        // Get the default device descriptor
        //
        Status = UsbIo->UsbGetDeviceDescriptor (
            UsbIo,
            &DeviceDescriptor
        );

        if(EFI_ERROR(Status)) {
            return Status;
        }

	Print (L"Product Id %x\n", DeviceDescriptor.IdProduct);
	if(DeviceDescriptor.IdProduct == 0x204F) {
            Print (L"Found GenericHidDevice\n");
            LedMap.Led1 = 1;
            LedMap.Led2 = 1;
            LedMap.Led3 = 1;
            LedMap.Led4 = 1;
           
            Status = UsbSetReportRequest(UsbIo, 0, 0, HID_OUTPUT_REPORT, 4, (UINT8 *)&LedMap);
            if(EFI_ERROR(Status)) {
                return Status;
            }
            Print (L"Report sent!\n");
	}
    }

    return EFI_SUCCESS;
}


