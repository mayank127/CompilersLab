
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
#include<typeinfo>

using namespace std;

#include"common-classes.hh"
#include"error-display.hh"
#include"user-options.hh"
#include"local-environment.hh"
#include"icode.hh"
#include"reg-alloc.hh"
#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"

string Op_Array[]={"LE","LT","GT","GE","EQ","NE"};

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast()
{
	stringstream msg;
	msg << "No check_ast() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Data_Type Ast::get_data_type()
{
	stringstream msg;
	msg << "No get_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Symbol_Table_Entry & Ast::get_symbol_entry()
{
	stringstream msg;
	msg << "No get_symbol_entry() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	stringstream msg;
	msg << "No print_value() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	stringstream msg;
	msg << "No get_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	stringstream msg;
	msg << "No set_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Code_For_Ast & Ast::create_store_stmt(Register_Descriptor * store_register)
{
	stringstream msg;
	msg << "No create_store_stmt() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}
int Ast::get_block_number()
{
	stringstream msg;
	msg << "No get_block_number() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line)
{
	lhs = temp_lhs;
	rhs = temp_rhs;

	ast_num_child = binary_arity;
	node_data_type = void_data_type;

	lineno = line;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

bool Assignment_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Assignment statement data type not compatible");
}

void Assignment_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Asgn:";

	file_buffer << "\n" << AST_NODE_SPACE << "LHS (";
	lhs->print(file_buffer);
	file_buffer << ")";

	file_buffer << "\n" << AST_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	CHECK_INPUT_AND_ABORT(result.is_variable_defined(), "Variable should be defined to be on rhs of Assignment_Ast", lineno);

	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print(file_buffer);

	lhs->print_value(eval_env, file_buffer);

	Eval_Result & zero_result = *(new Eval_Result_Value_Int());
	zero_result.set_value(0);
	return zero_result;
}

Code_For_Ast & Assignment_Ast::compile()
{
	/* 
		An assignment x = y where y is a variable is 
		compiled as a combination of load and store statements:
		(load) R <- y 
		(store) x <- R
		If y is a constant, the statement is compiled as:
		(imm_Load) R <- y 
		(store) x <- R
		where imm_Load denotes the load immediate operation.
	*/

	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	Code_For_Ast & load_stmt = rhs->compile();

	Register_Descriptor * load_register = load_stmt.get_reg();

	Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);

	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, load_register);

	return *assign_stmt;
}

Code_For_Ast & Assignment_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	if(typeid(*rhs) != typeid(Relational_Expr_Ast))
		lra.optimize_lra(mc_2m, lhs, rhs);
	Code_For_Ast load_stmt = rhs->compile_and_optimize_ast(lra);

	Register_Descriptor * result_register = load_stmt.get_reg();

	Symbol_Table_Entry * dest_symbol_entry = &(lhs->get_symbol_entry());
	if(dest_symbol_entry){
		if (dest_symbol_entry->get_register())
			dest_symbol_entry->free_register(dest_symbol_entry->get_register());

		dest_symbol_entry->update_register(result_register);
	}

	Code_For_Ast store_stmt = lhs->create_store_stmt(result_register);

	list<Icode_Stmt *> ic_list;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);

	return *assign_stmt;
}

/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line)
{
	variable_symbol_entry = &var_entry;

	CHECK_INVARIANT((variable_symbol_entry->get_variable_name() == name),
		"Variable's symbol entry is not matching its name");

	ast_num_child = zero_arity;
	node_data_type = variable_symbol_entry->get_data_type();
	lineno = line;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry->get_data_type();
}

Symbol_Table_Entry & Name_Ast::get_symbol_entry()
{
	return *variable_symbol_entry;
}

void Name_Ast::print(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_symbol_entry->get_variable_name();
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	Eval_Result * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << "\n" << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		CHECK_INVARIANT(loc_var_val->get_result_enum() == int_result, "Result type can only be int");
		file_buffer << loc_var_val->get_int_value() << "\n";
	}

	else
	{
		CHECK_INVARIANT(glob_var_val->get_result_enum() == int_result, 
			"Result type can only be int and float");

		if (glob_var_val == NULL)
			file_buffer << "0\n";
		else
			file_buffer << glob_var_val->get_int_value() << "\n";
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	if (eval_env.does_variable_exist(variable_name))
	{
		CHECK_INPUT_AND_ABORT((eval_env.is_variable_defined(variable_name) == true), 
					"Variable should be defined before its use", lineno);

		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	CHECK_INPUT_AND_ABORT((interpreter_global_table.is_variable_defined(variable_name) == true), 
				"Variable should be defined before its use", lineno);

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result * i;
	string variable_name = variable_symbol_entry->get_variable_name();

	if (variable_symbol_entry->get_data_type() == int_data_type)
		i = new Eval_Result_Value_Int();
	else
		CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

	if (result.get_result_enum() == int_result)
	 	i->set_value(result.get_int_value());
	else
		CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

Code_For_Ast & Name_Ast::compile()
{
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);
	Register_Descriptor * result_register = machine_dscr_object.get_new_register();
	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(load, opd, register_opd);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

Code_For_Ast & Name_Ast::create_store_stmt(Register_Descriptor * store_register)
{
	CHECK_INVARIANT((store_register != NULL), "Store register cannot be null");

	Ics_Opd * register_opd = new Register_Addr_Opd(store_register);
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

	Icode_Stmt * store_stmt = new Move_IC_Stmt(store, register_opd, opd);

	if (command_options.is_do_lra_selected() == false)
		variable_symbol_entry->free_register(store_register);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(store_stmt);

	Code_For_Ast & name_code = *new Code_For_Ast(ic_list, store_register);

	return name_code;
}

Code_For_Ast & Name_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;;

	bool load_needed = lra.is_load_needed();

	Register_Descriptor * result_register = lra.get_register();
	CHECK_INVARIANT((result_register != NULL), "Register cannot be null");
	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * load_stmt = NULL;
	if (load_needed)
	{
		Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

		load_stmt = new Move_IC_Stmt(load, opd, register_opd);

		ic_list.push_back(load_stmt);
	}

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type, int line)
{
	constant = number;
	node_data_type = constant_data_type;

	ast_num_child = zero_arity;

	lineno = line;
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
void Number_Ast<DATA_TYPE>::print(ostream & file_buffer)
{
	file_buffer << std::fixed;
	file_buffer << std::setprecision(2);

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

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile()
{
	Register_Descriptor * result_register = machine_dscr_object.get_new_register();
	CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(result_register);
	Ics_Opd * opd = new Const_Opd<int>(constant);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(load_stmt);

	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, result_register);

	return num_code;
}

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lra.get_register() != NULL), "Register assigned through optimize_lra cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(lra.get_register());
	Ics_Opd * opd = new Const_Opd<int>(constant);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, lra.get_register());

	return num_code;
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast(int line)
{
	lineno = line;
	node_data_type = void_data_type;
	ast_num_child = zero_arity;
}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Return <NOTHING>\n";
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *(new Eval_Result_Value_Int());
	result.set_value(-1);
	print(file_buffer);
	return result;
}

Code_For_Ast & Return_Ast::compile()
{
	Code_For_Ast & ret_code = *new Code_For_Ast();
	return ret_code;
}

Code_For_Ast & Return_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	Code_For_Ast & ret_code = *new Code_For_Ast();
	return ret_code;
}

template class Number_Ast<int>;


/////////////////////////////////////////////////////////////////
Relational_Expr_Ast::Relational_Expr_Ast(Ast* temp_lhs, Ast* temp_rhs,Relation_Op temp_op, int line){
	lhs = temp_lhs;
	rhs = temp_rhs;
	op = temp_op;
	lineno = line;
}

Relation_Op Relational_Expr_Ast::get_relational_op(){
	return op;
}

Data_Type Relational_Expr_Ast::get_data_type()
{
	return node_data_type;
}

bool Relational_Expr_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Assignment statement data type not compatible");
}

void Relational_Expr_Ast::print(ostream & file_buffer){
	file_buffer<<"\n";
	file_buffer << AST_NODE_SPACE << "Condition: ";
	file_buffer <<Op_Array[op]<<endl;

	file_buffer <<AST_NODE_SPACE<<"   LHS (";
	lhs->print(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "   RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";


}

Eval_Result & Relational_Expr_Ast::evaluate(Local_Environment& eval_env, ostream& file_buffer){
	CHECK_INVARIANT((rhs != NULL), "Lhs of Relational_Expr_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Rhs of Relational_Expr_Ast cannot be null");

	Eval_Result & resultLeft = lhs->evaluate(eval_env, file_buffer);
	CHECK_INPUT_AND_ABORT(resultLeft.is_variable_defined(), "Variable should be defined to be on lhs of Relational_Expr_Ast", lineno);
	Eval_Result & result = * new Eval_Result_Value_Int();
	Eval_Result & resultRight = rhs->evaluate(eval_env, file_buffer);
	CHECK_INPUT_AND_ABORT(resultRight.is_variable_defined(), "Variable should be defined to be on rhs of Relational_Expr_Ast", lineno);

	switch(op) {
		case LE:
			result.set_value(resultLeft.get_int_value() <= resultRight.get_int_value());
			break;
		case LT:
			result.set_value(resultLeft.get_int_value() < resultRight.get_int_value());
			break;
		case GT:
			result.set_value(resultLeft.get_int_value() > resultRight.get_int_value());
			break;
		case GE:
			result.set_value(resultLeft.get_int_value() >= resultRight.get_int_value());
			break;
		case EQ:
			result.set_value(resultLeft.get_int_value() == resultRight.get_int_value());
			break;
		case NE:
			result.set_value(resultLeft.get_int_value() != resultRight.get_int_value());
			break;
	}
	return result;
}

Relational_Expr_Ast::~Relational_Expr_Ast()
{
	delete lhs;
	delete rhs;
}
Code_For_Ast & Relational_Expr_Ast::compile()
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	Code_For_Ast & load_lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_register = load_lhs_stmt.get_reg();
	lhs_register->used_for_expr_result = true;

	Code_For_Ast & load_rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_register = load_rhs_stmt.get_reg();
	rhs_register->used_for_expr_result = true;


	Register_Descriptor * result_register = machine_dscr_object.get_new_register();
	result_register->used_for_expr_result = true;
	Ics_Opd * result = new Register_Addr_Opd(result_register);
	Ics_Opd * op1 = new Register_Addr_Opd(lhs_register);
	Ics_Opd * op2 = new Register_Addr_Opd(rhs_register);

	Icode_Stmt * store_stmt;
	switch(op) {
		case LE:
			store_stmt = new Compute_IC_Stmt(sle, op1, op2, result);
			break;
		case LT:
			store_stmt = new Compute_IC_Stmt(slt, op1, op2, result);
			break;
		case GT:
			store_stmt = new Compute_IC_Stmt(sgt, op1, op2, result);
			break;
		case GE:
			store_stmt = new Compute_IC_Stmt(sge, op1, op2, result);
			break;
		case EQ:
			store_stmt = new Compute_IC_Stmt(seq, op1, op2, result);
			break;
		case NE:
			store_stmt = new Compute_IC_Stmt(sne, op1, op2, result);
			break;
	}
	lhs_register->used_for_expr_result = false;
	rhs_register->used_for_expr_result = false;
	result_register->used_for_expr_result = false;
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_lhs_stmt.get_icode_list().empty() == false)
		ic_list = load_lhs_stmt.get_icode_list();

	if (load_rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), load_rhs_stmt.get_icode_list());

	ic_list.push_back(store_stmt);

	Code_For_Ast * rel_expr_stmt;
	if (ic_list.empty() == false)
		rel_expr_stmt = new Code_For_Ast(ic_list, result_register);

	return *rel_expr_stmt;
}

Code_For_Ast & Relational_Expr_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	if(typeid(*lhs) != typeid(Relational_Expr_Ast))
		lra.optimize_lra(mc_2r, NULL, lhs);
	Code_For_Ast & load_lhs_stmt = lhs->compile_and_optimize_ast(lra);
	Register_Descriptor * lhs_register = load_lhs_stmt.get_reg();
	lhs_register->used_for_expr_result = true;

	if(typeid(*rhs) != typeid(Relational_Expr_Ast))
		lra.optimize_lra(mc_2r, NULL, rhs);
	Code_For_Ast & load_rhs_stmt = rhs->compile_and_optimize_ast(lra);
	Register_Descriptor * rhs_register = load_rhs_stmt.get_reg();
	rhs_register->used_for_expr_result = true;


	Register_Descriptor * result_register = machine_dscr_object.get_new_register();
	result_register->used_for_expr_result = true;
	Ics_Opd * result = new Register_Addr_Opd(result_register);
	Ics_Opd * op1 = new Register_Addr_Opd(lhs_register);
	Ics_Opd * op2 = new Register_Addr_Opd(rhs_register);

	Icode_Stmt * store_stmt;
	switch(op) {
		case LE:
			store_stmt = new Compute_IC_Stmt(sle, op1, op2, result);
			break;
		case LT:
			store_stmt = new Compute_IC_Stmt(slt, op1, op2, result);
			break;
		case GT:
			store_stmt = new Compute_IC_Stmt(sgt, op1, op2, result);
			break;
		case GE:
			store_stmt = new Compute_IC_Stmt(sge, op1, op2, result);
			break;
		case EQ:
			store_stmt = new Compute_IC_Stmt(seq, op1, op2, result);
			break;
		case NE:
			store_stmt = new Compute_IC_Stmt(sne, op1, op2, result);
			break;
	}
	lhs_register->used_for_expr_result = false;
	rhs_register->used_for_expr_result = false;
	result_register->used_for_expr_result = false;
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_lhs_stmt.get_icode_list().empty() == false)
		ic_list = load_lhs_stmt.get_icode_list();

	if (load_rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), load_rhs_stmt.get_icode_list());

	ic_list.push_back(store_stmt);

	Code_For_Ast * rel_expr_stmt;
	if (ic_list.empty() == false)
		rel_expr_stmt = new Code_For_Ast(ic_list, result_register);

	return *rel_expr_stmt;
}
/////////////////////////////////////////////////////////////////

Goto_Stmt_Ast::Goto_Stmt_Ast(int temp_block_number, int line){
	block_number = temp_block_number;
	lineno = line;
}

Goto_Stmt_Ast::~Goto_Stmt_Ast(){
}

int Goto_Stmt_Ast::get_block_number(){
	return block_number;
}

void Goto_Stmt_Ast::print(ostream & file_buffer){
	file_buffer << endl<<AST_SPACE << "Goto statement:\n";
	file_buffer << AST_NODE_SPACE <<"Successor: "<<get_block_number();
}

Eval_Result & Goto_Stmt_Ast::evaluate(Local_Environment& eval_env, ostream& file_buffer){
	Eval_Result & result = *(new Eval_Result_Value_Int());
	result.set_value(block_number);
	print(file_buffer);
	file_buffer<<endl<<AST_SPACE<<"GOTO (BB "<<block_number<<")\n";
	return result;
}
Code_For_Ast & Goto_Stmt_Ast::compile()
{
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(goto_label, NULL, NULL, new Const_Opd<int>(block_number));
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(goto_stmt);
	Code_For_Ast * goto_stmt_code;

	if (ic_list.empty() == false)
		goto_stmt_code = new Code_For_Ast(ic_list, NULL);

	return * goto_stmt_code;
}

Code_For_Ast & Goto_Stmt_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(goto_label, NULL, NULL, new Const_Opd<int>(block_number));
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(goto_stmt);
	Code_For_Ast * goto_stmt_code;

	if (ic_list.empty() == false)
		goto_stmt_code = new Code_For_Ast(ic_list, NULL);

	return * goto_stmt_code;
}
/////////////////////////////////////////////////////////////////


If_Else_Stmt_Ast::If_Else_Stmt_Ast(Ast * condition_temp, Ast * true_goto_temp, Ast * false_goto_temp, int line)
{
	condition = condition_temp;
	true_goto = true_goto_temp;
	false_goto = false_goto_temp;
	lineno = line;
}
If_Else_Stmt_Ast::~If_Else_Stmt_Ast()
{
	delete condition;
	delete true_goto;
	delete false_goto;
}

void If_Else_Stmt_Ast::print(ostream & file_buffer)
{
	file_buffer <<endl<< AST_SPACE << "If_Else statement:";

	condition->print(file_buffer);
	file_buffer<<endl;
	file_buffer << AST_NODE_SPACE << "True Successor: "<<true_goto->get_block_number()<<endl;
	file_buffer << AST_NODE_SPACE << "False Successor: "<<false_goto->get_block_number();

}

Eval_Result & If_Else_Stmt_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	print(file_buffer);
	Eval_Result & condResult = condition->evaluate(eval_env, file_buffer);
	Eval_Result & result = *(new Eval_Result_Value_Int());
	file_buffer<<endl<<AST_SPACE;
	if(condResult.get_int_value() == 0){
		file_buffer<<"Condition False : Goto (BB "<<false_goto->get_block_number()<<")"<<endl;
		result.set_value(false_goto->get_block_number());
	}
	else{
		file_buffer<<"Condition True : Goto (BB "<<true_goto->get_block_number()<<")"<<endl;
		result.set_value(true_goto->get_block_number());
	}
	return result;
}

Code_For_Ast & If_Else_Stmt_Ast::compile()
{
	CHECK_INVARIANT((condition != NULL), "Condition cannot be null");

	Code_For_Ast & cond_stmt = condition->compile();
	Register_Descriptor * cond_reg = cond_stmt.get_reg();
	Ics_Opd * op1 = new Register_Addr_Opd(cond_reg);

	Register_Descriptor * z = machine_dscr_object.spim_register_table[zero];
	Ics_Opd * op2 = new Register_Addr_Opd(z);

	Icode_Stmt * cond_goto_stmt = new Control_Flow_IC_Stmt(bne, op1, op2, new Const_Opd<int>(true_goto->get_block_number()));
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(goto_label, NULL, NULL, new Const_Opd<int>(false_goto->get_block_number()));
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	Code_For_Ast * cond_goto_stmt_code;

	if (cond_stmt.get_icode_list().empty() == false)
		ic_list = cond_stmt.get_icode_list();
	ic_list.push_back(cond_goto_stmt);
	ic_list.push_back(goto_stmt);
	if (ic_list.empty() == false)
		cond_goto_stmt_code = new Code_For_Ast(ic_list, cond_reg);

	return * cond_goto_stmt_code;
}

Code_For_Ast & If_Else_Stmt_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((condition != NULL), "Condition cannot be null");

	Code_For_Ast & cond_stmt = condition->compile_and_optimize_ast(lra);
	Register_Descriptor * cond_reg = cond_stmt.get_reg();
	Ics_Opd * op1 = new Register_Addr_Opd(cond_reg);

	Register_Descriptor * z = machine_dscr_object.spim_register_table[zero];
	Ics_Opd * op2 = new Register_Addr_Opd(z);

	Icode_Stmt * cond_goto_stmt = new Control_Flow_IC_Stmt(bne, op1, op2, new Const_Opd<int>(true_goto->get_block_number()));
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(goto_label, NULL, NULL, new Const_Opd<int>(false_goto->get_block_number()));
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;


	Code_For_Ast * cond_goto_stmt_code;

	if (cond_stmt.get_icode_list().empty() == false)
		ic_list = cond_stmt.get_icode_list();
	ic_list.push_back(cond_goto_stmt);
	ic_list.push_back(goto_stmt);
	if (ic_list.empty() == false)
		cond_goto_stmt_code = new Code_For_Ast(ic_list, cond_reg);

	return * cond_goto_stmt_code;
}
/////////////////////////////////////////////////////////////////