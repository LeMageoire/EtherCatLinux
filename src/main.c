#include <stdio.h>
#include <string.h>
#include "ethercat.h"

#define EK1100_1 1
#define EL4001_1 2

#define NUMBER_OF_SLAVES (2)

int EK1100setup(uint16 slave)
{
    int retval;
    uint16 u16val;
    /* */

    retval = 0;
    uint16 map_1c12[1] = {0xFFFF};
    retval += ec_SDOwrite(slave, 0x1c12, 0x00, TRUE, sizeof(map_1c12), &map_1c12, EC_TIMEOUTSAFE);

    printf("coucou\n");
    return 1;
}

int EL2798setup(uint16 slave)
{
    int retval;
    uint16 u16val;
    /* */

    retval = 0;
    uint16 map_1c12[1] = {0xFFFF};
    retval += ec_SDOwrite(slave, 0, 0x00, TRUE, sizeof(map_1c12), &map_1c12, EC_TIMEOUTSAFE);

    printf("coucou : %d\n", retval);
    return (1);
}

void set_output_int16(uint16 slave_no, uint8 module_index, int16 value)
{
    uint8 *data_ptr;
    data_ptr = ec_slave[slave_no].outputs;
    /* Move pointer to correct module index*/
    data_ptr += module_index * 2;
    *data_ptr++ = (value >> 0) & 0xFF;
    *data_ptr++ = (value >> 8) & 0xFF;
}

int main()
{
    //const char *ifname = "enp0s31f6";
    const char *ifname = "enp0s31f6";
    if(ec_init(ifname) <= 0)
    {
        printf("ec_init failed\n");   
        return 1;
    }
    if(ec_config_init(FALSE) <= 0)
    {
        printf("ec_config_init failed\n");
        return 1;
    }
    if (ec_slavecount < NUMBER_OF_SLAVES)
    {
        printf("Found %d slaves, expected %d\n", ec_slavecount, NUMBER_OF_SLAVES);
        return 1;
    }

    if(strcmp(ec_slave[1].name, "EK1100") != 0)
    {
        printf("Slave 1 is not EK1100, but %s\n", ec_slave[1].name);
        ec_slave[1].PO2SOconfig = EK1100setup;
        return 1;
    }
    if(strcmp(ec_slave[2].name, "EL2798") != 0)
    {
        printf("Slave 1 is not EK2798, but %s\n", ec_slave[2].name);
        ec_slave[2].PO2SOconfig = EL2798setup;
        return 1;
    }
    
    //SETUP_IS_DONE
    printf("Setup is done\n");
    char IOmap[128];
    int usedmem;

    usedmem = ec_config_map(&IOmap);
    if(usedmem<= sizeof(IOmap))
        printf("Bonjour %d\n",usedmem);




    // Get the slave configuration structure for EL2798
    ec_slavet EL2798 = ec_slave[2];

    // Check if the slave is reachable
    if (EL2798.state != EC_STATE_OPERATIONAL)
    {
        fprintf(stderr, "EL2798 is not in operational state.\n");
        ec_close();
        return -1;
    }

     // Set the data to turn on the EL2798
    uint8_t enableData = 1;  // You may need to adjust this value based on the EL2798 specifications

    // Specify the offset in the process data to write the value
    unsigned int offset = 0;  // Assuming the offset is 0, update it based on the actual offset

    // Check if the offset is within bounds
    if (offset + sizeof(enableData) <= EL2798.Obits / 8)
    {
        // Create a pointer to the target location in the process data
        uint8_t *targetPtr = &(EL2798.outputs[0]) + offset;

        // Write the data to the EL2798
        memcpy(targetPtr, &enableData, sizeof(enableData));
    }
    else
    {
        fprintf(stderr, "Invalid offset for writing data.\n");
        ec_close();
        return -1;
    }
    


    // Request the master to send the frame
    ec_send_processdata();

    // Wait for the frame to be sent
    ec_receive_processdata(EC_TIMEOUTRET);

    // Close EtherCAT
    ec_close();

    return 0;
}

// You may need to adjust this value based on the EL2798 specifications // Specify the offset in the process data to write the value unsigned int offset = 0; // Assuming the offset is 0, update it based on the actual offset // Write the data to the EL2798 if (offset + sizeof(enableData) <= EL2798.Ibytes) { memcpy(&(EL2798.inputs\[0\] + offset), &enableData, sizeof(enableData)); } else { fprintf(stderr, "Invalid offset for writing data.\\n"); ec\_close(); return \-1; } // Request the master to send the frame ec\_send\_processdata(); // Wait for the frame to be sent ec\_receive\_processdata(EC\_TIMEOUTRET); // Close EtherCAT ec\_close();



    //ec_send_processdata();
    //printf("wkc = : %d\n", ec_receive_processdata(EC_TIMEOUTRET));
    //printf("workcounter : %d\n",ec_writestate(2)); //workcounter =1 
    //ec_statecheck(0, EC_STATE_OPERATIONAL, EC_TIMEOUTSTATE);
    //
    //EL2798setup(2);
    //ec_slave[2].Obits = 0xFF;
    //printf(" configaddr :%d \n", ec_slave[2].configadr);
    //
    //printf("workcounter : %d\n",ec_writestate(2));
    //
    //set_output_int16(1,0,ec_slave[1].outputs);

/*
void simpletest(char *ifname)
{
    if(ec_slave[slc].eep_man == 0x00000002 && (ec_slave[slc].eep_id == 0x1b773052))
    {
        printf("Found %s at position %d\n", ec_slave[slc].name, slc);
        ec_slave[slc].PO2SOconfig = NULL; //EL7031setup;
    }
}*/

/**
 * @brief 
 * 
 * @param slave 16-bit slave address
 * @return int 1 on success, 0 on failure
 */

int EL7031setup(uint16 slave)
{
    int retval;
    uint16 u16val;
    /* */

    retval = 0;
}



int EL7432setup(uint16 slave)
{
    int retval;
    uint16 u16val;
    /* */

    retval = 0;
}

int EL5152setup(uint16 slave)
{
    int retval;
    uint16 u16val;
    /* */

    retval = 0;
}


