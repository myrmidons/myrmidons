#ifndef DEBUGWINDOW_HPP
#define DEBUGWINDOW_HPP

#ifdef DEBUG

#include <QWidget>

class DebugWindow : public QWidget
{
    Q_OBJECT
public:
	static DebugWindow* instance();

    explicit DebugWindow(QWidget *parent = 0);

	// Call each frame
	void redraw();

protected:
	virtual void paintEvent(QPaintEvent* event);

private:
	void redrawImg();

	bool m_dirty;
	QImage m_img;

	static DebugWindow* s_instance;
};

#endif

#endif
