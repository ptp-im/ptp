#include "MsgSrvConn.h"
#include "BusinessSrvConn.h"

int main(int argc, char* argv[])
{
    if ((argc > 1) && (strcmp(argv[1], "-b") == 0)) {
        run_ptp_server_business(argc,argv);
        netlib_eventloop();
        return 0;
    }

    run_ptp_server_msg(argc,argv);
    netlib_eventloop();
    return 0;
}
