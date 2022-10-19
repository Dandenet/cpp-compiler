#include "Parser.h"

TParser::TParser(TScaner* scaner) : 
	m_Scaner(scaner) 
{}


void TParser::Parse()
{
	DescriptionDiagram();
}

void TParser::AndExprDiagram()
{
	EqualExprDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (type == TypeAnd) {

		type = m_Scaner->Scan(nullptr);

		EqualExprDiagram();

		type = m_Scaner->PeekNext(nullptr);
	}
}

void TParser::ExprDiagram()
{
	AndExprDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (type == TypeOr) {

		type = m_Scaner->Scan(nullptr);
		AndExprDiagram();
		type = m_Scaner->PeekNext(nullptr);
	}
}

void TParser::EqualExprDiagram()
{
	CmpExprDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (type == TypeEQ || type == TypeNE) {

		type = m_Scaner->Scan(nullptr);
		CmpExprDiagram();
		type = m_Scaner->PeekNext(nullptr);
	}
}

void TParser::CmpExprDiagram()
{
	AddExprDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (	type == TypeGT || type == TypeLT || 
			type == TypeGE || type == TypeLE) {

		type = m_Scaner->Scan(nullptr);
		AddExprDiagram();

		type = m_Scaner->PeekNext(nullptr);
	}
}

void TParser::AddExprDiagram()
{
	MulExprDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (type == TypePlus || type == TypeMinus) {

		type = m_Scaner->Scan(nullptr);
		MulExprDiagram();
		type = m_Scaner->PeekNext(nullptr);
	}
}

void TParser::MulExprDiagram()
{
	UnaryExpDiagram();

	LexType type = m_Scaner->PeekNext(nullptr);

	while (type == TypeMul || type == TypeDiv || type == TypeMod) {

		type = m_Scaner->Scan(nullptr);
		UnaryExpDiagram();

		type = m_Scaner->PeekNext(nullptr);
	}
}



void TParser::UnaryExpDiagram()
{
	LexType type = m_Scaner->PeekNext(nullptr);
	if (type == TypePlus || type == TypeMinus)
		type = m_Scaner->Scan(nullptr);

	ElementoryExprDiagram();
}

void TParser::ElementoryExprDiagram()
{
	LexType type = m_Scaner->PeekNext(nullptr);

	switch (type)
	{
	case TypeConst:
		type = m_Scaner->Scan(nullptr);
		break;

	case TypeIdent:
		NameDiagram();
		break;

	case TypeExp:
		type = m_Scaner->Scan(nullptr);
		break;

	case TypeLBracket:
		type = m_Scaner->Scan(nullptr);
		ExprDiagram();
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
}


void TParser::NameDiagram()
{
	LexType type;
	TScaner::State state;

	do {

		type = m_Scaner->Scan(nullptr);
		if (type != TypeIdent) {
			m_Scaner->PrintError("Expected a member name");
			exit(0);
		}

		state = m_Scaner->SaveState();
		type = m_Scaner->Scan(nullptr);

	} while (type == TypeDot);
	m_Scaner->Restore(state);
}

void TParser::DataDiagram()
{
	LexType type = m_Scaner->Scan(nullptr);

	if (type != TypeInt && type != TypeShort &&
		type != TypeLong && type != TypeDouble && type != TypeIdent) {
		m_Scaner->PrintError("Expected a type");
		exit(0);
	}

	do
	{
		type = m_Scaner->Scan(nullptr);

		if (type != TypeIdent) {
			m_Scaner->PrintError("Expected an identifier");
			exit(0);
		}

		type = m_Scaner->Scan(nullptr);

		// parse an assignment expression
		if (type == TypeAssign) {
			ExprDiagram();
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

		if (type == TypeInt || type == TypeShort ||
			type == TypeLong || type == TypeDouble || type == TypeIdent) {

			DataDiagram();
		}
		else if (type == TypeClass) {
			ClassDiagram();

		}
		else if (type == TypeVoid) {
			FunctionDiagram();
		}
		else {
			m_Scaner->PrintError("Expected a declaration");
			exit(0);
		}

		type = m_Scaner->PeekNext(nullptr);

	} while (type == TypeInt || type == TypeShort ||
		type == TypeLong || type == TypeDouble ||
		type == TypeIdent || type == TypeClass ||
		type == TypeVoid);
}

void TParser::ClassDiagram()
{
	LexType type = m_Scaner->Scan(nullptr);
	if (type != TypeClass) {
		m_Scaner->PrintError("Expected a 'class'");
		exit(0);
	}

	type = m_Scaner->Scan(nullptr);
	if (type != TypeIdent) {
		m_Scaner->PrintError("Expected an identifier");
		exit(0);
	}

	type = m_Scaner->Scan(nullptr);
	if (type != TypeLParenthesis) {
		m_Scaner->PrintError("Expected a '{'");
		exit(0);
	}

	DescriptionDiagram();

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
	LexType type = m_Scaner->Scan(nullptr);
	if (type != TypeVoid) {
		m_Scaner->PrintError("Expected a 'void'");
		exit(0);
	}

	type = m_Scaner->Scan(nullptr);
	if (type != TypeIdent && type != TypeMain) {
		m_Scaner->PrintError("Expected an identifier");
		exit(0);
	}

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
		type = m_Scaner->Scan(nullptr);

	}
	else if (type == TypeLParenthesis) {
		CompoundOperatorDiagram();

	} else if (type == TypeWhile) {
		WhileOperatorDiagram();

	} else if (type == TypeIdent) {
		NameDiagram();

		type = m_Scaner->Scan(nullptr);
		if (type == TypeAssign) {
			ExprDiagram();

		} else if (type == TypeLBracket) {
			
			type = m_Scaner->Scan(nullptr);
			if (type != TypeRBracket) {
				m_Scaner->PrintError("Expected a ')'");
				exit(0);
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
	
	OperatorDescriptionDiagram();

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
			auto next = m_Scaner->Scan(nullptr);
			next = m_Scaner->Scan(nullptr);

			if (next != TypeDot && next != TypeAssign && next != TypeLBracket)
				isType = true;

			m_Scaner->Restore(state);
		}


		if (isType || type == TypeInt || type == TypeLong ||
			type == TypeShort || type == TypeDouble) {
			DataDiagram();

		} else if (type == TypeIdent || type == TypeSemicolon || type == TypeWhile) {
			OperatorDiagram();

		}

		type = m_Scaner->PeekNext(nullptr);
	} while (type == TypeIdent || type == TypeInt || type == TypeLong || 
			 type == TypeShort || type == TypeDouble || type == TypeSemicolon ||
			 type == TypeWhile);
}


void TParser::WhileOperatorDiagram()
{
	LexType type = m_Scaner->Scan(nullptr);

	type = m_Scaner->Scan(nullptr);
	if (type != TypeLBracket) {
		m_Scaner->PrintError("Expected a '('");
		exit(0);
	}

	ExprDiagram();

	type = m_Scaner->Scan(nullptr);
	if (type != TypeRBracket) {
		m_Scaner->PrintError("Expected a ')'");
		exit(0);
	}

	OperatorDiagram();
}