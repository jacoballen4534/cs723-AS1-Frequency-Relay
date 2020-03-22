#ifndef __ALT_UP_PS2_PORT_REGS_H__
#define __ALT_UP_PS2_PORT_REGS_H__

#include <io.h>
#include <alt_types.h>


/* 
 * Data Register 
 */
#define ALT_UP_PS2_PORT_DATA_REG					0
#define IOADDR_ALT_UP_PS2_PORT_DATA_REG(base)		\
		__IO_CALC_ADDRESS_NATIVE(base, ALT_UP_PS2_PORT_DATA_REG)
#define IORD_ALT_UP_PS2_PORT_DATA_REG(base)			\
		IORD(base, ALT_UP_PS2_PORT_DATA_REG) 
#define IOWR_ALT_UP_PS2_PORT_DATA_REG(base, data)	\
		IOWR(base, ALT_UP_PS2_PORT_DATA_REG, data)

#define ALT_UP_PS2_PORT_DATA_REG_DATA_MSK			(0x000000FF)
#define ALT_UP_PS2_PORT_DATA_REG_DATA_OFST			(0)
#define ALT_UP_PS2_PORT_DATA_REG_RVALID_MSK	 		(0x00008000)
#define ALT_UP_PS2_PORT_DATA_REG_RVALID_OFST		(15)
#define ALT_UP_PS2_PORT_DATA_REG_RAVAIL_MSK			(0xFFFF0000)
#define ALT_UP_PS2_PORT_DATA_REG_RAVAIL_OFST		(16)

#define ALT_UP_PS2_PORT_DATA						0
#define IOADDR_ALT_UP_PS2_PORT_DATA(base)			\
		__IO_CALC_ADDRESS_DYNAMIC(base, ALT_UP_PS2_PORT_DATA)
#define IORD_ALT_UP_PS2_PORT_DATA(base)		 		\
		IORD_8DIRECT(base, ALT_UP_PS2_PORT_DATA) 
#define IOWR_ALT_UP_PS2_PORT_DATA(base, data)		\
		IOWR_8DIRECT(base, ALT_UP_PS2_PORT_DATA, data)

#define ALT_UP_PS2_PORT_DATA_MSK					(0x000000FF)
#define ALT_UP_PS2_PORT_DATA_OFST					(0)

#define ALT_UP_PS2_PORT_RVALID						1
#define IOADDR_ALT_UP_PS2_PORT_RVALID(base)			\
		__IO_CALC_ADDRESS_DYNAMIC(base, ALT_UP_PS2_PORT_RVALID)
#define IORD_ALT_UP_PS2_PORT_RVALID(base)		 	\
		IORD_8DIRECT(base, ALT_UP_PS2_PORT_RVALID) 
#define IOWR_ALT_UP_PS2_PORT_RVALID(base, data)		\
		IOWR_8DIRECT(base, ALT_UP_PS2_PORT_RVALID, data)

#define ALT_UP_PS2_PORT_RVALID_MSK	 				(0x00000080)
#define ALT_UP_PS2_PORT_RVALID_OFST					(7)

#define ALT_UP_PS2_PORT_RAVAIL						2
#define IOADDR_ALT_UP_PS2_PORT_RAVAIL(base)			\
		__IO_CALC_ADDRESS_DYNAMIC(base, ALT_UP_PS2_PORT_RAVAIL)
#define IORD_ALT_UP_PS2_PORT_RAVAIL(base)			\
		IORD_16DIRECT(base, ALT_UP_PS2_PORT_RAVAIL) 
#define IOWR_ALT_UP_PS2_PORT_RAVAIL(base, data)		\
		IOWR_16DIRECT(base, ALT_UP_PS2_PORT_RAVAIL, data)

#define ALT_UP_PS2_PORT_RAVAIL_MSK					(0x0000FFFF)
#define ALT_UP_PS2_PORT_RAVAIL_OFST					(0)

/*
 * Control Register 
 */
#define ALT_UP_PS2_PORT_CTRL_REG					1
#define IOADDR_ALT_UP_PS2_PORT_CTRL_REG(base)		\
		__IO_CALC_ADDRESS_NATIVE(base, ALT_UP_PS2_PORT_CTRL_REG)
#define IORD_ALT_UP_PS2_PORT_CTRL_REG(base)			\
		IORD(base, ALT_UP_PS2_PORT_CTRL_REG)
#define IOWR_ALT_UP_PS2_PORT_CTRL_REG(base, data)	\
		IOWR(base, ALT_UP_PS2_PORT_CTRL_REG, data)

#define ALT_UP_PS2_PORT_CTRL_REG_RE_MSK				(0x00000001)
#define ALT_UP_PS2_PORT_CTRL_REG_RE_OFST			(0)
#define ALT_UP_PS2_PORT_CTRL_REG_RI_MSK				(0x00000100)
#define ALT_UP_PS2_PORT_CTRL_REG_RI_OFST			(8)
#define ALT_UP_PS2_PORT_CTRL_REG_CE_MSK 			(0x00000400)
#define ALT_UP_PS2_PORT_CTRL_REG_CE_OFST 			(10)

#endif
