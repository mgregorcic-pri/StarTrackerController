
/**
 ********************************************************************************
 * @file    app.c
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "app.h"
#include "app_comm.h"
#include "system.h"
#include "sdrive.h"
#include "scheduler.h"
#include "mount_config.h"

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/

/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/
float StepModeConversionFactor[16] = {
    1.0f,       // MICROSTEP_FULL_100
    1.0f,       // MICROSTEP_FULL_71
    2.0f,       // MICROSTEP_NON_CIRC_1_2
    2.0f,       // MICROSTEP_1_2
    4.0f,       // MICROSTEP_1_4
    8.0f,       // MICROSTEP_1_8
    16.0f,      // MICROSTEP_1_16
    32.0f,      // MICROSTEP_1_32
    64.0f,      // MICROSTEP_1_64
    128.0f,     // MICROSTEP_1_128
    256.0f      // MICROSTEP_1_256
};

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
void GetInitialPosition(float *Altitude, float *Azimuth);

void ProcessPacket(packet_t *packet);
float BytesToFloat(uint8_t *bytes);
void FloatToBytes(float value, uint8_t *bytes);

void TASK_GetStepperData(void);
void TASK_GetCommCommands(void);
void TASK_SendStatus(void);

void Telescope_SetAzimuthVelocity(float velocity);
void Telescope_SetAltitudeVelocity(float velocity);
void Telescope_SetAzimuthPosition(float position);
void Telescope_SetAltitudePosition(float position);
void Telescope_SetFocuserVelocity(float velocity);
void Telescope_SetFocuserPosition(float position);

/************************************
 * GLOBAL VARIABLES
 ************************************/
sdrive_t SDriveAzimuth = { .canNodeID = 0x15 };
sdrive_t SDriveElevation = { .canNodeID = 0x16 };
sdrive_t SDriveFocus = { .canNodeID = 0x143u };

sdrive_settings_t SDriveSettingsTelescope = {
    .accelerationRamp = 65535.0f,
    .decelerationRamp = 65535.0f,
    .frequency = 8000.0f,
    .holdTorque = TRQ_DAC_100,
    .runTorque = TRQ_DAC_100,
    .phaseCurrent = 800u,
    .stepMode = MICROSTEP_1_16,
    .leftSensor = {
        .type = SENSOR_NONE,
        .polarity = SENSOR_POL_LOW,
        .hysteresisLvlHigh = 0u,
        .hysteresisLvlLow = 0u,
    },
    .rightSensor = {
        .type = SENSOR_NONE,
        .polarity = SENSOR_POL_LOW,
        .hysteresisLvlHigh = 0u,
        .hysteresisLvlLow = 0u,
    },  
    .requestedPosition = 0,
    .controlMode = CONTROL_POSITION,
    .temperatureSensor = NTC,
};

sdrive_settings_t SDriveSettingsFocus = {
    .accelerationRamp = 65535.0f,
    .decelerationRamp = 65535.0f,
    .frequency = 8000.0f,
    .holdTorque = TRQ_DAC_100,
    .runTorque = TRQ_DAC_100,
    .phaseCurrent = 800u,
    .stepMode = MICROSTEP_1_16,
    .leftSensor = {
        .type = SENSOR_NONE,
        .polarity = SENSOR_POL_LOW,
        .hysteresisLvlHigh = 0u,
        .hysteresisLvlLow = 0u,
    },
    .rightSensor = {
        .type = SENSOR_NONE,
        .polarity = SENSOR_POL_LOW,
        .hysteresisLvlHigh = 0u,
        .hysteresisLvlLow = 0u,
    },  
    .requestedPosition = 0,
    .controlMode = CONTROL_POSITION,
    .temperatureSensor = NTC,
};

task_t appTasks[] = 
{
    {.cycleTime = 10, .function = TASK_GetStepperData, .lastTime = 0},
    {.cycleTime = 10, .function = TASK_GetCommCommands, .lastTime = 0},
    {.cycleTime = 100, .function = TASK_SendStatus, .lastTime = 0},
};

scheduler_data_t appSchedule = 
{
    .sysTime = NULL,
    .numOfTasks = 3,
    .tasks = appTasks,
};

app_data_t ApplicationData;

sdrive_status_t azimuthStatus;
sdrive_status_t altitudeStatus;

/************************************
 * STATIC FUNCTIONS
 ************************************/
void ProcessPacket(packet_t *packet)
{
    switch(packet->packetCommand)
    {
        case ACTION_SET_ALT_AZ_VELOCITIES:
            uint32_t altitudeVelocity = 0;
            altitudeVelocity |= ((uint32_t)packet->data[0] << 0);
            altitudeVelocity |= ((uint32_t)packet->data[1] << 8);
            altitudeVelocity |= ((uint32_t)packet->data[2] << 16);
            altitudeVelocity |= ((uint32_t)packet->data[3] << 24);

            uint32_t azimuthVelocity = 0;
            azimuthVelocity |= ((uint32_t)packet->data[4] << 0);
            azimuthVelocity |= ((uint32_t)packet->data[5] << 8);
            azimuthVelocity |= ((uint32_t)packet->data[6] << 16);
            azimuthVelocity |= ((uint32_t)packet->data[7] << 24);

            bool altitudeDirection = packet->data[8] & 0x1;
            bool azimuthDirection = (packet->data[8] >> 1) & 0x1;

            SDRIVE_Run(&SDriveElevation, altitudeVelocity, altitudeDirection);
            SDRIVE_Run(&SDriveAzimuth, azimuthVelocity, azimuthDirection);
            break;

        case ACTION_SET_ALT_AZ_POSITION:
            uint32_t altitudePosition = 0;
            altitudePosition |= ((uint32_t)packet->data[0] << 0);
            altitudePosition |= ((uint32_t)packet->data[1] << 8);
            altitudePosition |= ((uint32_t)packet->data[2] << 16);
            altitudePosition |= ((uint32_t)packet->data[3] << 24);

            uint32_t azimuthPosition = 0;
            azimuthPosition |= ((uint32_t)packet->data[4] << 0);
            azimuthPosition |= ((uint32_t)packet->data[5] << 8);
            azimuthPosition |= ((uint32_t)packet->data[6] << 16);
            azimuthPosition |= ((uint32_t)packet->data[7] << 24);

            SDRIVE_SetPosition(&SDriveElevation, (int32_t)altitudePosition);
            SDRIVE_SetPosition(&SDriveAzimuth, (int32_t)azimuthPosition);
            break;

        case ACTION_SET_FOCUSER_VELOCITY:
            // TODO Implement focuser
            break;
            
        case ACTION_SET_FOCUSER_POSITION:
            // TODO Implement focuser
            break;

        case ACTION_GET_INITIAL_POSITION:
            float altitudeInitPos, azimuthInitPos;
            GetInitialPosition(&altitudeInitPos, &azimuthInitPos);
            uint8_t responseData[8];
            FloatToBytes(altitudeInitPos, &responseData[0]);
            FloatToBytes(azimuthInitPos, &responseData[4]);
            APP_COMM_SendPacket(RESPONSE_SEND_INITIAL_POSITION, responseData, sizeof(responseData));
            break;

        default:
            // TODO Comm error?
            break;
    }
}

float BytesToFloat(uint8_t *bytes)
{
    union{
        uint32_t u;
        float f;
    } number;

    uint32_t uNum = 0;
    uNum |= (bytes[0] << 0);
    uNum |= (bytes[1] << 8);
    uNum |= (bytes[2] << 16);
    uNum |= (bytes[3] << 24);
    number.u = uNum;
    return number.f;
}

void FloatToBytes(float value, uint8_t *bytes)
{
    union{
        uint32_t u;
        float f;
    } number;

    number.f = value;
    bytes[0] = (number.u >> 0) & 0xFF;
    bytes[1] = (number.u >> 8) & 0xFF;
    bytes[2] = (number.u >> 16) & 0xFF;
    bytes[3] = (number.u >> 24) & 0xFF;
}

void TASK_GetStepperData(void)
{
    
}

void TASK_GetCommCommands(void)
{
    packet_t rxPacket;
    APP_COMM_Handler();
    while(APP_COMM_GetPacket(&rxPacket))
    {
        ProcessPacket(&rxPacket);
    }
}

void TASK_SendStatus(void)
{
    uint8_t statusData[18];
    FloatToBytes(ApplicationData.status.altitude, &statusData[0]);
    FloatToBytes(ApplicationData.status.azimuth, &statusData[4]);
    FloatToBytes(ApplicationData.status.altitudeVelocity, &statusData[8]);
    FloatToBytes(ApplicationData.status.azimuthVelocity, &statusData[12]);
    statusData[16] = ApplicationData.status.altitudeEndswitchLow ? 1 : 0;
    statusData[16] |= (ApplicationData.status.altitudeEndswitchHigh ? 1 : 0) << 1;
    statusData[17] = (uint8_t)(ApplicationData.status.temperature >> 8);

    statusData[0] = altitudeStatus.position;
    statusData[1] = azimuthStatus.position;
    APP_COMM_SendPacket(RESPONSE_SEND_STATUS, statusData, sizeof(statusData));

    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
}

void GetInitialPosition(float *Altitude, float *Azimuth)
{
    // TODO implement
    *Altitude = 0.0f;
    *Azimuth = 0.0f;
}

void Telescope_SetAzimuthVelocity(float velocity)
{
    // Convert float to uint (m°/s to steps)
    float velocitySteps = velocity * ((0.001 * AZIMUTH_DEGREES_TO_STEPS) * StepModeConversionFactor[SDriveSettingsTelescope.stepMode]);
    velocitySteps = (uint16_t)velocity;
    SDRIVE_Run(&SDriveAzimuth, velocitySteps > 0 ? velocitySteps : -velocitySteps, velocitySteps < 0);
}

void Telescope_SetAltitudeVelocity(float velocity)
{
    // Convert float to uint (m°/s to steps)
    float velocitySteps = velocity * ((0.001 * ALTITUDE_DEGREES_TO_STEPS) * StepModeConversionFactor[SDriveSettingsTelescope.stepMode]);
    velocitySteps = (uint16_t)velocity;
    SDRIVE_Run(&SDriveElevation, velocitySteps > 0 ? velocitySteps : -velocitySteps, velocitySteps < 0);
}

void Telescope_SetAzimuthPosition(float position)
{
    // Convert float to uint (° to steps)
    int32_t positionSteps = (int32_t)(position * (AZIMUTH_DEGREES_TO_STEPS * StepModeConversionFactor[SDriveSettingsTelescope.stepMode]));
    SDRIVE_SetPosition(&SDriveAzimuth, positionSteps);
}

void Telescope_SetAltitudePosition(float position)
{
    // Convert float to uint (° to steps)
    int32_t positionSteps = (int32_t)(position * (ALTITUDE_DEGREES_TO_STEPS * StepModeConversionFactor[SDriveSettingsTelescope.stepMode]));
    SDRIVE_SetPosition(&SDriveElevation, positionSteps);
}

void Telescope_SetFocuserVelocity(float velocity)
{
    float velocitySteps;
    // Convert float to uint (° to steps)
    // TODO implement focuser
}

void Telescope_SetFocuserPosition(float position)
{
    int32_t positionSteps = 0;
    // TODO convert float to uint (° to steps)
    // TODO implement focuser
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void APP_Init(void)
{
    SDRIVE_Init(&SDriveAzimuth, SDriveSettingsTelescope);
    SDRIVE_Init(&SDriveElevation, SDriveSettingsTelescope);
    SDRIVE_Init(&SDriveFocus, SDriveSettingsFocus);
    SDRIVE_RegisterStatus(&altitudeStatus, SDriveElevation);
    SDRIVE_RegisterStatus(&azimuthStatus, SDriveAzimuth);

    APP_COMM_Init();

    SYSTEM_Init();

    // Init induction scheduler
    appSchedule.sysTime = SYSTEM_GetPtrToTick();
    SCHEDULER_Init(&appSchedule);
}

void APP_Handler(void)
{
    SCHEDULER_Handler(&appSchedule);
    SDRIVE_Handler();
}