#include "MsgSrvConn.h"
#include "BusinessSrvConn.h"

int main(int argc, char* argv[])
{

    run_ptp_server_business(argc,argv);
    run_ptp_server_msg(argc,argv);
    netlib_eventloop();
    return 0;
}
