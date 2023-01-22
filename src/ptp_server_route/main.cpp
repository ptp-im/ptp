#include "ptp_server/route/RouteConn.h"

int main(int argc, char* argv[])
{
    run_ptp_server_route(argc,argv);
    netlib_eventloop();
    return 0;
}
