#ifndef XMLNODE_H
#define XMLNODE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace XML
{
  /*
   * A XMLNode represent a XML angle bracket (<node attribute="1234">hello</node> or <node/>)
   * in a XML data structure. A node contains id, attributes, a value, children and parent.
   *
   * An example, the following:
   * <foo attributeA="A", attributeB="B">
   *  <bar/>
   *  <content>hello world</content>
   * </foo>
   *
   * Will give following:
   * [XMLNode=ID: "foo",
   *          ATTRIBUTES: {{attributeA, A}, {attributeB, B}},
   *          VALUE: "",
   *          PARENT: nullptr,
   *          CHILDREN:
   *          {
   *            [XMLNode=ID: "bar",
   *                     ATTRIBUTES: {},
   *                     VALUE: "",
   *                     PARENT: foo,
   *                     CHILDREN: {}],
   *            [XMLNode=ID: "content",
   *                     ATTRIBUTES: {},
   *                     VALUE: "hello world",
   *                     PARENT: foo,
   *                     CHILDREN: {}]
   *          }
   * ]
   */
  class XMLNode
  {
  private:
    std::string id;
    std::unordered_map<std::string, std::string> attributes;
    std::string value;

    XMLNode *parent;
    std::vector<XMLNode *> children;

    XMLNode(const XMLNode &) = delete;                  // Do not use copy constructor.
    const XMLNode &operator=(const XMLNode &) = delete; // Do not use assignment operator.
  public:
    XMLNode(std::string id, std::string node_content, XMLNode *parent);
    ~XMLNode();

    /* Add a child node to this node. */
    void add_child(XMLNode *node);
    // Add a value value to this node.
    void add_value(const std::string &value);
    // Get the id representing this node.
    std::string get_id();
    // Get value contained in this node.
    std::string get_value();
    // Get all children in this node.
    std::vector<XMLNode *> get_all_children();
    // Get children with a specific id.
    std::vector<XMLNode *> get_children(const std::string &id);
    // Get first node that occur containing a specific id.
    XMLNode *get_first_accounted_child(const std::string &id);
    // Validate whether an attribute for a given key exists.
    bool contain_attribute(const std::string &key) const;
    // Retrieve an attribute with given key.
    std::string get_attribute(const std::string &key) const;
    // Retrieve an attribute with given key casted to an int.
    int get_int_attribute(const std::string &key) const;
  };
}; // namespace XML

#endif
