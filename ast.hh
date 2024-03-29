
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
#include<iostream>
#include<iomanip>
#include<typeinfo>
#include<list>

#define AST_SPACE "         "
#define AST_NODE_SPACE "            "
#define AST_SUB_NODE_SPACE "               "

using namespace std;

enum Relation_Op {LE,LT,GT,GE,EQ,NE };

class Ast;

class Ast
{
protected:
	typedef enum
	{
		zero_arity = 0,
		unary_arity = 1,
		binary_arity = 2
	}Ast_Arity;

	Data_Type node_data_type;
	Ast_Arity ast_num_child;

	int lineno;

public:
	Ast();
	~Ast();

	virtual Data_Type get_data_type();
	virtual bool check_ast();
	virtual Symbol_Table_Entry & get_symbol_entry();

	virtual void print(ostream & file_buffer) = 0;
	virtual void print_value(Local_Environment & eval_env, ostream & file_buffer);

	virtual Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	virtual void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	virtual Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer) = 0;
	virtual int get_block_number();

	virtual Code_For_Ast & compile() = 0;
	virtual Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra) = 0;
	virtual Code_For_Ast & create_store_stmt(Register_Descriptor * store_register);
};

class Assignment_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;

public:
	Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line);
	~Assignment_Ast();

	bool check_ast();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Name_Ast:public Ast
{
	Symbol_Table_Entry * variable_symbol_entry;

public:
	Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line);
	~Name_Ast();

	Data_Type get_data_type();
	Symbol_Table_Entry & get_symbol_entry();

	void print(ostream & file_buffer);

	void print_value(Local_Environment & eval_env, ostream & file_buffer);
	Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
	Code_For_Ast & create_store_stmt(Register_Descriptor * store_register);
};

template <class T>
class Number_Ast:public Ast
{
	T constant;

public:
	Number_Ast(T number, Data_Type constant_data_type, int line);
	~Number_Ast();

	Data_Type get_data_type();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Return_Ast:public Ast
{

public:
	Return_Ast(int line);
	~Return_Ast();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};


class Relational_Expr_Ast:public Ast{
	Ast* lhs;
	Ast* rhs;
	Relation_Op op;
public:
	Relational_Expr_Ast(Ast* temp_lhs, Ast* temp_rhs, Relation_Op temp_op, int line);
	~Relational_Expr_Ast();

	Data_Type get_data_type();
	bool check_ast();

	Relation_Op get_relational_op();
	void print(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment& eval_env, ostream& file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);

};


class Goto_Stmt_Ast: public Ast {
	int block_number;
public:
	Goto_Stmt_Ast(int temp_block_num, int line);
	~Goto_Stmt_Ast();

	int get_block_number();
	void print(ostream& file_buffer);
	Eval_Result & evaluate(Local_Environment& eval_env, ostream& file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class If_Else_Stmt_Ast:public Ast
{
	Ast * condition;
	Ast * true_goto;
	Ast * false_goto;
public:
	If_Else_Stmt_Ast(Ast * condition_temp, Ast * true_goto_temp, Ast * false_goto_temp, int line);
	~If_Else_Stmt_Ast();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};


class Plus_Ast:public Ast{
	Ast* lhs;
	Ast* rhs;
public:
	Plus_Ast(Ast * temp_lhs, Ast * temp_rhs, int line);
	~Plus_Ast();

	void print(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	bool check_ast();
	Data_Type get_data_type();

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Minus_Ast:public Ast{
	Ast* lhs;
	Ast* rhs;
public:
	Minus_Ast(Ast * temp_lhs, Ast * temp_rhs, int line);
	~Minus_Ast();
	void print(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	bool check_ast();
	Data_Type get_data_type();

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};


class Multiplication_Ast:public Ast{
	Ast* lhs;
	Ast* rhs;
public:
	Multiplication_Ast(Ast * temp_lhs, Ast * temp_rhs, int line);
	~Multiplication_Ast();
	void print(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	bool check_ast();
	Data_Type get_data_type();

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Division_Ast:public Ast{
	Ast* lhs;
	Ast* rhs;
public:
	Division_Ast(Ast * temp_lhs, Ast * temp_rhs, int line);
	~Division_Ast();
	void print(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	bool check_ast();
	Data_Type get_data_type();

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Unary_Ast:public Ast{
	Ast* lhs;
public:
	Unary_Ast(Ast * temp_lhs, int line);
	~Unary_Ast();
	void print(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	bool check_ast();
	Data_Type get_data_type();

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class TypeCast_Ast:public Ast{
	Ast* lhs;
public:
	TypeCast_Ast(Ast * temp_lhs, Data_Type temp_data_type, int line);
	~TypeCast_Ast();
	void print(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	Data_Type get_data_type();

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};
#endif
