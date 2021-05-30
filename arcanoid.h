#pragma once

#include <vector>

namespace Arcanoid {

	//=============================================================================================

	enum EntityClass : short unsigned int {
		Block,
		Ball,
		Board,
		Bonus,
		Decoration
	};

	struct EntityData{
		EntityClass Class;
		short unsigned int Subclass;
	};

	//=============================================================================================



	//=============================================================================================

	class Arcanoid
	{
	private:
		const int width, height;
		std::vector<EntityData>* gameField;

	public:
		Arcanoid(int width, int height);
		~Arcanoid();

		inline int getWidth () const { return width; }
		inline int getHeight() const { return height; }
		inline const std::vector<const EntityData>* getGameField() const { return reinterpret_cast<const std::vector<const EntityData>*>(gameField); }

		void shiftBoard(int delta);
		void tick(long deltaTime);
	};
}
