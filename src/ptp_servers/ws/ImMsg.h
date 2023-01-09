#ifndef IMMSG_H_
#define IMMSG_H_


class CImMsg {
public:
    CImMsg();
    ~CImMsg();
private:

};

uint32_t GetMsgIdFromCache(string from_address,string to_address);

#endif /* IMMSG_H_ */