#pragma once
#include "SFML/Graphics.hpp"
#include  <string>

class Menu
{
public:
	Menu(float width, float height, std::string button1, std::string button2, std::string button3, std::string button4, int Max_Items);
	~Menu();

	void draw(sf::RenderWindow& window);
	void MoveUp();
	void MoveDown();
	int GetPressedItem() {
		return selectedItemIndex;
	}
private:
	int MaxItems;
	int selectedItemIndex;
	std::string button1;
	std::string button2;
	std::string button3;
	std::string button4;
	sf::Font font;
	sf::Text menu[10];

};

