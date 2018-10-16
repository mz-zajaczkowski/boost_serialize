#include <bitset>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <list>
#include <memory>
#include <vector>
using namespace std;

class Attribute {
public:
  Attribute() { m_flags.set(static_cast<size_t>(Flags::isSerializable)); }

  enum class Flags : uint8_t {
    isSerializable = 0,
    isWeird,
    isSthElse,
    is3,
    is4,
    is5,
    is6,
    is7,
    is8,
    SIZE
  };
  typedef std::bitset<static_cast<size_t>(Flags::SIZE)> Type;

  void setFlags(const Type &flags) { m_flags = flags; }

  inline bool isSerializable() {
    return m_flags.test(static_cast<size_t>(Flags::isSerializable));
  }
  inline bool isWeird() {
    return m_flags.test(static_cast<size_t>(Flags::isWeird));
  }
  inline bool isSthElse() {
    return m_flags.test(static_cast<size_t>(Flags::isSthElse));
  }

  inline void setWeird() { m_flags.set(static_cast<size_t>(Flags::isWeird)); }

private:
  Type m_flags;
};

class A {
public:
  A() {
    Attribute sth;
    m_attrs.push_back(sth);
  }
  std::vector<Attribute> &getAttrs() { return m_attrs; }

private:
  std::vector<Attribute> m_attrs;
};

class B {
public:
  B(const A &base) : attribs(base) {}

  inline bool isSer() {
    for (auto &attr : attribs.getAttrs()) {
      if (attr.isSerializable())
        return true;
    }
    return false;
  }

  inline bool isWeird() {
    for (auto &attr : attribs.getAttrs()) {
      if (attr.isWeird())
        return true;
    }
    return false;
  }

  inline bool isSthElse() {
    for (auto &attr : attribs.getAttrs()) {
      if (attr.isSthElse())
        return true;
    }
    return false;
  }

  inline std::vector<Attribute> &getA() { return attribs.getAttrs(); }

private:
  B() {}
  A attribs;
};

using namespace boost::property_tree;
template <typename T> void serialize(T &object, ptree &root) = delete;
template <typename T> T &deserialize(ptree &root) = delete;

template <template <class...> class Container, class T>
void serialize(Container<T> &cont, ptree &root) {
  for (auto &item : cont) {
    cout << "serializing container item" << std::endl;
    serialize(item, root);
  }
}

// template <typename T> void serialize(std::vector<T> &vec, ptree &root) {
//  for (auto &item : vec) {
//    cout << "serializing vector item" << std::endl;
//    serialize(item, root);
//  }
//}

template <> void serialize(Attribute &object, ptree &root) {
  root.put("isSerializable", object.isSerializable());
  root.put("isWeird", object.isWeird());
  root.put("isSthElse", object.isSthElse());
}

template <> void serialize(B &object, ptree &root) {
  ptree bTree;
  serialize(object.getA(), bTree);
  root.add_child("BTYPE", bTree);
}

int main() {
  A aObj;
  aObj.getAttrs()[0].setWeird();
  B bObj(aObj);
  ptree root;
  serialize(bObj, root);
  write_json(std::cout, root);
  cout << "Hello World!" << endl;
  return 0;
}
