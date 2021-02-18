#include "mainwindow.h"
#include "settings.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName("PGeorgiev");
	a.setOrganizationDomain("pgeorgiev.org");
	a.setApplicationName("c1_quadratic_spline");
	Settings s;

	MainWindow w;
	w.show();
	return a.exec();
}
