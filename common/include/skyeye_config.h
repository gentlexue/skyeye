/*
	skyeye_config.h - config file interface for skyeye.
	Copyright (C) 2003 Skyeye Develop Group
	for help please send mail to <skyeye-developer@lists.sf.linuxforum.net> 
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 
*/
/* 4/02/2003	add net option
 * 				walimis <walimi@peoplemail.com.cn>
 * 3/21/2003 	Add config file interface to skyeye. 
 * 		Rename memmap.conf to skyeye.conf
 * 		Move some code from armmem.c.
 *		walimis <walimi@peoplemail.com.cn> 		
 * 10/24/2005	add dbct test speed function
 *				teawater <c7code-uc@yahoo.com.cn>
 * */
#ifndef __SKYEYE_CONFIG_H_
#define __SKYEYE_CONFIG_H_

#include "skyeye_types.h"
#include "skyeye_device.h"
#include "skyeye_mach.h"
#include "skyeye_signal.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define DEFAULT_CONFIG_FILE "skyeye.conf"
#define MAX_FILE_NAME 256
typedef struct
{
	const char *cpu_arch_name;	/*cpu architecture version name.e.g. armv4t */
	const char *cpu_name;	/*cpu name. e.g. arm7tdmi or arm720t */
	uint32_t cpu_val;	/*CPU value; also call MMU ID or processor id;see
				   ARM Architecture Reference Manual B2-6 */
	uint32_t cpu_mask;	/*cpu_val's mask. */
	uint32_t cachetype;	/*this cpu has what kind of cache */
} cpu_config_t;



#define MAX_BANK 8
#define MAX_STR  1024

#define MEMTYPE_IO		0
#define MEMTYPE_RAM		1
#define MEMTYPE_ROM		2

/*ywc 2005-03-30*/
#define MEMTYPE_FLASH           3

/*ksh 2004-11-26,energy profile flag*/
typedef struct
{
	char logfile[MAX_FILE_NAME];
	char filename[MAX_FILE_NAME];
	uint32_t energy_prof;
} energy_config_t;

/*net option*/
typedef struct
{
	int state;		// "on"=1,"off"=0, "on" means nic exist, "off" means nic don't exist
	unsigned char macaddr[6];
	unsigned char hostip[4];	//the hostos ip addr, used in tuntap net device 
	int ethmod;
	int fd;			// the opend dev file handle
	int hubindex;		//the index means the hub number, such as vnet0, vnet1...
	//nic functions
	int (*net_init) (int index, unsigned char *macaddr, unsigned char *hostip);	//the index means nic index 
	unsigned char (*net_output) (int if_fd, void * state,
				     unsigned char startpage,
				     unsigned short packet_len);
	void (*net_input) (int if_fd, void * state);
} net_config_t;

#define NET_MAXNICNUM_PER_HOST  4

/*uart option*/
/*add by walimis. 20030808*/

/* 2007-01-18 added by Anthony Lee: see "device/uart/skyeye_uart.h" */
#define MAX_UART_DEVICE_NUM	8
struct uart_device;

typedef struct
{
	/* 2007-01-18 modified by Anthony Lee: see "device/uart/skyeye_uart.h" */
	int count;					/* count of device */
	struct uart_device *devs[MAX_UART_DEVICE_NUM];	/* list of device */
} uart_config_t;

//chy 2003-08-20, used to log the instr&regs states to log file
typedef struct
{
	FILE *log_fd;
	int logon;		//log or not
	int memlogon;		//log memory accesses or not
	unsigned long long start;	//start clock
	unsigned long long end;	//end clock
	unsigned long long length;	//the max instr length of log file
} log_config_t;

/* some devices may have the same parameters.
 * type: device's type name
 * name: device's instance name
 * base: I/O or memory base address 
 * size: size of I/O or memory.
 * interrupts: 
 * */
typedef struct common_config
{
	char *type;
	char *name;
	uint32_t base;
	uint32_t size;
	uint32_t interrupts[MAX_INTERRUPT];

} common_config_t;

/*config file option struct*/
int do_dummy_option ();
//int do_arch_option ();
#if 0
int do_cpu_option ();
int do_mach_option ();
int do_mem_bank_option ();
int do_net_option ();
int do_energy_option ();
int do_uart_option ();
int do_lcd_option ();
int do_flash_option ();
int do_touchscreen_option ();
int do_sound_option ();
int do_nandflash_option();
int do_regfile_option();/* to set the values for regfile in skyeye.conf */
int do_load_addr_option(); /* to load elf file to another address */
int do_code_cov_option(); /* to profile code coverage */

/*ywc 2005-04-01*/
int do_dbct_option ();

int do_log_option ();
int do_step_disassemble_option ();
#endif
//teawater add DBCT_TEST_SPEED 2005.10.04---------------------------------------
#ifdef DBCT_TEST_SPEED
int do_dbct_test_speed_sec();
#endif	//DBCT_TEST_SPEED
//AJ2D--------------------------------------------------------------------------

//#define MAX_OPTION_NAME 32
//#define MAX_PARAM_NAME  32
typedef struct
{
	char *arch_name;
	void (*init) ();
	void (*reset) ();
	void (*step_once) ();
	void (*set_pc) (WORD addr);
	WORD (*get_pc)();
	uint32 (*get_step)();
	int (*ICE_write_byte) (WORD addr, uint8_t data);
	int (*ICE_read_byte)(WORD addr, uint8_t *pv);	
	uint32 (*get_regval_by_id)(int id);
        exception_t (*set_regval_by_id)(int id, uint32 value);
	uint32 (*get_regnum)();
	char* (*get_regname_by_id)(int id);

	int (*parse_cpu) (const char *param[]);
	int (*parse_mach) (machine_config_t * mach, const char *param[]);
	int (*parse_mem) (int num_params, const char *params[]);
	int (*parse_regfile) (int num_params, const char *params[]);
	
	 /*
         * read a data by virtual address.
         */
	exception_t (*mmu_read)(short size, generic_address_t addr, uint32_t * value);
        /*
         * write a data by a virtual address.
         */
        exception_t (*mmu_write)(short size, generic_address_t addr, uint32_t value);
	/**
	 * get a signal from external
	 */
	exception_t (*signal)(interrupt_signal_t* signal);

	void (*exec) (uint32_t id);
	void (*stop) (uint32_t id);
} arch_config_t;

typedef struct code_cov_option code_cov_t;

typedef struct
{
	char *os_name;

} os_config_t;

typedef struct
{
	arch_config_t *arch;
	//cpu_config_t *cpu;
	machine_config_t *mach;
	//mem_config_t mem;
	os_config_t *os;
//chy 2003-09-12, now support more io banks
//      ioaddr_config_t ioaddr; //used for ARMul_notIOaddr funciton
	net_config_t net[NET_MAXNICNUM_PER_HOST];
	uart_config_t uart;
	log_config_t log;
	uint32_t start_address;

	/*ywc 2005-03-31, no_dbct used by Dynamic Binary Code Translation */
	uint32_t no_dbct;

	/*ksh 2004-11-26,energy profile flag */
	energy_config_t energy;
	/* move config_file to sky_pref_t */
	//char config_file[MAX_FILE_NAME];
	
	/* should defined in code_cov module */
	//code_cov_t code_cov;

	//oyangjian add here 2004-11-3
	uint32_t can_step_disassemble;
	//teawater add for new tb manage function 2005.07.10----------------------------
	uint32_t tb_tbt_size;
	uint32_t tb_tbp_size;
//teawater add DBCT_TEST_SPEED 2005.10.04---------------------------------------
#ifdef DBCT_TEST_SPEED
	int	dbct_test_speed_sec;
#endif	//DBCT_TEST_SPEED
//AJ2D--------------------------------------------------------------------------
} skyeye_config_t;

#if 0
static skyeye_option_t skyeye_options[] = {
/* move to the private implementation */
//	{"arch", do_arch_option, 0, 1},
	{"cpu", do_cpu_option, 0, 1},
	{"mach", do_mach_option, 0, 1},

	/*mem option */
	{"mem_bank", do_mem_bank_option, 0, MAX_BANK},

	{"net", do_net_option, 0, 10},
	{"energy", do_energy_option, 0, 1},
	{"uart", do_uart_option, 0, MAX_UART_DEVICE_NUM},
	{"lcd", do_lcd_option, 0, 1},
	{"flash", do_flash_option, 0, 1},
	{"nandflash",do_nandflash_option,0,1},
	{"touchscreen", do_touchscreen_option, 0, 1},
	{"sound", do_sound_option, 0, 1},

	/*ywc 2005-04-01 */
	//teawater add for new tb manage function 2005.07.10----------------------------
	{"dbct", do_dbct_option, 0, 3},

	/*log option */
	{"log", do_log_option, 0, 3},
	{"regfile", do_regfile_option, 0, 1},
	{"load_addr", do_load_addr_option, 0, 3},
	

	/*
	 * this option can disassmebl the next c statement 
	 * with assemble code and may be have some usefull
	 * 
	 */
  	{"step_disassemble", do_step_disassemble_option, 0, 1},
#ifdef DBCT_TEST_SPEED
	{"dbct_test_speed_sec", do_dbct_test_speed_sec, 0, 1},
#endif	//DBCT_TEST_SPEED
	{"code_coverage", do_code_cov_option, 0, 1},
};
#endif


exception_t skyeye_read_config (char* conf_filename);
skyeye_config_t* get_current_config();

#ifdef __cplusplus
}
#endif

#endif	/*__SKYEYE_CONFIG_H_*/
