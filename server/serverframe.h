#ifndef SERVER_FRAME_H
#define SERVER_FRAME_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QString>
#include <QTimer>
#include <QSpinBox>
#include <QUdpSocket>

using atomic_t = volatile unsigned int;

class ServerFrame : public QDialog {
	Q_OBJECT
public:
    ServerFrame(QWidget *parent = 0, const char *name = 0);

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
    QLabel		intervalLabel_;
    QSpinBox	intervalSpinBox_;
    QLabel		unitLabel_;
    QPushButton	refreshButton_;
	// Others
    QString		localHostIP_;
    QString		broadcastIP_;
    QString		workIP_;
    QString		uniqueFileName_;
    QTimer		broadCastTimer_;
    QTimer		workTimer_;
    QUdpSocket  sktDev_;
    int		    interval_;
    atomic_t	run_;
};

#endif
