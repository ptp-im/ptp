#include "ptp_server/msfs/MsfsHttpConn.h"

int main(int argc, char* argv[])
{
    run_ptp_server_msfs(argc,argv);
    netlib_eventloop();
    return 0;
}
