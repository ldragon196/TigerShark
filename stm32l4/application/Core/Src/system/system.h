/*
 *  system.h
 *
 *  Created on: Aug 29, 2025
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define FLASH_SECTOR_SIZE     (0x800)

#define ETX_APP_FLASH_ADDR    (0x0800A000)
#define ETX_CONFIG_FLASH_ADDR (ETX_APP_FLASH_ADDR - FLASH_SECTOR_SIZE)
#define ETX_APP_NPAGE         ((ETX_APP_FLASH_ADDR - 0x08000000) / FLASH_SECTOR_SIZE)

/* Reboot reason */
#define ETX_FIRST_TIME_BOOT   (0xFFFFFFFF)     /* First time boot */
#define ETX_NORMAL_BOOT       (0xBEEFFEED)     /* Normal Boot */
#define ETX_OTA_DONE_BOOT     (0xBEEFFDDE)     /* OTA done */
#define ETX_OTA_REQUEST       (0xDEADBEEF)     /* OTA request by application */
#define ETX_LOAD_PREV_APP     (0xFACEFADE)     /* App requests to load the previous version */

#define OTA_FIRMWARE_ADDRESS  (0x10000)
#define MAGIC_NUMBER          (0xAA555AA5)

typedef void (*application_func_t)(void);

typedef struct {
    int32_t fw_size;
    uint32_t fw_crc;
    uint32_t reserved1;
} __attribute__((packed)) ext_slot_t;

typedef struct {
    uint32_t reboot_cause;
    ext_slot_t slot_table;
} __attribute__((packed)) ext_general_cfg_t;

enum {
    JUER_MARINE_ID = 0,
    REB_TEK_ID,
    MANUFACTURER_COUNT
};

enum {
    SURFACE_CONTROL_STATE = 0,
    DIVE_CONTROL_STATE,
};

enum
{
    CLOSED_CIRCUIT = 0,
    OPENED_CIRCUIT
};

enum {
    BIG_NUMBER_MODE = 0,
    OLD_MAN_MODE,
    OLD_MAN_ADV_MODE,
    FIXED_SET_POINT_MODE,
    LEGACY_MODE,
    SCREEN_MODE_COUNT,
};

enum {
    ARROW_NONE = 0,
    ARROW_UP,
    ARROW_DOWN,
};

/* System definitions */
#define O2_SENSOR_NUM               3
#define DIVETHRESH_DECIMETERS       10  /* 1 meter */

#pragma pack(push, 1)

typedef struct {
    uint16_t blink  :1;
    uint16_t arrow  :2;
    uint16_t data   :13;
} ui_sensor_t;

typedef struct {
    uint16_t blink  :1;
    uint16_t data   :15;
} ui_setpoint_t;

typedef struct {
    uint16_t blink  :1;
    uint16_t data   :15;
} ui_tts_t;

typedef struct {
    uint16_t O2;
    uint16_t he;
} gas_mix_t;

typedef struct {
    uint32_t total_number_dives;
    uint32_t total_time_last_dive;
} last_dive_t;

typedef struct {
    /* Old configurations */
    uint32_t depth_units : 1;                 /* 0 = FSW,                   1 = MSW */
    uint32_t pp_o2_units : 1;                 /* 0 = atm_percent,           1 = bars */
    uint32_t diva_enabled : 1;                /* 0 = diva_disabled,         1 = diva_enabled */
    uint32_t diva_mode : 1;                   /* 0 = ppO2 Mode,             1 = Setpoint Mode */
    uint32_t signal_opt_show : 1;             /* 0 = hide signal opt,       1 = show signal opt */
    uint32_t flash_light_opt_show : 1;        /* 0 = hide flashlight opt,   1 = show flashlight opt */
    uint32_t enable_buddy_leds : 1;           /* 0 = buddy leds disabled,   1 = buddy leds enabled */
    uint32_t enable_signal_leds : 1;          /* 0 = signal leds off,       1 = signal leds on */
    uint32_t enable_flash_light : 1;          /* 0 = flashlight off,        1 = flashlight on */
    uint32_t enable_deco_leds : 1;            /* 0 = divaDecoLeds off,      1 = divaDecoLeds on */

    uint32_t show_deco : 1;                   /* 0 = Don't Show,            1 = Show Deco Info */
    uint32_t deco_mode : 1;                   /* 0 = Deco Off,              1 = Deco Mode On */
    uint32_t diva_setpoint_blink_repeat_sec : 3;
    uint32_t screen_rotation : 2;             /* 0 = Landscape Mode,        1 = Portrait Mode */

    /* New configurations */
    uint32_t dark_mode : 1;                   /* 0 = Light Mode,            1 = Dark Mode */
    uint32_t light_time : 2;                  /* light_time_t: Auto turn off light */
    uint32_t enable_led : 1;                  /* 0 = LED TS off,            1 = LED TS on */
    uint32_t fresh_water : 1;                 /* 0 = Saltwater,             1 = Freshwater */
    uint32_t can_enabled : 1;                 /* 0 = Disabled,              1 = Enabled */

    uint32_t reserved : 8;
} user_setting_flags_t;

typedef union {
    uint32_t all;
    user_setting_flags_t flags;
} user_setting_t;

typedef struct {
    uint32_t magic_number;
    char user_name[64];

    uint8_t screen_rotate;
    uint8_t manufacturer_id;
    uint8_t screen_mode;            /* See enum above BIG_NUMBER_MODE,... */
    uint8_t set_point;              /* Default set point */

    uint16_t menu_timeout_sec;
    uint16_t power_off_timeout_sec;

    user_setting_t user_setting;
} system_config_t;

typedef struct {
    int surface_time_sec;
    int dive_time_sec;

    uint8_t cc_mode;                /* 1 = OPENED_CIRCUIT, 0 = CLOSED_CIRCUIT */
    uint8_t dive_state;
    uint8_t deco_mode;
    uint16_t deco_depth;
    uint16_t deco_time;
    uint16_t current_distance;      /* in decimeters */

    /* UI */
    ui_sensor_t sensor[O2_SENSOR_NUM];
    ui_setpoint_t set_point;
    ui_tts_t time_to_surface;
    gas_mix_t gas_mix;

    last_dive_t last_dive_info;
} system_status_t;

#pragma pack(pop)

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

extern system_config_t system_config;
extern system_status_t system_status;

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

#endif /* _SYSTEM_H_ */
