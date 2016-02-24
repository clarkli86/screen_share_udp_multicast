#ifndef SERVER_FRAME_H
#define SERVER_FRAME_H

#include <QtWidgets/QDialog>

class QLabel;
class QPushButton;
class QPixmap;
class QString;
class QTimer;
class QSpinBox;
class QUdpSocket;

typedef volatile unsigned int atomic_t;

class ServerFrame : public QDialog {
	Q_OBJECT
public:
	ServerFrame(QWidget *parent = 0, const char *name = 0);
	void getUniqueFileName(char *);

protected:
	void closeEvent(QCloseEvent *);
	int getBltCount(QPixmap &);
	void fetchLocalHostIP();

protected slots:
	void refresh(void);
	void broadcast(void);
	void work(void);

private:
	// Member variables
	// Controls
	QLabel		*intervalLabel;
	QSpinBox	*intervalSpinBox;
	QLabel		*unitLabel;
	QPushButton	*refreshButton;
	// Others
	QString		localHostIP;
	QString		broadcastIP;
	QString		workIP;
	QString		uniqueFileName;
	QTimer		*broadCastTimer;
	QTimer		*workTimer;
	QUdpSocket *sktDev;
	int		interval;
	atomic_t	run;
};

#endif
