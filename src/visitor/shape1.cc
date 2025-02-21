/*
clang++ -std=c++20 shape1.cc \
*/


#include <iostream>
#include <vector>
#include <memory>



//=========================================================
class Shape {
public:
  virtual ~Shape() {}
  virtual void draw(std::ostream &os) = 0;
};


//=========================================================
class Circle : public Shape {
public:

  Circle(int x, int y, int radius) : m_x(x), m_y(y), m_radius(radius) {}

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(std::ostream &os) override
  {
    os << "Circle(" << m_x << ',' << m_y << ','
       << m_radius << ')' << std::endl;
  }

//---------------------------------------------------------
private:
  int m_x, m_y, m_radius;
};


//=========================================================
class Triangle : public Shape {
public:

  Triangle(int x, int y, int len) : m_x(x), m_y(y), m_len(len) {}

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(std::ostream &os) override
  {
    os << "Triangle(" << m_x << ',' << m_y << ','
       << m_len << ')' << std::endl;
  }

//---------------------------------------------------------
private:
  int m_x, m_y, m_len;
};


//=========================================================
class Rectangle : public Shape {
public:

  Rectangle(int x, int y, int w, int h) : m_x(x), m_y(y), m_w(w), m_h(h) {}

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(std::ostream &os) override
  {
    os << "Rectangle(" << m_x << ',' << m_y << ','
       << m_w << ',' << m_h << ')' << std::endl;
  }

//---------------------------------------------------------
private:
  int m_x, m_y, m_w, m_h;
};


//=========================================================
class Drawing : public Shape {
public:

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(std::ostream &os) override
  {
    for(auto &s : m_shape) { s->draw(os); }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  template<typename T, typename... A>
  auto &add(A... a)
  {
    auto &tmp = m_shape.emplace_back(std::make_unique<T>(std::forward<A>(a)...));
    return * static_cast<T*>(tmp.get());
  }

//---------------------------------------------------------
private:
  std::vector<std::unique_ptr<Shape>> m_shape;
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
