#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QString>
#include <QTimer>
#include <QBuffer>
#include <QtWidgets/QSpinBox>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QPixmap>
#include <QDataStream>
#include <QCloseEvent>
#include <QPainter>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "serverframe.h"

#define BROADCAST "224.0.0.1"
#define PORT 5000
#define LOCAL "127.0.0.1"
#define INVALID "224.0.1.255"

#define INTERVAL_MIN 500
#define INTERVAL_MAX 5000
#define INTERVAL_STEP 10

#define WIDGET_WIDTH 256
#define WIDGET_HEIGHT 235

#define GRABINTERVAL 45000

ServerFrame::ServerFrame(QWidget *parent, const char *name)
	: QDialog(parent, Qt::WindowStaysOnTopHint) {
	// init controls
	fetchLocalHostIP();
	setWindowTitle(tr("Host IP: ") + localHostIP);
	intervalLabel = new QLabel(tr("&Interval: "), this);
	intervalLabel -> move(6, 6);
	intervalSpinBox = new QSpinBox(this);
	intervalSpinBox -> setMinimum(INTERVAL_MIN);
	intervalSpinBox -> setMaximum(INTERVAL_MAX);
	intervalSpinBox -> setSingleStep(INTERVAL_STEP);
	intervalSpinBox -> setValue(INTERVAL_MIN);
	interval = INTERVAL_MIN;
	intervalLabel -> setBuddy(intervalSpinBox);
	intervalSpinBox -> move(60, 6);

	unitLabel = new QLabel(tr(" microsecond(s)"), this);
	unitLabel -> move(160, 6);
	refreshButton = new QPushButton(tr("&Refresh"), this);
	connect(refreshButton, SIGNAL(clicked()), this, SLOT(refresh()));
	refreshButton -> resize(236, 180);
	refreshButton -> move(10, 45);

	setMinimumWidth(WIDGET_WIDTH);
	setMinimumHeight(WIDGET_HEIGHT);
	setMaximumWidth(WIDGET_WIDTH);
	setMaximumHeight(WIDGET_HEIGHT);
	setSizeGripEnabled(false);

	run = 0;

	if (localHostIP == tr(LOCAL)) {
		refreshButton -> setEnabled(false);
		sktDev = NULL;
		broadCastTimer = NULL;
		workTimer = NULL;
	} else  {
		broadcastIP = tr(BROADCAST);

		sktDev = new QUdpSocket();

		broadCastTimer = new QTimer(this);
		connect(broadCastTimer, SIGNAL(timeout()),
					this, SLOT(broadcast()));
		broadCastTimer -> start(500);

		workTimer = new QTimer(this);
		connect(workTimer, SIGNAL(timeout()), this, SLOT(work()));
		workTimer -> start(500);
	}
}

void
ServerFrame::getUniqueFileName(char *name) {
	uniqueFileName = tr(name);
}

void
ServerFrame::closeEvent(QCloseEvent *event) {
	if (sktDev != NULL)
		delete sktDev;
	if (broadCastTimer != NULL) {
		broadCastTimer -> stop();
		delete broadCastTimer;
	}
	if (workTimer != NULL) {
		workTimer -> stop();
		delete workTimer;
	}
	unlink(reinterpret_cast<const char *>(uniqueFileName.data()));
	event -> accept();
}

void
ServerFrame::refresh(void) {
	if (interval != intervalSpinBox -> value()) {
		workTimer -> setInterval(intervalSpinBox -> value());
		interval = intervalSpinBox -> value();
	}
	work();
}

void
ServerFrame::broadcast(void) {
	QHostAddress	broadcastAddress;
	broadcastAddress.setAddress(broadcastIP);
	sktDev -> writeDatagram(reinterpret_cast<const char *>(workIP.data()), workIP.length(), broadcastAddress, PORT);
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
	if (run == 0) {
		run = 1;
		QHostAddress	workAddress;
		workAddress.setAddress(workIP);
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
            sktDev -> writeDatagram(reinterpret_cast<const char *>(ba.data()), ba.size(),
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
            sktDev -> writeDatagram(reinterpret_cast<const char *>(ba.data()), ba.size(),
					workAddress, PORT);
		}

		run = 0;
	}
}

void
ServerFrame::fetchLocalHostIP() {
	int		inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq	ifr;
	unsigned int	s_addr = 0X000100E0, s_mask = 0X0FF000000;

	strcpy(ifr.ifr_name, "eth0");
	if (ioctl(inet_sock, SIOCGIFADDR, &ifr) < 0) {
		localHostIP = tr(LOCAL);
		workIP = tr(INVALID);
	}
	else {
		in_addr	address =
			((struct sockaddr_in *) &(ifr.ifr_addr)) -> sin_addr;
		localHostIP = tr(inet_ntoa(address));
		s_mask &= address.s_addr;
		s_addr |= s_mask;
		address.s_addr = s_addr;
		workIP = tr(inet_ntoa(address));
	}
}

