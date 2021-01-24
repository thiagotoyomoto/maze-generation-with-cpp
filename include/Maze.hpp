#ifndef FE7CA114_BEC6_4391_A2E7_E422D6489BCD
#define FE7CA114_BEC6_4391_A2E7_E422D6489BCD

#include <SFML/Graphics/RectangleShape.hpp>

#include <memory>
#include <stack>
#include <thread>

class Maze : public sf::Drawable {
public:
  class Cell {
  public:
    class State {
    public:
      static const u_int8_t UNVISITED;
      static const u_int8_t NORTH;
      static const u_int8_t WEST;
      static const u_int8_t SOUTH;
      static const u_int8_t EAST;
      static const u_int8_t VISITED;
    };

    class Direction {
    public:
      static const u_int8_t NORTH;
      static const u_int8_t WEST;
      static const u_int8_t SOUTH;
      static const u_int8_t EAST;
      static const u_int8_t TOTAL;
      static const u_int8_t All[4];
      static const sf::Vector2i Offset[4];
      static const u_int8_t Opposite[4];
      static const u_int8_t ToState[4];
    };

    class Config {
    private:
      sf::Vector2f m_size;
      sf::Color m_color;

    public:
      Config(const sf::Vector2f &size, const sf::Color &color);

      inline sf::Vector2f getSize() const;
      inline sf::Color getColor() const;
    };
  };

  class Wall {
  public:
    class Config {
    private:
      float m_size;
      float m_halfSize;
      sf::Color m_color;

    public:
      Config(const float size, const sf::Color &color);

      inline float getSize() const;
      inline float getHalfSize() const;
      inline sf::Color getColor() const;
    };
  };

  class Config {
    private:
      Cell::Config m_cell;
      Wall::Config m_wall;

    public:
      Config(Cell::Config cell, Wall::Config wall);

      Maze::Cell::Config getCell() const;
      Maze::Wall::Config getWall() const;
  };

private:
  const Config m_config;

  sf::Vector2f m_position;
  sf::Vector2u m_size;
  sf::Vector2f m_sizeInPixels;
  unsigned int m_linearSize = 0;

  std::unique_ptr<int[]> m_cells;
  std::stack<sf::Vector2i> m_stack;

  sf::RectangleShape m_bgRectangle;
  std::unique_ptr<sf::RectangleShape[]> m_rectangles;

public:
  Maze(const unsigned int width, const unsigned int height);

  sf::Vector2f getPosition() const;
  void setPosition(const float x, const float y);
  void setPosition(const sf::Vector2f &position);

  sf::Vector2u getSize() const;
  sf::Vector2f getSizeInPixels() const;
  unsigned int getLinearSize() const;

  void generate();

public:
  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};

#endif /* FE7CA114_BEC6_4391_A2E7_E422D6489BCD */
