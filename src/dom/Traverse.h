#ifndef __StarFishTraverse__
#define __StarFishTraverse__

namespace StarFish {

//template<typename Func>
class Traverse {
  Traverse(){}

public:
  template<typename Func>
  static Node* findDescendant(Node* parent, Func matchingRule)
  {
      Node* child = parent->firstChild();
      while (child) {
          if (matchingRule(child))
              return child;
          else {
              Node* matchedDescendant = findDescendant(child, matchingRule);
              if (matchedDescendant)
                  return matchedDescendant;
          }
          child = child->nextSibling();
      }
      return nullptr;
  }

  template<typename Func>
  static Node* firstChild(Node* parent, Func matchingRule)
  {
      Node* child = parent->firstChild();
      while (child) {
          if (matchingRule(child))
              return child;
          else
              child = child->nextSibling();
      }
      return nullptr;
  }

  template<typename Func>
  static Node* lastChild(Node* parent, Func matchingRule)
  {
      Node* child = parent->lastChild();
      while (child) {
          if (matchingRule(child))
              return child;
          else
              child = child->previousSibling();
      }
      return nullptr;
  }

  template<typename Func>
  static unsigned long childCount(Node* parent, Func matchingRule)
  {
      unsigned long count = 0;
      Node* child = parent->firstChild();
      while (child) {
          if (matchingRule(child))
              count++;
          child = child->nextSibling();
      }
      return count;
  }
};

}

#endif
