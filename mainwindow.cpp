#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    labBlank = new QLabel(this);
    labBlank->setAlignment(Qt::AlignCenter);
    ui->statusBar->addPermanentWidget( labBlank, 3);

    labConnect = new QLabel(this);
    labConnect->setAlignment(Qt::AlignCenter);
    ui->statusBar->addPermanentWidget( labConnect, 1);

    fConnectSt = false;
    fOldConnectSt = false;

    lineEditMask = "Hh-";
    ui->lineEditSend->setInputMask(lineEditMask);
    ui->lineEditSend->installEventFilter(this);

    hidDevice = new HID_PnP();
    connect(hidDevice, SIGNAL(hid_comm_update(bool, unsigned long)), this, SLOT(update_gui(bool, unsigned long)));
}

MainWindow::~MainWindow()
{
    disconnect(hidDevice, SIGNAL(hid_comm_update(bool, unsigned long)), this, SLOT(update_gui(bool, unsigned long)));
    delete hidDevice;
    delete ui;
}

void MainWindow::on_lineEditSend_textEdited(const QString &arg1)
{
    // we use setInputMask("Hh-") which create hex field composed of two digits.
    // All hex field (XX-) have two digits.
    //   first digit/  \second digit
    // It at least have two QString for split("-"). The last QString of QStringList is empty QString.

    //We remove hex field has one or zero digit which was killed by user.
    QStringList list = arg1.split("-");
    int pos = ui->lineEditSend->cursorPosition();
    //qDebug("1. %s   %s",list.join("-").toStdString().c_str(), mask.toStdString().c_str());
    int listLen;
    int index = 0;
    while(1)
    {
        listLen = list.size()-2;
        if( listLen==0 || index>=listLen )
        {
            break;
        }

        if(list[index].length()<2 )
        {
            list.removeAt(index);
            lineEditMask.chop(3);
            continue;
        }

        index++;
    }
    //qDebug("2. %s   %s",list.join("-").toStdString().c_str(), mask.toStdString().c_str());

    // When user key in the "last" hex field it always fill zero number in the other digit.
    // ((pos%3)==1) mean it was at second digit.
    // ((pos%3)==0) mean it jump to next hex field first digit. It mean at (list.size()-1) hex field.
    listLen = list.size()-2;
    if( list[listLen].length()==1 )
    {
        if( (pos%3)==1 )
        {
            list[listLen] = list[listLen] + "0";
        }
        else if( (pos%3)==0 )
        {
            list[listLen] = "0" + list[listLen];
        }
        lineEditMask.append("Hh-");
    }

    ui->lineEditSend->setText(list.join("-").toUpper());
    ui->lineEditSend->setInputMask(lineEditMask);
    ui->lineEditSend->setCursorPosition(pos);
}

void MainWindow::on_lineEditSend_cursorPositionChanged(int arg1, int arg2)
{
    QString ProcessText = ui->lineEditSend->text();
    QStringList list = ProcessText.split("-");
    if(arg2 == ((list.size()-1)*3))
    {
        ui->lineEditSend->setCursorPosition(arg2-2);
    }

    arg2 = arg1; //Avoid compiler warnning
}

bool MainWindow::eventFilter(QObject* obj, QEvent *event)
{
    if (obj == ui->lineEditSend)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Backspace)
            {
                int pos = ui->lineEditSend->cursorPosition();
                int div = pos/3;
                int res = pos%3;

                //If have selection. Don't throw away the event
                if( ui->lineEditSend->selectionStart()!=-1 )
                {
                    return false;
                }

                //If it was in first hex field. Throw away the event.
                if( div<1 )
                {
                    return true;
                }

                // Move to the right position. Don't throw away the event.
                if ( res==1 )
                {
                    ui->lineEditSend->setCursorPosition(div*3);
                }
                else if( res==0 )
                {
                    ui->lineEditSend->setCursorPosition((div-1)*3+1);
                }
                return false;
            }
        }
        return false;
    }
    return QMainWindow::eventFilter(obj, event);
}


void MainWindow::on_textEditRec_textChanged()
{
    QScrollBar *sb = ui->textEditRec->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::on_pushButton_clicked()
{
    ui->textEditRec->clear();
}

void MainWindow::update_gui(bool fIsConnected, unsigned long dwCmdRecLen)
{
    if(fIsConnected)
    {
        fConnectSt = true;
        if( fConnectSt!=fOldConnectSt ) {
            fOldConnectSt = fConnectSt;
            labConnect->setText("Connect");
        }

        if(dwCmdRecLen != 0)
        {
            QString result;

            for(unsigned long i=0; i<dwCmdRecLen; i++)
            {
                if(i!=0 && (i%16)==0)
                {
                    result.append("\n");
                }
                result.append(tr("").sprintf("%02X ", hidDevice->pbCmdRec[i]));
            }
            result.append("\n");
            ui->textEditRec->append(result);

        }
    }
    else
    {
        fConnectSt = false;
        if( fConnectSt!=fOldConnectSt ) {
            fOldConnectSt = fConnectSt;
            labConnect->setText("Disconnect");
        }
    }
}

void MainWindow::on_btnSend_clicked()
{

    QStringList list = ui->lineEditSend->text().split("-");
    int iCmdSendLen;
    if( ((unsigned int)(list.size()-2))>sizeof(pbCmdSend) ) {
        iCmdSendLen = sizeof(pbCmdSend);
    } else {
        iCmdSendLen = list.size()-2;
    }

    if( iCmdSendLen==0 )
    {
        return;
    }

    bool ok;
    for(int i=0; i<iCmdSendLen; i++) {
        pbCmdSend[i] =list[i].toInt(&ok, 16);
        Q_ASSERT(ok == true);
    }

    hidDevice->CmdSend( pbCmdSend, iCmdSendLen);
}

