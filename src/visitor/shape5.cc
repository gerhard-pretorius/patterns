/*
clang++ -std=c++20 shape5.cc \
*/


#include <iostream>
#include <vector>
#include <memory>

//=========================================================
class Circle {
public:

  Circle(int x, int y, int radius) : m_x(x), m_y(y), m_radius(radius) {}

  std::tuple<int, int> getPosition() const { return {m_x, m_y}; }
  int getSize() const { return m_radius; }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(std::ostream &os)
  {
    os << "Circle(" << m_x << ',' << m_y << ','
       << m_radius << ')' << std::endl;
  }

//---------------------------------------------------------
private:
  int m_x, m_y, m_radius;
};

//=========================================================
class Triangle {
public:

  Triangle(int x, int y, int len) : m_x(x), m_y(y), m_len(len) {}

  std::tuple<int, int> getPosition() const { return {m_x, m_y}; }
  int getSize() const { return m_len; }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(std::ostream &os)
  {
    os << "Triangle(" << m_x << ',' << m_y << ','
       << m_len << ')' << std::endl;
  }

//---------------------------------------------------------
private:
  int m_x, m_y, m_len;
};


//=========================================================
class Rectangle {
public:

  Rectangle(int x, int y, int w, int h) : m_x(x), m_y(y), m_w(w), m_h(h) {}

  std::tuple<int, int> getPosition() const { return {m_x, m_y}; }
  std::tuple<int, int> getSize() const { return {m_w, m_h}; }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(std::ostream &os)
  {
    os << "Rectangle(" << m_x << ',' << m_y << ','
       << m_w << ',' << m_h << ')' << std::endl;
  }

//---------------------------------------------------------
private:
  int m_x, m_y, m_w, m_h;
};

//=========================================================
class Drawing;
using Shape = std::variant<Circle, Triangle, Rectangle, Drawing>;

//=========================================================
class Drawing {
public:

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(std::ostream &os)
  {
    for (auto &s : m_shape)
      std::visit([&](auto&& t) { t.draw(os); }, s);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  template<typename T, typename... A>
  auto &add(A... a)
  {
    auto &tmp = m_shape.emplace_back(std::in_place_type<T>, std::forward<A>(a)...);
    return std::get<T>(tmp);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  auto begin() { return m_shape.begin(); }
  auto begin() const { return m_shape.begin(); }
  auto cbegin() const { return m_shape.cbegin(); }

  auto end() { return m_shape.end(); }
  auto end() const { return m_shape.end(); }
  auto cend() const { return m_shape.cend(); }

//---------------------------------------------------------
private:
  std::vector<Shape> m_shape;
};



//=========================================================
int main()
{
  Drawing d;
  d.add<Circle>(100, 100, 50);

  auto &triangle = d.add<Triangle>(100, 200, 40);

  auto &d1 = d.add<Drawing>();
  d1.add<Rectangle>(50, 50, 25, 50);
  d1.add<Rectangle>(75, 75, 25, 50);

  auto &d2 = d1.add<Drawing>();
  d2.add<Rectangle>(50, 150, 25, 60);
  d2.add<Rectangle>(75, 175, 25, 60);

  d.draw(std::cout);

  return 0;
}
