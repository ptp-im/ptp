#include "push_app.h"
#include "timer/Timer.hpp"
#include <sys/signal.h>
#include "ptp_global/Helpers.h"

static bool stop = false;
void quit_push(){
    stop = true;
}

int boot_push(int argc, const char * argv[]) {
    // insert code here...
    signal(SIGPIPE, SIG_IGN);
    CPushApp::GetInstance()->Init();
    CPushApp::GetInstance()->Start();
    writePid();
    while (true) {
        if(stop){
            break;
        }
        S_Sleep(1000);
    }
    return 0;
}
