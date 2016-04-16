#pragma once

#include <exception>  
#include <vector>
#include <stdexcept>

enum PointState {
	EMPTY, FIRST_PLAYER, SECOND_PLAYER, INNER_FIRST, INNER_SECOND, INNER
};

struct Point {
	int x, y;

	Point(int x_, int y_) : x(x_), y(y_) {};
	Point() {};

	bool operator== (const Point& point) const {
		return point.x == x && point.y == y;
	}
};

struct Edge {
	bool isFirstPlayer;
	Point start, end;

	Edge() {};
	Edge(bool first, Point start, Point end) {
		this->start = start;
		this->end = end;
	};
};

const static std::vector<int> dx = { 1, 1, 0, -1, -1, -1, 0, 1 };
const static std::vector<int> dy = { 0, 1, 1, 1, 0, -1, -1, -1 };


enum Direction {
	RIGHT, RIGHT_DOWN, DOWN, DOWN_LEFT, LEFT, LEFT_UP, UP, UP_RIGHT
};

class Game {
private:
	std::vector < std::vector<PointState> > points;
	int widthCount;
	int heightCount;
	std::vector< Edge > edges;

	int firstResult = 0;
	int secondResult = 0;

	Direction nextDir(Direction dir) {
		if (dir != UP_RIGHT) {
			return (Direction)(dir + 1);
		}
		else {
			return RIGHT;
		}
	}

	Direction prevDir(Direction dir) {
		if (dir != RIGHT) {
			return (Direction)(dir - 1);
		}
		else {
			return UP_RIGHT;
		}
	}

	bool isInField(int x, int y) {
		return !(x < 0 || x > widthCount || y < 0 || y > heightCount);
	}

	bool isInField(Point point) {
		return isInField(point.x, point.y);
	}

	std::vector<Direction> getAvailableDirection(Direction lastDirection) {
		std::vector<Direction> result;

		Direction startDirection = prevDir(prevDir(prevDir(lastDirection)));
		Direction finishDirection = prevDir(startDirection);
		for (Direction dir = startDirection; dir != finishDirection; dir = nextDir(dir)) {
			result.push_back(dir);
		}

		return result;
	}

	PointState getState(Point point) {
		return points[point.x][point.y];
	}

	bool tryConsrtuctPath(Point vertex, Point startVertex, 
						  Direction lastDirection, 
						  std::vector<Point>& path,
						  std::vector < std::vector<bool> >& inPath) {

		path.push_back(vertex);

		for (Direction dir : getAvailableDirection(lastDirection)) {
			Point nextPoint;
			nextPoint.x = vertex.x + dx[dir];
			nextPoint.y = vertex.y + dy[dir];

			if (!isInField(nextPoint)) {
				continue;
			}

			if (getState(vertex) == getState(nextPoint) && !inPath[nextPoint.x][nextPoint.y]) {
				if (nextPoint == startVertex) {
					path.push_back(startVertex);
					return true;
				}

				if (tryConsrtuctPath(nextPoint, startVertex, dir, path, inPath)) {
					return true;
				}
			}
		}

		path.pop_back();
		return false;
	}

	bool isIntersect(int x, int y, Point start, Point finish) {
		if (x > start.x || x > finish.x || start.y == finish.y) {
			return false;
		}

		return (y >= start.y && y <= finish.y) || (y <= start.y && y >= finish.y);
	}

	bool isItInner(int x, int y) {
		bool first;
		if (points[x][y] == FIRST_PLAYER) {
			first = true;
		} else {
			first = false;
		}

		int intersectCount = 0;

		for (Edge e : edges) {
			if (e.isFirstPlayer ^ first && isIntersect(x, y, e.start, e.end)) {
				intersectCount += 1;
			}
		}

		return intersectCount % 2 == 1;
	}

public:
	Game() {}

	Game(int widthCount_, int heightCount_) : widthCount(widthCount_), heightCount(heightCount_) 
	{
		points = std::vector<std::vector<PointState>> (widthCount_, std::vector<PointState>(heightCount_, EMPTY));
	}

	PointState getPointState(int x, int y) const {
		return points[x][y];
	}

	bool markPoint(int x, int y, bool firstPlayer) {
		if (!isInField(x, y)) {
			throw std::invalid_argument("Illegal arguments");
		}

		if (points[x][y] == EMPTY) {
			if (firstPlayer) {
				points[x][y] = FIRST_PLAYER;
			} else {
				points[x][y] = SECOND_PLAYER;
			}
			return true;
		} else {
			return false;
		}
	}

	void buildGame() {
		edges = std::vector< Edge >();
		std::vector < std::vector<bool> > inPath = std::vector < std::vector<bool> >(widthCount,
			std::vector<bool>(heightCount, false));

		for (int x = 0; x < widthCount; ++x) {
			for (int y = 0; y < heightCount; ++y) {
				std::vector< Point > newPath;

				if (points[x][y] == FIRST_PLAYER || points[x][y] == SECOND_PLAYER) {
					bool wasPathFound = tryConsrtuctPath(Point(x, y), Point(x, y), RIGHT, newPath, inPath);

					if (wasPathFound) {
						for (int i = 0; i < newPath.size() - 1; ++i) {
							Point point = newPath[i];
							inPath[point.x][point.y] = true;

							bool first = points[x][y] == FIRST_PLAYER;
							edges.push_back(Edge(first, newPath[i], newPath[i + 1]));
						}
					}
				}
			}
		}
	}

	void updateInner() {
		for (int x = 0; x < widthCount; ++x) {
			for (int y = 0; y < heightCount; ++y) {
				if (points[x][y] == FIRST_PLAYER || points[x][y] == SECOND_PLAYER || points[x][y] == EMPTY) {
					if (isItInner(x, y)) {
						if (points[x][y] == FIRST_PLAYER) {
							secondResult += 1;
							points[x][y] = INNER_FIRST;
						}
						else if (points[x][y] == SECOND_PLAYER) {
							firstResult += 1;
							points[x][y] = INNER_SECOND;
						}
						else {
							points[x][y] = INNER;
						}

					}
				}
			}
		}
	}
};
