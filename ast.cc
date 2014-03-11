
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
#include<iomanip>

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	file_buffer  <<endl<<AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE"LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	if (result.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print_ast(file_buffer);
	file_buffer<<endl;
	lhs->print_value(eval_env, file_buffer);
	Eval_Result & zero_result = *(new Eval_Result_BB());
	zero_result.set_value(0);
	return zero_result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
			file_buffer << loc_var_val->get_value().i << "\n";
		else if (loc_var_val->get_result_enum() == float_result)
			file_buffer << loc_var_val->get_value().f << "\n";
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
				file_buffer << glob_var_val->get_value().i << "\n";
		}
		else if (glob_var_val->get_result_enum() == float_result)
		{
			if (glob_var_val == NULL)			//TODO may be wrong
				file_buffer << "0\n";
			else
				file_buffer << glob_var_val->get_value().f << "\n";
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
		if(!result->is_variable_defined()){
			report_error("Variable should be defined before its use", NOLINE);
		}
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
	 	i->set_value(result.get_value().i);
	}
	else if (result.get_result_enum() == float_result)
	{
		i = new Eval_Result_Value_Float();
	 	i->set_value(result.get_value().f);
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
	file_buffer << "Num : " << std::setprecision(2) << std::fixed <<constant;
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
	else if (node_data_type == float_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Float();
		result.set_value(constant);

		return result;
	}
}


template class Number_Ast<int>;
template class Number_Ast<double>;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast(Ast* expr,Procedure* p)
{
	expression = expr;
	procedure = p;
}

Return_Ast::~Return_Ast()
{
	delete expression;
}

Data_Type Return_Ast::get_data_type()
{
	return node_data_type;
}

bool Return_Ast::check_ast(int line)
{
	if(expression == NULL && procedure->get_return_type() == void_data_type){
		node_data_type = void_data_type;
		return true;
	}
	else if(expression == NULL){
		node_data_type = void_data_type;
		return false;
	}
	else if (expression->get_data_type() == procedure->get_return_type()){
		node_data_type = procedure->get_return_type();
		return true;
	}
	else{
		report_error("Return statement data type not compatible", line);
	}
}

void Return_Ast::print_ast(ostream & file_buffer)
{
	file_buffer <<endl<< AST_SPACE << "RETURN ";
	if(expression!=NULL)
		expression->print_ast(file_buffer);
	else
		file_buffer<<"<NOTHING>";
	file_buffer<<endl;
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	procedure->return_flag = true;
	print_ast(file_buffer);
	Eval_Result *result;
	if(expression!=NULL)
		result = &(expression->evaluate(eval_env, file_buffer));
	else{
		result = new Eval_Result_Value_Int();
	}
	return *result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
	file_buffer <<endl<< AST_SPACE << "If_Else statement:";

	condition->print_ast(file_buffer);
	file_buffer<<endl;
	file_buffer << AST_NODE_SPACE << "True Successor: "<<true_goto->get_block_number()<<endl;
	file_buffer << AST_NODE_SPACE << "False Successor: "<<false_goto->get_block_number();

}

Eval_Result & If_Else_Stmt_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	file_buffer <<endl<< AST_SPACE << "If_Else statement:";

	condition->print_ast(file_buffer);

	Eval_Result & condResult = condition->evaluate(eval_env, file_buffer);
	Eval_Result & result = *(new Eval_Result_BB());

	file_buffer<<endl;
	file_buffer << AST_NODE_SPACE << "True Successor: "<<true_goto->get_block_number()<<endl;
	file_buffer << AST_NODE_SPACE << "False Successor: "<<false_goto->get_block_number();

	file_buffer<<endl<<AST_SPACE;
	if(condResult.get_value().i == 0){
		file_buffer<<"Condition False : Goto (BB "<<false_goto->get_block_number()<<")\n";
		result.set_value(false_goto->get_block_number());
	}
	else{
		file_buffer<<"Condition True : Goto (BB "<<true_goto->get_block_number()<<")\n";
		result.set_value(true_goto->get_block_number());
	}
	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
		node_data_type = int_data_type;
		return true;
	}
	report_error("Assignment statement data type not compatible", line);
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

	double left;
	double right;
	if(resultLeft.get_result_enum()==int_result){
		left = resultLeft.get_value().i;
	}
	else if(resultLeft.get_result_enum()==float_result){
		left = resultLeft.get_value().f;
	}

	if(resultRight.get_result_enum()==int_result){
		right = resultRight.get_value().i;
	}
	else if(resultRight.get_result_enum()==float_result){
		right = resultRight.get_value().f;
	}

	switch(op) {
		case LE:
			result.set_value(left <= right);
			break;
		case LT:
			result.set_value(left < right);
			break;
		case GT:
			result.set_value(left > right);
			break;
		case GE:
			result.set_value(left >= right);
			break;
		case EQ:
			result.set_value(left == right);
			break;
		case NE:
			result.set_value(left != right);
			break;
	}
	return result;
}

Relational_Expr_Ast::~Relational_Expr_Ast()
{
	delete lhs;
	delete rhs;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Goto_Stmt_Ast::Goto_Stmt_Ast(int temp_block_number){
	block_number = temp_block_number;
}

Goto_Stmt_Ast::~Goto_Stmt_Ast(){
}

int Goto_Stmt_Ast::get_block_number(){
	return block_number;
}

void Goto_Stmt_Ast::print_ast(ostream & file_buffer){
	file_buffer <<endl<< AST_SPACE << "Goto statement:\n";
	file_buffer << AST_NODE_SPACE <<"Successor: "<<get_block_number();
}

Eval_Result & Goto_Stmt_Ast::evaluate(Local_Environment& eval_env, ostream& file_buffer){
	Eval_Result & result = *(new Eval_Result_BB());
	result.set_value(block_number);
	print_ast(file_buffer);
	file_buffer<<endl<<AST_SPACE<<"GOTO (BB "<<block_number<<")\n";
	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Plus_Ast::Plus_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
}

Plus_Ast::~Plus_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Plus_Ast::get_data_type()
{
	return node_data_type;
}

bool Plus_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Arithmetic(+) statement data type not compatible", line);
}

void Plus_Ast::print_ast(ostream & file_buffer)
{
	file_buffer  <<endl<<AST_SPACE << "   Arith: PLUS\n";

	file_buffer << AST_NODE_SPACE<<"   LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "   RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Plus_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & lresult = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & rresult = rhs->evaluate(eval_env, file_buffer);

	if (lresult.is_variable_defined() == false)
		report_error("Variable should be defined to be on lhs", NOLINE);
	if (rresult.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	Eval_Result_Value * result;
	if (node_data_type == float_data_type){
		result = new Eval_Result_Value_Float();
	 	result->set_value(lresult.get_value().f + rresult.get_value().f);
	}
	else{
		result = new Eval_Result_Value_Int();
	 	result->set_value(lresult.get_value().i + rresult.get_value().i);
	}
	return *result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Minus
Minus_Ast::Minus_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
}

Minus_Ast::~Minus_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Minus_Ast::get_data_type()
{
	return node_data_type;
}

bool Minus_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Arithmetic(-) statement data type not compatible", line);
}

void Minus_Ast::print_ast(ostream & file_buffer)
{
	file_buffer  <<endl<<AST_SPACE << "   Arith: MINUS\n";

	file_buffer << AST_NODE_SPACE<<"   LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "   RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Minus_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & lresult = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & rresult = rhs->evaluate(eval_env, file_buffer);

	if (lresult.is_variable_defined() == false)
		report_error("Variable should be defined to be on lhs", NOLINE);
	if (rresult.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	Eval_Result_Value * result;
	if (node_data_type == float_data_type){
		result = new Eval_Result_Value_Float();
	 	result->set_value(lresult.get_value().f - rresult.get_value().f);
	}
	else{
		result = new Eval_Result_Value_Int();
	 	result->set_value(lresult.get_value().i - rresult.get_value().i);
	}
	return *result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Division
Division_Ast::Division_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
}

Division_Ast::~Division_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Division_Ast::get_data_type()
{
	return node_data_type;
}

bool Division_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Arithmetic(/) statement data type not compatible", line);
}

void Division_Ast::print_ast(ostream & file_buffer)
{
	file_buffer  <<endl<<AST_SPACE << "   Arith: DIV\n";

	file_buffer << AST_NODE_SPACE<<"   LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "   RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Division_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & lresult = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & rresult = rhs->evaluate(eval_env, file_buffer);

	if (lresult.is_variable_defined() == false)
		report_error("Variable should be defined to be on lhs", NOLINE);
	if (rresult.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	Eval_Result_Value * result;
	if (node_data_type == float_data_type){
		result = new Eval_Result_Value_Float();
	 	result->set_value(lresult.get_value().f / rresult.get_value().f);
	}
	else{
		result = new Eval_Result_Value_Int();
	 	result->set_value(lresult.get_value().i / rresult.get_value().i);
	}
	return *result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//multiplpy

Multiplication_Ast::Multiplication_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
}

Multiplication_Ast::~Multiplication_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Multiplication_Ast::get_data_type()
{
	return node_data_type;
}

bool Multiplication_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Arithmetic(*) statement data type not compatible", line);
}

void Multiplication_Ast::print_ast(ostream & file_buffer)
{
	file_buffer  <<endl<<AST_SPACE << "   Arith: MULT\n";

	file_buffer << AST_NODE_SPACE<<"   LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "   RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Multiplication_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & lresult = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & rresult = rhs->evaluate(eval_env, file_buffer);

	if (lresult.is_variable_defined() == false)
		report_error("Variable should be defined to be on lhs", NOLINE);
	if (rresult.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	Eval_Result_Value * result;
	if (node_data_type == float_data_type){
		result = new Eval_Result_Value_Float();
	 	result->set_value(lresult.get_value().f * rresult.get_value().f);
	}
	else{
		result = new Eval_Result_Value_Int();
	 	result->set_value(lresult.get_value().i * rresult.get_value().i);
	}
	return *result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Unary

Unary_Ast::Unary_Ast(Ast * temp_lhs)
{
	lhs = temp_lhs;
}

Unary_Ast::~Unary_Ast()
{
	delete lhs;
}

Data_Type Unary_Ast::get_data_type()
{
	return node_data_type;
}

bool Unary_Ast::check_ast(int line)
{
	node_data_type = lhs->get_data_type();
	return true;
}

void Unary_Ast::print_ast(ostream & file_buffer)
{
	file_buffer  <<endl<<AST_SPACE << "   Arith: UMINUS\n";

	file_buffer << AST_NODE_SPACE<<"   LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Unary_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & lresult = lhs->evaluate(eval_env, file_buffer);

	if (lresult.is_variable_defined() == false)
		report_error("Variable should be defined to be on lhs", NOLINE);

	Eval_Result_Value * result;
	if (node_data_type == float_data_type){
		result = new Eval_Result_Value_Float();
	 	result->set_value(-1*lresult.get_value().f);
	}
	else{
		result = new Eval_Result_Value_Int();
	 	result->set_value(-1 * lresult.get_value().i);
	}
	return *result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


TypeCast_Ast::TypeCast_Ast(Ast * temp_lhs, Data_Type temp_data_type)
{
	lhs = temp_lhs;
	node_data_type = temp_data_type;
}

TypeCast_Ast::~TypeCast_Ast()
{
	delete lhs;
}

Data_Type TypeCast_Ast::get_data_type()
{
	return node_data_type;
}

void TypeCast_Ast::print_ast(ostream & file_buffer)
{
	lhs->print_ast(file_buffer);
}

Eval_Result & TypeCast_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & lresult = lhs->evaluate(eval_env, file_buffer);

	Eval_Result_Value * result;
	if (node_data_type == float_data_type){
		result = new Eval_Result_Value_Float();
		if(lresult.get_result_enum() == int_result){
			result->set_value((double)lresult.get_value().i);
		}
		else{
			result->set_value(lresult.get_value().f);
		}
	}
	else{
		result = new Eval_Result_Value_Int();
	 	if(lresult.get_result_enum() == float_result){
			result->set_value((int)lresult.get_value().f);
		}
		else{
			result->set_value(lresult.get_value().i);
		}
	}
	return *result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// call ast

Call_Ast::Call_Ast(Procedure* proc, vector<Ast*> expr_list)
{
	procedure = proc;
	expression_list = expr_list;
}

Call_Ast::~Call_Ast()
{
}

Data_Type Call_Ast::get_data_type()
{
	return node_data_type;
}

bool Call_Ast::check_ast(int line)
{
	vector<Data_Type> types;
	for(int i=0;i<expression_list.size();i++){
		types.push_back(expression_list[i]->get_data_type());
	}
	procedure->check_call(types, line);
	node_data_type = procedure->get_return_type();
	return true;
}

void Call_Ast::print_ast(ostream & file_buffer)
{
	file_buffer  <<endl<<AST_SPACE << "FN CALL: "<<procedure->get_proc_name();

	file_buffer<<"(";
	for(int i=0; i< expression_list.size();i++){
		file_buffer<<"\n"<<AST_NODE_SPACE;
		expression_list[i]->print_ast(file_buffer);
	}
	file_buffer << ")";
}

Eval_Result & Call_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{

	Eval_Result * result;
	vector<Eval_Result_Value*> args;
	for(int i=0;i<expression_list.size();i++){
		Eval_Result_Value * res;
		Eval_Result & res2 = expression_list[i]->evaluate(eval_env, file_buffer);
		if (res2.get_result_enum() == int_result)
		{
			res = new Eval_Result_Value_Int();
		 	res->set_value(res2.get_value().i);
		}
		else if (res2.get_result_enum() == float_result)
		{
			res = new Eval_Result_Value_Float();
		 	res->set_value(res2.get_value().f);
		}

		args.push_back(res);
	}
	result = &(procedure->evaluate(file_buffer,args));
	return *result;
}
