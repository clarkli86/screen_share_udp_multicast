#include <QString>
#include <QBuffer>
#include <QHostAddress>
#include <QPixmap>
#include <QDataStream>
#include <QCloseEvent>
#include <QPainter>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <QNetworkInterface>
#include <unistd.h>
#include "serverframe.h"
namespace {
const char * IP_STRING_SPLITTER = ".";
const char * MULTI_CAST_PREFIX = "224.0.0.";
const char * BROADCAST = "224.0.0.1";
constexpr int PORT = 5000;
const char * LOCAL = "127.0.0.1";
const char * INVALID = "224.0.1.255";

constexpr int INTERVAL_MIN = 500;
constexpr int INTERVAL_MAX = 5000;
constexpr int INTERVAL_STEP = 10;

constexpr int WIDGET_WIDTH = 256;
constexpr int WIDGET_HEIGHT = 235;

constexpr int GRABINTERVAL = 45000;
}

ServerFrame::ServerFrame(QWidget *parent, const char *name)	: 
    QDialog(parent, Qt::WindowStaysOnTopHint),
    intervalLabel_(tr("&Interval: "), this),
    intervalSpinBox_(this),
    unitLabel_(tr(" microsecond(s)"), this),
    refreshButton_(tr("&Refresh"), this)
{
	// init controls
	fetchLocalHostIP();
    setWindowTitle(tr("Host IP: ") + localHostIP_);
    intervalLabel_ .move(6, 6);
    intervalSpinBox_ .setMinimum(INTERVAL_MIN);
    intervalSpinBox_ .setMaximum(INTERVAL_MAX);
    intervalSpinBox_ .setSingleStep(INTERVAL_STEP);
    intervalSpinBox_ .setValue(INTERVAL_MIN);
    interval_ = INTERVAL_MIN;
    intervalLabel_ .setBuddy(&intervalSpinBox_);
    intervalSpinBox_ .move(60, 6);

    unitLabel_ . move(160, 6);
    connect(&refreshButton_, SIGNAL(clicked()), this, SLOT(refresh()));
    refreshButton_ . resize(236, 180);
    refreshButton_ . move(10, 45);

	setMinimumWidth(WIDGET_WIDTH);
	setMinimumHeight(WIDGET_HEIGHT);
	setMaximumWidth(WIDGET_WIDTH);
	setMaximumHeight(WIDGET_HEIGHT);
	setSizeGripEnabled(false);

    run_ = 0;

    if (localHostIP_ == tr(LOCAL)) {
        refreshButton_.setEnabled(false);
	} else  {
        broadcastIP_ = tr(BROADCAST);
        connect(&broadCastTimer_, SIGNAL(timeout()), this, SLOT(broadcast()));

        broadCastTimer_. start(500);
        connect(&workTimer_, SIGNAL(timeout()), this, SLOT(work()));
        workTimer_.start(500);
	}
}

void
ServerFrame::closeEvent(QCloseEvent *event) {   
    broadCastTimer_.stop();
    workTimer_ . stop();
	event -> accept();
}

void
ServerFrame::refresh(void) {
    if (interval_ != intervalSpinBox_.value()) {
        workTimer_ . setInterval(intervalSpinBox_.value());
        interval_ = intervalSpinBox_.value();
	}
	work();
}

void
ServerFrame::broadcast(void) {
	QHostAddress	broadcastAddress;
    broadcastAddress.setAddress(broadcastIP_);

    const auto ip = workIP_.toStdString();
    sktDev_.writeDatagram(ip.c_str(), ip.length(), broadcastAddress, PORT);
}

int
ServerFrame::getBltCount(QPixmap &win) {
	QByteArray      ba;    ;
    QDataStream     s(&ba, QIODevice::WriteOnly);
    s << win;
	return (ba.size() / 50000 + 1);
}

void
ServerFrame::work(void) {
    if (run_ == 0) {
        run_ = 1;
		QHostAddress	workAddress;
        workAddress.setAddress(workIP_);
		QPixmap		window = QPixmap::grabWindow(0);

		int		bltCount = getBltCount(window);
		int		grabWidth = window.width(),
				grabHeight = window.height() / bltCount,
				grabLeft = window.height() % bltCount,
				i = 0;

		for (; i < bltCount; ++i) {
			QPixmap		map(grabWidth, grabHeight);
            QPainter painter(&map);
            painter.drawPixmap(0, 0, window, 0, i * grabHeight, grabWidth, grabHeight);
			QByteArray      ba;
            QDataStream     s(&ba, QIODevice::WriteOnly);
			s << window.width() << window.height()
                << i * grabHeight << map;

            sktDev_.writeDatagram(reinterpret_cast<const char *>(ba.data()), ba.size(),
					workAddress, PORT);
			usleep(GRABINTERVAL);
		}

		if (grabLeft) {
			QPixmap         map(grabWidth, grabLeft);			
            QPainter painter(&map);
            painter.drawPixmap(0, 0, window, 0, i * grabHeight, grabWidth, grabLeft);
			QByteArray      ba;
            QDataStream     s(&ba, QIODevice::WriteOnly);
			s << window.width() << window.height()
				<< i * grabHeight << map;
            sktDev_.writeDatagram(reinterpret_cast<const char *>(ba.data()), ba.size(),
					workAddress, PORT);
		}

        run_ = 0;
	}
}

void
ServerFrame::fetchLocalHostIP() {
    const auto interfaces = QNetworkInterface::allInterfaces();
    for(auto & interface : interfaces) {
        const auto & addresses = interface.addressEntries();
        for(auto & address : addresses) {
        if(   address.ip().toString() != LOCAL
           && !(interface.flags() & QNetworkInterface::IsLoopBack)
           && interface.flags() & QNetworkInterface::IsUp
              ) {
            localHostIP_ = address.ip().toString();
            workIP_ = QString(MULTI_CAST_PREFIX)
                    + address.ip().toString().split(IP_STRING_SPLITTER).back();
            return;
        }
        }
    }
}

