#include "Dialog.h"
#include "Debug.h"
#include "Utility.h"

#include <algorithm>
#include <numeric>

using namespace std;

Dialog::Dialog(int x, int y, int w, int h) : UIComponent(x, y, w, h)
{
  debug::analyser.type_constructed(this);
}

Dialog::~Dialog()
{
  SDL_DestroyTexture(texture);
  debug::analyser.type_deconstructed(this);
}

std::shared_ptr<Dialog> Dialog::get_instance(int x, int y, int w, int h)
{
  return std::shared_ptr<Dialog>(new Dialog(x, y, w, h));
}

void Dialog::add(int row, std::shared_ptr<UIComponent> component)
{
  comps.insert({row, component});
}

template <class A, class B> static int count_keys(multimap<A, B> m)
{
  int keys = 0;
  for (auto it = m.cbegin(); it != m.cend(); it = m.upper_bound(it->first))
    ++keys;
  return keys;
}

template <class A, class B>
static int do_operation(const multimap<A, B> &m, const pair<A, B> &p,
                        function<void(int &, const B)> func)
{
  int sum = 0;
  auto range = m.equal_range(p.first);
  for (auto it = range.first; it != range.second; ++it)
    func(sum, it->second);
  return sum;
}

// Initialize components.
// Each component is attached to a specific row i the layout.
// Each row may several components, it depends on how the dialog have been set up. If two
// components are aligned in one row, the first attached one will be added first then the second.
// The components will be align centered both horizontal vertically to each other. The size of the
// row is calculated from the component with maximum size. When all rows have been setup, if there
// are any overflow size vertically and horizontally in the dialog, row and components will be
// centered both horizontally and vertically.
//  _________________________
// |                         |
// |    My label centered    | row 1: Label
// |     [text input...]     | row 2: TextBox
// |  [Button1]   [Button2]  | row 2: column 1: Button, column 2: Button
// |_________________________|
//  * extra space vertically and horizontally depending on dialog size.
void Dialog::initialize(std::shared_ptr<Context> context)
{
  SDL_Renderer *renderer = context->get_window()->get_renderer_ptr();

  // The total amount of rows to build.
  int amount_of_rows = count_keys(comps);

  // Get max height of each row and summarize them, giving the total vertical size.
  int h_sum = 0;
  for (auto it = comps.cbegin(); it != comps.cend(); it = comps.upper_bound(it->first))
    h_sum += do_operation<comp_key, comp_val>(comps, *it,
                                              [](auto &k, auto v) { k = max(k, v->dimension.h); });

  int cy = dimension.y;
  int y_indent = (dimension.h - h_sum) / static_cast<int>(amount_of_rows + 1);

  // Iterate through all component rows (y-axis).
  for (auto it = comps.cbegin(); it != comps.cend(); it = comps.upper_bound(it->first)) {
    // Summarize each components width.
    int w_sum =
        do_operation<comp_key, comp_val>(comps, *it, [](auto &k, auto v) { k += v->dimension.w; });

    // The amount of components in this row.
    int amount_of_components = static_cast<int>(comps.count(it->first));

    // Calculate indentation for horizontal axis from dialog width, total width of components
    // for current row and the amount of components plus one for right most space.
    int x_indent = (dimension.w - w_sum) / (amount_of_components + 1);

    cy += y_indent; // Add vertical indentation.
    int cx = dimension.x;
    // Iterate through all row components (x-axis);
    int h_max = do_operation<comp_key, comp_val>(
        comps, *it, [&context, x_indent, &cx, cy](auto &k, auto v) {
          // Calculate bounds for component using current y and x position for this row.
          // Add indentation on x axis then append the component width on x that will be used
          // for next next position.
          auto &dimension = v->get_dimension();
          k = max(k, dimension.h); // k will be the max height of this rows components.
          cx += x_indent;          // Add horizontal indentation
          dimension.x = cx;
          dimension.y = cy;
          v->initialize(context);
          cx += dimension.w;
        });
    // Get max value of row component and append it to y axis (for next row).
    cy += h_max;

    // Create dialog texture, destroyed in deconstructor.
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET,
                                dimension.w, dimension.h);

    // TODO: set customized colors for the dialog.
    utility::fill_and_outline(renderer, texture, 1, {88, 88, 88, 150}, {255, 255, 255, 255});
  }
}

void Dialog::paint(SDL_Renderer *renderer, const SDL_Rect &clip)
{
  // Set alpha, the transparency of this dialog.
  // TODO: make this customizable through parameters when instantiated.
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(texture, 200);

  // Draw dialog frame.
  SDL_RenderCopy(renderer, texture, NULL, &dimension);

  // Draw components in dialog.
  for (auto c : comps)
    c.second->paint(renderer, dimension);
}

void Dialog::on_event(const SDL_Event &e, std::unordered_map<std::string, std::string> &event_data)
{
  // Forward events to components.
  for (auto c : comps)
    c.second->on_event(e, event_data);
}
