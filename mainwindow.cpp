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
#include <QCheckBox>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_surface(new Surface)
{
	setCentralWidget(m_surface);

	QToolBar *toolBar = new QToolBar;

	QCheckBox *showControlPoly = new QCheckBox("Show control polygon");
	toolBar->addWidget(showControlPoly);

	addToolBar(Qt::ToolBarArea::BottomToolBarArea, toolBar);

	QToolButton *resetButton = new QToolButton;
	resetButton->setText("Reset");
	toolBar->addWidget(resetButton);

	QToolButton *equalizeButton = new QToolButton;
	equalizeButton->setText("Equalize");
	toolBar->addWidget(equalizeButton);

	showControlPoly->setChecked(s()->showControlPoly.get());
	connect(showControlPoly, &QCheckBox::stateChanged, [this](int state) {
		s()->showControlPoly.set(state);
		m_surface->update();
	});

	connect(resetButton, &QToolButton::clicked, m_surface, &Surface::reset);
	connect(equalizeButton, &QToolButton::clicked, m_surface, &Surface::equalize);
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
