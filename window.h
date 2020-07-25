// window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QSlider>
#include <QTimer>

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
};

#endif // WINDOW_H
