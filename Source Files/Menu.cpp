#include "Headers/Menu.hpp"
#include <iostream>

Menu::Menu(float width, float height, std::string button1, std::string button2, std::string button3, std::string button4,std::string button5, int Max_Items)
{
	MaxItems = Max_Items;//Assigning Total Number of Buttons for each Menu
	if (!font.loadFromFile("Resources\\Font\\arial.ttf"))
	{
		std::cout << "Erorr";//Error Check
	}

	menu[0].setFont(font);
	menu[0].setFillColor(sf::Color::Blue);//Initally First Button is highlighted as Blue
	menu[0].setString(button1);
	menu[0].setPosition(sf::Vector2f(width / 2, height / (Max_Items+1)*1));

	menu[1].setFont(font);
	menu[1].setFillColor(sf::Color::Yellow);
	menu[1].setString(button2);
	menu[1].setPosition(sf::Vector2f(width / 2, height / (Max_Items + 1) * 2));

	menu[2].setFont(font);
	menu[2].setFillColor(sf::Color::Yellow);
	menu[2].setString(button3);
	menu[2].setPosition(sf::Vector2f(width / 2, height / (Max_Items + 1) * 3));

	menu[3].setFont(font);
	menu[3].setFillColor(sf::Color::Yellow);
	menu[3].setString(button4);
	menu[3].setPosition(sf::Vector2f(width / 2, height / (Max_Items + 1) * 4));

	menu[4].setFont(font);
	menu[4].setFillColor(sf::Color::Yellow);
	menu[4].setString(button5);
	menu[4].setPosition(sf::Vector2f(width / 2, height / (Max_Items + 1) * 5));

	selectedItemIndex = 0;//Selects First Button By Default
}




Menu::~Menu()
{}

void Menu::draw(sf::RenderWindow & window)
{
	for (int i = 0; i < MaxItems; i++)
	{
		window.draw(menu[i]);
	}
}

//Move Up The Menu
void Menu::MoveUp()
{
	if (selectedItemIndex - 1 >= 0)
	{
		menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
		selectedItemIndex--;
		menu[selectedItemIndex].setFillColor(sf::Color::Blue);
	}
	else {
		selectedItemIndex = 0;
		menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
		selectedItemIndex = MaxItems-1;
		menu[selectedItemIndex].setFillColor(sf::Color::Blue);
	}
}
//Move Down the Menu
void Menu::MoveDown()
{
	if (selectedItemIndex + 1 < MaxItems)
	{
		menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
		selectedItemIndex++;
		menu[selectedItemIndex].setFillColor(sf::Color::Blue);
	}
	else {
		selectedItemIndex = MaxItems-1;
		menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
		selectedItemIndex = 0;
		menu[selectedItemIndex].setFillColor(sf::Color::Blue);
	}
}