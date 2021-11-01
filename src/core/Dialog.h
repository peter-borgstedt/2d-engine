#ifndef DIALOG_H
#define DIALOG_H

#include <Map>
#include <Memory>

#include "UIComponent.h"

// Ref: http://stackoverflow.com/a/247859
template <typename Pair> struct MultiMapEqual : public std::binary_function<Pair, Pair, bool>
{
  bool operator()(const Pair &x, const Pair &y) const
  {
    return x.first == y.first;
  }
};

class Dialog : UIComponent
{
public:
  typedef int comp_key;
  typedef std::shared_ptr<UIComponent> comp_val;
  typedef std::multimap<comp_key, comp_val> comp_multimap;

  ~Dialog();
  static std::shared_ptr<Dialog> get_instance(int x, int y, int w, int h);

  void add(int row, std::shared_ptr<UIComponent> component);

  void initialize(std::shared_ptr<Context> context) override;
  void paint(SDL_Renderer *renderer, const SDL_Rect &clip) override;
  void on_event(const SDL_Event &e,
                std::unordered_map<std::string, std::string> &event_data) override;

protected:
  Dialog(int x, int y, int w, int h);

private:
  SDL_Texture *texture;
  comp_multimap comps;
};

#endif
