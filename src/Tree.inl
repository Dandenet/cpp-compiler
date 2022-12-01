inline const Node *Tree::Get() const
{
    return m_Node;
}

inline Node *Tree::Get()
{
    return m_Node;
}


inline Tree *Tree::GetCurrent()
{
    return *m_Current;
}

inline void Tree::MakeCurrent()
{
    *m_Current = this;
}

inline Tree *Tree::SetDataType(DataType type)
{
    m_Node->dataType = type;

    return this;
}


inline bool Tree::IsDuplicate(Tree *from, const std::string &identifier) {

    return FindOneLevel(from, identifier) != nullptr;
}
