
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors  (theory and  lab)  at  IIT
           Bombay.

           Release  date  Jan  15, 2013.  Copyrights  reserved  by  Uday
           Khedker. This  implemenation  has been made  available purely
           for academic purposes without any warranty of any kind.

           Documentation (functionality, manual, and design) and related
           tools are  available at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

#ifndef AST_HH
#define AST_HH

#include<string>
#include <vector>

#define AST_SPACE "         "
#define AST_NODE_SPACE "            "

enum Relation_Op {LE,LT,GT,GE,EQ,NE };

using namespace std;

class Ast;

#include "basic-block.hh"
#include "procedure.hh"

class Ast
{
protected:
	Data_Type node_data_type;
public:
	Ast();
	~Ast();

	virtual Data_Type get_data_type();
	virtual bool check_ast(int line);

	virtual void print_ast(ostream & file_buffer) = 0;
	virtual void print_value(Local_Environment & eval_env, ostream & file_buffer);

	virtual Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	virtual void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	virtual Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer) = 0;

	virtual int get_block_number();
};

class Assignment_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;

public:
	Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs);
	~Assignment_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Relational_Expr_Ast:public Ast{
	Ast* lhs;
	Ast* rhs;
	Relation_Op op;
public:
	Relational_Expr_Ast(Ast* temp_lhs, Ast* temp_rhs, Relation_Op temp_op);
	~Relational_Expr_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);

	Relation_Op get_relational_op();
	void print_ast(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment& eval_env, ostream& file_buffer);

};


class Goto_Stmt_Ast: public Ast {
	int block_number;
public:
	Goto_Stmt_Ast(int temp_block_num);
	~Goto_Stmt_Ast();

	int get_block_number();
	void print_ast(ostream& file_buffer);
	Eval_Result & evaluate(Local_Environment& eval_env, ostream& file_buffer);	
};

class Name_Ast:public Ast
{
	string variable_name;
	Symbol_Table_Entry variable_symbol_entry;

public:
	Name_Ast(string & name, Symbol_Table_Entry & var_entry);
	~Name_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	void print_value(Local_Environment & eval_env, ostream & file_buffer);
	Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

template <class T>
class Number_Ast:public Ast
{
	T constant;

public:
	Number_Ast(T number, Data_Type constant_data_type);
	~Number_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	// T get_constant();
};

class Return_Ast:public Ast
{
	Ast* expression;
	Procedure* procedure;
public:
	Return_Ast(Ast* expr,Procedure* p);
	~Return_Ast();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class If_Else_Stmt_Ast:public Ast
{
	Ast * condition;
	Ast * true_goto;
	Ast * false_goto;
public:
	If_Else_Stmt_Ast(Ast * condition_temp, Ast * true_goto_temp, Ast * false_goto_temp);
	~If_Else_Stmt_Ast();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};


class Plus_Ast:public Ast{
	Ast* lhs;
	Ast* rhs;
public:
	Plus_Ast(Ast * temp_lhs, Ast * temp_rhs);
	~Plus_Ast();

	void print_ast(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	bool check_ast(int line);
	Data_Type get_data_type();
};

class Minus_Ast:public Ast{
	Ast* lhs;
	Ast* rhs;
public:
	Minus_Ast(Ast * temp_lhs, Ast * temp_rhs);
	~Minus_Ast();
	void print_ast(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	bool check_ast(int line);
	Data_Type get_data_type();
};


class Multiplication_Ast:public Ast{
	Ast* lhs;
	Ast* rhs;
public:
	Multiplication_Ast(Ast * temp_lhs, Ast * temp_rhs);
	~Multiplication_Ast();
	void print_ast(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	bool check_ast(int line);
	Data_Type get_data_type();
};

class Division_Ast:public Ast{
	Ast* lhs;
	Ast* rhs;
public:
	Division_Ast(Ast * temp_lhs, Ast * temp_rhs);
	~Division_Ast();
	void print_ast(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	bool check_ast(int line);
	Data_Type get_data_type();
};

class Unary_Ast:public Ast{
	Ast* lhs;
public:
	Unary_Ast(Ast * temp_lhs);
	~Unary_Ast();
	void print_ast(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	bool check_ast(int line);
	Data_Type get_data_type();
};

class TypeCast_Ast:public Ast{
	Ast* lhs;
public:
	TypeCast_Ast(Ast * temp_lhs, Data_Type temp_data_type);
	~TypeCast_Ast();
	void print_ast(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	Data_Type get_data_type();
};

class Call_Ast:public Ast{
	Procedure* procedure;
	vector<Ast*> expression_list;
public:
	Call_Ast(Procedure* proc, vector<Ast*> expr_list);
	~Call_Ast();

	void print_ast(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	bool check_ast(int line);
	Data_Type get_data_type();
};

#endif
