/*
 * FileHandler.h
 *
 *  Created on: 2013-12-17
 *      Author: crypto.service@proton.me
 */

#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_

#include "ptp_protobuf/ImPdu.h"

class CMsgConn;

class CFileHandler
{
public:
	virtual ~CFileHandler() {}

	static CFileHandler* getInstance();

    void HandleClientFileRequest(CMsgConn* pMsgConn, CImPdu* pPdu);
    void HandleClientFileHasOfflineReq(CMsgConn* pMsgConn, CImPdu* pPdu);
    void HandleClientFileAddOfflineReq(CMsgConn* pMsgConn, CImPdu* pPdu);
    void HandleClientFileDelOfflineReq(CMsgConn* pMsgConn, CImPdu* pPdu);
    void HandleFileHasOfflineRes(CImPdu* pPdu);
    void HandleFileNotify(CImPdu* pPdu);
private:
	CFileHandler() {}

private:
	static CFileHandler* s_handler_instance;
};


#endif /* FILEHANDLER_H_ */
