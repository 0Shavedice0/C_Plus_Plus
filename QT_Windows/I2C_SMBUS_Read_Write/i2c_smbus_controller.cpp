#include "i2c_smbus_controller.h"
#include <tchar.h>
bool I2C_SMBUS_Controler::Detected_SMBUS_Driver_Exist(){
    //set SMBUS
    try
    {
        std::string data_string,
                    data2_string,
                    address;
        QProcess process;
        QString data;


        process.start("wmic path win32_pnpentity get name,deviceid");
        process.waitForStarted();
        process.waitForFinished();
        data=QString::fromLocal8Bit(process.readAllStandardOutput());
        data_string = data.toUtf8().constData();

        process.start("wmic path Win32_PnPAllocatedResource");
        process.waitForStarted();
        process.waitForFinished();
        data=QString::fromLocal8Bit(process.readAllStandardOutput());
        data2_string = data.toUtf8().constData();
        process.close();

        int current_line_count =0,
            current_line_len=0,
            start=0,
            end=-1;
        std::string current_search_line;
        do{
            start=end+1;
            end=data_string.find("\n",start);
            current_line_count=end+1;
            current_line_len=end-start;
            current_search_line=data_string.substr(start,current_line_len);
            //every computer detected "SMBUS" string uppercase and lowercase are different
            //so just transform it to uppercase for "string.find",PCI always uppercase so
            //it doesn't influences our want
            std::transform(current_search_line.begin(), current_search_line.end(),current_search_line.begin(), ::toupper);
            if(current_search_line.find("SMBUS")!=std::string::npos){
                if(current_search_line.find("INTEL")!=std::string::npos){
                    intel_device=true;
                }
                start = current_search_line.find("DEV_")+4; //not include DEV_
                end   = current_search_line.find("&SUBSYS");
                SMBUS = current_search_line.substr(start,end-start);
                break;
            }
        }while(data_string.find("\n",current_line_count)!=std::string::npos);
        if(SMBUS==""){
            QMessageBox::about(NULL,"Error!","Did not detected SMBUS!"
                               "\nPlease check driver is the last version,"
                               "\nand use administrator execution this application."
                               "\nThe application will be shutdown.");
            //make sure user can't control smbus
            return false;
            exit(0);
        }
        if(intel_device){
            do{
                start=end+1;
                end=data2_string.find("\n",start);
                current_line_count=end+1;
                current_line_len=end-start;

                current_search_line=data2_string.substr(start,current_line_len);
                if(current_search_line.find(SMBUS)!=std::string::npos &&
                   current_search_line.find("PortResource.StartingAddress")!=std::string::npos){
                    start = current_search_line.find("\"")+1;
                    end   = current_search_line.find("\"",start);
                    address = current_search_line.substr(start,end-start);
                    break;
                }
            }while(data2_string.find("\n",current_line_count)!=std::string::npos);
            if(address==""){
                //make sure user can't control smbus
                QMessageBox::about(NULL,"Error!","Did not detected SMBUS!"
                                   "\nPlease check driver is the last version,"
                                   "\nand use administrator execution this application."
                                   "\nThe application will be shutdown.");
                return false;
                exit(0);
            }
            SMBUS_Base_Address = std::stoi(address);
        }
        else{
            //AMD smbus base addres is static at 0x0B00
            SMBUS_Base_Address = 0x0B00;
        }
        Already_Detected_SMBUS_Driver_Exist = true;
        return true;
    }
    catch (char const* error){
        QMessageBox::about(NULL,"Detected_SMBUS_Driver_Exist",error);
    }
}

int I2C_SMBUS_Controler::Read_SMBUS(int Controller_Address,int Register_Address)
{
    Register_Data = 0xffffffff;
    try{
        if(!Already_Detected_SMBUS_Driver_Exist){
            Detected_SMBUS_Driver_Exist();
        }
        Out32(SMBHSTADD, (Controller_Address << 1) | (read_data & 0x01));
        Out32(SMBHSTCMD, Register_Address);
        status = Inp32(SMBHSTSTS);
        status &= STATUS_FLAGS;
        if (status)
        {
            Out32(SMBHSTSTS, status);
            status = Inp32(SMBHSTSTS) & STATUS_FLAGS;
        }
        Out32(SMBHSTCNT, I801_BYTE_DATA | SMBHSTCNT_START);
        do
        {
            status = Inp32(SMBHSTSTS);
        }
        while (((status & SMBHSTSTS_HOST_BUSY) || !(status & (STATUS_ERROR_FLAGS | SMBHSTSTS_INTR))));
        Register_Data = Inp32(SMBHSTDAT0);
//        qDebug()<<"Controller Address : "<<Controller_Address;
//        qDebug()<<"Register Address : "<<Register_Address;
//        qDebug()<<"Register_Data : "<<Register_Data;
//        qDebug()<<"-------------------------------------------";
        return Register_Data;
    }
    catch (char const* error){
        QMessageBox::about(NULL,"Read_SMBUS",error);
    }
}

void I2C_SMBUS_Controler::Write_SMBUS(int Controller_Address,int Register_Address,int Write_Data)
{
    try{
        if(!Already_Detected_SMBUS_Driver_Exist){
            Detected_SMBUS_Driver_Exist();
        }
        Out32(SMBHSTADD, ((Controller_Address & 0x7f) << 1) | (write_data & 0x01));
        Out32(SMBHSTCMD, Register_Address);
        Out32(SMBHSTDAT0, Write_Data);
        status = Inp32(SMBHSTSTS);
        status &= STATUS_FLAGS;
        if (status)
        {
            Out32(SMBHSTSTS, status);
            Sleep(WAIT_SMBUS_ACK);
            status = Inp32(SMBHSTSTS) & STATUS_FLAGS;
        }
        Out32(SMBHSTCNT, I801_BYTE_DATA | SMBHSTCNT_START);
        do
        {
            status = Inp32(SMBHSTSTS);
            if(status & SMBHSTSTS_DEV_ERR) break;
        } while (((status & SMBHSTSTS_HOST_BUSY) || !(status & (STATUS_ERROR_FLAGS | SMBHSTSTS_INTR))));
    }
    catch (char const* error){
        QMessageBox::about(NULL,"Controller_Address",error);
    }
}

bool I2C_SMBUS_Controler::Check_DIMM_Exist(int Controller_Address){
    try{
        if(!Already_Detected_SMBUS_Driver_Exist){
            Detected_SMBUS_Driver_Exist();
        }
        status = 0;
        Out32(SMBHSTADD, (Controller_Address << 1) | (read_data & 0x01));
        Out32(SMBHSTSTS, STATUS_FLAGS);
        Out32(SMBHSTCNT, I801_BYTE_DATA | SMBHSTCNT_START);
        do
        {
            status = Inp32(SMBHSTSTS);

            if(status & SMBHSTSTS_DEV_ERR) return false;
        }
        while (((status & SMBHSTSTS_HOST_BUSY) || !(status & (STATUS_ERROR_FLAGS | SMBHSTSTS_INTR))));
        return true;
    }
    catch (char const* error){
        QMessageBox::about(NULL,"Check_DIMM_Exist",error);
    }
}

bool I2C_SMBUS_Controler::Write_SMBUS_And_Verify(int Controller_Address, int Register_Address, int Write_Data){
    try {
        int Verify = 0;
        Write_SMBUS(Controller_Address,Register_Address,Write_Data);
        Verify = Read_SMBUS(Controller_Address,Register_Address);
        if(Write_Data != Verify){
            return false;
        }
        return true;
    }
    catch (char const* error){
        QMessageBox::about(NULL,"Verify_Write_SMBUS",error);
    }
}
