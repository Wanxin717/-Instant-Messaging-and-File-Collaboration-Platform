#include "protocol.h"
#include<stdlib.h>
#include<string.h>


PDU *mkPDU(uint uiMsgLen)
{
    //计算总程度，并申请空间
    uint uiPDULen=sizeof (PDU)+uiMsgLen;
    PDU* pdu=(PDU*)malloc(uiPDULen);
    if(pdu==NULL){
      exit(1);
}
    //将申请的内存对象初始化0，大小是uiPUDLen
    memset(pdu,0,uiPDULen);
    //两个长度的成员赋初值
    pdu->uiMsgLen=uiMsgLen;
    pdu->uiPDULen=uiPDULen;
    return pdu;
}
