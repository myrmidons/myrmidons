#ifdef DEBUG

#include "DebugWindow.hpp"
#include "Map.hpp"
#include "Room.hpp"
#include "State.hpp"
#include "Util.hpp"
#include <QPainter>

const int Mult = 8; // pixels per grid cell

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
	resize(mapSize.x()*Mult, mapSize.y()*Mult);
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
		//} while (r+g+b < 200 || r+g+b > 650); // avoid blacks and whites
	} while (r+g+b < 250); // Racist code (avoid blacks)

	return qRgb(r,g,b);
}

void DebugWindow::redrawImg() {
	LOG_DEBUG("Rooms::dumpImage");

	const QRgb voidColor = qRgb(60,60,60);
	const QRgb wallColor = qRgb(0,0,0);

	Vec2 size = g_map->size();
	m_img = QImage(Mult*size.x(), Mult*size.y(), QImage::Format_ARGB32);
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

			for (int xi=0; xi<Mult; ++xi)
				for (int yi=0; yi<Mult; ++yi)
					m_img.setPixel(x*Mult+xi, y*Mult+yi, color);
		}
	}

	//////////////////////////

	{
		QPainter painter(&m_img);
		painter.setPen(Qt::white);

		// Add graph info
		std::map<Room*, QPointF> centers;
		ITC(RoomList, rit, rooms) {
			Room* r = *rit;
			Pos c = r->centerPos();
			centers[r] = Mult*QPointF(c.x()+.5f, c.y()+.5f);
		}

		//painter.setClipping(true);

		ITC(RoomList, rit, rooms) {
			Room* r = *rit;
			QPointF a = centers[r];
			const RoomSet& neighs = r->neighborRooms();
			ITC(RoomSet, rit2, neighs) {
				QPointF b = centers[*rit2];

				bool xwrap = (Abs(a.x()-b.x()) > m_img.width()/2);
				bool ywrap = (Abs(a.y()-b.y()) > m_img.height()/2);
				if (xwrap && ywrap) {
					// TODO
				} else if (xwrap) {
					if (a.x() < b.x())
						std::swap(a,b);

					painter.drawLine(a, b + QPointF(m_img.width(), 0));
					painter.drawLine(a - QPointF(m_img.width(), 0), b);
				} else if (ywrap) {
					if (a.y() < b.y())
						std::swap(a,b);

					painter.drawLine(a, b + QPointF(0, m_img.height()));
					painter.drawLine(a - QPointF(0, m_img.height()), b);
				} else {
					painter.drawLine(a,b);
				}
			}
		}

		float rad = 0.4f*Mult;

		ITC(RoomList, rit, rooms)
			painter.drawEllipse(centers[*rit], rad, rad);
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
