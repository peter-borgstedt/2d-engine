#include "QuadTree.h"
#include "CollisionUtils.h"
#include "Debug.h"
#include <algorithm>
#include <iostream>

using namespace std;

QuadTree::QuadTree(const int &level, const SDL_Rect &bounds)
    : level(level),
      objects(),
      bounds(bounds),
      horizontal_mid_point(bounds.x + (bounds.w / 2)),
      vertical_mid_point(bounds.y + (bounds.h / 2))
{
  debug::analyser.type_constructed(this);
}

QuadTree::~QuadTree()
{
  objects.clear();
  for (int i = 0; i < nodes.size(); i++) {
    delete nodes[i];
    nodes[i] = nullptr;
  }
  debug::analyser.type_deconstructed(this);
}

shared_ptr<QuadTree> QuadTree::get_instance(const int &level, const SDL_Rect &bounds)
{
  return shared_ptr<QuadTree>(new QuadTree(level, bounds));
}

void QuadTree::reset(const SDL_Rect &bounds)
{
  clear();
  QuadTree::bounds = bounds;
  horizontal_mid_point = bounds.x + (bounds.w / 2);
  vertical_mid_point = bounds.y + (bounds.h / 2);
}

// Splits the node into 4 sub nodes.
void QuadTree::split()
{
  int sub_width = static_cast<int>(bounds.w / 2);
  int sub_height = static_cast<int>(bounds.h / 2);
  int x = bounds.x;
  int y = bounds.y;

  // Create top left node.
  nodes[0] = new QuadTree(level + 1, {x, y, sub_width, sub_height});
  // Create top right node.
  nodes[1] = new QuadTree(level + 1, {x + sub_width, y, sub_width, sub_height});
  // Create bottom left node.
  nodes[2] = new QuadTree(level + 1, {x, y + sub_height, sub_width, sub_height});
  // Create bottom right node.
  nodes[3] = new QuadTree(level + 1, {x + sub_width, y + sub_height, sub_width, sub_height});
}

// Clear the node and all its children (usually used when clearing the whole tree).
void QuadTree::clear()
{
  objects.clear(); // Clear objects in this node.
  for (int i = 0; i < nodes.size(); ++i) {
    if (nodes[i]) {
      // Will trigger child deconstructor and remove any of its content.
      delete nodes[i];
      nodes[i] = nullptr;
    }
  }
}

void QuadTree::do_delete(uint8_t index_state, std::shared_ptr<Sprite> object,
                         bool (QuadTree::*fptr)(std::shared_ptr<Sprite>))
{
  {
    if (index_state == 0) {
      return;
    }

    if (index_state & QT_TOP) {
      if (index_state & QT_LEFT) {
        (nodes[0]->*fptr)(object);
      }
      if (index_state & QT_RIGHT) {
        (nodes[1]->*fptr)(object);
      }
    }

    if (index_state & QT_BOTTOM) {
      if (index_state & QT_LEFT) {
        (nodes[2]->*fptr)(object);
      }
      if (index_state & QT_RIGHT) {
        (nodes[3]->*fptr)(object);
      }
    }
    if (nodes[0]->objects.size() == 0 && nodes[1]->objects.size() == 0 &&
        nodes[2]->objects.size() == 0 && nodes[3]->objects.size() == 0) {
      nodes[0] = nullptr;
      nodes[1] = nullptr;
      nodes[2] = nullptr;
      nodes[3] = nullptr;
    }
  }
}

void QuadTree::do_insert(uint8_t index_state, std::shared_ptr<Sprite> obj)
{
  {
    if (index_state == 0) {
      return;
    }

    if (index_state & QT_TOP) {
      if (index_state & QT_LEFT) {
        nodes[0]->insert(obj);
      }
      if (index_state & QT_RIGHT) {
        nodes[1]->insert(obj);
      }
    }

    if (index_state & QT_BOTTOM) {
      if (index_state & QT_LEFT) {
        nodes[2]->insert(obj);
      }
      if (index_state & QT_RIGHT) {
        nodes[3]->insert(obj);
      }
    }
  }
}

void QuadTree::do_retrieve(uint8_t index_state,
                           std::vector<std::shared_ptr<Sprite>> &return_objects,
                           std::shared_ptr<Sprite> obj)
{
  {
    if (index_state & QT_TOP) {
      if (index_state & QT_LEFT) {
        nodes[0]->retrieve(return_objects, obj);
      }
      if (index_state & QT_RIGHT) {
        nodes[1]->retrieve(return_objects, obj);
      }
    }

    if (index_state & QT_BOTTOM) {
      if (index_state & QT_LEFT) {
        nodes[2]->retrieve(return_objects, obj);
      }
      if (index_state & QT_RIGHT) {
        nodes[3]->retrieve(return_objects, obj);
      }
    }
  }
}

// Insert the object into the quad tree. If the node exceeds the capacity, it will split and add
// all objects to their corresponding nodes.
void QuadTree::insert(std::shared_ptr<Sprite> obj)
{
  if (nodes[0]) {
    auto index_state = get_index_state(obj);
    do_insert(index_state, obj);
  }

  objects.push_back(obj);
  if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS) {
    if (!nodes[0]) {
      split();
    }

    while (objects.size() > 0) {
      auto object = objects.back();
      auto index_state = get_index_state(object);
      do_insert(index_state, object);
      objects.pop_back();
    }
  }
}

/*
 * Return all objects that could collide with the given object
 */
void QuadTree::retrieve(std::vector<std::shared_ptr<Sprite>> &return_objects,
                        std::shared_ptr<Sprite> obj)
{
  if (nodes[0]) {
    do_retrieve(get_index_state(obj), return_objects, obj);
  } else {
    // Alot fast than: return_objects.insert(return_objects.end(),
    // objects.begin(), objects.end());
    std::copy(objects.begin(), objects.end(), back_inserter(return_objects));
  }
}

bool QuadTree::remove(std::shared_ptr<Sprite> object)
{
  if (nodes[0]) {
    do_delete(get_index_state(object), object, &QuadTree::remove);
  } else {
    size_t prev_size = objects.size();
    // Use erase remove idiom.
    objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
    if (--prev_size != objects.size()) {
      cout << "borttagen" << endl;
    }
  }
  return objects.size() == 0;
}

void QuadTree::update(std::shared_ptr<Sprite> object)
{
  insert(object);
}

int QuadTree::get_level()
{
  return level;
}

int QuadTree::get_levels()
{
  int levels = 1;
  for (auto node : nodes)
    if (node) {
      levels += node->get_levels();
    }
  return levels;
}

const std::array<QuadTree *, 4> &QuadTree::get_nodes() const
{
  return nodes;
}

const SDL_Rect &QuadTree::get_bounds() const
{
  return bounds;
}
