
## Visitor Pattern

The [GOF book](https://en.wikipedia.org/wiki/Design_Patterns) describes the
[Visitor](https://en.wikipedia.org/wiki/Visitor_pattern) design pattern as:

*Represent an operation to be performed on elements of an object structure.*  
*Visitor lets you define a new operation without changing the classes of the
elements on which it operates.*

We explore this pattern by first using a set of dummy/test shapes to create a
'drawing'. Later we replace these with shapes from the 
[SFML](https://www.sfml-dev.org/index.php) library.



-----------------------------------------------------------

### The object structure

We start by implementing a traditional inheritance hierarchy in order to
illustrate the problem the Visitor pattern attempts to solve.

Here Circles, Triangles and Rectangles are Shapes and a Drawing is a
collection of Shapes. *draw*ing simply prints to stdout.

Only snippets of code is shown here.  
You can find the complete implementation for this section in
[shape1.cc](./shape1.cc).  

```C++
class Shape {
public:
  virtual ~Shape() {}
  virtual void draw(std::ostream &os) = 0;
};

class Circle : public Shape {
public:
  Circle(int x, int y, int radius) : m_x(x), m_y(y), m_radius(radius) {}

  void draw(std::ostream &os) override
  {
    os << "Circle(" << m_x << ',' << m_y << ','
       << m_radius << ')' << std::endl;
  }

private:
  int m_x, m_y, m_radius;
};

```

A `Drawing` is a [composite](https://en.wikipedia.org/wiki/Composite_pattern)
that represents both a Shape and a container of Shapes.

```C++
class Drawing : public Shape {
public:

  void draw(std::ostream &os) override
  {
    for(auto &s : m_shape) { s->draw(os); }
  }

  ...

private:
  std::vector<std::unique_ptr<Shape>> m_shape;
};

```
Note that `Drawing` is a container of `Shape`s and therefore iterates over each
shape calling the appropriate method.

This allows us to create a simple drawing like this:

```C++
  Drawing d;
  d.add<Circle>(100, 100, 50);

  auto &triangle = d.add<Triangle>(100, 200, 40);

  auto &d1 = d.add<Drawing>();
  d1.add<Rectangle>(50, 50, 25, 50);
  d1.add<Rectangle>(75, 75, 25, 50);

  ...

  d.draw(std::cout);

```

This completes the initial setup.  


-----------------------------------------------------------

### Adding a new operation (without the Visitor)

To better understand this pattern, we first add a new operation directly to the
`Shape` hierarchy - that is we do NOT use the Visitor pattern.  
Later we compare this with an implementation that uses the Visitor pattern to
add operations without changing the `Shape` hierarchy.

For this example, we decide to save our drawing to a file in
[JSON](https://en.wikipedia.org/wiki/JSON) format. This requires an operation
that serializes the drawing.

First we add the new operation to our base class:

```C++
class Shape {
public:
  virtual ~Shape() {}
  virtual void draw(std::ostream &os) = 0;
  virtual void toJSON(std::ostream &os) = 0;
};

```

And then we implement the operation for each element:


```C++
class Circle : public Shape {
public:
  ...

  void toJSON(std::ostream &os) override
  {
    os << "\"circle\": {\n"
       << "  \"x\": " << m_x << ",\n"
       << "  \"y\": " << m_y << ",\n"
       << "  \"radius\": " << m_radius << "\n}";
  }
...

```

Finally, we can use the operation like this:

```C++
  Drawing d;
  d.add<Circle>(100, 100, 50);

  auto &triangle = d.add<Triangle>(100, 200, 40);
  ...

  d.toJSON(std::cout);

```

This works fine but each time a new operation is added, the `Shape` interface
and all of the elements are changed.  
For example, saving to a different format
like [YAML](https://en.wikipedia.org/wiki/YAML):


```C++
class Shape {
public:
  virtual ~Shape() {}
  virtual void draw(std::ostream &os) = 0;
  virtual void toJSON(std::ostream &os) = 0;
  virtual void toYAML(std::ostream &os) = 0;
};

```

You can find the complete implementation for this section in
[shape2.cc](./shape2.cc) and [shape3.cc](./shape3.cc).

-----------------------------------------------------------

### Adding a new operation using the Visitor pattern

If you don't want to *pollute* the `Shape` interface, then you can use the
Visitor pattern to add new operations. 

For example, you may want to use the `Shape` interface for operations related to
rendering the drawing in a window and use another interface or class hierarchy
for operations related to saving to a file.

We start by defining our `Visitor` interface:

```C++
class Visitor {
public:

  virtual ~Visitor() {}

  virtual void visit(Circle &s) = 0;
  virtual void visit(Triangle &s) = 0;
  virtual void visit(Rectangle &s) = 0;
  virtual void visit(Drawing &s) = 0;
};
```

We then add a method for dispatching to the visitor in our base class:

```C++
class Shape {
public:
  virtual ~Shape() {}

  virtual void draw(std::ostream &os) = 0;
  virtual void accept(Visitor &visitor) = 0;
};

};
```

And implement that method for each element:

```C++
class Circle : public Shape {
public:
  ...

  void accept(Visitor &visitor) { visitor.visit(*this); }
};
```

*Note that although we changed the `Shape` interface by adding the `accept()`
method, we can add new operations without further changes to the interface.*

We create a concrete implementation of the `Visitor` for each new operation:


```C++
class ToJSON : public Visitor {
public:

  void visit(Circle &s)
  {
    auto [x,y] = s.getPosition();
    auto radius = s.getSize();

    m_os << "\"circle\": {\n"
         << "  \"x\": " << x << ",\n"
         << "  \"y\": " << y << ",\n"
         << "  \"radius\": " << radius << "\n}";
  }

  void visit(Triangle &s)
  ...

};

class ToYAML : public Visitor {
public:

  void visit(Circle &s)
  ...

```


Finally, we can use the new operations like this:

```C++
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
  d.accept(json);

  ToYAML yaml(std::cout);
  d.accept(yaml);

```

The Visitor pattern now allows us to add any number of new operations without
changing the `Shape` interface or any of the elements.

You can find the complete implementation for this section in [shape4.cc](./shape4.cc).

-----------------------------------------------------------

### Using std::visit and std::variant

[std::visit](https://en.cppreference.com/w/cpp/utility/variant/visit)
is an implementation of the Visitor design pattern in the C++ library.  
It uses
[std::variant](https://en.cppreference.com/w/cpp/utility/variant) which is
[tagged union](https://en.wikipedia.org/wiki/Tagged_union#) holding one of
several types.

This section re-writes the previous examples using `std::variant` instead of
inheritance via the `Shape` interface.

Again start by defining the elements of a drawing:


```C++
class Circle {
public:

  Circle(int x, int y, int radius);
  void draw(std::ostream &os);

};

...
using Shape = std::variant<Circle, Triangle, Rectangle, Drawing>;
```

Note that these no longer implement the `Shape` interface. Instead we define a
`Shape` to be a variant - that is, a Shape is one of a `Circle`, `Triangle`,
`Rectangle` or `Drawing`.


```C++
class Drawing {
public:
  
  void draw(std::ostream &os)
  { 
    for (auto &s : m_shape)
      std::visit([&](auto &&t) { t.draw(os); }, s);
  }

  template<typename T, typename... A> auto &add(A... a)
  ...

private:
  std::vector<Shape> m_shape;

};
```

Note in this example, each Shape type is still expected to provide a *draw*
method - that is a constraint (a type requirement) has replaced the previous
interface requirement of the base class Shape.


In addition, a drawing now stores Shapes directly instead of using (unique_)
pointers.

As in the previous examples, a `Drawing` is a container of `Shape`s and
therefore iterates over each shape. However, we can no longer dispatch through a
virtual method since Shapes lack a common interface. Instead `std::visit` is
used with a lambda. In this case all variant types are treated the same - that
is simply delegate to the Shape and expect each Shape to draw itself.

You can find the complete implementation for this section in
[shape5.cc](./shape5.cc).


We can now define Visitors that implements the JSON and YAML operations using
the interface expected by `std::visit`:

```C++
class ToJSON {
public:

  void operator()(Circle &s)
  {
    auto [x,y] = s.getPosition();
    auto radius = s.getSize();

    m_os << "\"circle\": {\n"
         << "  \"x\": " << x << ",\n"
         << "  \"y\": " << y << ",\n"
         << "  \"radius\": " << radius << "\n}";
  }

  ...

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

  ...

private:
  std::ostream &m_os;
};
```

Note that each Visitor overloads `operator()` for every type in the `Shape`
variant. This replaces the overloaded `visit()` method used in the original
`Visitor` base class. Visitors no longer share a common interface but have the
type requirement to overload `operator()`.

Finally, we can use the new operations like this:

```C++
  Drawing d;
  d.add<Circle>(100, 100, 50);

  auto &triangle = d.add<Triangle>(100, 200, 40);

  auto &d1 = d.add<Drawing>();
  d1.add<Rectangle>(50, 50, 25, 50);
  d1.add<Rectangle>(75, 75, 25, 50);

  ...

  ToJSON json(std::cout);
  json(d);

  ToYAML yaml(std::cout);
  yaml(d);


```

You can find the complete implementation for this section in
[shape6.cc](./shape6.cc).  


-----------------------------------------------------------
