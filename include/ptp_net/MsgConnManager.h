/*
 * This is the source code of tgnet library v. 1.1
 * It is licensed under GNU GPL v. 2 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright Nikolai Kudashov, 2015-2018.
 */

#ifndef MsgConnManager_H
#define MsgConnManager_H
#include <unordered_map>

#ifdef ANDROID
#include <jni.h>
#include "../../tgnet/FileLog.h"
#include "../../tgnet/Defines.h"
#else
#include "ptp_common/Logger.h"
#include "ptp_net/Defines.h"
#endif

using namespace std;

class MsgConnManager {
public:
    static void onConnectionStateChanged(ConnectionState state, uint32_t accountId);
    static void onNotify(NativeByteBuffer *buffer);
    static void setDelegate(ConnectiosManagerDelegate* delegate);
    static void invoke(NativeByteBuffer *request);
    static void initMsgConn(uint32_t accountId);
    static void resetMsgConn();
    static void close(uint32_t accountId);
};

#endif
