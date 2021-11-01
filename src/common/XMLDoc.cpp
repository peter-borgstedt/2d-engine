#include "XMLDoc.h"
#include "Common.h"
#include "Debug.h"

#include <fstream>
#include <sstream>

using namespace XML;
using namespace std;

/* Private module methods. */
static void getValueAndFirstOpenAngledBracket(const string &content, string::iterator &it,
                                              XMLNode *parent)
{
  char curr_ch;
  string node_value; // Node string value
  while (it != content.end()) {
    if ((curr_ch = *it++) == '<') {
      if (parent) {
        parent->add_value(trim(node_value));
      }
      break;
    }
    node_value.push_back(curr_ch);
  }
}

static XMLNode *getNode(const string &content, string::iterator &it, XMLNode *parent)
{
  char curr_ch, prev_ch; // Current and previous character
  string node_content;   // Node string content
  getValueAndFirstOpenAngledBracket(content, it, parent);

  XMLNode *node = nullptr;
  while (it != content.end()) {
    if ((curr_ch = *it++) == '>') {
      // Retrieve name then remove from attributes, as this will be redundant in
      // the class instance.
      string name = node_content.substr(0, node_content.find(" "));
      node_content.erase(0, name.length()); // TODO: is this correct erase?

      if (parent && name.compare(string("/" + parent->get_id())) == 0) {
        return NULL; // End of angled bracket, no more children.
      }
      node = new XMLNode(name, node_content, parent);

      if (!prev_ch || prev_ch != '/') {
        while (XMLNode *child = getNode(content, it, node))
          node->add_child(child);
      }
      break;
    }
    node_content.push_back(prev_ch = curr_ch);
  }
  return node;
}

static XMLDoc *parseString(string &content)
{
  return new XMLDoc(getNode(content, *&content.begin(), nullptr));
}

static void setPaths(string path, unordered_map<string, vector<XMLNode *>> &paths, XMLNode *node)
{
  const string key = path += node->get_id() + "\\";
  if (paths.count(key)) {
    paths[key].push_back(node);
  } else {
    (paths[key] = vector<XMLNode *>()).push_back(node);
  }

  for (XMLNode *child : node->get_all_children())
    setPaths(path, paths, child);
}

/* XMLDocument */
XMLDoc::XMLDoc(XMLNode *root) : root(root)
{
  setPaths(string(), paths, root);
  debug::analyser.type_constructed(this);
}

XMLDoc::~XMLDoc()
{
  delete root;
  debug::analyser.type_deconstructed(this);
}

vector<XMLNode *> XML::XMLDoc::get(string path)
{
  return paths.find(path) == paths.end() ? vector<XMLNode *>() : paths.at(path); // TODO: Redo
}

XMLDoc *XML::parse_file(string file_path)
{
  ifstream in(file_path);
  string content;
  if (in.is_open()) {
    stringstream buffer;
    buffer << in.rdbuf();
    content = string(buffer.str());
  } else {
    throw "Cannot open file: " + file_path;
  }
  in.close();

  // TODO: parse data directly from instead of retrieving all data and then
  // parse the string.
  if (!content.empty()) {
    return parseString(content);
  }
  return nullptr;
}
