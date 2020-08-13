// window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QSlider>
#include <QTimer>

#include <QSerialPort>

#define COEFFS_GROUPS_COUNT 1
#define ENCODERS_COUNT 3

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
    bool handleSerialPortInput(int &x, int &y, int &z, int &enc1, int &enc2, int &enc3);
    void SHL(const int shift);
    double getOffsetForTensor(int, int, int, int);
    static const unsigned int encodersCount;
    static const double tensorZeroThreshold;

    inline void cutThreshold(double &v) {
        if (abs(v) > tensorZeroThreshold) {
            v -= (v>0?tensorZeroThreshold:-tensorZeroThreshold);
        } else {
            v = 0;
        }
    };
};

#endif // WINDOW_H
