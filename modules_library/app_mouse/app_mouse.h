/**
 ****************************************************************************************
 *
 * \file app_mouse.h
 *
 * \brief This module provides an API for acquiring data from a mouse sensor.
 *
 * Define symbol HAS_MOUSE to include this module in the application.
 *
 * Copyright (C) 2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information  
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com>
 *
 *****************************************************************************************
 */
 
/**
 ****************************************************************************************
 * \addtogroup APP_UTILS
 * \{
 * \addtogroup MOUSE
 * \{
 * \addtogroup APP_MOUSE
 * \{
 ****************************************************************************************
 */  


#ifndef APP_MOUSE_H_
#define APP_MOUSE_H_

#include <port_platform.h>
#include "mouse_sensor.h"
#include <app_mouse_config.h>

/**
 ****************************************************************************************
 * \brief 
 ****************************************************************************************
 */
void app_mouse_poll_sensor(void);

/**
 ****************************************************************************************
 * \brief 
 *
 * \return 
 ****************************************************************************************
 */
bool app_mouse_sensor_data_accumulated_over_quota(void);

/**
 ****************************************************************************************
 * \brief 
 *
 * \return 
 ****************************************************************************************
 */
bool app_mouse_has_valid_data(void);

/**
 ****************************************************************************************
 * \brief
 *
 * \param[out]   data
 ****************************************************************************************
 */
void app_mouse_get_data(burst_data_t *data);

/**
 ****************************************************************************************
 * \brief 
 *
 * \return 
 ****************************************************************************************
 */
bool app_mouse_is_active(void);

#ifdef MOUSE_GENERATE_TEST_PATTERN
/**
 ****************************************************************************************
 * \brief 
 ****************************************************************************************
 */
void app_mouse_prepare_next_test_data_sample(void);
#endif

#endif // APP_MOUSE_H_

/**
 * \}
 * \}
 * \}
 */
