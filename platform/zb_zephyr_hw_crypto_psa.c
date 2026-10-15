/*
 * Copyright 2026 NXP
 *
 * NXP Proprietary.
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms. By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 *
 */

/* 
 * for ZBOSS, replacing the slow NaCl zb_sw_crypto_scalarmult.
 * hardware-accelerated Curve25519 (X25519) scalar multiplication
 *
 * Backend: PSA Crypto (psa_raw_key_agreement, ECDH over Curve25519).
 * On RW612 the PSA driver routes X25519 to the ELS hardware accelerator.
 */

#define ZB_TRACE_FILE_ID 30001

#include "zboss_api_core.h"
#include "psa/crypto.h"
#include <string.h>

#include "hw_crypto_api.h"

#define X25519_KEY_LEN  32U

/* One-time PSA init. psa_crypto_init() is idempotent; call it from your
 * platform init as well. Kept here as a guard for standalone use. */
static zb_bool_t s_psa_ready = ZB_FALSE;

static psa_status_t ensure_psa_init(void)
{
    if (!s_psa_ready)
    {
        psa_status_t st = psa_crypto_init();
        if (st != PSA_SUCCESS)
        {
            return st;
        }
        s_psa_ready = ZB_TRUE;
    }

    return PSA_SUCCESS;
}

/*
 * X25519 scalar multiplication: result_point = scalar * point
 *   scalar  : 32-byte little-endian private scalar   (peer/local private key)
 *   point   : 32-byte little-endian u-coordinate      (peer public key)
 *   result  : 32-byte little-endian shared u-coordinate
 *
 * Returns 0 on success (NaCl / ZBOSS convention), non-zero on error.
 */
zb_int_t zb_hw_crypto_scalarmult(zb_uint8_t *result_point,
                                 const zb_uint8_t *scalar,
                                 const zb_uint8_t *point)
{
    psa_status_t          status;
    psa_key_attributes_t  attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t          key_id = PSA_KEY_ID_NULL;
    size_t                out_len = 0U;
    zb_int_t              rc = -1;

    if (result_point == NULL || scalar == NULL || point == NULL)
    {
        return -1;
    }

    if (ensure_psa_init() != PSA_SUCCESS)
    {
        return -1;
    }

    /* Import the 32-byte little-endian scalar as an X25519 (Montgomery-255)
     * private key usable for key agreement. PSA applies RFC-7748 clamping
     * internally, so pass the raw scalar as ZBOSS provides it. */
    psa_set_key_type(&attr,
                     PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY));
    psa_set_key_bits(&attr, 255U);
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&attr, PSA_ALG_ECDH);
    /* Volatile key in RAM; ELS transparent driver handles it. */
    psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_VOLATILE);

    status = psa_import_key(&attr, scalar, X25519_KEY_LEN, &key_id);
    if (status != PSA_SUCCESS)
    {
        goto cleanup;
    }

    /* Raw X25519: shared = scalar * point.
     * PSA peer public key for Montgomery curves is exactly the 32-byte
     * little-endian u-coordinate (no 0x04 prefix, unlike Weierstrass). */
    status = psa_raw_key_agreement(PSA_ALG_ECDH,
                                   key_id,
                                   point, X25519_KEY_LEN,
                                   result_point, X25519_KEY_LEN,
                                   &out_len);
    if (status != PSA_SUCCESS || out_len != X25519_KEY_LEN)
    {
        goto cleanup;
    }

    rc = 0; /* success */

cleanup:
    if (key_id != PSA_KEY_ID_NULL)
    {
        (void)psa_destroy_key(key_id);
    }
    psa_reset_key_attributes(&attr);

    return rc;
}
