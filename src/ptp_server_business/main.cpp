#include "ptp_server/BusinessSrvConn.h"

int main(int argc, char* argv[])
{
    run_ptp_server_business(argc,argv);
    netlib_eventloop();
    return 0;
}
