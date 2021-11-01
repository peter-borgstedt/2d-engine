#include "Debug.h"

#include <functional> // std::plus
#include <iomanip>
#include <numeric> // std::accumulate

using namespace std;

namespace debug
{
  MemoryAnalyser analyser;

  void MemoryAnalyser::print()
  {
    int constructed_sum = 0, deconstructed_sum = 0;

    for (auto p : constructed_types)
      constructed_sum += p.second;

    for (auto p : deconstructed_types)
      deconstructed_sum += p.second;

    cout << "\n- Memory statistics -";
    cout << "\n* Constructed " << constructed_sum << " instances from " << constructed_types.size()
         << " types";
    cout << "\n* Deconstructed " << deconstructed_sum << " instances from "
         << constructed_types.size() << " types" << endl;

    cout << endl << left << setfill(' ');
    cout << setw(27) << "Type name:";
    cout << setw(25) << "Constructed:";
    cout << setw(29) << "Deconstructed:";

    for (auto type : constructed_types) {
      int constructed = type.second;
      int deconstructed = deconstructed_types[type.first];
      cout << setw(25) << "\n[" + type.first + "]"
           << " | ";
      cout << setw(22) << constructed << " | ";
      cout << setw(29) << deconstructed;
    }
    cout << endl;
  }
} // namespace debug
