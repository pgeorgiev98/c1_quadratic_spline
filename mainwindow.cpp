#include "mainwindow.h"
#include "surface.h"
#include "utilities.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <QLineEdit>
#include <QDoubleValidator>

static QWidget *labeledWidget(const QString &label, QWidget *widget)
{
	QWidget *w = new QWidget;
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(new QLabel(label));
	layout->addWidget(widget);
	w->setLayout(layout);
	return w;
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_surface(new Surface)
{
	setCentralWidget(m_surface);

	QToolBar *toolBar = new QToolBar;

	QDoubleSpinBox *controlPointSize = new QDoubleSpinBox;
	toolBar->addWidget(labeledWidget("Control point size: ", controlPointSize));

	addToolBar(Qt::ToolBarArea::RightToolBarArea, toolBar);

	controlPointSize->setSingleStep(1);
	controlPointSize->setRange(2, 50);
	controlPointSize->setValue(s()->controlPointSize.get());

	connect(controlPointSize, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
		m_surface->setControlPointSize(value);
	});
	m_surface->setControlPointSize(controlPointSize->value());

	connect(m_surface, &Surface::splineChanged, this, &MainWindow::updateToolBar);
	connect(m_surface, &Surface::selectionChanged, this, &MainWindow::updateToolBar);
	updateToolBar();

	resize(800, 600);
}

void MainWindow::updateToolBar()
{
}

void MainWindow::updateSpline()
{
}
