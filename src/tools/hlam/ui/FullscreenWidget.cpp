#include <QKeyEvent>
#if QT_VERSION_MAJOR < 6
#include <QtPlatformHeaders/QWindowsWindowFunctions>
#endif

#include "ui/EditorContext.hpp"
#include "ui/FullscreenWidget.hpp"

namespace ui
{
FullscreenWidget::FullscreenWidget(QWidget* parent)
	: QMainWindow(parent)
{
	//This is not a primary window so don't keep the app running if we're still alive
	setAttribute(Qt::WidgetAttribute::WA_QuitOnClose, false);

	//This has to be a native window for there to be a window handle
	setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);

	//TODO - Investigate if that is still the case with Qt 6
#if QT_VERSION_MAJOR < 6
	//Without this going fullscreen will cause black flickering
	QWindowsWindowFunctions::setHasBorderInFullScreen(this->windowHandle(), true);
#endif
}

FullscreenWidget::~FullscreenWidget() = default;

void FullscreenWidget::ExitFullscreen()
{
	this->hide();
	//This ensures the old widget is deleted now to avoid having a dangling reference to an unloaded asset
	auto oldWidget = this->takeCentralWidget();

	delete oldWidget;
}

bool FullscreenWidget::ProcessKeyEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case ExitFullscreenKey:
	{
		ExitFullscreen();
		return true;
	}

	case ToggleFullscreenKey:
	{
		if (this->isFullScreen())
		{
			this->showMaximized();
		}
		else
		{
			this->showFullScreen();
		}

		return true;
	}

	default: return false;
	}
}

bool FullscreenWidget::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::Type::KeyPress)
	{
		const auto keyEvent = static_cast<QKeyEvent*>(event);

		return ProcessKeyEvent(keyEvent);
	}

	return false;
}

void FullscreenWidget::keyPressEvent(QKeyEvent* event)
{
	if (ProcessKeyEvent(event))
	{
		return;
	}

	QMainWindow::keyPressEvent(event);
}
}
