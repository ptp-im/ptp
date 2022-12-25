#include <gtest/gtest.h>
#include "ptp_common/Logger.h"
#include "ptp_common/AesSimple.h"

TEST(ptp_common, log) {
   Logger::d("sss,%s","www");
    ASSERT_EQ(1,1);
}


TEST(ptp_common, AesSimple) {
    string key = "tes";
    string key_hash;
    CMd5::MD5_Calculate(key.data(),key.size(),key_hash);
    CAes cAes = CAes(key_hash);
    string strAudio = "[语音]";
    char* pAudioEnc;
    uint32_t nOutLen;
    if(cAes.Encrypt(strAudio.c_str(), strAudio.length(), &pAudioEnc, nOutLen) == 0)
    {
        Logger::d("pAudioEnc:%s",pAudioEnc);
        cAes.Free(pAudioEnc);
    }
    CAes *pAes = new CAes(key_hash);

    char* msg_out = NULL;
    uint32_t msg_out_len;
    string msg_data;
    if (pAes->Decrypt(pAudioEnc, nOutLen, &msg_out, msg_out_len) == 0)
    {
        msg_data = string(msg_out, msg_out_len);
    }
    pAes->Free(msg_out);
    ASSERT_EQ(msg_data,strAudio);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
