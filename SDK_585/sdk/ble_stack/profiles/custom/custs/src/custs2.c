/**
 ****************************************************************************************
 *
 * @file custs2.c
 *
 * @brief Custom Service profile source file.
 *
 * Copyright (C) 2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"              // SW configuration

#if (BLE_CUSTOM2_SERVER)
#include "custs2.h"
#include "custs2_task.h"
#include "user_custs2_def.h"
#include "attm_db.h"
#include "gapc.h"
#include "prf.h"

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the CUSTS2 module.
 * This function performs all the initializations of the Profile module.
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    env        Collector or Service allocated environment data.
 * @param[in|out] start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     app_task   Application task number.
 * @param[in]     sec_lvl    Security level (AUTH, EKS and MI field of @see enum attm_value_perm_mask)
 * @param[in]     param      Configuration parameters of profile collector or service (32 bits aligned)
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
static uint8_t custs2_init(struct prf_task_env *env, uint16_t *start_hdl, uint16_t app_task, uint8_t sec_lvl, struct custs2_db_cfg *params)
{
    //------------------ create the attribute database for the profile -------------------

    // DB Creation Status
    uint8_t status = ATT_ERR_NO_ERROR;

    status = attm_svc_create_db_128(start_hdl, NULL,
            CUST2_IDX_NB, NULL, env->task, custs2_att_db,
            (sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) | PERM(SVC_PRIMARY, ENABLE));

    //-------------------- allocate memory required for the profile  ---------------------
    if (status == ATT_ERR_NO_ERROR)
    {
        struct custs2_env_tag *custs2_env =
                (struct custs2_env_tag *) ke_malloc(sizeof(struct custs2_env_tag), KE_MEM_ATT_DB);

        // allocate CUSTS2 required environment variable
        env->env = (prf_env_t *)custs2_env;
        custs2_env->shdl = *start_hdl;
        custs2_env->max_nb_att = CUST2_IDX_NB;
        custs2_env->prf_env.app_task = app_task
                | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        custs2_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_CUSTS2;
        env->desc.idx_max           = CUSTS2_IDX_MAX;
        env->desc.state             = custs2_env->state;
        env->desc.default_handler   = &custs2_default_handler;
        co_list_init(&(custs2_env->values));
        custs2_init_ccc_values(custs2_att_db, CUST2_IDX_NB);

        // service is ready, go into an Idle state
        ke_state_set(env->task, CUSTS2_IDLE);
    }

    return status;
}
/**
 ****************************************************************************************
 * @brief Destruction of the CUSTS2 module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static void custs2_destroy(struct prf_task_env *env)
{
    struct custs2_env_tag *custs2_env = (struct custs2_env_tag *)env->env;

    // remove all values present in list
    while (!co_list_is_empty(&(custs2_env->values)))
    {
        struct co_list_hdr *hdr = co_list_pop_front(&(custs2_env->values));
        ke_free(hdr);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(custs2_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void custs2_create(struct prf_task_env *env, uint8_t conidx)
{
    int att_idx;
    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);

    // Find all ccc fields and clean them
    for (att_idx = 1; att_idx < CUST2_IDX_NB; att_idx++)
    {
        // Find only CCC characteristics
        if (custs2_att_db[att_idx].uuid_size == ATT_UUID_16_LEN &&
            *(uint16_t *)custs2_att_db[att_idx].uuid == ATT_DESC_CLIENT_CHAR_CFG)
        {
            // Clear CCC value
            custs2_set_ccc_value(conidx, att_idx, 0);
        }
    }
}

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void custs2_cleanup(struct prf_task_env *env, uint8_t conidx, uint8_t reason)
{
    int att_idx;
    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);

    // Find all ccc fields and clean them
    for (att_idx = 1; att_idx < CUST2_IDX_NB; att_idx++)
    {
        // Find only CCC characteristics
        if (custs2_att_db[att_idx].uuid_size == ATT_UUID_16_LEN &&
            *(uint16_t *)custs2_att_db[att_idx].uuid == ATT_DESC_CLIENT_CHAR_CFG)
        {
            // Clear CCC value
            custs2_set_ccc_value(conidx, att_idx, 0);
        }
    }
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// CUSTS2 Task interface required by profile manager
const struct prf_task_cbs custs2_itf =
{
        (prf_init_fnct) custs2_init,
        custs2_destroy,
        custs2_create,
        custs2_cleanup,
};

#if 0
struct custs2_env_tag custs2_env __attribute__((section("retention_mem_area0"),zero_init)); //@RETENTION MEMORY

/// Custom Service task descriptor
static const struct ke_task_desc TASK_DESC_CUSTS2 = {custs2_state_handler, &custs2_default_handler, custs2_state, CUSTS2_STATE_MAX, CUSTS2_IDX_MAX};
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* custs2_prf_itf_get(void)
{
    return &custs2_itf;
}

#endif // (BLE_CUSTOM2_SERVER)
