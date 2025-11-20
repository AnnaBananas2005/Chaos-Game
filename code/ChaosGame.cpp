#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System/FileInputStream.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>

//Make the code easier to type with "using namespace"
using namespace sf;
using namespace std;

int main()
{
	// Create a video mode object
	VideoMode vm(1920, 1080);
	// Create and open a window for the game
	RenderWindow window(vm, "Chaos Game!!", Style::Default);

  	//Done by Anna
	srand(static_cast<unsigned int>(time(0))); 

	//https://fonts.google.com/specimen/Roboto
	Font newFont;
	newFont.loadFromFile("fonts/Roboto-Black.ttf"); // Changed to use a relative path instead of a static path

	Text newText("Please press three dots", newFont, 50);
	newText.setFillColor(Color::White);
	newText.setStyle(Text::Bold);
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^

	vector<Vector2f> vertices;
	vector<Vector2f> points;

	while (window.isOpen())
	{
		/*
		****************************************
		Handle the players input
		****************************************
		*/
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				// Quit the game when the window is closed
				window.close();
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					// prints user input to the console
					std::cout << "the left button was pressed" << std::endl;
					std::cout << "mouse x: " << event.mouseButton.x << std::endl;
					std::cout << "mouse y: " << event.mouseButton.y << std::endl;

					// store the first 3 click locations 
					if (vertices.size() < 3)
					{
						vertices.push_back(Vector2f(event.mouseButton.x, event.mouseButton.y));
					}
					// after that it stores the first click as a point and then begins the chaos math
					else if (points.size() == 0)
					{
						//Done by Ted
						points.push_back(Vector2f(event.mouseButton.x, event.mouseButton.y));
					}
				}
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
		/*
		****************************************
		Update
		****************************************
		*/

		if (points.size() > 0)
		{
			///Chaos Math done by Anna
			Vector2f pt1 = vertices[0];
			Vector2f pt2 = vertices[1];
			Vector2f pt3 = vertices[2];

			int newRand = rand() % 3;
			Vector2f vertex = vertices[newRand];

			Vector2f lastPt = points.back();
			Vector2f newPt;

			newPt.x = (lastPt.x + vertex.x) / 2;
			newPt.y = (lastPt.y + vertex.y) / 2;

			points.push_back(newPt);

		}

		/*
		****************************************
		Draw
		****************************************
		*/

		///Drawing Code done by Ted
		// Clears the framebuffer before drawing the next frame
		window.clear();
		// For each of the corners of the triangles it draws a 10x10 rectangle(which makes it a square)
		for (int i = 0; i < vertices.size(); i++)
		{
			RectangleShape rect(Vector2f(10, 10));	//10x10 rectangle
			rect.setPosition(Vector2f(vertices[i].x, vertices[i].y)); //puts the rectangle at that point
			rect.setFillColor(Color::Blue); //blue :3
			window.draw(rect); //draw it
		}		
		// for each of the points it draws a circle there
		for (int i = 0; i < points.size(); i++)
		{
			CircleShape point(2.0f); //radius of 2
			point.setPosition(points[i]); //sets position there
			point.setFillColor(Color::Blue); // blue :3
			window.draw(point); //draw it
		}
		// draw the text as well
		window.draw(newText);
		//update the display with the new frame buffer, effectively pushing the frame we just drew to the display 
		window.display();
	}
	return 0;
}
