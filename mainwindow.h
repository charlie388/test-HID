#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QKeyEvent>
#include <QScrollBar>
#include "hid_pnp.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    unsigned char pbCmdSend[7];

protected:
    bool eventFilter(QObject* obj, QEvent *event);

private:
    Ui::MainWindow *ui;
    QLabel *labBlank;
    QLabel *labConnect;
    HID_PnP *hidDevice;
    bool fConnectSt;
    bool fOldConnectSt;
    QString lineEditMask;

public slots:
    void update_gui(bool fIsConnected, unsigned long dwRecCmdLen);

private slots:
    void on_lineEditSend_textEdited(const QString &arg1);
    void on_lineEditSend_cursorPositionChanged(int arg1, int arg2);
    void on_btnSend_clicked();
    void on_textEditRec_textChanged();
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
