#include "Tree.h"
#include "Scaner.h"
#include <format>


Tree::Tree(TScaner *scaner) :
    m_Parent(nullptr),  m_Left(nullptr),
    m_Right(nullptr),   m_Node(new Node {"", ObjectEmpty, Undefined}),
    m_MutableElement(nullptr),   m_Scaner(scaner),
    m_Current(new Tree * (nullptr))
{

}

Tree::Tree(Tree* parent, Tree* left, Tree* right, const Node* node,
           const std::shared_ptr<Tree*>& current, TScaner* scaner) :

    m_Parent(parent),   m_Left(left),
    m_Right(right),     m_Scaner(scaner),
    m_Current(current)

{
    m_Node = new Node;
    m_Node->identifier = node->identifier;
    m_Node->objType = node->objType;
    m_Node->dataType = node->dataType;
}


Tree::~Tree()
{
    delete m_Node;
    delete m_Left;

    if (m_Right && m_Right->m_Parent == this)
        delete m_Right;
}


Tree *Tree::InsertLeft(const Node *node)
{
    m_Left = new Tree(this, nullptr, nullptr, node, m_Current, m_Scaner);
    return m_Left;
}

Tree *Tree::InsertRight(const Node *node)
{
    m_Right = new Tree(this, nullptr, nullptr, node, m_Current, m_Scaner);
    return m_Right;
}

Tree *Tree::FindOneLevel(Tree *from, const std::string &identifier)
{
    for (auto i = from; i->m_Parent && i->m_Parent->m_Right != i; i = i->m_Parent)
        if (i->m_Node->identifier == identifier)
            return i;

    return nullptr;
}

Tree* Tree::FindUp(Tree* from, const std::string &identifier)
{
    for (Tree* i = from; i->m_Parent; i = i->m_Parent) {
        if (i->m_Node->identifier == identifier)
            return i;
    }

    return nullptr;
}


Tree *Tree::Add(const std::string &identifier, ObjectType type)
{
    Tree* tree = *m_Current;

    if (IsDuplicate(tree, identifier)) {
        m_Scaner->PrintError(std::format("Redifinition of '{}'", identifier));
    } else {

        Node node = {identifier, type, Undefined};
        tree->InsertLeft(&node)->MakeCurrent();

        return tree->m_Left;
    }

    return tree;
}

Tree *Tree::AddBlock()
{
    Tree* current = *m_Current;
    Node node = {"", ObjectEmpty, Undefined};

    Tree* newBlock = current->InsertRight(&node);

    return newBlock;

}

Tree *Tree::SetCustomDataType(const std::string &identifier)
{
    Tree* current = *m_Current;

    Tree* classType = FindUp(current, identifier);

    if (!classType) {
        m_Scaner->PrintError(std::format("Unknown type name '{}'", identifier));
    } else {
        current->m_Right = classType->m_Right;
        current->m_Node->dataType = DataTypeCustom;
    }


    return current;
}

DataType Tree::ExpressionResultType(DataType operand1, DataType operand2, LexType type)
{
    DataType dataType = Undefined;

    if (operand1 == DataTypeCustom || operand2 == DataTypeCustom)
        m_Scaner->PrintError("Invalid binary operation");
    else if ((operand1 == DataTypeDouble || operand2 == DataTypeDouble) && type == TypeMod)
        m_Scaner->PrintError("Operand has type 'double'");
    else if (operand1 != Undefined && operand2 != Undefined)
        dataType = operand1 > operand2 ? operand1 : operand2;

    return dataType;
}

Tree *Tree::FindMember(const std::string &identifier)
{
    Tree* current = *m_Current;

    for (auto* i = current->m_Right; i; i = i->m_Left) {
        if (i->m_Node->identifier == identifier) {
            i->MakeCurrent();
            return i;
        }
    }

    m_Scaner->PrintError(std::format("No member named '{}'", identifier));

    return nullptr;
}

DataType Tree::DataTypeFromLexem(LexType type)
{
    switch (type) {
    case TypeInt:
        return DataTypeInt;
    case TypeShort:
        return DataTypeShort;
    case TypeLong:
        return DataTypeLong;
    case TypeDouble:
        return DataTypeDouble;
    default:
        break;
    }

    return Undefined;
}


void Tree::CheckAssignableFrom(Tree *from)
{

    auto* node = from->Get();

    if (m_Node->objType == ObjectTypeClass && node->objType == ObjectTypeClass && from->m_Right != m_Right)
        m_Scaner->PrintError(std::format("cannot convert from '{}' to '{}'",
                                         from->m_Right->m_Parent->m_Node->identifier,
                                         m_Right->m_Parent->m_Node->identifier));

    else if (m_Node->dataType == DataTypeInt && node->objType == ObjectTypeClass)
        m_Scaner->PrintError(std::format("cannot convert from '{}' to 'int'",
                                         from->m_Right->m_Parent->m_Node->identifier));

    else if (m_Node->dataType == DataTypeLong && node->objType == ObjectTypeClass)
        m_Scaner->PrintError(std::format("cannot convert from '{}' to 'long'",
                                         from->m_Right->m_Parent->m_Node->identifier));

    else if (m_Node->dataType == DataTypeShort && node->objType == ObjectTypeClass)
        m_Scaner->PrintError(std::format("cannot convert from '{}' to 'short'",
                                         from->m_Right->m_Parent->m_Node->identifier));

    else if (m_Node->dataType == DataTypeDouble && node->objType == ObjectTypeClass)
        m_Scaner->PrintError(std::format("cannot convert from '{}' to 'double'",
                                         from->m_Right->m_Parent->m_Node->identifier));
}

void Tree::CheckAssignableFrom(DataType type)
{
    if (m_Node->objType == ObjectTypeClass && type == DataTypeInt)
        m_Scaner->PrintError(std::format("cannot convert from 'int' to '{}'",
                                         m_Right->m_Parent->m_Node->identifier));
    else if (m_Node->objType == ObjectTypeClass && type == DataTypeShort)
        m_Scaner->PrintError(std::format("cannot convert from 'short' to '{}'",
                                         m_Right->m_Parent->m_Node->identifier));
    else if (m_Node->objType == ObjectTypeClass && type == DataTypeLong)
        m_Scaner->PrintError(std::format("cannot convert from 'long' to '{}'",
                                         m_Right->m_Parent->m_Node->identifier));
    else if (m_Node->objType == ObjectTypeClass && type == DataTypeDouble)
        m_Scaner->PrintError(std::format("cannot convert from 'double' to '{}'",
                                         m_Right->m_Parent->m_Node->identifier));
    else if (m_Node->objType != ObjectTypeClass && m_Node->objType != ObjectVar)
                        m_Scaner->PrintError(std::format("'{}': Illegal left operand",
                                                         m_Node->identifier));

}

void Tree::CheckForIntegralType(DataType type)
{
    if (type == DataTypeCustom)
        m_Scaner->PrintError("a conditional expression of custom type is not valid");
}

Tree* Tree::GetMutableElement() const
{
    return m_MutableElement;
}

void Tree::setMutableElement(Tree *newMutableElement)
{
    m_MutableElement = newMutableElement;
}




Tree *Tree::FindIdentifier(const std::string &identifier)
{
    Tree* tree = FindUp(*m_Current, identifier);

    if (!tree)
        m_Scaner->PrintError(std::format("Unknown identifier '{}'", identifier));
    else
        tree->MakeCurrent();

    return tree;
}
