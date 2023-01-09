//
// Created by jack on 2023/1/9.
//

#include "boot_msfs.h"

#include <iostream>
#include <csignal>
#include "ptp_global/NetLib.h"
#include "ptp_global/ConfigFileReader.h"
#include "ptp_global/ThreadPool.h"
#include "ptp_global/Logger.h"
#include "ptp_global/Helpers.h"
#include "HttpConn.h"
#include "FileManager.h"
#include "../boot.h"

using namespace msfs;

FileManager *FileManager::m_instance = nullptr;
FileManager *g_fileManager = nullptr;
CThreadPool g_PostThreadPool;
CThreadPool g_GetThreadPool;

CConfigFileReader config_file(CONFIG_PATH_MSFS);

static void closeall(int fd) {
    int fdlimit = sysconf(_SC_OPEN_MAX);

    while (fd < fdlimit)
        close(fd++);
}

static int daemon(int nochdir, int noclose, int asroot) {
    switch (fork()) {
        case 0:
            break;
        case -1:
            return -1;
        default:
            _exit(0);          /* exit the original process */
    }

    if (setsid() < 0)               /* shoudn't fail */
        return -1;

    if (!asroot && (setuid(1) < 0))              /* shoudn't fail */
        return -1;

    /* dyke out this switch if you want to acquire a control tty in */
    /* the future -- not normally advisable for daemons */

    switch (fork()) {
        case 0:
            break;
        case -1:
            return -1;
        default:
            _exit(0);
    }

    if (!nochdir)
        chdir("/");

    if (!noclose) {
        closeall(0);
        dup(0);
        dup(0);
    }

    return 0;
}

// for client connect in
static void http_callback(void *callback_data, uint8_t msg, uint32_t handle,
                   void *pParam) {
    if (msg == NETLIB_MSG_CONNECT) {
        auto *pConn = new CHttpConn();
//        CHttpTask* pTask = new CHttpTask(handle, pConn);
//        g_ThreadPool.AddTask(pTask);
        pConn->OnConnect(handle);
    } else {
        DEBUG_D("!!!error msg: %d", msg);
    }
}

static void doQuitJob() {
    char fileCntBuf[20] = {0};
    snprintf(fileCntBuf, 20, "%llu", g_fileManager->getFileCntCurr());
    config_file.SetConfigValue("FileCnt", fileCntBuf);
    FileManager::destroyInstance();
    netlib_destroy();
    DEBUG_D("I'm ready quit...");
}

void quit_msfs(){
    netlib_stop_event();
    doQuitJob();
}

static void Stop(int signo) {
    DEBUG_D("receive signal:%d", signo);
    switch (signo) {
        case SIGINT:
        case SIGTERM:
        case SIGQUIT:
            doQuitJob();
            _exit(0);
            break;
        default:
            cout << "unknown signal" << endl;
            _exit(0);
    }
}

int boot_msfs(int argc, char *argv[]) {
    for (int i = 0; i < argc; ++i) {
        if (strncmp(argv[i], "-d", 2) == 0) {
            if (daemon(1, 0, 1) < 0) {
                cout << "daemon error" << endl;
                return -1;
            }
            break;
        }
    }

    char listen_ip[50];
    uint16_t listen_port;
    char base_dir[100];
    int filesPerDir;
    int nPostThreadCount;
    int nGetThreadCount;
    int ret = get_msfs_config(
            listen_ip,
            listen_port,
            base_dir,
            filesPerDir,
            nPostThreadCount,
            nGetThreadCount);
    if(ret == -1){
        return ret;
    }
    char *str_file_cnt = config_file.GetConfigName("FileCnt");
    if(!str_file_cnt){
        DEBUG_E("str_file_cnt is null");
        return ret;
    }

    long long int fileCnt = atoll(str_file_cnt);

    if (nPostThreadCount <= 0 || nGetThreadCount <= 0) {
        DEBUG_E("thread count is invalied");
        return -1;
    }

    g_PostThreadPool.Init(nPostThreadCount);
    g_GetThreadPool.Init(nGetThreadCount);

    g_fileManager = FileManager::getInstance(listen_ip, base_dir, fileCnt, filesPerDir);
    ret = g_fileManager->initDir();
    if (ret) {
        DEBUG_D("The BaseDir is set incorrectly :%s", base_dir);
        return ret;
    }
    ret = netlib_init();
    if (ret == NETLIB_ERROR)
        return ret;

    CStrExplode listen_ip_list(listen_ip, ';');
    for (uint32_t i = 0; i < listen_ip_list.GetItemCnt(); i++) {
        ret = netlib_listen(listen_ip_list.GetItem(i), listen_port,
                            http_callback, nullptr);
        if (ret == NETLIB_ERROR)
            return ret;
    }

    signal(SIGINT, Stop);
    signal(SIGTERM, Stop);
    signal(SIGQUIT, Stop);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    DEBUG_D("server start listen on: %s:%d", listen_ip, listen_port);
    init_http_conn();
    DEBUG_D("now enter the event loop...");
    writePid("ptp_server_msfs.pid");
    netlib_eventloop();
    return 0;
}
