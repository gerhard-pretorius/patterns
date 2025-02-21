/*
clang++ -std=c++20 shape6.cc \
*/


#include <iostream>
#include <vector>
#include <memory>


//=========================================================
class Indenter {
public:

  Indenter(int num_space = 2) : m_num_space(num_space)
  {
    m_ilevel += m_num_space;
  }

  ~Indenter()
  {
    m_ilevel -= m_num_space;
    if (m_ilevel < 0)
      m_ilevel = 0;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  friend std::ostream& operator<<(std::ostream &os, const Indenter &ind)
  {
    for (int i = 0; i < ind.m_ilevel; ++i)
      os << ' ';
    return os;
  }

//---------------------------------------------------------
private:
  static int m_ilevel;
  int m_num_space;
};

int Indenter::m_ilevel = 0;



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
class ToJSON {
public:

  ToJSON(std::ostream &os) : m_os(os) {}

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Circle &s)
  {
    auto [x,y] = s.getPosition();
    auto radius = s.getSize();

    m_os << "\"circle\": {\n"
         << "  \"x\": " << x << ",\n"
         << "  \"y\": " << y << ",\n"
         << "  \"radius\": " << radius << "\n}";
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Triangle &s)
  {
    auto [x,y] = s.getPosition();
    auto len = s.getSize();

    m_os << "\"triangle\": {\n"
         << "  \"x\": " << x << ",\n"
         << "  \"y\": " << y << ",\n"
         << "  \"len\": " << len << "\n}";
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Rectangle &s)
  {
    auto [x,y] = s.getPosition();
    auto [w, h] = s.getSize();

    m_os << "\"rectangle\": {\n"
         << "  \"x\": " << x << ",\n"
         << "  \"y\": " << y << ",\n"
         << "  \"w\": " << w << ",\n"
         << "  \"h\": " << h << "\n}";
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Drawing &d)
  {
    const char *p = "";
    const char *postfix = ",\n";

    m_os << "\"drawing\": [\n";
    for (auto &s : d)
    {
      m_os << p;
      std::visit(*this, s);
      p = postfix;
    }
    m_os << "]\n";
  }

//---------------------------------------------------------
private:
  std::ostream &m_os;
};


//=========================================================
class ToYAML {
public:

  ToYAML(std::ostream &os) : m_os(os) {}

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Circle &s)
  {
    Indenter ind;

    auto [x,y] = s.getPosition();
    auto radius = s.getSize();

    m_os << "circle:\n"
         << ind << "- x: " << x << '\n'
         << ind << "- y: " << y << '\n'
         << ind << "- radius: " << radius << '\n';
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Triangle &s)
  {
    Indenter ind;

    auto [x,y] = s.getPosition();
    auto len = s.getSize();

    m_os << "triangle:\n"
       << ind << "- x: " << x << '\n'
       << ind << "- y: " << y << '\n'
       << ind << "- len: " << len << '\n';

  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Rectangle &s)
  {
    Indenter ind;

    auto [x, y] = s.getPosition();
    auto [w, h] = s.getSize();

    m_os << "rectangle: \n"
         << ind << "- x: " << x << '\n'
         << ind << "- y: " << y << '\n'
         << ind << "- w: " << w << '\n'
         << ind << "- h: " << h << '\n';

  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()(Drawing &d)
  {
    Indenter ind;

    m_os << "drawing:\n";
    for (auto &s : d)
    {
      m_os << ind << "- ";
      std::visit(*this, s);
    }
  }

//---------------------------------------------------------
private:
  std::ostream &m_os;
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

  ToJSON json(std::cout);
  json(d);

  ToYAML yaml(std::cout);
  yaml(d);

  return 0;
}
