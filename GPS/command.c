#include "all.h" 
u8 Stop_Flag[3];
extern union fi64_to_u8 M0SI_GPS;
extern u8 GPS_ERROR_STATE;
void CheckFloat(void)
{
    MISO_CMD = NO_COMMAND;
    MOSI_CMD = CHECK_FLOAT;
    Write_Database(M_G_CMD_ID);
}


void G_M_CMD_FUNC(void)
{
    if(*(Can_Database[G_M_CMD_ID].Data_ptr) == CF_NORMAL)
    {
        
    }
    else if(*(Can_Database[G_M_CMD_ID].Data_ptr) == CF_ERROR)
    {
        
    }
}

void S_GPS_X_FUNC(void)
{
    GPS.position.x = MISO_GPS_X.f64_data;
}

void S_GPS_Y_FUNC(void)
{
    GPS.position.y = MISO_GPS_Y.f64_data;
}

void S_GPS_ANG_FUNC(void)
{
    GPS.radian = MISO_GPS_ANG.f64_data;
}

void S_GYRO_C1_FUNC(void)
{
    Gyro.convert1 = MISO_GYRO_C1.f64_data;
}

void S_GYRO_C2_FUNC(void)
{
    Gyro.convert2 = MISO_GYRO_C2.f64_data;
}

void S_GYRO_RAD_FUNC(void)
{
    Gyro.angle = MISO_GYRO_RAD.f64_data;
}

void S_ENC1_C1_FUNC(void)
{
    Encoder[0].convert1 = MISO_ENC1_C1.f64_data;
}

void S_ENC1_C2_FUNC(void)
{
    Encoder[0].convert2 = MISO_ENC1_C2.f64_data;
}

void S_ENC1_RAD_FUNC(void)
{
    Encoder[0].r = MISO_ENC1_RAD.f64_data;
}

void S_ENC1_ANG_FUNC(void)
{
    Encoder[0].radian = MISO_ENC1_ANG.f64_data;
}

void S_ENC1_DIS_FUNC(void)
{
    Encoder[0].dis = MISO_ENC1_DIS.f64_data;
}

void S_ENC2_C1_FUNC(void)
{
    Encoder[1].convert1 = MISO_ENC2_C1.f64_data;
}

void S_ENC2_C2_FUNC(void)
{
    Encoder[1].convert2 = MISO_ENC2_C2.f64_data;
}

void S_ENC2_RAD_FUNC(void)
{
    Encoder[1].r = MISO_ENC2_RAD.f64_data;
}

void S_ENC2_ANG_FUNC(void)
{
    Encoder[1].radian = MISO_ENC2_ANG.f64_data;
}

void S_ENC2_DIS_FUNC(void)
{
    Encoder[1].dis = MISO_ENC2_DIS.f64_data;
}

void S_FACTOR_FUNC(void)
{                                                                                                                                                    
//     Factor = MOSI_FACTOR.f32_data;
}
void S_STATUS_FUNC(void)
{
    
}

void S_CONTINUE_FUNC(void)
{
//     Write_Database(W_RECEIVE_FLAG_ID);
}

void S_PASS_GEREGE_FUNC(void)
{

}

void S_GYRO_FLOAT_FUNC(void)
{
//     MISO_CMD = CF_NORMAL;
}
extern u8 FilterFlag[6];
extern u8 GPS_ERROR_STATE;
void S_GYRO_ERROR_FUNC(void)
{

}

