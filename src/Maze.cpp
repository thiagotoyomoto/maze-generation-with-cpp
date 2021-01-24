#include "../include/Maze.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <cstdlib>
#include <iostream>

// Maze::Cell::State ===========================================================
const u_int8_t Maze::Cell::State::UNVISITED = 0x00;
const u_int8_t Maze::Cell::State::NORTH = 0x01;
const u_int8_t Maze::Cell::State::WEST = 0x02;
const u_int8_t Maze::Cell::State::SOUTH = 0x04;
const u_int8_t Maze::Cell::State::EAST = 0x08;
const u_int8_t Maze::Cell::State::VISITED = 0x10;
// =============================================================================

// Maze::Cell::Direction =======================================================
const u_int8_t Maze::Cell::Direction::NORTH = 0;
const u_int8_t Maze::Cell::Direction::WEST = 1;
const u_int8_t Maze::Cell::Direction::SOUTH = 2;
const u_int8_t Maze::Cell::Direction::EAST = 3;
const u_int8_t Maze::Cell::Direction::TOTAL = 4;
const u_int8_t Maze::Cell::Direction::All[4] = {NORTH, WEST, SOUTH, EAST};

const sf::Vector2i Maze::Cell::Direction::Offset[4] = {
    sf::Vector2i(0, -1), sf::Vector2i(-1, 0), sf::Vector2i(0, 1),
    sf::Vector2i(1, 0)};

const u_int8_t Maze::Cell::Direction::Opposite[4] = {SOUTH, EAST, NORTH, WEST};

const u_int8_t Maze::Cell::Direction::ToState[4] = {
    State::NORTH, State::WEST, State::SOUTH, State::EAST};
// =============================================================================

// Maze::Cell::Config ==========================================================
Maze::Cell::Config::Config(const sf::Vector2f &size, const sf::Color &color)
    : m_size(size), m_color(color) {}

inline sf::Vector2f Maze::Cell::Config::getSize() const { return m_size; }
inline sf::Color Maze::Cell::Config::getColor() const { return m_color; }
// =============================================================================

// Maze::Wall::Config ==========================================================
Maze::Wall::Config::Config(const float size, const sf::Color &color)
    : m_size(size), m_halfSize(size / 2), m_color(color) {}

inline float Maze::Wall::Config::getSize() const { return m_size; }
inline float Maze::Wall::Config::getHalfSize() const { return m_halfSize; }
inline sf::Color Maze::Wall::Config::getColor() const { return m_color; }
// =============================================================================

//
Maze::Config::Config(Cell::Config cell, Wall::Config wall) : m_cell(cell), m_wall(wall) {
  
}

Maze::Cell::Config Maze::Config::getCell() const {
  return m_cell;
}

Maze::Wall::Config Maze::Config::getWall() const {
  return m_wall;
}
// =============================================================================

// Maze ========================================================================
Maze::Maze(const unsigned int width, const unsigned int height) :
  m_config(Cell::Config(sf::Vector2f(16, 16), sf::Color(255, 255, 255)), Wall::Config(4, sf::Color(0, 0, 0))),
  m_position(0, 0),
  m_size(width, height),
  m_sizeInPixels(
    m_size.x * m_config.getCell().getSize().x + (m_size.x + 1) * m_config.getWall().getSize(),
    m_size.y * m_config.getCell().getSize().y + (m_size.y + 1) * m_config.getWall().getSize()
  ),
  m_linearSize(m_size.x * m_size.y),
  m_cells(std::make_unique<int[]>(m_linearSize)),
  m_bgRectangle(m_sizeInPixels),
  m_rectangles(std::make_unique<sf::RectangleShape[]>(5))
{
  std::fill(m_cells.get(), m_cells.get() + m_linearSize, Cell::State::UNVISITED);

  const sf::Vector2f sizes[3] = {
      m_config.getCell().getSize(),
      sf::Vector2f(m_config.getCell().getSize().x, m_config.getWall().getHalfSize()),
      sf::Vector2f(m_config.getWall().getHalfSize(), m_config.getCell().getSize().y),
  };

  const auto& rectangles = m_rectangles.get();

  m_bgRectangle.setPosition(m_position);
  m_bgRectangle.setFillColor(m_config.getWall().getColor());
  for (unsigned int i = 0; i < 5; ++i) {
    const auto sizesIndex = ([&, i]() {
      if (i == 0)
        return 0;
      else if (i % 2 == 1)
        return 1;
      else
        return 2;
    })();
    rectangles[i] = sf::RectangleShape(sizes[sizesIndex]);
    rectangles[i].setFillColor(m_config.getCell().getColor());
  }
}

sf::Vector2f Maze::getPosition() const { return m_position; }
void Maze::setPosition(const float x, const float y) {
  m_position.x = x;
  m_position.y = y;

  m_bgRectangle.setPosition(x, y);
}
void Maze::setPosition(const sf::Vector2f &position) {
  m_position = position;

  m_bgRectangle.setPosition(position);
}

sf::Vector2u Maze::getSize() const { return m_size; }
sf::Vector2f Maze::getSizeInPixels() const { return sf::Vector2f(0, 0); }
unsigned int Maze::getLinearSize() const { return m_linearSize; }

void Maze::generate() {
  using Direction = Cell::Direction;
  using State = Cell::State;

  const auto& cells = m_cells.get();

  const auto rx = std::rand() % m_size.x;
  const auto ry = std::rand() % m_size.y;
  const auto ri = rx + ry * m_size.x;

  cells[ri] |= State::VISITED;
  m_stack.push(sf::Vector2i(rx, ry));

  while (!m_stack.empty()) {
    char possibleDirectionsCounter = 0;
    char possibleDirections[4];

    const auto& top = m_stack.top();
    const auto index = top.x + top.y * m_size.x;
    const auto offset = [&, top](const int x, const int y) {
      return (top.x + x) + (top.y + y) * m_size.x;
    };

    const bool boundaries[4] = {top.y > 0, top.x > 0, top.y < m_size.y - 1,
                                top.x < m_size.x - 1};
    const unsigned int indexes[4] = {offset(0, -1), offset(-1, 0), offset(0, 1),
                                     offset(1, 0)};

    for (unsigned int i = 0; i < 4; ++i) {
      if (boundaries[i])
        if (!(cells[indexes[i]] & State::VISITED))
          possibleDirections[possibleDirectionsCounter++] =
              Direction::All[i];
    }

    if (possibleDirectionsCounter == 0) {
      m_stack.pop();
      continue;
    }

    const auto direction = possibleDirections[std::rand() % possibleDirectionsCounter];
    const auto opposite = Direction::Opposite[direction];

    m_stack.push(top + Direction::Offset[direction]);

    const auto& newTop = m_stack.top();
    const auto newIndex = newTop.x + newTop.y * m_size.x;

    cells[index] |= Direction::ToState[direction];
    cells[newIndex] |= State::VISITED | Direction::ToState[opposite];
  }
}

void Maze::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  using State = Cell::State;

  const auto &cells = m_cells.get();

  target.draw(m_bgRectangle, states);
  for (unsigned int j = 0; j < m_size.y; ++j) {
    for (unsigned int i = 0; i < m_size.x; ++i) {
      const auto index = i + j * m_size.x;
      if (cells[index] & State::VISITED) {
        const auto& rectangles = m_rectangles.get();
        const auto x = m_position.x + i * m_config.getCell().getSize().x +
                       (i + 1) * m_config.getWall().getSize();
        const auto y = m_position.y + j * m_config.getCell().getSize().y +
                       (j + 1) * m_config.getWall().getSize();

        const sf::Vector2f positions[5] = {
            sf::Vector2f(x, y),
            sf::Vector2f(x, y - m_config.getWall().getHalfSize()),
            sf::Vector2f(x - m_config.getWall().getHalfSize(), y),
            sf::Vector2f(x, y + m_config.getCell().getSize().y),
            sf::Vector2f(x + m_config.getCell().getSize().x, y)
        };
        const bool conditions[5] = {
          true,
          static_cast<bool>(cells[index] & State::NORTH),
          static_cast<bool>(cells[index] & State::WEST),
          static_cast<bool>(cells[index] & State::SOUTH),
          static_cast<bool>(cells[index] & State::EAST)
        };

        for (unsigned int k = 0; k < 5; ++k) {
          if(conditions[k]) {
            rectangles[k].setPosition(positions[k]);
            target.draw(rectangles[k], states);
          }
        }
      }
    }
  }
}
// =============================================================================
