// window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QSlider>
#include <QTimer>

#include <QSerialPort>

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);
    ~Window();

public slots:
    void updateThings();

signals:
    void newData(int, int, int);
    void forceRedraw();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::Window *ui;
    QTimer *periodic_timer;
    int counter;

    QSerialPort *serial;
    char serialBuffer[2048];
    int serialBuffFilled;
    bool handleSerialPortInput(int &x, int &y, int &z);
    void SHL(const int shift);
};

#endif // WINDOW_H
