#ifndef _COMMAND_H
#define _COMMAND_H
    #include "stm32f4xx.h"
//     #define SA_READ(a)        (MISO_SWITCH[0]>>(a-1) & 0x01)
//     #define ST_READ(a)        (MISO_SWITCH[1]>>(a-1) & 0x01)
    #define M_IO_READ(a,b)     (IO_State[a-W_MOTOR1_OLD_ID] >> (b-1)    & 0x01)
    
    //SLAVER--GPS
    void CheckFloat(void);
    void G_M_CMD_FUNC(void);
    void S_GPS_X_FUNC(void);
    void S_GPS_Y_FUNC(void);
    void S_GPS_ANG_FUNC(void);
    void S_GYRO_C1_FUNC(void);
    void S_GYRO_C2_FUNC(void);
    void S_GYRO_RAD_FUNC(void);
    void S_ENC1_C1_FUNC(void);
    void S_ENC1_C2_FUNC(void);
    void S_ENC1_RAD_FUNC(void);
    void S_ENC1_ANG_FUNC(void);
    void S_ENC1_DIS_FUNC(void);
    void S_ENC2_C1_FUNC(void);
    void S_ENC2_C2_FUNC(void);
    void S_ENC2_RAD_FUNC(void);
    void S_ENC2_ANG_FUNC(void);
    void S_FACTOR_FUNC(void);
    void S_STATUS_FUNC(void);
    void S_ENC2_DIS_FUNC(void);
    void S_CONTINUE_FUNC(void);
    void S_RECEIVE_FUNC(void);
    void S_GYRO_FLOAT_FUNC(void);
    void S_PASS_GEREGE_FUNC(void);
    void S_LASER_LENGTH_1_FUNC(void);
    void S_LASER_LENGTH_2_FUNC(void);
    void S_GYRO_ERROR_FUNC(void);

    #define Restart               190

#endif
