#ifdef DEBUG

#include "DebugWindow.hpp"
#include "Map.hpp"
#include "Room.hpp"
#include "State.hpp"
#include "Util.hpp"
#include "Tracker.hpp"
#include <QPainter>

const int Zoom = 12; // pixels per grid cell

DebugWindow* DebugWindow::s_instance = NULL;

DebugWindow* DebugWindow::instance() {
	if (!s_instance) {
		s_instance = new DebugWindow();
		s_instance->show();
	}
	return s_instance;
}

/////////////////////////////////////////////////////////

DebugWindow::DebugWindow(QWidget *parent) : QWidget(parent), m_dirty(true)
{
	setWindowTitle("Debug window");
	Vec2 mapSize = g_map->size();
	resize(mapSize.x()*Zoom, mapSize.y()*Zoom);
}

void DebugWindow::redraw() {
	LOG_DEBUG("DebugWindow::redraw");

	m_dirty = true;
	redrawImg();
	update();
}

void DebugWindow::paintEvent(QPaintEvent* event) {
	LOG_DEBUG("DebugWindow::paintEvent");

	if (m_dirty)
		redrawImg();

	QPainter painter(this);
	painter.drawImage(0, 0, m_img);
}

QRgb randomColor(Room* room) {
	//srand(reinterpret_cast<long>(room));
	int id = room->id;

	int r,g,b;
	do {
		/*
		r = rand()%255;
		g = rand()%255;
		b = rand()%255;
		/*/
		r = (12345   * id) % 255;
		g = (123456  * id) % 255;
		b = (1234578 * id) % 255;
		id += 78901;
		/**/
	} while (r+g+b < 200 || r+g+b > 650); // avoid blacks and whites
	//} while (r+g+b < 250); // Racist code (avoid blacks)

	return qRgb(r,g,b);
}

QPointF toQP(Vec2 pos) {
	return Zoom*QPointF(pos.x()+.5f, pos.y()+.5f);
}

void drawWrappedLine(QPainter& painter, QPointF a, QPointF b) {
	int w = painter.device()->width();
	int h = painter.device()->height();

	bool xwrap = (Abs(a.x()-b.x()) > w/2);
	bool ywrap = (Abs(a.y()-b.y()) > h/2);

	if (xwrap && ywrap) {
		if (a.x() < b.x())
			std::swap(a,b);

		painter.drawLine(a, b + QPointF(w, h));
		painter.drawLine(a - QPointF(w, h), b);
	} else if (xwrap) {
		if (a.x() < b.x())
			std::swap(a,b);

		painter.drawLine(a, b + QPointF(w, 0));
		painter.drawLine(a - QPointF(w, 0), b);
	} else if (ywrap) {
		if (a.y() < b.y())
			std::swap(a,b);

		painter.drawLine(a, b + QPointF(0, h));
		painter.drawLine(a - QPointF(0, h), b);
	} else {
		painter.drawLine(a,b);
	}
}

void DebugWindow::redrawImg() {
	LOG_DEBUG("Rooms::dumpImage");

	const QRgb voidColor = qRgb(60,60,60);
	const QRgb wallColor = qRgb(0,0,0);

	Vec2 size = g_map->size();
	m_img = QImage(Zoom*size.x(), Zoom*size.y(), QImage::Format_ARGB32);
	m_img.fill(voidColor);

	std::map<Room*, QRgb, RoomComp> colorMap;
	RoomList rooms = g_rooms->rooms();
	ITC(RoomList, rit, rooms)
		colorMap[*rit] = randomColor(*rit);

	for (int y=0; y<size.y(); ++y) {
		for (int x=0; x<size.x(); ++x) {
			Square& s = g_map->square(Pos(x,y));
			QRgb color;
			if (s.room)
				color = colorMap[s.room];
			else if (s.isWater)
				color = wallColor;
			else
				continue; // Undiscovered

			for (int xi=0; xi<Zoom; ++xi)
				for (int yi=0; yi<Zoom; ++yi)
					m_img.setPixel(x*Zoom+xi, y*Zoom+yi, color);
		}
	}

	//////////////////////////

	{
		QPainter painter(&m_img);
		painter.setPen(Qt::white);

		// Add graph info
		std::map<Room*, QPointF> centers;
		ITC(RoomList, rit, rooms)
			centers[*rit] = toQP((*rit)->centerPos());

		ITC(RoomList, rit, rooms) {
			Room* r = *rit;
			QPointF a = centers[r];
			const RoomSet& neighs = r->neighborRooms();
			ITC(RoomSet, rit2, neighs) {
				QPointF b = centers[*rit2];
				drawWrappedLine(painter, a, b);
			}
		}

		float roomCenterRad = 0.3f*Zoom;

		ITC(RoomList, rit, rooms)
			painter.drawEllipse(centers[*rit], roomCenterRad, roomCenterRad);

		//////////////////////////////////////////////////
		// Draw ants

		const AntSet& ants = g_tracker->getLiveAnts();
		ITC(AntSet, ait, ants) {
			Ant* ant = *ait;
			QPointF pos = toQP(ant->pos());
			float antRad = 0.35f*Zoom;
			QRectF rect(pos.x()-antRad, pos.y()-antRad, 2*antRad, 2*antRad);
			painter.fillRect(rect, Qt::red);

			painter.setPen(Qt::white);
			painter.drawRect(rect);

			if (ant->state() != Ant::STATE_NONE) {
				const Path& path = ant->path();
				if (path.isValid()) {
					QPointF dest = toQP(path.dest());
					painter.setPen(Qt::black);
					drawWrappedLine(painter, pos, dest);
					float destRad = 0.15f * Zoom;
					painter.drawEllipse(dest, destRad, destRad);
				}
			}
		}

		// TODO Draw food
	}

	/////////////////////////

	static int s_nr = 0;
	char nameBuf[100] = {};
	sprintf(nameBuf, "rooms_%05d.png", s_nr++);
	LOG_DEBUG("Dumping rooms to " << nameBuf);
	m_img.save(nameBuf);

	/////////////////////////

	m_dirty = false;
}

#endif
