#include <stdio.h>
#include <string.h>
#include "ethercat.h"

ec_master_t *master = ecrt_request_master(0);
ec_master_state_t master_state;

if (ecrt_master(m) && !ecrt_master_state(m, &master_state)) {
    printf("EtherCAT master state: %s\n", ecrt_master_state_to_string(master_state));
    return -1;
}


int main(){
    
    while(!ecrt_master_state(m, &master_state) && master_state != EC_STATE_OPERATIONAL){
        ecrt_master_receive(m);
        ecrt_domain_process(domain);
        ecrt_master_send(m);
        ecrt_master_state(m, &master_state);

        usleep(1000);
    }
}