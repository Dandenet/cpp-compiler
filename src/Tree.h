#ifndef TREE_H
#define TREE_H

#include <string>
#include <memory>

#include "Types.h"

struct Node {

    std::string identifier;
    ObjectType  objType;
    DataType    dataType;
};

class TScaner;


class Tree
{
public:
    Tree(TScaner* scaner);

    ~Tree();

    Tree* GetCurrent();
    void MakeCurrent();

    const Node* Get() const;
    Node* Get();


    Tree* Add(const std::string& identifier, ObjectType type);
    Tree* AddBlock();
    Tree* SetDataType(DataType type);
    Tree* SetCustomDataType(const std::string& identifier);

    DataType ExpressionResultType(DataType operand1, DataType operand2, LexType type);

    Tree* FindIdentifier(const std::string& identifier);
    Tree* FindMember(const std::string& identifier);

    static DataType DataTypeFromLexem(LexType type);

    void CheckAssignableFrom(Tree* from);
    void CheckAssignableFrom(DataType type);

    void CheckForIntegralType(DataType type);

    Tree* GetMutableElement() const;
    void setMutableElement(Tree *newMutableElement);

private:
    Tree* m_Parent;
    Tree* m_Left;
    Tree* m_Right;

    Node* m_Node;
    Tree* m_MutableElement;

    TScaner* m_Scaner;


    // share pointer to current tree between the all children
    std::shared_ptr<Tree*> m_Current;


    Tree(Tree* parent, Tree* left, Tree* right, const Node* node,
         const std::shared_ptr<Tree*>& current, TScaner* scaner);

    Tree* InsertLeft(const Node* node);
    Tree* InsertRight(const Node* node);

    static Tree* FindUp(Tree* from, const std::string& identifier);
    static Tree* FindOneLevel(Tree* from, const std::string& identifier);
    static bool  IsDuplicate(Tree* from, const std::string& identifier);
};





#include "Tree.inl"
#endif // TREE_H
