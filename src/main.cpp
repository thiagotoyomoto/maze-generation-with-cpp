#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>
#include <memory>
#include <stack>
#include <string.h>
#include <thread>

#include "../include/CellState.hpp"
#include "../include/Direction.hpp"

using namespace std::chrono_literals;

const sf::Vector2u MAZE_SIZE(20, 20);
const unsigned int MAZE_WALL_SIZE =
    8; // Recomenda-se utilizar somente números pares
const unsigned int HALF_MAZE_WALL_SIZE = MAZE_WALL_SIZE / 2;
const unsigned int MAZE_CELL_SIZE = 16;
const unsigned int LINEAR_MAZE_SIZE = MAZE_SIZE.x * MAZE_SIZE.y;

sf::RectangleShape createRectangle(const int x, const int y, const int w,
                                   const int h) {
  auto rectangle = sf::RectangleShape(sf::Vector2f(w, h));
  rectangle.setPosition(x, y);
  rectangle.setFillColor(sf::Color(255, 255, 255));
  return rectangle;
}

CellState directionToCellState(Direction direction) {
  switch (direction) {
  case Direction::NORTH:
    return CellState::NORTH_OPEN;
  case Direction::WEST:
    return CellState::WEST_OPEN;
  case Direction::SOUTH:
    return CellState::SOUTH_OPEN;
  case Direction::EAST:
    return CellState::EAST_OPEN;
  }
}

int positionToIndex(const int x, const int y) { return x + y * MAZE_SIZE.x; }

int main() {
  const sf::VideoMode VIDEO_MODE(MAZE_WALL_SIZE + MAZE_SIZE.x * MAZE_CELL_SIZE +
                                     MAZE_SIZE.x * MAZE_WALL_SIZE,
                                 MAZE_WALL_SIZE + MAZE_SIZE.y * MAZE_CELL_SIZE +
                                     MAZE_SIZE.y * MAZE_WALL_SIZE);

  unsigned int mazeVisitedCells = 0;
  std::unique_ptr<int> mazeCells(new int[LINEAR_MAZE_SIZE]);
  std::stack<std::pair<int, int>> mazeStackOfVisitedCells;

  sf::RenderWindow window(VIDEO_MODE, "Maze Generator");

  for (unsigned int j = 0; j < MAZE_SIZE.y; ++j) {
    for (unsigned int i = 0; i < MAZE_SIZE.x; ++i) {
      mazeCells.get()[positionToIndex(i, j)] =
          static_cast<int>(CellState::UNVISITED);
    }
  }

  mazeCells.get()[0] |= static_cast<int>(CellState::VISITED);
  mazeStackOfVisitedCells.emplace(0, 0);

  std::srand(std::time(0));

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    if (!mazeStackOfVisitedCells.empty()) {
      const auto &currentCell = mazeStackOfVisitedCells.top();
      const auto &currentX = currentCell.first;
      const auto &currentY = currentCell.second;
      const auto currentIndex = currentX + currentY * MAZE_SIZE.x;

      const auto currentOffsetIndex = [&, currentCell, MAZE_SIZE](int x,
                                                                  int y) {
        return (currentCell.first + x) + (currentCell.second + y) * MAZE_SIZE.x;
      };

      unsigned char possibleDirectionsCounter = 0;
      Direction possibleDirections[static_cast<int>(Direction::TOTAL)];

      if (currentY > 0) {
        if (!(mazeCells.get()[currentOffsetIndex(0, -1)] &
              static_cast<int>(CellState::VISITED))) {
          possibleDirections[possibleDirectionsCounter++] = Direction::NORTH;
        }
      }
      if (currentX > 0) {
        if (!(mazeCells.get()[currentOffsetIndex(-1, 0)] &
              static_cast<int>(CellState::VISITED))) {
          possibleDirections[possibleDirectionsCounter++] = Direction::WEST;
        }
      }
      if (currentY < MAZE_SIZE.y - 1) {
        if (!(mazeCells.get()[currentOffsetIndex(0, 1)] &
              static_cast<int>(CellState::VISITED))) {
          possibleDirections[possibleDirectionsCounter++] = Direction::SOUTH;
        }
      }
      if (currentX < MAZE_SIZE.x - 1) {
        if (!(mazeCells.get()[currentOffsetIndex(1, 0)] &
              static_cast<int>(CellState::VISITED))) {
          possibleDirections[possibleDirectionsCounter++] = Direction::EAST;
        }
      }

      if (possibleDirectionsCounter == 0) {
        mazeStackOfVisitedCells.pop();
        continue;
      }

      const auto newDirection =
          possibleDirections[std::rand() % possibleDirectionsCounter];

      const auto oppositeDirection = ([&, newDirection]() {
        switch (newDirection) {
        case Direction::NORTH:
          return Direction::SOUTH;
        case Direction::WEST:
          return Direction::EAST;
        case Direction::SOUTH:
          return Direction::NORTH;
        case Direction::EAST:
          return Direction::WEST;
        }
      })();

      mazeCells.get()[currentIndex] |=
          static_cast<int>(directionToCellState(newDirection));

      mazeStackOfVisitedCells.push(([&, currentX, currentY, newDirection]() {
        switch (newDirection) {
        case Direction::NORTH:
          return std::pair<int, int>(currentX, currentY - 1);
        case Direction::WEST:
          return std::pair<int, int>(currentX - 1, currentY);
        case Direction::SOUTH:
          return std::pair<int, int>(currentX, currentY + 1);
        case Direction::EAST:
          return std::pair<int, int>(currentX + 1, currentY);
        }
      })());

      const auto &newCell = mazeStackOfVisitedCells.top();
      const auto newIndex = newCell.first + newCell.second * MAZE_SIZE.x;
      mazeCells.get()[newIndex] |=
          static_cast<int>(CellState::VISITED) |
          static_cast<int>(directionToCellState(oppositeDirection));
    }

    window.clear();

    for (unsigned int j = 0; j < MAZE_SIZE.y; ++j) {
      for (unsigned int i = 0; i < MAZE_SIZE.x; ++i) {
        const auto index = i + (j * MAZE_SIZE.x);
        const int x = MAZE_WALL_SIZE + i * MAZE_CELL_SIZE + i * MAZE_WALL_SIZE;
        const int y = MAZE_WALL_SIZE + j * MAZE_CELL_SIZE + j * MAZE_WALL_SIZE;
        if (mazeCells.get()[index] & static_cast<int>(CellState::VISITED)) {
          auto square = createRectangle(x, y, MAZE_CELL_SIZE, MAZE_CELL_SIZE);
          window.draw(square);

          if (mazeCells.get()[index] &
              static_cast<int>(CellState::NORTH_OPEN)) {
            auto rectangle =
                createRectangle(x, y - HALF_MAZE_WALL_SIZE, MAZE_CELL_SIZE,
                                HALF_MAZE_WALL_SIZE);
            window.draw(rectangle);
          }
          if (mazeCells.get()[index] & static_cast<int>(CellState::WEST_OPEN)) {
            auto rectangle =
                createRectangle(x - HALF_MAZE_WALL_SIZE, y, HALF_MAZE_WALL_SIZE,
                                MAZE_CELL_SIZE);
            window.draw(rectangle);
          }
          if (mazeCells.get()[index] &
              static_cast<int>(CellState::SOUTH_OPEN)) {
            auto rectangle = createRectangle(
                x, y + MAZE_CELL_SIZE, MAZE_CELL_SIZE, HALF_MAZE_WALL_SIZE);
            window.draw(rectangle);
          }
          if (mazeCells.get()[index] & static_cast<int>(CellState::EAST_OPEN)) {
            auto rectangle = createRectangle(
                x + MAZE_CELL_SIZE, y, HALF_MAZE_WALL_SIZE, MAZE_CELL_SIZE);
            window.draw(rectangle);
          }
        }
      }
    }
    window.display();

    std::this_thread::sleep_for(10ms);
  }

  return 0;
}