#pragma once

struct Vector2Int {
public:
	int x = 0, y = 0;

	inline Vector2Int operator+(Vector2Int other) { return Vector2Int{ x + other.x, y + other.y }; }
	inline Vector2Int operator-(Vector2Int other) { return Vector2Int{ x - other.x, y - other.y }; }
};