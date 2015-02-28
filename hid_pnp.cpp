#include "hid_pnp.h"

HID_PnP::HID_PnP(QObject *parent) : QObject(parent) {
    fIsConnected = false;
    dwCmdRecLen = 0;

    device = NULL;
    pbCmdSendBuf[0] = 0x00; //the report ID need to be correct
    
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(PollUSB()));

    cmdRecSt = CMD_IDLE_ST;
    timer->start(250);
}

HID_PnP::~HID_PnP() {
    disconnect(timer, SIGNAL(timeout()), this, SLOT(PollUSB()));
}

void HID_PnP::PollUSB()
{
    if (fIsConnected == false) {
        device = hid_open(0x04d8, 0x003f, NULL);

        if (device) {
            fIsConnected = true;
            hid_set_nonblocking(device, true);
            hid_comm_update(fIsConnected, dwCmdRecLen);
            timer->start(15);
        }
    }
    else {

        if(dwCmdSendBufLen != 0) {
            if (hid_write(device, pbCmdSendBuf, sizeof(pbCmdSendBuf)) == -1)
            {
                CloseDevice();
                return;
            }
            dwCmdSendBufLen = 0;
        }

        memset(pbCmdRecBuf, 0x00, sizeof(pbCmdRecBuf));
        int recBufCnt = hid_read(device, pbCmdRecBuf, 64);
        if(recBufCnt == -1)
        {
            CloseDevice();
            return;
        }

        //the device always send 64 bytes of block.
        if(recBufCnt != 64) { return; }

        for(unsigned int i=0; i<64; i++)
        {
            switch(cmdRecSt)
            {
                case CMD_IDLE_ST :
                    if(pbCmdRecBuf[i] == CMD_CTL_START)
                    {
                        dwCmdRecLen = 0;
                        cmdRecSt = CMD_DATA_ST;
                    }
                break;

                case CMD_DATA_ST :
                    if(pbCmdRecBuf[i] == CMD_CTL_START)
                    {
                        dwCmdRecLen = 0;
                        cmdRecSt = CMD_DATA_ST;
                    }
                    else if(pbCmdRecBuf[i] == CMD_CTL_END)
                    {
                        cmdRecSt = CMD_IDLE_ST;
                        //return TRUE; // The command reception was completed.
                        hid_comm_update(fIsConnected, dwCmdRecLen);
                        dwCmdRecLen = 0;
                    }
                    else if(pbCmdRecBuf[i] == CMD_CTL_ESC)
                    {
                        cmdRecSt =CMD_ESC_ST;
                    }
                    else //It's really part to receive the cmd data
                    {
                        pbCmdRec[dwCmdRecLen++] = pbCmdRecBuf[i];
                    }
                break;

                case CMD_ESC_ST :
                    cmdRecSt =CMD_DATA_ST;
                    if( (pbCmdRecBuf[i] == CMD_CTL_START) || (pbCmdRecBuf[i] == CMD_CTL_END) || (pbCmdRecBuf[i] == CMD_CTL_ESC) )
                    {
                        pbCmdRec[dwCmdRecLen++] = pbCmdRecBuf[i];
                    }
                    else
                    {
                        dwCmdRecLen = 0;
                        cmdRecSt = CMD_IDLE_ST;
                    }
                break;
            }
        }
    }

}

unsigned long HID_PnP::CmdSend(unsigned char *pbCmdSend, unsigned long dwLen) {
    if(dwLen > ((ENDPOINT_BUF_SIZE-2)/2) ) return -1;

    dwCmdSendBufLen = 0;
    pbCmdSendBuf[dwCmdSendBufLen++] = 0x00; //the report ID need to be correct
    pbCmdSendBuf[dwCmdSendBufLen++] = 0xC0;
    for(unsigned long i=0; i<dwLen; i++)
    {
        if(pbCmdSend[i] == CMD_CTL_START)
        {
            pbCmdSendBuf[dwCmdSendBufLen++] = CMD_CTL_ESC;
            pbCmdSendBuf[dwCmdSendBufLen++] = CMD_CTL_START;
        }
        else if(pbCmdSend[i] == CMD_CTL_END)
        {
            pbCmdSendBuf[dwCmdSendBufLen++] = CMD_CTL_ESC;
            pbCmdSendBuf[dwCmdSendBufLen++] = CMD_CTL_END;
        }
        else if(pbCmdSend[i] == CMD_CTL_ESC)
        {
            pbCmdSendBuf[dwCmdSendBufLen++] = CMD_CTL_ESC;
            pbCmdSendBuf[dwCmdSendBufLen++] = CMD_CTL_ESC;
        }
        else
        {
            pbCmdSendBuf[dwCmdSendBufLen++] = pbCmdSend[i];
        }
    }
    pbCmdSendBuf[dwCmdSendBufLen++] = 0xD0;
    memset((void*)&pbCmdSendBuf[dwCmdSendBufLen], 0x00, sizeof(pbCmdSendBuf) - dwCmdSendBufLen);
    return 0;
}

void HID_PnP::CloseDevice() {
    hid_close(device);
    device = NULL;
    fIsConnected = false;
    dwCmdRecLen = 0;
    hid_comm_update(fIsConnected, dwCmdRecLen);
    timer->start(250);
}
