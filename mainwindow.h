#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Surface;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);

private slots:
	void updateToolBar();
	void updateSpline();

private:
	Surface *m_surface;
};
#endif // MAINWINDOW_H
