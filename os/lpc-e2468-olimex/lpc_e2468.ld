/****************************************************************************
 *  
 * Project: LPC-E2468 blinking led demo
 *
 * Copyright: Ivan Vasilev, Olimex Ltd. All rights reserved.
 * 
 * File: $File lpc_e2468.cmd $
 * Description: Board-specific USB initialization
 * Developer: Ivan Vasilev, <ivan at l123.org>
 *
 * Last change: $Date: 2008-04-08 09:23:26 +0300 (вторник, 08 Април 2008) $
 * Revision: $Revision: 4 $
 * Id: $Id: lcp_e2468.cmd 4 2008-04-08 06:23:26Z Ivan $
 * Author: $Author: Ivan $
 *
 *  This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 ****************************************************************************/
 
ENTRY(_start)

/* LPC-2468 memory map */
MEMORY 
{
	ram   			: ORIGIN = 0xa0008000, LENGTH = 16M
}

/* we may get averwritten by the ISP/IAP */
_stack_end = 0xa0000000 + 16M;
_vStackTop = _stack_end;

SECTIONS 
{
	. = 0xa0008000;
	
  .start : { *(.start)} >ram
  
  .text :  ALIGN(16) SUBALIGN(16)
    { 
        _text = .;
        KEEP(*(vectors))
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.glue_7t)
        *(.glue_7)
        *(.gcc*)
        *(.ctors)
        *(.dtors)
    } > ram

    .ARM.extab : {*(.ARM.extab* .gnu.linkonce.armextab.*)}

    __exidx_start = .;
    .ARM.exidx : {*(.ARM.exidx* .gnu.linkonce.armexidx.*)} > ram
    __exidx_end = .;

    .eh_frame_hdr : {*(.eh_frame_hdr)}

    .eh_frame : ONLY_IF_RO {*(.eh_frame)}

    . = ALIGN(4);
    _etext = .;
    _textdata = _etext;

    .data :
    {
        _data = .;
        *(.data)
        . = ALIGN(4);
        *(.data.*)
        . = ALIGN(4);
        *(.ramtext)
        . = ALIGN(4);
        _edata = .;
    } > ram AT > ram

    .bss :
    {
        _bss_start = .;
        *(.bss)
        . = ALIGN(4);
        *(.bss.*)
        . = ALIGN(4);
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > ram    

	. = ALIGN(4);
	_bss_end = . ;
	_end = .;
}
