/*
clang++ -std=c++20 shape3.cc \
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


//=========================================================
class Shape {
public:
  virtual ~Shape() {}
  virtual void draw(std::ostream &os) = 0;
  virtual void toJSON(std::ostream &os) = 0;
  virtual void toYAML(std::ostream &os) = 0;
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

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void toJSON(std::ostream &os) override
  {
    os << "\"circle\": {\n"
       << "  \"x\": " << m_x << ",\n"
       << "  \"y\": " << m_y << ",\n"
       << "  \"radius\": " << m_radius << "\n}";
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void toYAML(std::ostream &os) override
  {
    Indenter ind;

    os << "circle:\n"
       << ind << "- x: " << m_x << '\n'
       << ind << "- y: " << m_y << '\n'
       << ind << "- radius: " << m_radius << '\n';
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

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void toJSON(std::ostream &os) override
  {
    os << "\"triangle\": {\n"
       << "  \"x\": " << m_x << ",\n"
       << "  \"y\": " << m_y << ",\n"
       << "  \"len\": " << m_len << "\n}";
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void toYAML(std::ostream &os) override
  {
    Indenter ind;

    os << "triangle:\n"
       << ind << "- x: " << m_x << '\n'
       << ind << "- y: " << m_y << '\n'
       << ind << "- len: " << m_len << '\n';
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

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void toJSON(std::ostream &os) override
  {
    os << "\"rectangle\": {\n"
       << "  \"x\": " << m_x << ",\n"
       << "  \"y\": " << m_y << ",\n"
       << "  \"w\": " << m_w << ",\n"
       << "  \"h\": " << m_h << "\n}";
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void toYAML(std::ostream &os) override
  {
    Indenter ind;

    os << "rectangle: \n"
       << ind << "- x: " << m_x << '\n'
       << ind << "- y: " << m_y << '\n'
       << ind << "- w: " << m_w << '\n'
       << ind << "- h: " << m_h << '\n';
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
  void toJSON(std::ostream &os) override
  {
    const char *p = "";
    const char *postfix = ",\n";

    os << "\"drawing\": [\n";
    for (auto &s : m_shape)
    {
      os << p;
      s->toJSON(os);
      p = postfix;
    }
    os << "]\n";
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void toYAML(std::ostream &os) override
  {
    Indenter ind;

    os << "drawing:\n";
    for (auto &s : m_shape)
    {
      os << ind << "- ";
      s->toYAML(os);
    }
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

  d.toYAML(std::cout);

  return 0;
}
