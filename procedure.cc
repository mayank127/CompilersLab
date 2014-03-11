
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

#include<string>
#include<fstream>
#include<iostream>
#include<iomanip>
using namespace std;

#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"

Procedure::Procedure(Data_Type proc_return_type, string proc_name, vector<Symbol_Table_Entry*> arguments)
{
	return_type = proc_return_type;
	name = proc_name;
	argument_list = arguments;
	for(int i=0;i<argument_list.size();i++){
		local_symbol_table.push_symbol(argument_list[i]);
	}
	local_symbol_table.set_table_scope(local);
}

Procedure::~Procedure()
{
	list<Basic_Block *>::iterator i;
	for (i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		delete (*i);
}

string Procedure::get_proc_name()
{
	return name;
}

void Procedure::set_basic_block_list(list<Basic_Block *> bb_list)
{
	basic_block_list = bb_list;
}

void Procedure::set_local_list(Symbol_Table & new_list)
{
	local_symbol_table = new_list;
	local_symbol_table.set_table_scope(local);
	for(int i=0;i<argument_list.size();i++){
		local_symbol_table.push_symbol(argument_list[i]);
	}

}

Data_Type Procedure::get_return_type()
{
	return return_type;
}

void Procedure::set_return_type(Data_Type type)
{
	return_type = type;
}

bool Procedure::check_arguments(vector<Symbol_Table_Entry*> tmp_argument_list, int line){
	if(tmp_argument_list.size() ==  argument_list.size()){
		for(int i = 0; i < tmp_argument_list.size();i++){
			if(!(tmp_argument_list[i]->get_data_type() == argument_list[i]->get_data_type() &&
				tmp_argument_list[i]->get_variable_name() == argument_list[i]->get_variable_name())){
				report_error("Formal parameter list of the procedure and its prototype should match.", line);
			}
		}
		return true;
	}
	report_error("Formal parameter list of the procedure and its prototype should match.", line);
	return false;
}


void Procedure::check_call(vector<Data_Type> types, int line){
	if(types.size() ==  argument_list.size()){
		for(int i = 0; i < types.size();i++){
			if(!(types[i] == argument_list[i]->get_data_type())){
				report_error("Actual and formal parameters donot match", line);
			}
		}
		return;
	}
	report_error("Actual and formal parameters donot match", line);
	return;
}

bool Procedure::variable_in_symbol_list_check(string variable)
{
	return local_symbol_table.variable_in_symbol_list_check(variable);
}

Symbol_Table_Entry & Procedure::get_symbol_table_entry(string variable_name)
{
	return local_symbol_table.get_symbol_table_entry(variable_name);
}

void Procedure::print_ast(ostream & file_buffer)
{
	file_buffer << PROC_SPACE << "Procedure: "<< name << "\n";

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		(*i)->print_bb(file_buffer);
	file_buffer<<endl;
}

Basic_Block & Procedure::get_start_basic_block()
{
	list<Basic_Block *>::iterator i;
	i = basic_block_list.begin();
	return **i;
}

Basic_Block * Procedure::get_next_bb(Basic_Block & current_bb, int previous_result)
{
	bool flag = false;
	if(previous_result == 0)
	{
		list<Basic_Block *>::iterator i;
		for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		{
			if((*i)->get_bb_number() == current_bb.get_bb_number())
			{
				flag = true;
				continue;
			}
			if (flag)
				return (*i);
		}
	}
	else
	{
		list<Basic_Block *>::iterator i;
		for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		{
			if((*i)->get_bb_number() == previous_result)
			{
				return *i;
			}
		}
	}

	return NULL;
}

Eval_Result & Procedure::evaluate(ostream & file_buffer, vector<Eval_Result_Value*> args)
{
	return_flag = false;
	Local_Environment & eval_env = *new Local_Environment();
	local_symbol_table.create(eval_env);


	for(int i=0; i<argument_list.size(); i++){
		eval_env.put_variable_value(*(args[i]), argument_list[i]->get_variable_name());
	}

	Eval_Result * result = NULL;

	file_buffer << PROC_SPACE << "Evaluating Procedure << " << name << " >>\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (before evaluating):\n";
	eval_env.print(file_buffer);
	file_buffer << "\n";

	Basic_Block * current_bb = &(get_start_basic_block());
	while (current_bb)
	{
		result = &(current_bb->evaluate(eval_env, file_buffer));
		if(return_flag){
			break;
		}
		if(result)
			current_bb = get_next_bb(*current_bb, result->get_value().i);
		else
			current_bb = NULL;
	}

	file_buffer << "\n\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (after evaluating) Function: << "<< name <<" >>\n";
	if(result->is_variable_defined()){
		if(result->get_result_enum() == int_result){
			Eval_Result_Value *res = new Eval_Result_Value_Int();
			res->set_value(result->get_value().i);
			eval_env.put_variable_value(*res, "return");
		}
		else{
			Eval_Result_Value *res = new Eval_Result_Value_Float();
			res->set_value(result->get_value().f);
			eval_env.put_variable_value(*res, "return");
		}
			
	}
	eval_env.print(file_buffer);
	return_flag = false;
	
	return *result;
}


void Procedure::bb_check_goto_number_exist(int line)
{
  int size = basic_block_list.size() + 1;
  vector<int>::iterator i;
  for(i = goto_numbers.begin(); i != goto_numbers.end(); i++)
  {
  	list<Basic_Block*>::iterator j;
  	int flag = 0;
  	for(j = basic_block_list.begin(); j!= basic_block_list.end(); j++){
  		if((*j)->get_bb_number() == *i){
  			flag = 1;
  			break;
  		}
  	}
  	if(flag == 0){
  		char buffer[128];
    	sprintf(buffer, "bb %d doesn't exist", (*i));
    	report_error(buffer, line);
  	}
  }
}

void Procedure::check_with_argument_list(string var, int line){
	for(int i=0;i<argument_list.size();i++){
		if(var == argument_list[i]->get_variable_name()){
			report_error("Formal parameter and local variable name cannot be same", line);
		}
	}
}