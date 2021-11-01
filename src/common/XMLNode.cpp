#include "XMLNode.h"
#include "Common.h"
#include "Debug.h"
#include <regex>

using namespace XML;
using namespace std;

// Use static to limit scope visibility of functions and variables to this file. C-technique, so
// this is not pretty if we are going to be writing in pure c++ style. But this method should not
// be part of the class, it is a help method and therefor only needed in the namespace privately.

static const regex delimiter("\"\\s|=\"|\"");
static const sregex_token_iterator end_of_Sequence;

static unordered_map<string, string> getAttributes(string &node_content)
{
  // NOTE: regular expression may not have the best performance but in this case it give us
  // more benefits (compact code) than its disadvantage. Speed is not the essential part of
  // this parser. Removing this and implementing it by "hand" will probably give some
  // performance boost.
  sregex_token_iterator token(node_content.begin(), node_content.end(), delimiter, -1);
  unordered_map<string, string> attributes;
  while (token != end_of_Sequence) {
    string key = *token++;
    if (token != end_of_Sequence) {
      attributes[trim(key)] = *token++;
    }
  }
  return attributes;
}

XMLNode::XMLNode(string id, string node_content, XMLNode *parent)
{
  debug::analyser.type_constructed(this);
  XMLNode::id = id;
  XMLNode::parent = parent;
  XMLNode::attributes = getAttributes(node_content);
}

XMLNode::~XMLNode()
{
  debug::analyser.type_deconstructed(this);
  // No need to delete parent, it has already been trigged and is actually the stimuli for this
  // deconstructor being run. All deconstruction is done higher up in the hierarchy.
  for (XMLNode *node : children)
    delete node; // Will trigger the deconstructor for all children and their children and so on.
}

void XMLNode::add_child(XMLNode *node)
{
  children.push_back(node);
}

void XMLNode::add_value(const std::string &value)
{
  XMLNode::value = value;
}

string XMLNode::get_id()
{
  return id;
}

string XMLNode::get_value()
{
  return value;
}

vector<XMLNode *> XMLNode::get_all_children()
{
  return children;
}

std::vector<XMLNode *> XMLNode::get_children(const std::string &id)
{
  vector<XMLNode *> children;
  for (auto child : XMLNode::children)
    if (to_upper(child->get_id()).compare(to_upper(id)) == 0) {
      children.push_back(child);
    }
  return children;
}

XMLNode *XMLNode::get_first_accounted_child(const std::string &id)
{
  for (auto child : XMLNode::children)
    if (to_upper(child->get_id()).compare(to_upper(id)) == 0) {
      return child;
    }
  return nullptr;
}

bool XMLNode::contain_attribute(const std::string &key) const
{
  return attributes.count(key) != 0;
}

std::string XMLNode::get_attribute(const std::string &key) const
{
  return attributes.at(key);
}

int XMLNode::get_int_attribute(const std::string &key) const
{
  // Message to Jozef:
  // If you use MinGW GCC 4.7.2 to compile, stoi wont work without
  // the patch (found in the link below), or you could active the
  // code snippet below.

  // Keep code for backup, see: http://stackoverflow.com/q/16132176
  // istringstream buffer(get_attribute(id));
  // int value;
  // buffer >> value;
  // return value;

  // Ref: http://en.cppreference.com/w/cpp/string/basic_string/stol
  // Instead of using atoi (c specific) or iostream to cast int, we use
  // std::stoi string function (c++11), which is a new approach for
  // casting string to int.
  return stoi(get_attribute(key));
}
