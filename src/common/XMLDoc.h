#ifndef XMLDOC_H
#define XMLDOC_H

#include "XMLNode.h"

namespace XML
{
  /*
   * A XMLDoc contain the root (top) node and have indexed paths to each node in the tree
   * for fast lookup and data retrieving. It is basically a wrapper around the root XMLNode.
   */
  class XMLDoc
  {
  private:
    XMLNode *root;
    std::unordered_map<std::string, std::vector<XMLNode *>> paths;

    XMLDoc(const XMLDoc &) = delete;                  // Do not use copy constructor.
    const XMLDoc &operator=(const XMLDoc &) = delete; // Do not use assignment operator.
  public:
    XMLDoc(XMLNode *root);
    ~XMLDoc();

    /*
     * Quick access to a data nodes contained in the XML tree.
     * A key uses the format: root/child-a/child-ab/...
     * I.e.
     * "root/" -> XMLNode(id: root)
     * "root/child-a/" -> returns XMLNode (with id child-a)
     * "root/child-a/child-aa" -> XMLNode (with id child-aa)
     * "root/child-a/child-ab" -> XMLNode (with id child-ab)
     */
    std::vector<XMLNode *> get(std::string path);
  };

  // Basically does what the function name indicates, parses a file (path) into a XMLDoc
  // which will contain all data in the form of nodes.
  XMLDoc *parse_file(std::string file_path);
} // namespace XML

#endif
