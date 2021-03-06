/*
 * cdc_acm_protocol.h
 * 
 * Greaseweazle protocol over CDC ACM streams.
 * 
 * Written & released by Keir Fraser <keir.xen@gmail.com>
 * 
 * This is free and unencumbered software released into the public domain.
 * See the file COPYING for more details, or visit <http://unlicense.org>.
 */


/*
 * GREASEWEAZLE COMMAND SET
 */

/* CMD_GET_INFO, length=3, 0. Returns 32 bytes after ACK. */
#define CMD_GET_INFO        0
/* [BOOTLOADER] CMD_UPDATE, length=6, <update_len>. 
 * Host follows with <update_len> bytes.
 * Bootloader finally returns a status byte, 0 on success. */
#define CMD_UPDATE          1
/* CMD_SEEK, length=3, cyl#. Seek to cyl# on selected drive. */
#define CMD_SEEK            2
/* CMD_SIDE, length=3, side# (0=bottom) */
#define CMD_SIDE            3
/* CMD_SET_PARAMS, length=3+nr, idx, <nr bytes> */
#define CMD_SET_PARAMS      4
/* CMD_GET_PARAMS, length=4, idx, nr_bytes. Returns nr_bytes after ACK. */
#define CMD_GET_PARAMS      5
/* CMD_MOTOR, length=4, drive#, on/off. Turn on/off a drive motor. */
#define CMD_MOTOR           6
/* CMD_READ_FLUX, length=2-3. Optionally include all or part of gw_read_flux.
 * Returns flux readings until EOStream. */
#define CMD_READ_FLUX       7
/* CMD_WRITE_FLUX, length=2-3. Optionally include all or part of gw_write_flux.
 * Host follows with flux readings until EOStream. */
#define CMD_WRITE_FLUX      8
/* CMD_GET_FLUX_STATUS, length=2. Last read/write status returned in ACK. */
#define CMD_GET_FLUX_STATUS 9
/* CMD_GET_INDEX_TIMES, length=4, first, nr.
 * Returns nr*4 bytes after ACK. */
#define CMD_GET_INDEX_TIMES 10
/* CMD_SWITCH_FW_MODE, length=3, <mode> */
#define CMD_SWITCH_FW_MODE 11
/* CMD_SELECT, length=3, drive#. Select drive# as current unit. */
#define CMD_SELECT         12
/* CMD_DESELECT, length=2. Deselect current unit (if any). */
#define CMD_DESELECT       13
/* CMD_SET_BUS_TYPE, length=3, bus_type. Set the bus type. */
#define CMD_SET_BUS_TYPE   14
/* CMD_SET_PIN, length=4, pin#, level. */
#define CMD_SET_PIN        15
/* CMD_RESET, length=2. Reset all state to initial (power on) values. */
#define CMD_RESET          16
/* CMD_ERASE_FLUX, length=6. Argument is gw_erase_flux. */
#define CMD_ERASE_FLUX     17
/* CMD_SOURCE_BYTES, length=6. Argument is gw_sink_source_bytes. */
#define CMD_SOURCE_BYTES   18
/* CMD_SINK_BYTES, length=6. Argument is gw_sink_source_bytes. */
#define CMD_SINK_BYTES     19
#define CMD_MAX            19


/*
 * CMD_SET_BUS CODES
 */
#define BUS_NONE            0
#define BUS_IBMPC           1
#define BUS_SHUGART         2


/*
 * ACK RETURN CODES
 */
#define ACK_OKAY            0
#define ACK_BAD_COMMAND     1
#define ACK_NO_INDEX        2
#define ACK_NO_TRK0         3
#define ACK_FLUX_OVERFLOW   4
#define ACK_FLUX_UNDERFLOW  5
#define ACK_WRPROT          6
#define ACK_NO_UNIT         7
#define ACK_NO_BUS          8
#define ACK_BAD_UNIT        9
#define ACK_BAD_PIN        10


/*
 * CONTROL-CHANNEL COMMAND SET:
 * We abuse SET_LINE_CODING requests over endpoint 0, stashing a command
 * in the baud-rate field.
 */
#define BAUD_NORMAL        9600
#define BAUD_CLEAR_COMMS  10000


/*
 * COMMAND PACKETS
 */

/* CMD_GET_INFO, index 0 */
#define GETINFO_FIRMWARE 0
struct packed gw_info {
    uint8_t fw_major;
    uint8_t fw_minor;
    uint8_t max_index;
    uint8_t max_cmd;
    uint32_t sample_freq;
    uint16_t hw_type;
};

/* CMD_GET_INFO, index 1 */
#define GETINFO_BW_STATS 1
struct packed gw_bw_stats {
    struct packed {
        uint32_t bytes;
        uint32_t usecs;
    } min_bw, max_bw;
};

/* CMD_READ_FLUX */
struct packed gw_read_flux {
    uint8_t nr_idx; /* default: 2 */
};

/* CMD_WRITE_FLUX */
struct packed gw_write_flux {
    uint8_t terminate_at_index; /* default: 0 */
};

/* CMD_ERASE_FLUX */
struct packed gw_erase_flux {
    uint32_t erase_ticks;
};

/* CMD_SINK_SOURCE_BYTES */
struct packed gw_sink_source_bytes {
    uint32_t nr_bytes;
};

/* CMD_{GET,SET}_PARAMS, index 0 */
#define PARAMS_DELAYS 0
struct packed gw_delay {
    uint16_t select_delay; /* usec */
    uint16_t step_delay;   /* usec */
    uint16_t seek_settle;  /* msec */
    uint16_t motor_delay;  /* msec */
    uint16_t auto_off;     /* msec */
};

/* CMD_SWITCH_FW_MODE */
#define FW_MODE_BOOTLOADER 0
#define FW_MODE_NORMAL     1

/*
 * Local variables:
 * mode: C
 * c-file-style: "Linux"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
