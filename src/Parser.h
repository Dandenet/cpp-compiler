#ifndef PARSER_H
#define PARSER_H

#include "Scaner.h"


class TParser 
{
public:
	TParser(TScaner* scaner);

	void Parse();

private:
	TScaner* m_Scaner;

	void AndExprDiagram();
	void ExprDiagram();
	void EqualExprDiagram();
	void CmpExprDiagram();
	void AddExprDiagram();
	void MulExprDiagram();
	void ElementoryExprDiagram();
	void UnaryExpDiagram();

	void DescriptionDiagram();
	void DataDiagram();
	void ClassDiagram();
	void FunctionDiagram();


	void OperatorDiagram();
	void OperatorDescriptionDiagram();
	void CompoundOperatorDiagram();
	void WhileOperatorDiagram();

	void NameDiagram();
};

#endif // !PARSER_H
