#ifndef HID_PNP_H
#define HID_PNP_H

#include <QObject>
#include <QTimer>
#include "../hidapi/hidapi/hidapi.h"

#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#define ENDPOINT_BUF_SIZE 64

typedef enum _CMD_STATE
{
    CMD_IDLE_ST,
    CMD_DATA_ST,
    CMD_ESC_ST
} CMD_STATE;

#define CMD_CTL_START	0xC0
#define CMD_CTL_END		0xD0
#define CMD_CTL_ESC		0xDB

class HID_PnP : public QObject
{
    Q_OBJECT
public:
    explicit HID_PnP(QObject *parent = 0);
    ~HID_PnP();
    unsigned long CmdSend(unsigned char *pbCmdSend, unsigned long dwLen);
    unsigned long dwCmdRecLen;
    unsigned char pbCmdRec[257]; // the maximun RecCmd length is 256 plus one status byte.

signals:
    void hid_comm_update(bool fIsConnected, unsigned long dwCmdRecLen);

public slots:
    void PollUSB();

private:
    CMD_STATE cmdRecSt;
    bool fIsConnected;
    unsigned long dwCmdSendBufLen;
    unsigned char pbCmdSendBuf[ENDPOINT_BUF_SIZE+1]; //The first byte is REPORT ID
    unsigned char pbCmdRecBuf[ENDPOINT_BUF_SIZE];

    hid_device *device;
    QTimer *timer;

    void CloseDevice();
};

#endif // HID_PNP_H
