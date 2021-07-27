#include "dimm_checker.h"

bool DIMM_Checker::Detected_AMD_CPU(){
    try{
        process.start(CMD_Get_CPU_Manufacturer);
        process.waitForFinished(MAX_WAIT_5_SECOUND);
        process_out_put_data = process.readAllStandardOutput();
        process.close();
        CPU_Alread_Check = true;
        if(process_out_put_data.contains("AMD")){
            return true;
        }
        else{
            return false;
        }
    }
    catch(const char * error){
        QMessageBox::about(NULL,"AMD_CPU",error);
    }
}

int DIMM_Checker::CPU_Support_DIMM_Count(){
    try{
        process.start(CMD_Get_CPU_Support_DIMM_Count);
        process.waitForFinished(MAX_WAIT_5_SECOUND);
        process_out_put_data = process.readAllStandardOutput();
        process.close();

        temp = process_out_put_data.toStdString();
        start_position = temp.find("\n") + 1;
        end_position = temp.find(" ",start_position);
        temp = temp.substr(start_position,end_position - start_position);

//        DIMM_MAX_COUNT = atoi(temp.c_str());
        return atoi(temp.c_str());
    }
    catch(const char *error){
        QMessageBox::about(NULL,"CPU_Support_DIMM_Count",error);
    }
   return 0;
}

bool** DIMM_Checker::Scan_DIMM_Channel_And_Slot(){
    try{
        int Detected_Channel = 0,
            Detected_DIMM = 0,
            English_Letter=26,
            Temp_Detected_Channel,
            Temp_Position;
        std::string Channel,
                    Search_Target;

        if(!CPU_Alread_Check){
            AMD_CPU = Detected_AMD_CPU();
        }

        if(DIMM_MAX_COUNT == 0){
            DIMM_MAX_COUNT = CPU_Support_DIMM_Count();
        }

        DIMM_Exist = new bool *[DIMM_MAX_COUNT / Every_Channel_MAX_DIMM];
        for(int Current_Channel=0; Current_Channel <= DIMM_MAX_COUNT / Every_Channel_MAX_DIMM; Current_Channel++){
            DIMM_Exist[Current_Channel] = new bool[Every_Channel_MAX_DIMM];

            for(int Current_DIMM=0; Current_DIMM < Every_Channel_MAX_DIMM; Current_DIMM++){
                DIMM_Exist[Current_Channel][Current_DIMM] = false;
            }
        }

        process.start(CMD_Get_DIMM_Channel_And_Slot);
        process.waitForFinished(MAX_WAIT_5_SECOUND);
        process_out_put_data = process.readAllStandardOutput();
        temp = process_out_put_data.toStdString();
        std::transform(temp.begin(), temp.end(),temp.begin(), ::toupper);

        start_position=0;
        end_position=0;

        if(AMD_CPU){
//            while(start_position != temp.find_last_of("DIMM ") + Search_Target.size())
            for(int Current_DIMM=0; Current_DIMM < DIMM_MAX_COUNT; Current_DIMM++){
//                start_position = temp.find("CHANNEL ",start_position);
//                if(start_position == Can_not_find_target)break;
//                start_position += 8;

                Detected_Channel=0;
                Search_Target = "CHANNEL ";
                start_position = temp.find(Search_Target,start_position);
                if(start_position == Can_not_find_target){
                    break;
                }
                start_position += Search_Target.size();
                Temp_Position = start_position;

                end_position = temp.find(" ",start_position);
                Channel = temp.substr(start_position,end_position - start_position);

                for(int Channel_Word_Position = 0; Channel_Word_Position < Channel.length(); Channel_Word_Position++){
                    for(int ASCII_English_Letter = (int)'A'; ASCII_English_Letter <= (int)'Z'; ASCII_English_Letter++){
                        if(Channel.at(Channel_Word_Position) == (char)ASCII_English_Letter){
                            Temp_Detected_Channel = pow(English_Letter,Channel.length() - Channel_Word_Position -1);
                            Temp_Detected_Channel *= (ASCII_English_Letter - (int)'A'+1);
                            if(Channel_Word_Position == Channel.length()-1) Temp_Detected_Channel -=1;
                            break;
                        }
                    }
                    Detected_Channel += Temp_Detected_Channel;
                    Temp_Detected_Channel=0;
                }
                qDebug()<<"CHANNEL"<<Channel.c_str()<<Detected_Channel;

                start_position = temp.find("DIMM ",start_position);
                start_position += 5;

                Search_Target = "DIMM ";
                start_position = temp.find(Search_Target,start_position);
                start_position += Search_Target.size();

                end_position = temp.find(" ",start_position);
                Detected_DIMM = atoi(temp.substr(start_position,end_position - start_position).c_str());
                DIMM_Exist[Detected_Channel][Detected_DIMM]=true;
                start_position = Temp_Position;
            }
        }
        else{
            for(int Current_DIMM=0; Current_DIMM < DIMM_MAX_COUNT; Current_DIMM++){
//                start_position = temp.find("CHANNEL");
//                if(start_position == Can_not_find_target)break;
//                start_position +=7;

                Search_Target = "CHANNEL";
                start_position = temp.find(Search_Target,start_position);
                if(start_position == Can_not_find_target){
                    break;
                }
                start_position += Search_Target.size();
                Temp_Position = start_position;

                end_position = temp.find("-",start_position);
                Channel = temp.substr(start_position,end_position - start_position);

                for(int Channel_Word_Position = 0; Channel_Word_Position < Channel.length(); Channel_Word_Position++){
                    for(int ASCII_English_Letter = (int)'A'; ASCII_English_Letter <= (int)'Z'; ASCII_English_Letter++){
                        if(Channel.at(Channel_Word_Position) == (char)ASCII_English_Letter){
                            Temp_Detected_Channel = pow(English_Letter,Channel.length() - Channel_Word_Position -1);
                            Temp_Detected_Channel *= (ASCII_English_Letter - (int)'A'+1);
                            if(Channel_Word_Position == Channel.length()-1) Temp_Detected_Channel -=1;
                            break;
                        }
                    }
                    Detected_Channel += Temp_Detected_Channel;
                    Temp_Detected_Channel=0;
                }
                qDebug()<<"CHANNEL"<<Channel.c_str()<<Detected_Channel;

//                start_position = temp.find("DIMM");
//                start_position +=4;

                Search_Target = "DIMM";
                start_position = temp.find(Search_Target,start_position);
                start_position += Search_Target.size();

                end_position = temp.find("\n",start_position);
                Detected_DIMM = atoi(temp.substr(start_position,end_position - start_position).c_str());

                DIMM_Exist[Detected_Channel][Detected_DIMM]=true;
                start_position = Temp_Position;
            }
        }
        qDebug()<<"Detected_DIMM_Channel_And_Slot:"<<process_out_put_data;
        qDebug()<<"DIMM"<<DIMM_Exist[0][0]<<DIMM_Exist[0][1]<<DIMM_Exist[1][0]<<DIMM_Exist[1][1];
        process.close();
    }
    catch(const char *error){
        QMessageBox::about(NULL,"Detected_DIMM_Channel_And_Slot",error);
    }
    return DIMM_Exist;
}

int DIMM_Checker::Get_Channel_MAX_Count(){
    try{
        if(DIMM_MAX_COUNT == 0){
            DIMM_MAX_COUNT = CPU_Support_DIMM_Count();
        }
        return (DIMM_MAX_COUNT / Every_Channel_MAX_DIMM);
    }
    catch(const char *error){
        QMessageBox::about(NULL,"Get_Channel_MAX_Count",error);
    }
}
