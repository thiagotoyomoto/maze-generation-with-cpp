#include "../include/Maze.hpp"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>

int main()
{
  Maze maze(25, 25);

  sf::RenderWindow window(sf::VideoMode(600, 600), "Maze Generator");

  std::srand(std::time(0));

  maze.setPosition(10, 10);
  maze.generate();

  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear(sf::Color(186, 221, 252));
    window.draw(maze);
    window.display();
  }
}