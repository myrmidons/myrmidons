#ifdef DEBUG

#include "DebugWindow.hpp"
#include "Map.hpp"
#include "Room.hpp"
#include "State.hpp"
#include "Util.hpp"
#include "Tracker.hpp"
#include "Logger.hpp"
#include <QPainter>

const int Zoom = 10; // pixels per grid cell

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

void DebugWindow::paintEvent(QPaintEvent* /*event*/) {
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
const QRgb EnemyHillColor = qRgb(255, 0,  0);

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
		// Deterministicly pseudo-random colors:
		int r = (12345   * id) % 256;
		int g = (123456  * id) % 256;
		int b = (1234578 * id) % 256;
		color = qRgb(r,g,b);
		id += 78901;
	} while (colorClose(color, FoodColor) || colorClose(color, VoidColor) || colorClose(color, WallColor) ||
			 colorClose(color, FriendColor) || colorClose(color, EnemyColor) || colorClose(color, EnemyHillColor));
	//} while (r+g+b < 250); // Racist code (avoid blacks)

	return color;
}

QPointF toQPf(Vec2 pos) {
	return Zoom*QPointF(pos.x()+.5f, pos.y()+.5f);
}

QPoint toQPi(int x, int y) {
	return Zoom*QPoint(x, y);
}

QPoint toQPi(Vec2 pos) {
	return toQPi(pos.x(), pos.y());
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

void drawSquare(QPainter& painter, QPointF pos, float r, QRgb color, bool outline=true) {
	QRectF rect(pos.x()-r, pos.y()-r, 2*r, 2*r);
	painter.fillRect(rect, color);

	if (outline) {
		painter.setPen(Qt::white);
		painter.drawRect(rect);
	}
}

void drawHill(QPainter& painter, Pos pos) {
	const float antHillRad = 0.4f*Zoom;
	Square& s = g_map->square(pos);
	drawSquare(painter, toQPf(pos), antHillRad, s.hillTeam == OUR_TEAM ? FriendColor : EnemyHillColor, false);
	if (!s.hillAlive) {
		// Cross out
		painter.setPen(Qt::black);
		painter.drawLine(toQPi(pos.x(), pos.y()  ), toQPi(pos.x()+1, pos.y()+1));
		painter.drawLine(toQPi(pos.x(), pos.y()+1), toQPi(pos.x()+1, pos.y()  ));
	}
}

void DebugWindow::redrawImg() {
	LOG_DEBUG("Rooms::dumpImage");

	Vec2 size = g_map->size();
	m_img = QImage(Zoom*size.x(), Zoom*size.y(), QImage::Format_ARGB32);
	m_img.fill(VoidColor);

	std::map<Room*, QRgb> colorMap;
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
					//m_img.setPixel(x*Zoom+xi, y*Zoom+yi, ((yi/2+1)%2) ? color : oddColor);
					m_img.setPixel(x*Zoom+xi, y*Zoom+yi, (yi%2) ? color : oddColor);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	{
		QPainter painter(&m_img);
		painter.setPen(Qt::white);


		{
			painter.setOpacity(0.1f); // Grid is vague

			// Draw helpful grid lines:
			for (int a=0; a<2; ++a) {
				for (int i=1; i<size[a]; ++i) {
					Pos start, end;
					start[a] = end[a] = i;
					start[1-a] = 0;
					end[1-a] = size[1-a];
					painter.drawLine(toQPi(start), toQPi(end));
				}
			}

			painter.setOpacity(1);
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////

		{
			painter.setOpacity(0.125f); // Room grpah info is drawn vaguely.

			// Add graph info
			std::map<Room*, QPointF> centers;
			ITC(RoomList, rit, rooms)
				centers[*rit] = toQPf((*rit)->centerPos());

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

			painter.setOpacity(1);
		}

		//////////////////////////////////////////////////
		// Draw ants
		const float antRad     = 0.35f*Zoom;
		const float foodRad    = 0.4f*Zoom;

		const EnemySet& enemies = g_tracker->getEnemies();
		ITC(EnemySet, eit, enemies)
			drawSquare(painter, toQPf(eit->pos), antRad, EnemyColor);

		const AntSet& ants = g_tracker->getAnts();
		ITC(AntSet, ait, ants) {
			Ant* ant = *ait;
			QPointF pos = toQPf(ant->pos());
			drawSquare(painter, pos, antRad, FriendColor);

			if (ant->state() != Ant::STATE_NONE) {
				const Path& path = ant->path();
				if (path.isValid()) {
					float destRad = 0.15f * Zoom;
					painter.setPen(Qt::black);

					/*
					QPointF dest = toQPf(path.dest());
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
						QPointF wpPos = toQPf(wayPoints[i].pos);

						drawWrappedLine(painter, p, wpPos);
						painter.drawEllipse(wpPos, destRad, destRad);

						p = wpPos;
					}
					QPointF wpPos = toQPf(path.dest());
					drawWrappedLine(painter, p, wpPos);
					painter.drawEllipse(wpPos, destRad, destRad);
					/**/
				}
			}

			if (ant->expectedPos() != ant->pos()) {
				// Draw where ant is expected to go
				painter.setPen(Qt::white);
				drawWrappedLine(painter, pos, toQPf(ant->expectedPos()));
			}
		}

		// Draw food
		const PosSet& food = g_tracker->getFood();
		ITC(PosSet, pit, food) {
			QPointF pos = toQPf(*pit);
			/*
			painter.setPen(FoodColor);
			painter.drawEllipse(pos, 1, 1);
			painter.drawEllipse(pos, 2, 2);
			painter.drawEllipse(pos, 3, 3);
			/*/
			drawSquare(painter, pos, foodRad, FoodColor, false);
			/**/
		}

		// Draw hills
		const EnemyHillSet& enemyHills = g_tracker->enemyHills();
		if (!enemyHills.empty()) {
			LOG_DEBUG("Drawing " << enemyHills.size() << " enemy hills!");
			ITC(EnemyHillSet, hit, enemyHills) {
				drawHill(painter, hit->pos);
			}
		}
		const PosSet& ourHills = g_tracker->ourHills();
		ITC(PosSet, hit, ourHills) {
			drawHill(painter, *hit);
		}
	}

	/////////////////////////

	char nameBuf[100] = {};
	if (g_state->gameover)
		strcpy(nameBuf, "rooms__endgame.png");
	else
		sprintf(nameBuf, "rooms_%04d.png", g_tracker->turn());
	LOG_DEBUG("Dumping rooms to " << nameBuf);
	m_img.save(nameBuf);

	/////////////////////////

	m_dirty = false;
}

#endif
