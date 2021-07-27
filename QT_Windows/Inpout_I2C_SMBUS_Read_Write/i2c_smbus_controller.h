#ifndef I2C_SMBUS_CONTROLER_H
#define I2C_SMBUS_CONTROLER_H

#include <QDebug>
#include <QString>
#include <QProcess>
#include <QMessageBox>
#include <QApplication>
#include <Windows.h>
#include "inpout32_1501/Win32/inpout32.h"
#include "inpout32_1501/x64/inpout32.h"

/* I801 SMBus address offsets */
#define SMBHSTSTS       (0 + SMBUS_Base_Address)
#define SMBHSTCNT       (2 + SMBUS_Base_Address)
#define SMBHSTCMD       (3 + SMBUS_Base_Address)
#define SMBHSTADD       (4 + SMBUS_Base_Address)
#define SMBHSTDAT0      (5 + SMBUS_Base_Address)

/* I801 command constants */
#define I801_BYTE_DATA      0x08

/* I801 Host Control register bits */
#define SMBHSTCNT_INTREN    1   //0000 0001
#define SMBHSTCNT_START     64  //0100 0000


/* I801 Hosts Status register bits */
#define SMBHSTSTS_BYTE_DONE     128 //1000 0000
#define SMBHSTSTS_INUSE_STS     64  //0100 0000
#define SMBHSTSTS_SMBALERT_STS  32  //0010 0000
#define SMBHSTSTS_FAILED        16  //0001 0000
#define SMBHSTSTS_BUS_ERR       8   //0000 1000
#define SMBHSTSTS_DEV_ERR       4   //0000 0100
#define SMBHSTSTS_INTR          2   //0000 0010
#define SMBHSTSTS_HOST_BUSY     1   //0000 0001

/* Host Notify Status register bits */
#define SMBSLVSTS_HST_NTFY_STS  BIT(0)

/* Host Notify Command register bits */
#define SMBSLVCMD_HST_NTFY_INTREN   BIT(0)

#define STATUS_ERROR_FLAGS  (SMBHSTSTS_FAILED | SMBHSTSTS_BUS_ERR | SMBHSTSTS_DEV_ERR) //0001 1100
#define STATUS_FLAGS        (SMBHSTSTS_BYTE_DONE | SMBHSTSTS_INTR | STATUS_ERROR_FLAGS)//1001 1110

enum{
    read_data=1,
    write_data=0,

    WAIT_SMBUS_ACK=1,

    DRAM_MAX_COUNT=4,
};

class I2C_SMBUS_Controler
{
public:
    bool Detected_SMBUS_Driver_Exist();

    int Read_SMBUS(int Controller_Address,int Register_Address);

    void Write_SMBUS(int Controller_Address,int Register_Address,int Write_Data);

    bool Check_DIMM_Exist(int Controller_Address);

    bool Write_SMBUS_And_Verify(int Controller_Address,int Register_Address,int Write_Data);

private:
    int status = 0,
        Register_Address =0,
        Controller_Address=0,
        SMBUS_Base_Address,
        Register_Data;;

    bool Already_Detected_SMBUS_Driver_Exist = false,
         intel_device =false;

    std::string SMBUS;
};

#endif // I2C_SMBUS_CONTROLER_H
