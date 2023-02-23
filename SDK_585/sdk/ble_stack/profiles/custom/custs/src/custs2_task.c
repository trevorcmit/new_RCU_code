/**
 ****************************************************************************************
 *
 * @file custs2_task.c
 *
 * @brief Custom Service profile task source file.
 *
 * Copyright (C) 2014 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */

#include "rwble_config.h"              // SW configuration

#if (BLE_CUSTOM2_SERVER)
#include "custs2_task.h"
#include "custs2.h"
#include "custom_common.h"
#include "app_customs.h"
#include "attm_db_128.h"
#include "ke_task.h"
#include "gapc.h"
#include "gapc_task.h"
#include "gattc_task.h"
#include "attm_db.h"
#include "prf_utils.h"
#include "app_prf_types.h"

#if (BLE_CUSTOM_SERVER)
#include "user_custs_config.h"
#endif // (BLE_CUSTOM_SERVER)

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Stores characteristic value.
 * @param[in] att_idx  Custom attribut index.
 * @param[in] length   Value length.
 * @param[in] data     Pointer to value data.
 * @return 0 on success.
 ****************************************************************************************
 */
static int custs2_att_set_value(uint8_t att_idx, uint16_t length, const uint8_t *data)
{
    struct custs2_env_tag *custs2_env = PRF_ENV_GET(CUSTS2, custs2);
    // Check value in already present in service
    struct custs2_val_elmt *val = (struct custs2_val_elmt *) co_list_pick(&(custs2_env->values));
    // loop until value found
    while (val != NULL)
    {
        // value is present in service
        if (val->att_idx == att_idx)
        {
            // Value present but size changed, free old value
            if (length != val->length)
            {
                co_list_extract(&custs2_env->values, &val->hdr, 0);
                ke_free(val);
                val = NULL;
            }
            break;
        }

        val = (struct custs2_val_elmt *)val->hdr.next;
    }

    if (val == NULL)
    {
        // allocate value data
        val = (struct custs2_val_elmt *) ke_malloc(sizeof(struct custs2_val_elmt) + length, KE_MEM_ATT_DB);
        // insert value into the list
        co_list_push_back(&custs2_env->values, &val->hdr);
    }
    val->att_idx = att_idx;
    val->length = length;
    memcpy(val->data, data, length);

    return 0;
}

/**
 ****************************************************************************************
 * @brief Read characteristic value from.
 * Function checks if attribute exists, and if so return its length and pointer to data.
 * @param[in]  att_idx  Custom attribute index.
 * @param[out] length   Pointer to variable that receive length of the attribute.
 * @param[out] data     Pointer to variable that receive pointer characteristic value.
 * @return 0 on success, ATT_ERR_ATTRIBUTE_NOT_FOUND if there is no value for such attribyte.
 ****************************************************************************************
 */
static int custs2_att_get_value(uint8_t att_idx, uint16_t *length, const uint8_t **data)
{
    struct custs2_env_tag *custs2_env = PRF_ENV_GET(CUSTS2, custs2);
    // Check value in already present in service
    struct custs2_val_elmt *val = (struct custs2_val_elmt *) co_list_pick(&custs2_env->values);
    ASSERT_ERR(data);
    ASSERT_ERR(length);

    // loop until value found
    while (val != NULL)
    {
        // value is present in service
        if (val->att_idx == att_idx)
        {
            *length = val->length;
            *data = val->data;
            break;
        }

        val = (struct custs2_val_elmt *)val->hdr.next;
    }

    if (val == NULL)
    {
        *length = 0;
        *data = NULL;
    }
    return val ? 0 : ATT_ERR_ATTRIBUTE_NOT_FOUND;
}

/**
 ****************************************************************************************
 * @brief Sets initial values for all Clinet Characteristic Configurations.
 * @param[in]  att_db     Custom service attribute definition table.
 * @param[in]  max_nb_att Number of elements in att_db.
 ****************************************************************************************
 */
void custs2_init_ccc_values(const struct attm_desc_128 *att_db, int max_nb_att)
{
    // Default values 0 means no notification
    uint8_t ccc_values[BLE_CONNECTION_MAX] = {0};
    int i;

    // Start form 1, skip service description
    for (i = 1; i < max_nb_att; i++)
    {
        // Find only CCC characteristics
        if (att_db[i].uuid_size == ATT_UUID_16_LEN &&
            *(uint16_t *)att_db[i].uuid == ATT_DESC_CLIENT_CHAR_CFG)
        {
            // Set default values for all possible connections
            custs2_att_set_value(i, sizeof(ccc_values), ccc_values);
        }
    }
}

/**
 ****************************************************************************************
 * @brief Set value of CCC for given attribute and connection index.
 * @param[in] conidx   Connection index.
 * @param[in] att_idx  CCC attribute index.
 * @param[in] cc       Value to store.
 ****************************************************************************************
 */
void custs2_set_ccc_value(uint8_t conidx, uint8_t att_idx, uint16_t ccc)
{
    uint16_t length;
    const uint8_t *value;
    uint8_t new_value[BLE_CONNECTION_MAX];
    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);

    custs2_att_get_value(att_idx, &length, &value);
    ASSERT_ERR(length);
    ASSERT_ERR(value);
    memcpy(new_value, value, length);
    // For now there are only two valid values for ccc, store just one byte other is 0 anyway
    new_value[conidx] = (uint8_t)ccc;
    custs2_att_set_value(att_idx, length, new_value);
}

/**
 ****************************************************************************************
 * @brief Read value of CCC for given attribute and connection index.
 * @param[in]  conidx   Connection index.
 * @param[in]  att_idx  Custom attribute index.
 * @return Value of CCC.
 ****************************************************************************************
 */
static uint16_t custs2_get_ccc_value(uint8_t conidx, uint8_t att_idx)
{
    uint16_t length;
    const uint8_t *value;
    uint16_t ccc_value;

    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);

    custs2_att_get_value(att_idx, &length, &value);
    ASSERT_ERR(length);
    ASSERT_ERR(value);

    ccc_value = value[conidx];

    return ccc_value;
}

static void custs2_exe_operation(void)
{
    struct custs2_env_tag *custs2_env = PRF_ENV_GET(CUSTS2, custs2);
    ASSERT_ERR(custs2_env->operation != NULL);
    bool notification_sent = false;
    const uint8_t *ccc_values;
    uint16_t length;

    struct custs2_val_ntf_ind_req *app_req = (struct custs2_val_ntf_ind_req *)ke_msg2param(custs2_env->operation);

    custs2_att_get_value(custs2_env->ccc_idx, &length, &ccc_values);
    ASSERT_ERR(length == BLE_CONNECTION_MAX);

    // loop on all connections
    while (!notification_sent && custs2_env->cursor < BLE_CONNECTION_MAX)
    {
        struct gattc_send_evt_cmd *req;
        uint8_t cursor = custs2_env->cursor++;

        // Check if notification or indication is set for connection
        if ((app_req->notification && ((ccc_values[cursor] & PRF_CLI_START_NTF) == 0)) ||
            (!app_req->notification && ((ccc_values[cursor] & PRF_CLI_START_IND) == 0)))
            continue;

        notification_sent = true;

        // Allocate the GATT notification message
        req = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
            KE_BUILD_ID(TASK_GATTC, cursor), custs2_env->operation->dest_id, gattc_send_evt_cmd, app_req->length);

        // Fill in the parameter structure
        req->operation = app_req->notification ? GATTC_NOTIFY : GATTC_INDICATE;
        req->handle = custs2_env->shdl + app_req->handle;
        req->length = app_req->length;
        memcpy(req->value, app_req->value, app_req->length);

        // Send the event
        ke_msg_send(req);
    }

    // check if operation finished
    if (!notification_sent)
    {
        if (app_req->notification)
        {
            // Inform the application that the notification PDU has been sent over the air.
            struct custs2_val_ntf_cfm *cfm = KE_MSG_ALLOC(CUSTS2_VAL_NTF_CFM,
                                                          custs2_env->operation->src_id, custs2_env->operation->dest_id,
                                                          custs2_val_ntf_cfm);
            cfm->handle = app_req->handle;
            cfm->status = GAP_ERR_NO_ERROR;
            ke_msg_send(cfm);
        }
        else
        {
            // Inform the application that the indication has been confirmed by the peer device.
            struct custs2_val_ind_cfm *cfm = KE_MSG_ALLOC(CUSTS2_VAL_IND_CFM,
                                                          custs2_env->operation->src_id, custs2_env->operation->dest_id,
                                                          custs2_val_ind_cfm);
            cfm->handle = app_req->handle;
            cfm->status = GAP_ERR_NO_ERROR;
            ke_msg_send(cfm);
        }
        ke_free(custs2_env->operation);
        custs2_env->operation = NULL;
        ke_state_set(prf_src_task_get(&(custs2_env->prf_env), 0), CUSTS2_IDLE);
    }
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_CMP_EVT message.
 * @details The GATTC_CMP_EVT message that signals the completion of a GATTC_NOTIFY
 *          operation is sent back as soon as the notification PDU has been sent over
 *          the air.
 *          The GATTC_CMP_EVT message that signals the completion of a GATTC_INDICATE
 *          operation is sent back as soon as the ATT_HANDLE_VALUE_CONFIRMATION PDU is
 *          received confirming that the indication has been correctly received by the
 *          peer device.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,
                                 struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    struct custs2_env_tag *custs2_env = PRF_ENV_GET(CUSTS2, custs2);

    if (param->operation == GATTC_NOTIFY || param->operation == GATTC_INDICATE)
    {
        custs2_exe_operation();
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref CUSTS2_VAL_SET_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int custs2_val_set_req_handler(ke_msg_id_t const msgid,
                                      struct custs2_val_set_req const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    struct custs2_env_tag *custs2_env = PRF_ENV_GET(CUSTS2, custs2);
    // Update value in DB
    attmdb_att_set_value(custs2_env->shdl + param->handle, param->length, 0, (uint8_t *)&param->value);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref CUSTS2_VAL_NTF_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int custs2_val_ntf_req_handler(ke_msg_id_t const msgid,
                                      struct custs2_val_ntf_ind_req const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    struct custs2_env_tag *custs2_env = PRF_ENV_GET(CUSTS2, custs2);
    uint16_t ccc_hdl;
    uint16_t handle = custs2_env->shdl + param->handle;
    uint8_t ccc_idx;

    const uint8_t *ccc_val;
    att_size_t ccc_length;

    uint8_t state = ke_state_get(dest_id);
    if (state == CUSTS2_BUSY)
    {
        return KE_MSG_SAVED;
    }

    // Update value in DB
    // TODO: Check if value should be stored in DB at all
    attmdb_att_set_value(handle, param->length, 0, (uint8_t *)&param->value);

    // Find the handle of the Characteristic Client Configuration
    ccc_hdl = get_cfg_handle(handle);
    ASSERT_ERR(ccc_hdl);

    // Convert handle to index
    custs2_get_att_idx(ccc_hdl, &ccc_idx);

    custs2_att_get_value(ccc_idx, &ccc_length, &ccc_val);

    ke_state_set(dest_id, CUSTS2_BUSY);
    custs2_env->operation = ke_param2msg(param);
    custs2_env->cursor = 0;
    custs2_env->ccc_idx = ccc_idx;

    // Trigger notification
    custs2_exe_operation();

    return KE_MSG_NO_FREE;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref CUSTS2_VAL_IND_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int custs2_val_ind_req_handler(ke_msg_id_t const msgid,
                                      struct custs2_val_ind_req const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    struct custs2_env_tag *custs2_env = PRF_ENV_GET(CUSTS2, custs2);
    uint16_t cfg_hdl;
    uint8_t status = ATT_ERR_NO_ERROR;

    uint8_t *cfg_val;
    att_size_t length;

    // Update value in DB
    // TODO: Check if value should go to db at all
    attmdb_att_set_value(param->handle, param->length, 0, (uint8_t *)&param->value);

    // Find the handle of the Characteristic Client Configuration
    cfg_hdl = get_cfg_handle(custs2_env->shdl + param->handle);
    ASSERT_ERR(cfg_hdl);

    // Check if indications are enabled.
    attmdb_get_value(cfg_hdl, &length, &cfg_val);

    // Send indication through GATT
    if (*((uint16_t*)cfg_val) == PRF_CLI_START_IND)
    {
        // Allocate the GATT notification message
        struct gattc_send_evt_cmd *req = KE_MSG_ALLOC(GATTC_SEND_EVT_CMD,
            KE_BUILD_ID(TASK_GATTC, param->conidx), dest_id, gattc_send_evt_cmd);

        // Fill in the parameter structure
        req->operation = GATTC_INDICATE;
        req->handle = param->handle;

        // Send the event
        ke_msg_send(req);
    }
    else
    {
        status = PRF_ERR_IND_DISABLED;
    }

    if (status != ATT_ERR_NO_ERROR)
    {
        // Send CFM to APP that value has been sent or not
        struct custs2_val_ind_cfm *cfm = KE_MSG_ALLOC(CUSTS2_VAL_IND_CFM,
                                                      src_id, dest_id,
                                                      custs2_val_ind_cfm);

        cfm->handle = param->handle;
        cfm->conidx = param->conidx;
        cfm->status = status;

        ke_msg_send(cfm);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_READ_REQ_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_req_ind_handler(ke_msg_id_t const msgid, struct gattc_read_req_ind const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct gattc_read_cfm * cfm;
    uint8_t att_idx = 0;
    uint8_t conidx = KE_IDX_GET(src_id);
    // retrieve handle information
    uint8_t status = custs2_get_att_idx(param->handle, &att_idx);
    uint16_t length = 0;
    uint16_t ccc_val = 0;
    struct custs2_env_tag *custs2_env = PRF_ENV_GET(CUSTS2, custs2);

    // If the attribute has been found, status is GAP_ERR_NO_ERROR
    if (status == GAP_ERR_NO_ERROR)
    {
        const struct cust_prf_func_callbacks *callbacks = custs_get_func_callbacks(TASK_ID_CUSTS2);

        if (callbacks->att_db[att_idx].uuid_size == ATT_UUID_16_LEN &&
            *(uint16_t *)callbacks->att_db[att_idx].uuid == ATT_DESC_CLIENT_CHAR_CFG)
        {
            ccc_val = custs2_get_ccc_value(conidx, att_idx);
            length = 2;
        }
        else
        {
            status = PRF_APP_ERROR;
        }
    }

    // Send read response
    cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, length);
    cfm->handle = param->handle;
    cfm->status = status;
    cfm->length = length;

    if (status == GAP_ERR_NO_ERROR)
    {
        memcpy(cfm->value, &ccc_val, length);
    }

    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_WRITE_REQ_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_write_req_ind_handler(ke_msg_id_t const msgid, const struct gattc_write_req_ind *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct custs2_env_tag *custs2_env = PRF_ENV_GET(CUSTS2, custs2);
    struct gattc_write_cfm * cfm;
    uint8_t att_idx = 0;
    uint8_t conidx = KE_IDX_GET(src_id);
    // retrieve handle information
    uint8_t status = custs2_get_att_idx(param->handle, &att_idx);
    att_perm_type perm;

    ASSERT_ERR(param->offset == 0);
    // If the attribute has been found, status is ATT_ERR_NO_ERROR
    if (status == ATT_ERR_NO_ERROR)
    {
        const struct cust_prf_func_callbacks *callbacks = custs_get_func_callbacks(TASK_ID_CUSTS2);

        if (callbacks->att_db[att_idx].uuid_size == ATT_UUID_16_LEN &&
            *(uint16_t *)callbacks->att_db[att_idx].uuid == ATT_DESC_CLIENT_CHAR_CFG)
        {
            struct attm_elmt elem = {0};

            // Find the handle of the Characteristic Value
            uint16_t value_hdl = get_value_handle(param->handle);
            ASSERT_ERR(value_hdl);

            // Get permissions to identify if it is NTF or IND.
            attmdb_att_get_permission(value_hdl, &perm, 0, &elem);
            status = check_client_char_cfg(PERM_IS_SET(perm, NTF, ENABLE), param);

            if (status == ATT_ERR_NO_ERROR)
            {
                custs2_set_ccc_value(conidx, att_idx, *(uint16_t *)param->value);
            }
        }
        else
        {
            if (callbacks != NULL && callbacks->value_wr_validation_func != NULL)
                status = callbacks->value_wr_validation_func(att_idx, param->offset, param->length, (uint8_t *)&param->value[0]);

            if (status == ATT_ERR_NO_ERROR)
            {
                // Set value in the database
                status = attmdb_att_set_value(param->handle, param->length, param->offset, (uint8_t *)&param->value[0]);
            }
        }

        if (status == ATT_ERR_NO_ERROR)
        {
            // Inform APP
            struct custs2_val_write_ind *req_id = KE_MSG_ALLOC_DYN(CUSTS2_VAL_WRITE_IND,
                                                    prf_dst_task_get(&(custs2_env->prf_env), KE_IDX_GET(src_id)),
                                                    dest_id, custs2_val_write_ind,
                                                    param->length);
            memcpy(req_id->value, param->value, param->length);
            //req_id->conhdl = gapc_get_conhdl(custs2_env.con_info.conidx);
            req_id->handle = att_idx;
            req_id->length = param->length;

            ke_msg_send(req_id);
        }
    }

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
    cfm->handle = param->handle;
    cfm->status = status;
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler custs2_default_state[] =
{
    {GATTC_READ_REQ_IND,            (ke_msg_func_t)gattc_read_req_ind_handler},
    {GATTC_WRITE_REQ_IND,           (ke_msg_func_t)gattc_write_req_ind_handler},
    {GATTC_CMP_EVT,                 (ke_msg_func_t)gattc_cmp_evt_handler},
    {CUSTS2_VAL_NTF_REQ,            (ke_msg_func_t)custs2_val_ntf_req_handler},
    {CUSTS2_VAL_SET_REQ,            (ke_msg_func_t)custs2_val_set_req_handler},
    {CUSTS2_VAL_IND_REQ,            (ke_msg_func_t)custs2_val_ind_req_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler custs2_default_handler = KE_STATE_HANDLER(custs2_default_state);

#endif // BLE_CUSTOM2_SERVER