#include <format>


#include "Parser.h"



TParser::TParser(TScaner* scaner) : 
    m_Scaner(scaner), m_SemTree(m_Scaner)
{
    m_SemTree.MakeCurrent();
}


void TParser::Parse()
{
	DescriptionDiagram();
}


DataType TParser::AndExprDiagram()
{
    DataType dataType = EqualExprDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (type == TypeAnd) {

        (void)m_Scaner->Scan(nullptr);

        dataType = m_SemTree.ExpressionResultType(dataType, EqualExprDiagram(), type);

		type = m_Scaner->PeekNext(nullptr);
	}

    return dataType;
}



DataType TParser::ExprDiagram()
{
    DataType dataType = AndExprDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (type == TypeOr) {

        m_Scaner->Scan(nullptr);
        dataType = m_SemTree.ExpressionResultType(dataType, AndExprDiagram(), type);
        type = m_Scaner->PeekNext(nullptr);
    }

    return dataType;
}



DataType TParser::EqualExprDiagram()
{
    DataType dataType = CmpExprDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (type == TypeEQ || type == TypeNE) {

        m_Scaner->Scan(nullptr);
        dataType = m_SemTree.ExpressionResultType(dataType, CmpExprDiagram(), type);
		type = m_Scaner->PeekNext(nullptr);
	}

    return dataType;
}


DataType TParser::CmpExprDiagram()
{
    DataType dataType = AddExprDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (	type == TypeGT || type == TypeLT || 
			type == TypeGE || type == TypeLE) {

        m_Scaner->Scan(nullptr);
        dataType = m_SemTree.ExpressionResultType(dataType, AddExprDiagram(), type);
		type = m_Scaner->PeekNext(nullptr);
	}

    return dataType;
}



DataType TParser::AddExprDiagram()
{
    DataType dataType = MulExprDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (type == TypePlus || type == TypeMinus) {

        m_Scaner->Scan(nullptr);
        dataType = m_SemTree.ExpressionResultType(dataType, MulExprDiagram(), type);
		type = m_Scaner->PeekNext(nullptr);
	}

    return dataType;
}



DataType TParser::MulExprDiagram()
{
    DataType dataType = UnaryExpDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (type == TypeMul || type == TypeDiv || type == TypeMod) {

        m_Scaner->Scan(nullptr);
        dataType = m_SemTree.ExpressionResultType(dataType, UnaryExpDiagram(), type);
		type = m_Scaner->PeekNext(nullptr);
	}

    return dataType;
}

DataType TParser::UnaryExpDiagram()
{
	LexType type = m_Scaner->PeekNext(nullptr);
	if (type == TypePlus || type == TypeMinus)
        m_Scaner->Scan(nullptr);

    return ElementoryExprDiagram();
}


DataType TParser::ElementoryExprDiagram()
{
    DataType dataType = Undefined;
	LexType type = m_Scaner->PeekNext(nullptr);

    Tree* element = nullptr;

	switch (type)
	{
	case TypeConst:
        m_Scaner->Scan(nullptr);
        dataType = DataTypeLong;
		break;

    case TypeIdent:
        element = NameDiagram();

        if (element && element->Get()->objType != ObjectVar &&
                       element->Get()->objType != ObjectTypeClass)
                 m_Scaner->PrintError("illegal operand");

        else if (element)
            dataType = element->Get()->dataType;

        break;

	case TypeExp:
        m_Scaner->Scan(nullptr);
        dataType = DataTypeDouble;
		break;


	case TypeLBracket:
        m_Scaner->Scan(nullptr);
        dataType = ExprDiagram();
        type = m_Scaner->Scan(nullptr);
		if (type != TypeRBracket) {
			m_Scaner->PrintError("Expected a ')'");
			exit(0);
		}
		break;

	default:
		m_Scaner->PrintError("Expected an expression");
		exit(0);
		break;
    }

    m_SemTree.setMutableElement(element);

    return dataType;
}



Tree *TParser::NameDiagram()
{
    Tree* tmp = m_SemTree.GetCurrent();

    std::string identifier;
    LexType type = m_Scaner->Scan(&identifier);

    if (type != TypeIdent) {
        m_Scaner->PrintError("Expected a member name");
        exit(0);
    }

    // find name root
    Tree* member = m_SemTree.FindIdentifier(identifier);
    bool isInvalid = member == nullptr;
    identifier.clear();

    // find members
    type = m_Scaner->PeekNext(nullptr);
    while (type == TypeDot) {
       m_Scaner->Scan(nullptr);

       type = m_Scaner->Scan(&identifier);
       if (type != TypeIdent) {
           m_Scaner->PrintError("Expected a member name");
           exit(0);
       }

       if (!isInvalid) {
           member = m_SemTree.FindMember(identifier);
           isInvalid = (member == nullptr);
       }

       identifier.clear();

       type = m_Scaner->PeekNext(nullptr);
    }


    tmp->MakeCurrent();

    return member;
}



void TParser::DataDiagram()
{
    std::string identifier;

    // check data type to existence
    std::string typeName;
    LexType dataType = m_Scaner->Scan(&typeName);

    LexType type;
	do
	{
        type = m_Scaner->Scan(&identifier);
		if (type != TypeIdent) {
			m_Scaner->PrintError("Expected an identifier");
			exit(0);
		}

        Tree* newElement;
        if (dataType == TypeIdent)
            newElement = m_SemTree.Add(identifier, ObjectTypeClass)->SetCustomDataType(typeName);
        else
            newElement = m_SemTree.Add(identifier, ObjectVar)->SetDataType(Tree::DataTypeFromLexem(dataType));



		// parse an assignment expression
        type = m_Scaner->Scan(nullptr);
		if (type == TypeAssign) {
            auto result = ExprDiagram();
            if (result == DataTypeCustom)
                newElement->CheckAssignableFrom(m_SemTree.GetMutableElement());
            else
                newElement->CheckAssignableFrom(result);

			type = m_Scaner->Scan(nullptr);
		}


	} while (type == TypeComma);

	if (type != TypeSemicolon) {
        m_Scaner->PrintError("Expected a ';'");
		exit(0);
	}	
}


void TParser::DescriptionDiagram()
{	
	LexType type;
	do {

		type = m_Scaner->PeekNext(nullptr);

        if (type == TypeInt    || type == TypeShort  || type == TypeLong ||
            type == TypeDouble || type == TypeIdent) {
			DataDiagram();

        } else if (type == TypeClass) {
			ClassDiagram();

        } else if (type == TypeVoid) {
			FunctionDiagram();

        } else {
			m_Scaner->PrintError("Expected a declaration");
			exit(0);
		}

		type = m_Scaner->PeekNext(nullptr);

    } while (type == TypeInt   || type == TypeShort  ||
             type == TypeLong  || type == TypeDouble ||
             type == TypeIdent || type == TypeClass  ||
             type == TypeVoid);
}

void TParser::ClassDiagram()
{

    m_Scaner->Scan(nullptr); // skip 'class' lexem

    std::string identifier;
    LexType type = m_Scaner->Scan(&identifier);
	if (type != TypeIdent) {
		m_Scaner->PrintError("Expected an identifier");
		exit(0);
    }

    m_SemTree.Add(identifier, ObjectClass);

	type = m_Scaner->Scan(nullptr);
	if (type != TypeLParenthesis) {
		m_Scaner->PrintError("Expected a '{'");
		exit(0);
    }


    Tree* tmpTree = m_SemTree.GetCurrent();
    m_SemTree.AddBlock()->MakeCurrent();

	DescriptionDiagram();

    tmpTree->MakeCurrent();


	type = m_Scaner->Scan(nullptr);
	if (type != TypeRParenthesis) {
		m_Scaner->PrintError("Expected a '}'");
		exit(0);
	}


	type = m_Scaner->Scan(nullptr);
	if (type != TypeSemicolon) {
		m_Scaner->PrintError("Expected a ';'");
		exit(0);
	}
}

void TParser::FunctionDiagram()
{
    m_Scaner->Scan(nullptr); // skip void lexem

    std::string name;
    LexType type = m_Scaner->Scan(&name);
	if (type != TypeIdent && type != TypeMain) {
		m_Scaner->PrintError("Expected an identifier");
		exit(0);
	}

    m_SemTree.Add(name, ObjectFunc);

	type = m_Scaner->Scan(nullptr);
	if (type != TypeLBracket) {
		m_Scaner->PrintError("Expected a '('");
		exit(0);
	}


	type = m_Scaner->Scan(nullptr);
	if (type != TypeRBracket) {
		m_Scaner->PrintError("Expected a ')'");
		exit(0);
	}

	CompoundOperatorDiagram();
}


void TParser::OperatorDiagram()
{
	LexType type = m_Scaner->PeekNext(nullptr);

	if (type == TypeSemicolon) {
        m_Scaner->Scan(nullptr);

    } else if (type == TypeLParenthesis) {
		CompoundOperatorDiagram();

	} else if (type == TypeWhile) {
        WhileOperatorDiagram();

	} else if (type == TypeIdent) {
        Tree* member = NameDiagram();

		type = m_Scaner->Scan(nullptr);
		if (type == TypeAssign) {
            DataType dataType = ExprDiagram();

            // check assignabiltity
            if (member && dataType == DataTypeCustom)
                member->CheckAssignableFrom(m_SemTree.GetMutableElement());
            else if (member) {
                member->CheckAssignableFrom(dataType);
            }

		} else if (type == TypeLBracket) {
			
			type = m_Scaner->Scan(nullptr);
			if (type != TypeRBracket) {
				m_Scaner->PrintError("Expected a ')'");
				exit(0);
			}

            if (member && member->Get()->objType != ObjectFunc) {
                m_Scaner->PrintError("term does not evaluate to a function taking 0 arguments");
            }

		} else {
			m_Scaner->PrintError("Expected a ';'");
			exit(0);
        }

		type = m_Scaner->Scan(nullptr);
		if (type != TypeSemicolon) {
			m_Scaner->PrintError("Expected a ';'");
			exit(0);
		}
	}
}

void TParser::CompoundOperatorDiagram()
{
	LexType type = m_Scaner->Scan(nullptr);
	if (type != TypeLParenthesis) {
		m_Scaner->PrintError("Expected a '{'");
		exit(0);
	}
	
    auto tmpTree = m_SemTree.GetCurrent();
    m_SemTree.AddBlock()->MakeCurrent();

    OperatorDescriptionDiagram();

    tmpTree->MakeCurrent();

	type = m_Scaner->Scan(nullptr);
	if (type != TypeRParenthesis) {
		m_Scaner->PrintError("Expected a '}'");
		exit(0);
	}
}


void TParser::OperatorDescriptionDiagram()
{
	LexType type = m_Scaner->PeekNext(nullptr);
	do {

        bool isType = false;
		if (type == TypeIdent) {
			auto state = m_Scaner->SaveState();
            m_Scaner->Scan(nullptr);
            auto next = m_Scaner->Scan(nullptr);

			if (next != TypeDot && next != TypeAssign && next != TypeLBracket)
				isType = true;

			m_Scaner->Restore(state);
		}


		if (isType || type == TypeInt || type == TypeLong ||
            type == TypeShort || type == TypeDouble)
			DataDiagram();

        else if (type == TypeIdent || type == TypeSemicolon || type == TypeWhile)
			OperatorDiagram();

		type = m_Scaner->PeekNext(nullptr);
	} while (type == TypeIdent || type == TypeInt || type == TypeLong || 
			 type == TypeShort || type == TypeDouble || type == TypeSemicolon ||
			 type == TypeWhile);
}


void TParser::WhileOperatorDiagram()
{
    m_Scaner->Scan(nullptr); // skip 'while' lexem

    LexType type = m_Scaner->Scan(nullptr);
	if (type != TypeLBracket) {
		m_Scaner->PrintError("Expected a '('");
		exit(0);
	}

    m_SemTree.CheckForIntegralType(ExprDiagram());

	type = m_Scaner->Scan(nullptr);
	if (type != TypeRBracket) {
		m_Scaner->PrintError("Expected a ')'");
		exit(0);
	}

	OperatorDiagram();
}
