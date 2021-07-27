#ifndef DIMM_CHECKER_H
#define DIMM_CHECKER_H
#include <QString>
#include <QProcess>
#include <QMessageBox>
#include <QDebug>
#include <Windows.h>

enum{
    MAX_WAIT_5_SECOUND = 5000,
    Can_not_find_target =-1,
    Every_Channel_MAX_DIMM=2,

    Exist = true,

};

class DIMM_Checker
{
public:
    bool Detected_AMD_CPU();

    int CPU_Support_DIMM_Count();

    bool** Scan_DIMM_Channel_And_Slot();

    int Get_Channel_MAX_Count();
//    bool **DIMM;
private:
    QProcess process;
    QString process_out_put_data;
    std::string temp;
    int start_position,
        end_position,
        DIMM_MAX_COUNT = 0;

    bool CPU_Alread_Check = false,
         AMD_CPU=false,
         **DIMM_Exist;

    QString CMD_Get_CPU_Manufacturer = "wmic cpu get description",
            CMD_Get_CPU_Support_DIMM_Count = "wmic memphysical get memorydevices",
            CMD_Get_DIMM_Channel_And_Slot = "wmic memorychip get devicelocator,banklabel";
};


#endif // DIMM_CHECKER_H
