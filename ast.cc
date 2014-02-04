
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

#include<iostream>
#include<fstream>

using namespace std;

#include"user-options.hh"
#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"

string Op_Array[]={"LE","LT","GT","GE","EQ","NE"};

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast(int line)
{
	report_internal_error("Should not reach, Ast : check_ast");
}

Data_Type Ast::get_data_type()
{
	report_internal_error("Should not reach, Ast : get_data_type");
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	report_internal_error("Should not reach, Ast : print_value");
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	report_internal_error("Should not reach, Ast : get_value_of_evaluation");
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	report_internal_error("Should not reach, Ast : set_value_of_evaluation");
}
int Ast::get_block_number()
{
	report_internal_error("Should not reach, Ast : get_block_number");
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Assignment_Ast::get_data_type()
{
	return node_data_type;
}

bool Assignment_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Assignment statement data type not compatible", line);
}

void Assignment_Ast::print_ast(ostream & file_buffer)
{
	file_buffer  <<AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE"LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")\n";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	if (result.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print_ast(file_buffer);

	lhs->print_value(eval_env, file_buffer);
	Eval_Result & zero_result = *(new Eval_Result_BB());
	zero_result.set_value(0);
	return zero_result;
}




////////////////////////////////////////////////////////
Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry)
{
	variable_name = name;
	variable_symbol_entry = var_entry;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry.get_data_type();
}

void Name_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_name;
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result_Value * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result_Value * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		if (loc_var_val->get_result_enum() == int_result)
			file_buffer << loc_var_val->get_value() << "\n";
		else
			report_internal_error("Result type can only be int and float");
	}

	else
	{
		if (glob_var_val->get_result_enum() == int_result)
		{
			if (glob_var_val == NULL)			//TODO may be wrong
				file_buffer << "0\n";
			else
				file_buffer << glob_var_val->get_value() << "\n";
		}
		else
			report_internal_error("Result type can only be int and float");
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	if (eval_env.does_variable_exist(variable_name))
	{
		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result_Value * i;
	if (result.get_result_enum() == int_result)
	{
		i = new Eval_Result_Value_Int();
	 	i->set_value(result.get_value());
	}

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type)
{
	constant = number;
	node_data_type = constant_data_type;
}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	return node_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print_ast(ostream & file_buffer)
{
	file_buffer << "Num : " << constant;
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value(constant);

		return result;
	}
}
///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast()
{}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Return <NOTHING>\n";
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *(new Eval_Result_BB());	
	result.set_value(-1);
	print_ast(file_buffer);
	return result;
}

template class Number_Ast<int>;


/////////////////////////////////////////////////////////////////////////////////////

If_Else_Stmt_Ast::If_Else_Stmt_Ast(Ast * condition_temp, Ast * true_goto_temp, Ast * false_goto_temp)
{
	condition = condition_temp;
	true_goto = true_goto_temp;
	false_goto = false_goto_temp;
}
If_Else_Stmt_Ast::~If_Else_Stmt_Ast()
{
	delete condition;
	delete true_goto;
	delete false_goto;
}

void If_Else_Stmt_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "If_Else statement:";

	condition->print_ast(file_buffer);
	file_buffer<<endl;
	file_buffer << AST_NODE_SPACE << "True Successor: "<<true_goto->get_block_number()<<endl;
	file_buffer << AST_NODE_SPACE << "False Successor: "<<false_goto->get_block_number()<<endl;

}

Eval_Result & If_Else_Stmt_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	print_ast(file_buffer);
	Eval_Result & condResult = condition->evaluate(eval_env, file_buffer);
	Eval_Result & result = *(new Eval_Result_BB());
	file_buffer<<AST_SPACE;
	if(condResult.get_value() == 0){
		file_buffer<<"Condition False : Goto (BB "<<false_goto->get_block_number()<<")"<<endl;
		result.set_value(false_goto->get_block_number());
	}
	else{
		file_buffer<<"Condition True : Goto (BB "<<true_goto->get_block_number()<<")"<<endl;
		result.set_value(true_goto->get_block_number());
	}
	return result;
}

////////////////////////////////////////////////////////////////
Relational_Expr_Ast::Relational_Expr_Ast(Ast* temp_lhs, Ast* temp_rhs,Relation_Op temp_op){
	lhs = temp_lhs;
	rhs = temp_rhs;
	op = temp_op;
}

Relation_Op Relational_Expr_Ast::get_relational_op(){
	return op;
}

Data_Type Relational_Expr_Ast::get_data_type()
{
	return node_data_type;
}

bool Relational_Expr_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Relational Expression data type not compatible", line);
}

void Relational_Expr_Ast::print_ast(ostream & file_buffer){
	file_buffer<<"\n";
	file_buffer << AST_NODE_SPACE << "Condition: ";
	file_buffer <<Op_Array[op]<<endl;

	file_buffer <<AST_NODE_SPACE<<"   LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "   RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";


}

Eval_Result & Relational_Expr_Ast::evaluate(Local_Environment& eval_env, ostream& file_buffer){
	Eval_Result & resultLeft = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & result = * new Eval_Result_Value_Int();
	Eval_Result & resultRight = rhs->evaluate(eval_env, file_buffer);

	switch(op) {
		case LE:
			result.set_value(resultLeft.get_value() <= resultRight.get_value());
			break;
		case LT:
			result.set_value(resultLeft.get_value() < resultRight.get_value());
			break;
		case GT:
			result.set_value(resultLeft.get_value() > resultRight.get_value());
			break;
		case GE:
			result.set_value(resultLeft.get_value() >= resultRight.get_value());
			break;
		case EQ:
			result.set_value(resultLeft.get_value() == resultRight.get_value());
			break;
		case NE:
			result.set_value(resultLeft.get_value() != resultRight.get_value());
			break;
	}
	return result;
}

Relational_Expr_Ast::~Relational_Expr_Ast()
{
	delete lhs;
	delete rhs;
}

/////////////////////////////////////////////////////////////////

Goto_Stmt_Ast::Goto_Stmt_Ast(int temp_block_number){
	block_number = temp_block_number;
}

Goto_Stmt_Ast::~Goto_Stmt_Ast(){
}

int Goto_Stmt_Ast::get_block_number(){
	return block_number;
}

void Goto_Stmt_Ast::print_ast(ostream & file_buffer){
	file_buffer << AST_SPACE << "Goto statement:\n";
	file_buffer << AST_NODE_SPACE <<"Successor: "<<get_block_number()<<endl;
}

Eval_Result & Goto_Stmt_Ast::evaluate(Local_Environment& eval_env, ostream& file_buffer){
	Eval_Result & result = *(new Eval_Result_BB());
	result.set_value(block_number);
	print_ast(file_buffer);
	file_buffer<<AST_SPACE<<"GOTO (BB "<<block_number<<")\n";
	return result;
}