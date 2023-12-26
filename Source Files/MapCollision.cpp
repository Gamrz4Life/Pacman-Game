#include <array>
#include <cmath>
#include <iostream>
#include "Headers/Global.hpp"
#include "Headers/MapCollision.hpp"

bool map_collision(bool i_collect_pellets, bool i_use_door, short i_x, short i_y, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map)
{
	bool output = 0;

	//Getting the exact position.
	float cell_x = i_x / static_cast<float>(CELL_SIZE);
	float cell_y = i_y / static_cast<float>(CELL_SIZE);

	//A ghost/Pacman can intersect 4 cells at most.
	for (unsigned char a = 0; a < 4; a++)
	{
		short x = 0;
		short y = 0;

		switch (a)
		{
			case 0: //Top left cell
			{
				x = static_cast<short>(floor(cell_x));
				y = static_cast<short>(floor(cell_y));

				break;
			}
			case 1: //Top right cell
			{
				x = static_cast<short>(ceil(cell_x));
				y = static_cast<short>(floor(cell_y));

				break;
			}
			case 2: //Bottom left cell
			{
				x = static_cast<short>(floor(cell_x));
				y = static_cast<short>(ceil(cell_y));

				break;
			}
			case 3: //Bottom right cell
			{
				x = static_cast<short>(ceil(cell_x));
				y = static_cast<short>(ceil(cell_y));
			}
		}

		//Making sure that the position is inside the map.
		if (0 <= x && 0 <= y && MAP_HEIGHT > y && MAP_WIDTH > x)
		{
			if (0 == i_collect_pellets) //Checking Wall and door interactions/collisions
			{
				if (Cell::Wall == i_map[x][y])
				{
					output = 1;
				}
				else if (0 == i_use_door && Cell::Door == i_map[x][y])
				{
					output = 1;
				}
			}
			else //Checking Pellet and Energizer Collisions
			{
				if (Cell::Energizer == i_map[x][y])
				{
					output = 1;
					Score += 10;
					i_map[x][y] = Cell::Empty;
				}
				else if (Cell::Pellet == i_map[x][y])
				{
					Score++;
					i_map[x][y] = Cell::Empty;
				}
			}
		}
	}

	return output;//confirms collision
}

//Returns Score
int getscore() {
	return Score;
}

//Resets Score and Checks if High Score Broken
void resetscore() {
	highscorein.open("HighScore.txt");
	highscorein >> Highscore;
	highscorein.close();
	HighScore = stoi(Highscore);
	if (HighScore < Score) {
		HighScore = Score;
		Highscore = std::to_string(HighScore);
		highscoreout.open("HighScore.txt");
		highscoreout << Highscore;
		highscoreout.close();
	}
	Score = 0;
}

//Reads and Returns High Score
int gethighscore() {
	highscorein.open("HighScore.txt");
	highscorein >> Highscore;
	highscorein.close();
	HighScore = stoi(Highscore);
	return HighScore;
}