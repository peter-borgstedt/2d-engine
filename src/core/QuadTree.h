#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#define QT_TOP    0x00000001U
#define QT_BOTTOM 0x00000002U
#define QT_LEFT   0x00000004U
#define QT_RIGHT  0x00000008U

#include <SDL.h>
#include <array>
#include <map>
#include <memory>
#include <unordered_set>
#include <vector>

#include "CollisionUtils.h"
#include "Sprite.h"

// TODO: fix the quad tree, it has bottlenecks and flaws at the moment.
class QuadTree
{
private:
  int MAX_OBJECTS = 4;
  int MAX_LEVELS = 5;

  int level;
  SDL_Rect bounds;
  std::vector<std::shared_ptr<Sprite>> objects;
  // TODO: remove
  // std::unordered_set<std::shared_ptr<Sprite>, std::vector<QuadTree*>> lookup;

  double vertical_mid_point;
  double horizontal_mid_point;

  std::array<QuadTree *, 4> nodes = {nullptr, nullptr, nullptr, nullptr};

  void split();
  void clear();

  // Determine which node the object belongs to.
  inline uint8_t QuadTree::get_index_state(std::shared_ptr<Sprite> obj)
  {
    const SDL_Rect &rect = obj->get_position();
    uint8_t state = 0;
    // Object fit within the top quadrants
    if (rect.y < vertical_mid_point) {
      state |= QT_TOP;
    }
    // Object fit within the bottom quadrants
    else if (rect.y > vertical_mid_point || rect.y + rect.h > vertical_mid_point) {
      state |= QT_BOTTOM;
    }

    // Object fit within the left quadrants
    if (rect.x < horizontal_mid_point) {
      state |= QT_LEFT;
    }
    // Object fit within the right quadrants
    else if (rect.x > horizontal_mid_point || rect.x + rect.w > horizontal_mid_point) {
      state |= QT_RIGHT;
    }

    return state; // should never happen.
  }

  // Process objects in qualified nodes depending on the index state of this object.
  // A member function pointer is required to a process function.
  void do_delete(uint8_t index_state, std::shared_ptr<Sprite> object,
                 bool (QuadTree::*fptr)(std::shared_ptr<Sprite>));

  void do_insert(uint8_t index_state, std::shared_ptr<Sprite> obj);

  // Retrieve objects in nodes qualified by the the index state of this source object.
  void do_retrieve(uint8_t index_state, std::vector<std::shared_ptr<Sprite>> &return_objects,
                   std::shared_ptr<Sprite> obj);

protected:
  QuadTree(const int &level, const SDL_Rect &bounds);

public:
  ~QuadTree();

  // Get an instance quad tree.
  static std::shared_ptr<QuadTree> get_instance(const int &level, const SDL_Rect &bounds);

  // Clear all node and its children.
  void reset(const SDL_Rect &bounds);

  // Insert object into qualified nodes depending on the index state of this object.
  void insert(std::shared_ptr<Sprite> obj);

  // Retrieve objects in in nodes which given target is overlapping.
  void retrieve(std::vector<std::shared_ptr<Sprite>> &retriever, std::shared_ptr<Sprite> object);

  // Remove object from tree.
  bool remove(std::shared_ptr<Sprite> object);

  // Update object position in tree.
  void update(std::shared_ptr<Sprite> object);

  // At the moment only used for debugging purpose.
  int get_level();

  // At the moment only used for debugging purpose.
  int get_levels();

  // At the moment only used for debugging purpose.
  const SDL_Rect &get_bounds() const;
  // At the moment only used for debugging purpose.
  const std::array<QuadTree *, 4> &get_nodes() const;

  QuadTree(const QuadTree &) = delete;
  const QuadTree &operator=(const QuadTree &) = delete;
};

#endif
