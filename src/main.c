#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "ethercat.h"

#define EK1100_1 1
#define EL4001_1 2

#define NUMBER_OF_SLAVES (2)

#if 0
void readPDOmapping(uint16 slave)
{
    uint8  mapSize;
    uint32 bytesRead;
    uint16 index;
    uint8 subIndex;
    uint32 value;

    index = 0x1C12;
    subIndex = 0x00;
    ec_SDOread(slave, index, subIndex, FALSE, &bytesRead, &mapSize, EC_TIMEOUTRXM);
    for(int i = 1; i <= mapSize; i++) {
        ec_SDOread(slave, index, i, FALSE, &bytesRead, &value, EC_TIMEOUTRXM);
        printf("Mapped RxPDO index at position %d : 0x%04X\n", i , value);
    }
}
#endif 

void set_output_on_EL2798(int slave_position, int output_number){
    int byte_index = 0;
    int bit_position = output_number;
    if(output_number < 0 || output_number >= 8)
    {    
        printf("Invalid outputnumber\n");
        return ;
    }
    ec_slave[slave_position].outputs[byte_index] |= (1 << bit_position);
   // ec_send_processdata();
   // ec_receive_processdata(EC_TIMEOUTRET);
}
void set_output_off_EL2798(int slave_position, int output_number){
    int byte_index = 0;
    int bit_position = output_number;
    if(output_number < 0 || output_number >= 8)
    {    
        printf("Invalid outputnumber\n");
        return ;
    }
    ec_slave[slave_position].outputs[byte_index] &= ~(1 << bit_position);
   // ec_send_processdata();
   // ec_receive_processdata(EC_TIMEOUTRET);
}
#if 0
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
#endif

typedef struct {
    int inputData;
} EtherCAT_Data;

volatile EtherCAT_Data globalData;
volatile int shouldStop = 0;
pthread_mutex_t dataMutex;

void *EtherCATThread(void *arg) {
    //const char *ifname = "enp0s31f6";
    const char *ifname = "enp0s31f6";
    if(ec_init(ifname) <= 0)
    {
        printf("ec_init failed\n");   
        return NULL;
    }
    if(ec_config_init(FALSE) <= 0)
    {
        printf("ec_config_init failed\n");
        return NULL;
    }
    if (ec_slavecount < NUMBER_OF_SLAVES)
    {
        printf("Found %d slaves, expected %d\n", ec_slavecount, NUMBER_OF_SLAVES);
        return NULL;
    }
    printf("Setup is done\n");
    char IOmap[4096];
    int usedmem;

    usedmem = ec_config_map(&IOmap);
    if(usedmem<= sizeof(IOmap))
        printf("Bonjour %d\n",usedmem);
    ec_configdc();
    ec_statecheck(0, EC_STATE_PRE_OP, EC_TIMEOUTSTATE);
    ec_slave[0].state = EC_STATE_SAFE_OP;
    ec_writestate(0);
    int chk = 40;
    do {
        ec_send_processdata();
        ec_receive_processdata(EC_TIMEOUTRET);
        ec_statecheck(0,EC_STATE_SAFE_OP, 50000);
    } while(chk-- && ec_slave[0].state != EC_STATE_SAFE_OP);

    if(ec_slave[0].state == EC_STATE_SAFE_OP) {
        printf("All slaves reached SAFE_OP state.\n");
    }
    ec_slave[0].state = EC_STATE_OPERATIONAL;
    ec_writestate(0);
    chk = 40;
    do {
        ec_send_processdata();
        ec_receive_processdata(EC_TIMEOUTRET);
        ec_statecheck(0,EC_STATE_OPERATIONAL, 50000);
    } while (chk-- && ec_slave[0].state != EC_STATE_OPERATIONAL);

    if(ec_slave[0].state == EC_STATE_OPERATIONAL) {
        printf("All slaves reached EC_STATE_OPERATIONAL state.\n");
    }

    printf("Start\n");
    set_output_off_EL2798(2,4);
    set_output_off_EL2798(2,3);
    set_output_off_EL2798(2,5);
    set_output_off_EL2798(2,6);
    set_output_off_EL2798(2,7);
    set_output_on_EL2798(2,0);
    set_output_on_EL2798(2,1);
    set_output_on_EL2798(2,2);

    while(!shouldStop) {
        ec_send_processdata();
        ec_receive_processdata(EC_TIMEOUTRET);
        pthread_mutex_lock(&dataMutex);
        pthread_mutex_unlock(&dataMutex);
        usleep(1000);
    }
    
    return (NULL);
}

void *ProcessingThread(void *arg) 
{
    while(!shouldStop){
        pthread_mutex_lock(&dataMutex);
        printf("%d \n", globalData.inputData);
        pthread_mutex_unlock(&dataMutex);
        usleep(5000);
    }
    return NULL;
}

int main() 
{
    pthread_t ecThread, processingThread;
    pthread_mutex_init(&dataMutex, NULL);

    if(pthread_create(&ecThread, NULL, EtherCATThread, NULL)) {
        fprintf(stderr, "Error creating EtherCat thread \n");
    }

    if(pthread_create(&processingThread, NULL, ProcessingThread,NULL)) {
        fprintf(stderr, "Error creating processing thread \n");
        return 1;
    }

    shouldStop = 0;
    pthread_join(ecThread, NULL);
    pthread_join(processingThread, NULL);
    pthread_mutex_destroy(&dataMutex);
    
    return 0;
}


#if 0
int main()
{
    

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
    
    while(1)
    {
        ec_send_processdata();
        //printf("receive %d\n",ec_receive_processdata(EC_TIMEOUTRET));
        osal_usleep(10000);
        //ec_receive_processdata();
    }

    //while(1);
    //printf("writestate : %d\n",ec_writestate(0)); 
    //printf("ec_writestate %d \n",ec_writestate(2));
    //printf("ec_statecheck %d \n",(ec_statecheck(2, EC_STATE_OPERATIONAL, EC_TIMEOUTSTATE)));
    //printf("ec_readstate %d \n",ec_readstate());

    //ec_configdc();
    // Get the slave configuration structure for EL2798
    //ec_slavet EL2798 = ec_slave[2];
    
    // Check if the slave is reachable
    //printf("ecx_statecheck %d \n",(ecx_statecheck(1, EC_STATE_OPERATIONAL, EC_TIMEOUTSTATE)));
    //ec_send_processdata();
    //int volatile wkc = ec_receive_processdata(EC_TIMEOUTRET);
    //printf("ec_writestate %d \n",ec_writestate(0));
    //printf("ec_statecheck %d \n",(ec_statecheck(0, EC_STATE_OPERATIONAL, EC_TIMEOUTSTATE)));
    //ec_writestate(0);
    //while(ec_statecheck(0,EC_STATE_SAFE_OP, EC_TIMEOUTSTATE) != EC_STATE_SAFE_OP )
    //    ;
    //printf("config done\n");


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

#endif

