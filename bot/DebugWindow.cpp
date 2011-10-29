#ifdef DEBUG

#include "DebugWindow.hpp"
#include "Map.hpp"
#include "Room.hpp"
#include "State.hpp"
#include "Util.hpp"
#include "Tracker.hpp"
#include "Logger.hpp"
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

const QRgb FoodColor = qRgb(0, 160, 0); // Dark green. Inviting.
const QRgb VoidColor = qRgb(60,60, 60); // Undiscovered
const QRgb WallColor = qRgb(0, 0,  0);
const QRgb FriendColor = qRgb(0, 0,  255);
const QRgb EnemyColor = qRgb(255, 0,  0);

bool compClose(int x, int y) {
	return Abs(x-y) < 75;
}

bool colorClose(QRgb x, QRgb y) {
	return
			compClose(qRed(x),   qRed(y))   &&
			compClose(qGreen(x), qGreen(y)) &&
			compClose(qBlue(x),  qBlue(y));
}

int darkenComp(int c) {
	return c/2;
}

QRgb darken(QRgb color) {
	return qRgb(
				darkenComp(qRed(color)),
				darkenComp(qGreen(color)),
				darkenComp(qBlue(color)));
}

QRgb randomColor(Room* room) {
	//srand(reinterpret_cast<long>(room));
	int id = room->id;

	QRgb color;
	do {
		/*
		r = rand()%255;
		g = rand()%255;
		b = rand()%255;
		/*/
		int r = (12345   * id) % 255;
		int g = (123456  * id) % 255;
		int b = (1234578 * id) % 255;
		color = qRgb(r,g,b);
		id += 78901;
		/**/
	} while (colorClose(color, FoodColor) || colorClose(color, VoidColor) || colorClose(color, WallColor) ||
			 colorClose(color, FriendColor) || colorClose(color, EnemyColor));
	//} while (r+g+b < 250); // Racist code (avoid blacks)

	return color;
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

void drawSquare(QPainter& painter, QPointF pos, float r, QRgb color) {
	QRectF rect(pos.x()-r, pos.y()-r, 2*r, 2*r);
	painter.fillRect(rect, color);

	painter.setPen(Qt::white);
	painter.drawRect(rect);
}

void DebugWindow::redrawImg() {
	LOG_DEBUG("Rooms::dumpImage");

	Vec2 size = g_map->size();
	m_img = QImage(Zoom*size.x(), Zoom*size.y(), QImage::Format_ARGB32);
	m_img.fill(VoidColor);

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
				color = WallColor;
			else
				continue; // Undiscovered

			QRgb oddColor = color;
			if (!s.visible())
				oddColor = darken(color); // Draw fog of war using striped

			for (int xi=0; xi<Zoom; ++xi)
				for (int yi=0; yi<Zoom; ++yi)
					m_img.setPixel(x*Zoom+xi, y*Zoom+yi, ((yi/2+1)%2) ? color : oddColor);
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
		float antRad = 0.35f*Zoom;

		const EnemySet& enemies = g_tracker->getEnemies();
		ITC(EnemySet, eit, enemies)
			drawSquare(painter, toQP(eit->pos), antRad, EnemyColor);

		const AntSet& ants = g_tracker->getAnts();
		ITC(AntSet, ait, ants) {
			Ant* ant = *ait;
			QPointF pos = toQP(ant->pos());
			drawSquare(painter, pos, antRad, FriendColor);

			if (ant->state() != Ant::STATE_NONE) {
				const Path& path = ant->path();
				if (path.isValid()) {
					float destRad = 0.15f * Zoom;
					painter.setPen(Qt::black);

					/*
					QPointF dest = toQP(path.dest());
					drawWrappedLine(painter, pos, dest);
					painter.drawEllipse(dest, destRad, destRad);
					/*/
					Room* antRoom = g_map->roomAt(ant->pos());
					QPointF p = pos;
					painter.drawEllipse(p, 1, 1);
					const WPList& wayPoints = path.wayPoints();
					bool draw=false;
					for (int i=0; i<(int)wayPoints.size(); ++i) {
						if (!draw) {
							draw = (wayPoints[i].room == antRoom);
							continue; // Start drawing waypoint in next room
						}
						QPointF wpPos = toQP(wayPoints[i].pos);

						drawWrappedLine(painter, p, wpPos);
						painter.drawEllipse(wpPos, destRad, destRad);

						p = wpPos;
					}
					QPointF wpPos = toQP(path.dest());
					drawWrappedLine(painter, p, wpPos);
					painter.drawEllipse(wpPos, destRad, destRad);
					/**/
				}
			}

			if (ant->expectedPos() != ant->pos()) {
				// Draw where ant is expected to go
				painter.setPen(Qt::white);
				drawWrappedLine(painter, pos, toQP(ant->expectedPos()));
			}
		}

		// Draw food
		const PosSet& food = g_tracker->getFood();
		ITC(PosSet, pit, food) {
			painter.setPen(FoodColor);
			QPointF pos = toQP(*pit);
			painter.drawEllipse(pos, 1, 1);
			painter.drawEllipse(pos, 2, 2);
			painter.drawEllipse(pos, 3, 3);
		}
	}

	/////////////////////////

	char nameBuf[100] = {};
	sprintf(nameBuf, "rooms_%04d.png", g_tracker->turn());
	LOG_DEBUG("Dumping rooms to " << nameBuf);
	m_img.save(nameBuf);

	/////////////////////////

	m_dirty = false;
}

#endif
