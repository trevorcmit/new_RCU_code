/**
 ****************************************************************************************
 *
 * @file port_ble_gap.h
 *
 * @brief BLE GAP definitions
 *
 * Copyright (C) 2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information  
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com>
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * \addtogroup APP_UTILS
 * \{
 * \addtogroup GAP
 * \{
 * \addtogroup PORT_BLE_GAP
 * \{
 ****************************************************************************************	 
 */

#ifndef PORT_BLE_GAP_H_
#define PORT_BLE_GAP_H_

/** GAP device external appearance */
typedef enum {
        BLE_GAP_APPEARANCE_UNKNOWN = 0,
        BLE_GAP_APPEARANCE_GENERIC_PHONE = 64,
        BLE_GAP_APPEARANCE_GENERIC_COMPUTER = 128,
        BLE_GAP_APPEARANCE_GENERIC_WATCH = 192,
        BLE_GAP_APPEARANCE_WATCH_SPORTS_WATCH = 193,
        BLE_GAP_APPEARANCE_GENERIC_CLOCK = 256,
        BLE_GAP_APPEARANCE_GENERIC_DISPLAY = 320,
        BLE_GAP_APPEARANCE_GENERIC_REMOTE_CONTROL = 384,
        BLE_GAP_APPEARANCE_GENERIC_EYE_GLASSES = 448,
        BLE_GAP_APPEARANCE_GENERIC_TAG = 512,
        BLE_GAP_APPEARANCE_GENERIC_KEYRING = 576,
        BLE_GAP_APPEARANCE_GENERIC_MEDIA_PLAYER = 640,
        BLE_GAP_APPEARANCE_GENERIC_BARCODE_SCANNER = 704,
        BLE_GAP_APPEARANCE_GENERIC_THERMOMETER = 768,
        BLE_GAP_APPEARANCE_THERMOMETER_EAR = 769,
        BLE_GAP_APPEARANCE_GENERIC_HEART_RATE_SENSOR = 832,
        BLE_GAP_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT = 833,
        BLE_GAP_APPEARANCE_GENERIC_BLOOD_PRESSURE = 896,
        BLE_GAP_APPEARANCE_BLOOD_PRESSURE_ARM = 897,
        BLE_GAP_APPEARANCE_BLOOD_PRESSURE_WRIST = 898,
        BLE_GAP_APPEARANCE_GENERIC_HID = 960,
        BLE_GAP_APPEARANCE_HID_KEYBOARD = 961,
        BLE_GAP_APPEARANCE_HID_MOUSE = 962,
        BLE_GAP_APPEARANCE_HID_JOYSTICK = 963,
        BLE_GAP_APPEARANCE_HID_GAMEPAD = 964,
        BLE_GAP_APPEARANCE_HID_DIGITIZER_TABLET = 965,
        BLE_GAP_APPEARANCE_HID_CARD_READER = 966,
        BLE_GAP_APPEARANCE_HID_DIGITAL_PEN = 967,
        BLE_GAP_APPEARANCE_HID_BARCODE_SCANNER = 968,
        BLE_GAP_APPEARANCE_GENERIC_GLUCOSE_METER = 1024,
        BLE_GAP_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR = 1088,
        BLE_GAP_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE = 1089,
        BLE_GAP_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE = 1090,
        BLE_GAP_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP = 1091,
        BLE_GAP_APPEARANCE_GENERIC_CYCLING = 1152,
        BLE_GAP_APPEARANCE_CYCLING_CYCLING_COMPUTER = 1153,
        BLE_GAP_APPEARANCE_CYCLING_SPEED_SENSOR = 1154,
        BLE_GAP_APPEARANCE_CYCLING_CADENCE_SENSOR = 1155,
        BLE_GAP_APPEARANCE_CYCLING_POWER_SENSOR = 1156,
        BLE_GAP_APPEARANCE_CYCLING_SPEED_AND_CADENCE_SENSOR = 1157,
        BLE_GAP_APPEARANCE_GENERIC_PULSE_OXIMETER = 3136,
        BLE_GAP_APPEARANCE_PULSE_OXIMETER_FINGERTIP = 3137,
        BLE_GAP_APPEARANCE_PULSE_OXIMETER_WRIST_WORN = 3138,
        BLE_GAP_APPEARANCE_GENERIC_WEIGHT_SCALE = 3200,
        BLE_GAP_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACTIVITY = 5184,
        BLE_GAP_APPEARANCE_OUTDOOR_SPORTS_ACT_LOCATION_DISPLAY = 5185,
        BLE_GAP_APPEARANCE_OUTDOOR_SPORTS_ACT_LOCATION_AND_NAVIGATION_DISPLAY = 5186,
        BLE_GAP_APPEARANCE_OUTDOOR_SPORTS_ACT_LOCATION_POD = 5187,
        BLE_GAP_APPEARANCE_OUTDOOR_SPORTS_ACT_LOCATION_AND_NAVIGATION_POD = 5188,
        // dummy appearance ID
        BLE_GAP_APPEARANCE_LAST,
} gap_appearance_t;

/**
 * GAP Advertising Data Types, as defined by Bluetooth Core 4.2 specification
 *
 * \note: only data types valid for Advertising Data are included
 */
typedef enum {
        GAP_DATA_TYPE_FLAGS               = 0x01,
        GAP_DATA_TYPE_UUID16_LIST_INC     = 0x02,
        GAP_DATA_TYPE_UUID16_LIST         = 0x03,
        GAP_DATA_TYPE_UUID32_LIST_INC     = 0x04,
        GAP_DATA_TYPE_UUID32_LIST         = 0x05,
        GAP_DATA_TYPE_UUID128_LIST_INC    = 0x06,
        GAP_DATA_TYPE_UUID128_LIST        = 0x07,
        GAP_DATA_TYPE_SHORT_LOCAL_NAME    = 0x08,
        GAP_DATA_TYPE_LOCAL_NAME          = 0x09,
        GAP_DATA_TYPE_TX_POWER_LEVEL      = 0x0A,
        GAP_DATA_TYPE_SLAVE_CONN_INTV     = 0x12,
        GAP_DATA_TYPE_UUID16_SOLIC        = 0x14,
        GAP_DATA_TYPE_UUID32_SOLIC        = 0x1F,
        GAP_DATA_TYPE_UUID128_SOLIC       = 0x15,
        GAP_DATA_TYPE_UUID16_SVC_DATA     = 0x16,
        GAP_DATA_TYPE_UUID32_SVC_DATA     = 0x20,
        GAP_DATA_TYPE_UUID128_SVC_DATA    = 0x21,
        GAP_DATA_TYPE_PUBLIC_ADDRESS      = 0x17,
        GAP_DATA_TYPE_RANDOM_ADDRESS      = 0x18,
        GAP_DATA_TYPE_APPEARANCE          = 0x19,
        GAP_DATA_TYPE_ADV_INTERVAL        = 0x1A,
        GAP_DATA_TYPE_MANUFACTURER_SPEC   = 0xFF,
} gap_data_type_t;
        
#endif // PORT_BLE_GAP_H_

/**
 * \}
 * \}
 * \}
 */