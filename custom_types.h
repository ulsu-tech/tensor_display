#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#pragma pack(push, 1)
typedef struct data {
    uint8_t tag;
    int16_t sensor_x;
    int16_t sensor_y;
    int16_t sensor_z;    // optosensor (3*2=6)
    uint8_t button;      // Button ball (1)
    uint16_t tenzor[3];  // 3*2=        (6)
            int32_t enc1;  // position of shaft 1
            int32_t enc2;  // position of shaft 2
            int32_t enc3;  // position of shaft 3
    uint16_t crc;
} PositionalData; 
#pragma pack(pop)

#endif
