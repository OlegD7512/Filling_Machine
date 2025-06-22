#include "mb.h"
#include "mbport.h"
#include "mt_port.h"
#include "port.h"

#include "main.h"
#include "cmsis_os.h"

eMBErrorCode eMBMasterRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
  eMBErrorCode eStatus = MB_ENOERR;
  return eStatus;
}
/*----------------------------------------------------------------------------*/
eMBErrorCode eMBMasterRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
  eMBErrorCode eStatus = MB_ENOERR;
  return eStatus;
}
/*----------------------------------------------------------------------------*/
eMBErrorCode eMBMasterRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode)
{
  eMBErrorCode eStatus = MB_ENOERR;
  return eStatus;
}
/*----------------------------------------------------------------------------*/
eMBErrorCode eMBMasterRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  eMBErrorCode eStatus = MB_ENOERR;
  return eStatus;
}