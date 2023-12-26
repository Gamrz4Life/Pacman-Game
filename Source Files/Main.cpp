#include <array>
#include <vector>
#include <chrono>
#include <ctime>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "Headers/Global.hpp"
#include "Headers/DrawText.hpp"
#include "Headers/Pacman.hpp"
#include "Headers/Ghost.hpp"
#include "Headers/GhostManager.hpp"
#include "Headers/ConvertSketch.hpp"
#include "Headers/DrawMap.hpp"
#include "Headers/MapCollision.hpp"
#include "Headers/Menu.hpp"
void mainmenu(sf::RenderWindow& window, std::array<std::string, MAP_HEIGHT> map_sketch);
void diffmenu(sf::RenderWindow& window, std::array<std::string, MAP_HEIGHT> map_sketch);
void changediff(float multiplier);
void newmenuwindow(sf::RenderWindow& oldwindow, std::array<std::string, MAP_HEIGHT> map_sketch);
void mapmenu(sf::RenderWindow& window, std::array<std::string, MAP_HEIGHT> map_sketch);
void colormenu(sf::RenderWindow& window, std::array<std::string, MAP_HEIGHT> map_sketch);
void changecolor(std::string Color);
std::array<std::string, MAP_HEIGHT> changemap( int mapselect, std::array<std::string, MAP_HEIGHT> map_sketch);
GhostManager ghost_manager;
Pacman pacman;

//The Game Running Function
int play(sf::RenderWindow& menuwindow, std::array<std::string, MAP_HEIGHT> map_sketch)
{
	pacman.reset();
	menuwindow.close();
	//Is the game won?
	bool game_won = 0;

	//Used to make the game framerate-independent.
	unsigned lag = 0;

	unsigned char level = 0;
	resetscore();
	//Similar to lag, used to make the game framerate-independent.
	std::chrono::time_point<std::chrono::steady_clock> previous_time;

	//It's not exactly like the map from the original Pac-Man game, but it's close enough.
	
	std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> map{};

	//Initial ghost positions.
	std::array<Position, 4> ghost_positions;

	//SFML thing. Stores events, I think.
	sf::Event event;

	sf::RenderWindow window(sf::VideoMode(CELL_SIZE * MAP_WIDTH * SCREEN_RESIZE, (FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT) * SCREEN_RESIZE), "Pac-Man", sf::Style::Fullscreen);

	window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * MAP_WIDTH, FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT)));


	//Menu menu(window.getSize().x, window.getSize().y);
	//Generating a random seed.
	srand(static_cast<unsigned>(time(0)));

	map = convert_sketch(map_sketch, ghost_positions, pacman);

	ghost_manager.reset(level, ghost_positions);

	//Get the current time and store it in a variable.
	previous_time = std::chrono::steady_clock::now();

	while (1 == window.isOpen())
	{
		//Here we're calculating the lag.
		unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();

		lag += delta_time;

		previous_time += std::chrono::microseconds(delta_time);

		//While the lag exceeds the maximum allowed frame duration.
		while (FRAME_DURATION <= lag)
		{
			//We decrease the lag.
			lag -= FRAME_DURATION;

			while (1 == window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
				{
					//Making sure the player can close the window.
					window.close();
					break;
				}
				case sf::Event::KeyReleased:
					switch (event.key.code) {
					case sf::Keyboard::Escape:
						newmenuwindow(window, map_sketch);
						break;
					}
				}
			}

			if (0 == game_won && 0 == pacman.get_dead())
			{
				game_won = 1;

				pacman.update(level, map);

				ghost_manager.update(level, map, pacman);

				//We're checking every cell in the map.
				for (const std::array<Cell, MAP_HEIGHT>& column : map)
				{
					for (const Cell& cell : column)
					{
						if (Cell::Pellet == cell) //And if at least one of them has a pellet.
						{
							game_won = 0; //The game is not yet won.

							break;
						}
					}

					if (0 == game_won)
					{
						break;
					}
				}

				if (1 == game_won)
				{
					pacman.set_animation_timer(0);
				}
			}
			else if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) //Restarting the game.
			{
				game_won = 0;

				if (1 == pacman.get_dead())
				{
					resetscore();
					level = 0;
				}
				else
				{
					//After each win we reduce the duration of attack waves and energizers.
					level++;
				}

				map = convert_sketch(map_sketch, ghost_positions, pacman);

				ghost_manager.reset(level, ghost_positions);

				pacman.reset();
			}

			//I don't think anything needs to be explained here.
			if (FRAME_DURATION > lag)
			{
				window.clear();

				if (0 == game_won && 0 == pacman.get_dead())
				{
					draw_map(map, window);

					ghost_manager.draw(GHOST_FLASH_START >= pacman.get_energizer_timer(), window);

					draw_text(0, 0, CELL_SIZE * MAP_HEIGHT, "Level: " + std::to_string(1 + level), window);
					draw_text(0, 100, CELL_SIZE * MAP_HEIGHT, "Score: " + std::to_string(getscore()), window);
					draw_text(0, 200, CELL_SIZE * MAP_HEIGHT, "High Score: " + std::to_string(gethighscore()), window);
				}

				pacman.draw(game_won, window);

				if (1 == pacman.get_animation_over())
				{
					if (1 == game_won)
					{
						draw_text(1, 0, 0, "Level " + std::to_string(level + 2), window);
					}
					else
					{
						draw_text(1, 0, 0, "Game Over!", window);
					}
				}

				window.display();
			}
		}
	}

	return 0;
}

int main()
{
	//playing Background Music
	sf::Music music;
	if (!music.openFromFile("Resources\\Audio\\Music.ogg"))
	{
		std::cout << "ERROR" << std::endl;
	}
	music.play();
	music.setLoop(true);

//Initializing the Map
	std::array<std::string, MAP_HEIGHT> map_sketch = {

		" ################### ",
		" #........#........# ",
		" #o##.###.#.###.##o# ",
		" #.................# ",
		" #.##.#.#####.#.##.# ",
		" #....#...#...#....# ",
		" ####.### # ###.#### ",
		"    #.#   0   #.#    ",
		"#####.# ##=## #.#####",
		"     .  #123#  .     ",
		"#####.# ##### #.#####",
		"    #.#       #.#    ",
		" ####.# ##### #.#### ",
		" #........#........# ",
		" #.##.###.#.###.##.# ",
		" #o.#.....P.....#.o# ",
		" ##.#.#.#####.#.#.## ",
		" #....#...#...#....# ",
		" #.######.#.######.# ",
		" #.................# ",
		" ################### "

	

	};


	//Creating Window
	sf::RenderWindow window(sf::VideoMode(600, 480), "Pacman",sf::Style::Fullscreen);



mainmenu(window, map_sketch);


}

//Main Menu Funtion
void mainmenu(sf::RenderWindow& window, std::array<std::string, MAP_HEIGHT> map_sketch){
	Menu menu(window.getSize().x, window.getSize().y, "Start", "Game Speed", "Choose Map", "Change Color", "Exit", 5);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::KeyReleased:
				switch (event.key.code) {
				case sf::Keyboard::Up:
					menu.MoveUp();
					break;

				case sf::Keyboard::Down:
					menu.MoveDown();
					break;

				case sf::Keyboard::Return:
					switch (menu.GetPressedItem()) {
					case 0:
						play(window, map_sketch);
						break;
					case 1:
						window.clear();
						diffmenu(window, map_sketch);
						break;
					case 2:
						mapmenu(window, map_sketch);
						break;
					case 3:
						colormenu(window, map_sketch);
						break;
					case 4:
						window.close();
						break;
					}
				}
				break;
			case sf::Event::Closed:
				window.close();
				break;
			}
		}
		window.clear();
		menu.draw(window);
		window.display();
	}

}

//Game Speed  Menu Function
void diffmenu(sf::RenderWindow& window, std::array<std::string, MAP_HEIGHT> map_sketch) {
	Menu diffmenu(window.getSize().x, window.getSize().y, "Slow", "Normal", "Fast", "Super Fast","", 4);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::KeyReleased:
				switch (event.key.code) {
				case sf::Keyboard::Up:
					diffmenu.MoveUp();
					break;

				case sf::Keyboard::Down:
					diffmenu.MoveDown();
					break;
				case sf::Keyboard::Escape:
					mainmenu(window, map_sketch);
					break;
				case sf::Keyboard::Return:
					switch (diffmenu.GetPressedItem()) {
					case 0:
						changediff(0.5);
						mainmenu(window, map_sketch);
						break;
					case 1:
						changediff(1);
						mainmenu(window, map_sketch);
						break;
					case 2:
						changediff(2);
						mainmenu(window, map_sketch);
						break;
					case 3:
						changediff(4);
						mainmenu(window, map_sketch);
						break;
					}
				}
				break;
			case sf::Event::Closed:
				window.close();
				break;
			}
		}
		window.clear();
		diffmenu.draw(window);
		window.display();
	}

}

//Updates Pacman and Ghost Speed
void changediff(float multiplier) {
	pacman.changespeed(multiplier);
	ghost_manager.changespeedman(multiplier);
}

//Used to Go Back from Game to Main Menu
void newmenuwindow(sf::RenderWindow& oldwindow, std::array<std::string, MAP_HEIGHT> map_sketch){
	oldwindow.close();
	sf::RenderWindow window(sf::VideoMode(600, 480), "Pacman", sf::Style::Fullscreen);
	mainmenu(window, map_sketch);
}

//Map Selection Menu Function
void mapmenu(sf::RenderWindow& window, std::array<std::string, MAP_HEIGHT> map_sketch) {
	Menu mapmenu(window.getSize().x, window.getSize().y, "Map 1", "Map 2", "Map 3", "","", 3);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::KeyReleased:
				switch (event.key.code) {
				case sf::Keyboard::Up:
					mapmenu.MoveUp();
					break;

				case sf::Keyboard::Down:
					mapmenu.MoveDown();
					break;
				case sf::Keyboard::Escape:
					mainmenu(window, map_sketch);
					break;
				case sf::Keyboard::Return:
					switch (mapmenu.GetPressedItem()) {
					case 0:
						map_sketch = changemap(1, map_sketch);
						mainmenu(window, map_sketch);
						break;
					case 1:
						map_sketch = changemap(2, map_sketch);
						mainmenu(window, map_sketch);
						break;
					case 2:
						map_sketch = changemap(3, map_sketch);
						mainmenu(window, map_sketch);
						break;
					}
				}
				break;
			case sf::Event::Closed:
				window.close();
				break;
			}
		}
		window.clear();
		mapmenu.draw(window);
		window.display();
	}

}
//Selects Map
std::array<std::string,MAP_HEIGHT> changemap(int mapselect, std::array<std::string, MAP_HEIGHT> map_sketch){
	switch (mapselect) {
	case 1:
		map_sketch = {
" ################### ",
" #........#........# ",
" #o##.###.#.###.##o# ",
" #.................# ",
" #.##.#.#####.#.##.# ",
" #....#...#...#....# ",
" ####.### # ###.#### ",
"    #.#   0   #.#    ",
"#####.# ##=## #.#####",
"     .  #123#  .     ",
"#####.# ##### #.#####",
"    #.#       #.#    ",
" ####.# ##### #.#### ",
" #........#........# ",
" #.##.###.#.###.##.# ",
" #o.#.....P.....#.o# ",
" ##.#.#.#####.#.#.## ",
" #....#...#...#....# ",
" #.######.#.######.# ",
" #.................# ",
" ################### "

		};
		break;
	case 2:
		map_sketch = {
		" ################### ",
		" #o...............o# ",
		" #.###.###.###.###.# ",
		" #.................# ",
		" #.####.#####.####.# ",
		" #.................# ",
		" #.######.#.######.# ",
		" #.....   0   .....# ",
		" #.##.# ##=## #.##.# ",
		" #   .  #123#  .   # ",
		" #.##.# ##### #.##.# ",
		" #.....       .....# ",
		" #.#.#.#.#.#.#.#.#.# ",
		" #.................# ",
		" #.######.#.######.# ",
		" #........P........# ",
		" #.####.#####.####.# ",
		" #.................# ",
		" #.###.###.###.###.# ",
		" #o...............o# ",
		" ################### "

		};
		break;
	case 3:
		map_sketch = {
		" ################### ",
		" #o...............o# ",
		" #.###############.# ",
		" #.................# ",
		" #.#.#.#######.#.#.# ",
		" #.#.#.........#.#.# ",
		" #.#.#.#.....#.#.#.# ",
		" #.#.#..  0  ..#.#.# ",
		" #.#.#  ##=##  #.#.# ",
		" # # #  #123#  # # # ",
		" #.#.#  #####  #.#.# ",
		" #.#.#..     ..#.#.# ",
		" #.#.#.#..P..#.#.#.# ",
		" #.#.#.#..#..#.#.#.# ",
		" #.#.#.#.###.#.#.#.# ",
		" #.#.#....#....#.#.# ",
		" #.#.#.#######.#.#.# ",
		" #.................# ",
		" #.###############.# ",
		" #o...............o# ",
		" ################### "
		};
		break;
	}



	return map_sketch;
}
//Color Menu Function
void colormenu(sf::RenderWindow& window, std::array<std::string, MAP_HEIGHT> map_sketch) {
	Menu colormenu(window.getSize().x, window.getSize().y, "Yellow", "Blue", "Green", "Orange", "Red", 5);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::KeyReleased:
				switch (event.key.code) {
				case sf::Keyboard::Up:
					colormenu.MoveUp();
					break;

				case sf::Keyboard::Down:
					colormenu.MoveDown();
					break;
				case sf::Keyboard::Escape:
					mainmenu(window, map_sketch);
					break;
				case sf::Keyboard::Return:
					switch (colormenu.GetPressedItem()) {
					case 0:
						pacman.changeColor("Yellow");
						mainmenu(window, map_sketch);
						break;
					case 1:
						pacman.changeColor("Blue");
						mainmenu(window, map_sketch);
						break;
					case 2:
						pacman.changeColor("Green");
						mainmenu(window, map_sketch);
						break;
					case 3:
						pacman.changeColor("Orange");
						play(window, map_sketch);
						break;
					case 4:
						pacman.changeColor("Red");
						mainmenu(window, map_sketch);
						break;
					}
				}
				break;
			case sf::Event::Closed:
				window.close();
				break;
			}
		}
		window.clear();
		colormenu.draw(window);
		window.display();
	}

}
//Updates Pacman's Color
void changecolor(std::string Color){}