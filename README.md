# EtherCatLinux
test of SOEM lib for Open EtherCAT Master

---
- EL2798 8xdigital ouput
- EK1100
- EL5152 : 2x incremental encoder interface
- EL1819 : 16xdigitalinput
- EL7342 : 2xDCmotors
---

## Notes of useful functions
```C
    //Check actual slave state . blocking function
    // 0 = all salve
 	uint16 ecx_statecheck(
        ecx_contextt    *context,
		uint16  	    slave,
		uint16  	    reqstate,
		int  	        timeout 
	)

    ecx_readstate() 	

    // 0 = master
    // return Workcounter or EC_NOFRAME 
    int ecx_writestate(
        ecx_contextt *context,
		uint16  	 slave 
	) 	

    //

```





---
# Notes
- you can make EtherCAT slave groups
- a slave can be in only one group

- to exchange process data for given

```C
ec_send_processdata_group(X);
ec_receive_processdata_group(X, EC_TIMEOUTRET);
```

- IOmap is the fatest mechanism for accessing slaves IO data.
ec_slave struct keeps pointers to the
---

# Set an output int 16 value when memory alignement needs to be considered, arguments is:
- slave numer in ethercat network
```C
void set_output_int16 (uint16 slave_no, uint8 module_index, int 16 value)
{
    uint8 *data_ptr;
    data_ptr = ec_slave[slave_no].outputs;
    data_ptr += module_index * 2;

    *data_ptr++ = (value >> 0) & 0xFF;
    *data_ptr++ = (value >> 8) & 0xFF;
}

//set_output_int16(EL4001_1,0,slave_EL4001_1.out1);


```


---
# Accesing SDO and PDO
- PDO is Process Data Objects
- multilpe ways a slave can communicate with the

---
# Theory about Ethercat
- Ethernet for Control Automation Technologie
- open technology under International standads
    - (IEC61158)
    - 61784
    - 61800
    - ISO 15745
- Ethercat frame ends witha 16-bit WorkingCounter (WKC)
- incremented by the slave
- it's DAISY-CHAINED
- low-jitter synchronization (<= 1us)
- Uses IEEE 802.3 Ethernet physical layer and standard Ethernet frames
- up to 65535 devices (16bits)
- you have duplex communication on ethernet
- you can have network redundancy
    - only if line breaks
## Frame Structure
- 1 datagram per frame
- Data frame strcuture =
    - Header
    - Data
    - Working Counter
- The WKC (Working Counter) is number of interactions contained in agiven datagram
- WC is ++ by each target, if the WC return isn't the on expected = error;
- 14 bytes headers
- Ethernet Data
    - 2 Bytes Len + 01
    - 44*-1498 Byte
- FCS
    - 4 bytes
---
- Ethernet data is composed :
- Datag. Header (10 Bytes)
    - CMD (8bit)
    - IDX (8bit)
    - Address (32 bits)
    - Len (11 bit) of data
    - R (2)
    - C (1) = Circulating dtagram ?
    - R (1)
    - M (1) = More ? datagrams
    - IRQ (16 bits)
- Data (max. 1486 Byte)
- WKC - 2bytes

---
# multiple models
- AutoIncrement (start-up ?)
- Fixed Addressing (once identified)
- BroadCast (for init all target)
- Logical (use the FMMU to map data)

---
# EtherCAT state machine
- init   (0x01) only access to the DL-information registers
- PreOp  (0x02) Mailbox communication on the application layer, but no process data com 
- SafeOp (0x04) Mailbox com on the app layer process input data (in safeOp only inputs are available)
- Op     (0x08) : PDA and Outputs are valid

- EtherCAT slave controller (ESC)

---
EtherCat Command types
- 0 NOP 
- 1 Auto Increament Read
- 2 Auto Incremeant Write
- 3 Auto Increment Read Write
- 4 CONFIGURED ADDRESS READ
- 5 CONFIGURED ADDRESS WRITE
- 6 CONFIGURED READ WRITE
- 7 BROADCAST READ
- 8 BROADCAT READ WRITE
- 9  Broadcast Read Write
-10 Logical Memory Read
-11 Logical Memory Write
-12 Logical Memory Read Write
-13 Auto Increment Read Multiple Write

---
- Sync tasks ?
---
 
# Notes
- To enter state OP we need to send valid data to outputs.
- The etherCAT frame handling is split into ec_send_processdata and ec_receive_processdata
