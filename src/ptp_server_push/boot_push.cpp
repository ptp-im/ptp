#include "push_app.h"
#include "timer/Timer.hpp"
#include <sys/signal.h>
#include "ptp_global/Helpers.h"

int boot_push(int argc, char *argv[]){
    // insert code here...
    signal(SIGPIPE, SIG_IGN);
    CPushApp::GetInstance()->Init();
    CPushApp::GetInstance()->Start();
    writePid();
    while (true) {
        S_Sleep(1000);
    }
    return 0;
}
