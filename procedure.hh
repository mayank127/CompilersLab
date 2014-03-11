
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

#ifndef PROCEDURE_HH
#define PROCEDURE_HH

#include<string>
#include<map>
#include<list>
#include<vector>

#define PROC_SPACE "   "
#define LOC_VAR_SPACE "      "

using namespace std;

class Procedure;

class Procedure
{
	Data_Type return_type;
	string name;
	Symbol_Table local_symbol_table;
	list<Basic_Block *> basic_block_list;

public:
	bool return_flag;
	vector<Symbol_Table_Entry*> argument_list;
	vector<int> goto_numbers;

	Procedure(Data_Type proc_return_type, string proc_name, vector<Symbol_Table_Entry*> argument_list);
	~Procedure();

	string get_proc_name();
	void set_basic_block_list(list<Basic_Block *> bb_list);
	void set_local_list(Symbol_Table & new_list);
	Data_Type get_return_type();
	void set_return_type(Data_Type type);
	Symbol_Table_Entry & get_symbol_table_entry(string variable_name);
	bool check_arguments(vector<Symbol_Table_Entry*> argument_list, int line);

	void print_ast(ostream & file_buffer);

	Basic_Block * get_next_bb(Basic_Block & current_bb, int previous_result);
	Basic_Block & get_start_basic_block();

	Eval_Result & evaluate(ostream & file_buffer, vector<Eval_Result_Value*> args);

	bool variable_in_symbol_list_check(string variable);
	void bb_check_goto_number_exist(int line);
	void check_call(vector<Data_Type> types, int line);
	void check_with_argument_list(string var, int line);
};

#endif
