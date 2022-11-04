#pragma once
#include <types.h>


//primary controller from port 0x1F0 - 0x1F7
#define LBA_PRIMARY_CONTROLLER_PORT_BASE 0x1F0
#define LOW_LBA_PORT_PRIMARY 0x1F3
#define LBA_PRIMARY_DRIVE_SELECT_PORT 0x1F6
#define LBA_PRIMARY_CONTROLLER_PORT_STATUS_PORT 0x1F7
//secondary controller from port 0x170 - 0x177
#define LBA_SECONDARY_CONTROLLER_PORT_BASE 0x170
#define LOW_LBA_PORT_SECONDARY 0x173
#define LBA_SECONDARY_DRIVE_SELECT_PORT 0x176
#define LBA_SECONDARY_CONTROLLER_STATUS_PORT 0x177






#define ATA_PRIMARY  0x1F0
#define ATA_SECONDARY  0x170
#define ATA_PRIMARY_DCR  0x3F6
#define ATA_SECONDARY_DCR  0x376
#define ATA_PRIMARY_MASTER  1
#define ATA_PRIMARY_SLAVE  2
#define ATA_SECONDARY_MASTER  3
#define ATA_SECONDARY_SLAVE  4
#define ATA_reg_data  0
#define ATA_reg_error_feature  1
#define ATA_reg_sector_count  2
#define ATA_reg_lba0  3
#define ATA_reg_lba1  4
#define ATA_reg_lba2  5
#define ATA_reg_selector  6
#define ATA_reg_command_status  7
#define ATA_reg_sector_count2  8
#define ATA_reg_lba3  9
#define ATA_reg_lba4  10
#define ATA_reg_lba5  11
#define ATA_reg_control_status  12
#define ATA_reg_dev_address  13
#define ATA_type_master  0xA0
#define ATA_type_slave  0xB0
#define ATA_state_busy  0x80
#define ATA_state_fault  0x20
#define ATA_state_sr_drq  0x08
#define ATA_state_error  0x01
#define ATA_cmd_identify  0xEC
#define ATA_cmd_read  0x24
#define ATA_cmd_write 0x34


void lba_init();
enum drives{
    first_drive,
    second_drive
};
enum ide_controller{
    primary_controller,
    secondary_controller
};
void ata_write(bool primary, uint16_t ata_register, uint16_t whattowrite);
uint8_t ata_read(bool primary, uint16_t ata_register);
void read_ata_primary_controller_first_drive(uint64_t where, uint32_t count, uint8_t *buffer);
void read_ata_primary_controller_second_drive(uint64_t where, uint32_t count, uint8_t *buffer);
void read_ata_secondary_controller_first_drive(uint64_t where, uint32_t count, uint8_t *buffer);
void read_ata_secondary_controller_second_drive(uint64_t where, uint32_t count, uint8_t *buffer);
// void write_ata_primary_controller_first_drive(uint64_t where, uint32_t count, uint8_t *buffer);
// void write_ata_primary_controller_second_drive(uint64_t where, uint32_t count, uint8_t *buffer);
// void write_ata_secondary_controller_first_drive(uint64_t where, uint32_t count, uint8_t *buffer);
// void write_ata_secondary_controller_second_drive(uint64_t where, uint32_t count, uint8_t *buffer);
static inline uint8_t poll_status()
    {
        uint8_t status = ata_read(true, ATA_reg_command_status);

        while ((status & 0x80))
        {
            status = ata_read(true, ATA_reg_command_status);
            asm volatile("pause");
        }
        return status;
    }
static inline void wait(uint8_t time)
    {
        for (int i = 0; i < time; i++)
        {
            ata_read(true, ATA_reg_command_status);
        }
    }
bool lba_identify(enum ide_controller controller, enum drives drive, uint16_t* buffer) ;
bool drive_present(enum ide_controller controller, enum drives drive);
uint64_t get_drive_size(enum ide_controller controller, enum drives drive);
extern void write_ata_primary_controller_first_drive(uint64_t sector, uint8_t count, uint8_t* buffer);
extern void write_ata_primary_controller_second_drive(uint64_t sector, uint8_t count, uint8_t* buffer);
extern void write_ata_secondary_controller_first_drive(uint64_t sector, uint8_t count, uint8_t* buffer);
extern void write_ata_secondary_controller_second_drive(uint64_t sector, uint8_t count, uint8_t* buffer);
