/*
clang++ -std=c++20 shape7.cc \
  -I ~/opt/include \
  -L ~/opt/lib -lsfml-graphics -lsfml-window -lsfml-system

  Press <Esc> to close the graphic window.
*/


#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <memory>
#include <tuple>
#include <variant>
#include <utility>

#include <SFML/Graphics.hpp>


//=========================================================
template <typename ...Leaf>
class Composite {
public:
  using value_type = std::variant<Leaf..., Composite>;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  template<typename T, typename... A>
  auto &emplace_back(A... a)
  {
    auto &tmp = m_composite.emplace_back(std::in_place_type<T>,
      std::forward<A>(a)...);
    return std::get<T>(tmp);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  template <typename T>
  void accept(T &visitor)
  {
    for (auto &s : m_composite)
    {
      std::visit(visitor, s);
    }
  }

//---------------------------------------------------------
private:
  std::vector<value_type> m_composite;
};



//===================================================================
using Color = sf::Color;
using Pos = sf::Vector2f;
using V2f = sf::Vector2f;

using Circle = sf::CircleShape;
using Rectangle = sf::RectangleShape;

//---------------------------------------------------------
class Triangle : public Circle {
public:
  Triangle(float radius) : Circle (radius, 3) {}
};


using Drawing = Composite<Circle, Triangle, Rectangle>;

//---------------------------------------------------------
class Window : public sf::RenderWindow {
public:
  Window(const int width, const int height, const std::string &title)
    : sf::RenderWindow(sf::VideoMode(width, height), title.c_str())
  {
    setVerticalSyncEnabled(true);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Circle &s) { draw(s); }
  void operator()(Triangle &s) { draw(s); }
  void operator()(Rectangle &s) { draw(s); }
  void operator()(Drawing &d) { d.accept(*this); }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void show(Drawing &d)
  {
    while(isOpen())
    {
      sf::Event event;
      while (pollEvent(event))
      {
        switch(event.type)
        {
          case sf::Event::Closed: close(); break;
          case sf::Event::KeyPressed:
            switch (event.key.code)
            {
              case sf::Keyboard::Escape: close(); break;
              default: break;
            }
          break;

          default: break;
        }
      }

      clear();
      (*this)(d);
      display();
    }
  }
};

//=========================================================
class Scale {
public:

  Scale(float ratio) : m_ratio(ratio) {}

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Circle &s) { s.setRadius(s.getRadius() * m_ratio); }
  void operator()(Triangle &s) { s.setRadius(s.getRadius() * m_ratio); }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Rectangle &s)
  {
    auto sz = s.getSize();
    s.setSize(V2f{sz.x * m_ratio, sz.y * m_ratio});
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Drawing &d) { d.accept(*this); }

//---------------------------------------------------------
private:
  float m_ratio;
};

class FillColor {
public:

  FillColor(Color c) : m_color(c) {}

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Circle &s) { s.setFillColor(m_color); }
  void operator()(Triangle &s) { s.setFillColor(m_color); }  
  void operator()(Rectangle &s) { s.setFillColor(m_color); }
  void operator()(Drawing &d) { d.accept(*this); }

//---------------------------------------------------------
private:
  Color m_color;
};


//=========================================================
int main()
{
  Window window(300, 400, "visitor");

  Drawing d;
  auto & circle = d.emplace_back<Circle>(50.f);
  circle.setFillColor(Color::Red);
  circle.setPosition(Pos(100,100));

  auto &triangle = d.emplace_back<Triangle>(50.f);
  triangle.setPosition(Pos(100,200));
  triangle.setFillColor(Color::Green);

  auto &d1 = d.emplace_back<Drawing>();
  auto &r1 = d1.emplace_back<Rectangle>(V2f{25, 50});
  r1.setPosition(Pos(50,50));

  auto &r2 = d1.emplace_back<Rectangle>(V2f{25, 50});
  r2.setPosition(Pos(75, 75));
  r2.setFillColor(Color::Blue);

  FillColor yellow(Color::Yellow);
  yellow(d1);

  Scale bigger(2.0);
  bigger(d);
  window.show(d);

  return 0;
}
