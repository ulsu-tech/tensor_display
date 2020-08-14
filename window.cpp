// window.cpp

#include <QtWidgets>
#include "window.h"
#include "ui_window.h"

#include "myglwidget.h"
#include <math.h>
#include <iostream>

#include "custom_types.h"

Window::Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Window)
    , serial(nullptr)
    , serialBuffFilled (0)
{
    ui->setupUi(this);

    connect(ui->myGLWidget, SIGNAL(xRotationChanged(int)), ui->rotXSlider, SLOT(setValue(int)));
    connect(ui->myGLWidget, SIGNAL(yRotationChanged(int)), ui->rotYSlider, SLOT(setValue(int)));
    connect(ui->myGLWidget, SIGNAL(zRotationChanged(int)), ui->rotZSlider, SLOT(setValue(int)));

    periodic_timer = new QTimer(this);
    connect(periodic_timer, SIGNAL(timeout()), this, SLOT(updateThings()));
    periodic_timer->start(50);
    counter = 0;
    connect(this, SIGNAL(newData(int,int, int)),
            ui->myGLWidget, SLOT(appendNewVector(int, int, int)));
    connect(this, SIGNAL(forceRedraw()), ui->myGLWidget, SLOT(redraw()));
}

Window::~Window()
{
    periodic_timer->stop();
    delete periodic_timer;
    delete ui;
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}

void shiftZero( int &x, int &y, int &z)
{
    x -= 2048;
    y -= 2048;
    z -= 2048;
}

void generateInternalXYZ(int &x, int &y, int &z, const int & counter)
{
    double l = cos(4. * counter / 180. * M_PI) * 2048;
    x = sin(3.*counter / 180. * M_PI) * l + 2048;
    y = sin(counter / 180. * M_PI) * l + 2048;
    z = sin(2.* counter / 180. * M_PI) * l + 2048;
}

void Window::updateThings()
{
    int x=0 ,y=0,z=0;
    int enc1 =0, enc2 = 0, enc3 = 0;
    bool hasData = false;
    if (serial == nullptr)
    {
        serial = new QSerialPort("ttyUSB0", this);
        if (serial != nullptr) {
            serial->setBaudRate(QSerialPort::Baud115200);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setParity(QSerialPort::NoParity);
            if (! serial->open(QIODevice::ReadWrite))
            {
                //serial->close();
                delete serial;
                serial = nullptr;
            }
        }
        generateInternalXYZ(x,y,z, counter);
        hasData = true;
    } else { 
        hasData = handleSerialPortInput(x,y,z, enc1, enc2, enc3);
    }
    //TODO implement also reading of serial data. for now only will
    // push data to GL widget
    //shiftZero(x,y,z);
    //std::cout<<"emitting x="<<x<<"  y="<<y<<"  z="<<z<<std::endl;
    if (hasData) {

        double t1 = getOffsetForTensor(enc1, enc2, enc3, 0);
        double t2 = getOffsetForTensor(enc1, enc2, enc3, 1);
        double t3 = getOffsetForTensor(enc1, enc2, enc3, 2);
        t1 = x - t1;
        t2 = y - t2;
        t3 =  z - t3;
        cutThreshold(t1);
        cutThreshold(t2);
        cutThreshold(t3);
        // TODO convert values from oblique coordinate system to "normal"
        // for drawing
        x = t1;
        y = t2;
        z = t3;

//    shiftZero(x,y,z);
        emit newData(x, y, z);
    }
    emit forceRedraw();
    counter++;
}

static const uint16_t crc16tab[256]= {
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
    0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
    0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
    0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
    0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
    0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
    0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
    0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
    0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
    0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
    0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
    0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
    0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
    0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
    0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
    0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
    0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
    0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
    0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
    0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
    0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
    0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
    0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
    0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
    0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
    0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
    0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
    0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
    0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
    0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
    0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

uint16_t crc16(const char *buf, int len) {
    const uint8_t *pdata = reinterpret_cast<const uint8_t*>(buf);
    uint16_t crc = 0;

    for( ; len > 0; len--, pdata++) {
        crc += (*pdata) * 44111;
       //printf( "crc: crc=0x%04x\n", crc);
    }

    return crc ^ (crc >> 8);

}

int checkPossibleMessage(const char *data, const int maxSize)
{
    //for now only data_pos messages are held
    if ( data[0] == 0x50 && maxSize >= sizeof(PositionalData))
    {
        auto mSize =  sizeof(PositionalData) - sizeof(uint16_t);
        auto crc_att = crc16(data, mSize);
        if (crc_att != *(reinterpret_cast<const uint16_t*>(data +mSize)))
        {
            qDebug()<<" on CRC check received "<<crc_att<<"  VS  "<<
                *(reinterpret_cast<const uint16_t*>(data +mSize));
            return -1;
        }
        return sizeof(PositionalData); //sizeof( DATA_POS_struct)
    }
    return -1;
}

void fillXYZfromDATA_POS_message(const char *data, int &x, int &y, int&z)
{
    struct data const &received(*(reinterpret_cast<struct data const *>(data)));
    x = received.tenzor[0];
    y = received.tenzor[1];
    z = received.tenzor[2];
	qDebug()<<"after message received x="<<x<<"  y="<<y<<"   z="<<z;
}

void fillENCODERSfromDATA_POS_message(const char * data,
    int &enc1, int &enc2, int &enc3)
{
    struct data const &received(*(reinterpret_cast<struct data const *>(data)));
    enc1 = received.enc1;
    enc2 = received.enc2;
    enc3 = received.enc3;
}

bool Window::handleSerialPortInput(int &x, int &y, int &z, int &enc1, int &enc2, int &enc3)
{
    bool rv = false;
    int temp = serial->read(serialBuffer + serialBuffFilled, 2048 - serialBuffFilled);
    if (temp > 0)
        serialBuffFilled += temp;
    if (serialBuffFilled > 0)
    {
        for(int i=0; i < serialBuffFilled; ++i)
        {
            auto validSize = checkPossibleMessage(serialBuffer + i, serialBuffFilled - i);
            if (validSize > 0) {
                fillXYZfromDATA_POS_message(serialBuffer + i, x,y,z);
                fillENCODERSfromDATA_POS_message(serialBuffer + i,
                    enc1, enc2, enc3);
                SHL(i + validSize);
                rv = true;
            }
        }
    }
    return rv;
}

void Window::SHL(const int shift)
{
    if (shift == serialBuffFilled)
    {
        serialBuffFilled = 0;
        return;
    }
    memmove(serialBuffer, serialBuffer + shift, serialBuffFilled - shift);
    serialBuffFilled -= shift;
}

double Window::getOffsetForTensor(int enc1, int enc2, int enc3, int section)
{
    double coeefs[COEFFS_GROUPS_COUNT][ENCODERS_COUNT][7] = {
 {
    // group from sheet _169
  {  0.0042670552, -0.001177697, -0.0005183442,
        -7.5067725e-8, 1.6e-8, -2.65646976e-8, 2441.68629
  },
  { 0.0033252889, -0.0004951877, -0.0016182633,
        -3.7455457e-8, 9.63065116e-8, -4.4462e-8, 1869.2946
  },
  { -0.0117989483, 0.0027467499, -0.002497153,
        9.5796589e-8, -5.25122848e-8, 7.57885e-8, 1883.3314
  }
 }
    };

    if (section <0 || section >= encodersCount)
        return -1.;

    int usedGroup = 0;

    return coeefs[usedGroup][section][6] +
        coeefs[usedGroup][section][0] * enc1 +
        coeefs[usedGroup][section][1] * enc2 +
        coeefs[usedGroup][section][2] * enc3 +
        coeefs[usedGroup][section][3] * enc1 * enc1 +
        coeefs[usedGroup][section][4] * enc2 * enc2 +
        coeefs[usedGroup][section][5] * enc3 * enc3;
}

const unsigned int Window::encodersCount = 3;
const double Window::tensorZeroThreshold = 150;
