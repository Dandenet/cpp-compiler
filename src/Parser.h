#ifndef PARSER_H
#define PARSER_H

#include "Scaner.h"
#include "Tree.h"


class TParser 
{
public:
    TParser(TScaner* scaner);


	void Parse();

private:
    TScaner* m_Scaner;
    Tree     m_SemTree;


    DataType AndExprDiagram();
    DataType ExprDiagram();
    DataType EqualExprDiagram();
    DataType CmpExprDiagram();
    DataType AddExprDiagram();
    DataType MulExprDiagram();
    DataType ElementoryExprDiagram();
    DataType UnaryExpDiagram();
    Tree*    NameDiagram();


    void DescriptionDiagram();
	void DataDiagram();
	void ClassDiagram();
	void FunctionDiagram();


	void OperatorDiagram();
	void OperatorDescriptionDiagram();
	void CompoundOperatorDiagram();
	void WhileOperatorDiagram();

};

#endif // !PARSER_H
