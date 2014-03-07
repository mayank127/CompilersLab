/*
********************************************************************************************

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

%scanner ../scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union
{
	int integer_value;
	float float_value;
	std::string * string_value;
	list<Ast *> * ast_list;
	Ast * ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	vector<Symbol_Table_Entry*>* symbol_entry_vector;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;
	Relation_Op relation_op;
	vector<Ast*>* ast_vector;
};

%token <integer_value> INTEGER_NUMBER BASIC_BLOCK
%token <float_value> FNUM
%token <string_value> NAME

%token RETURN INTEGER FLOAT DOUBLE VOID IF ELSE GOTO ASSIGN_OP

%left <relation_op> NE EQ
%left <relation_op> LT LE GT GE

%left '+' '-'
%left '/' '*'


%type <symbol_table> declaration_statement_list
%type <symbol_entry> declaration_statement
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast_list> executable_statement_list
%type <ast_list> assignment_statement_list
%type <ast> assignment_statement
%type <ast> if_statement
%type <ast> goto_statement
%type <ast> expression
%type <ast> basic_expression
%type <ast> arithmetic_expression
%type <ast> conditional_expression
%type <ast> variable
%type <ast> constant
%type <ast> return_statement
%type <ast> atomic_expression
%type <ast> function_call
%type <integer_value> basic_block_number
%type <procedure> procedure_name_decl
%type <symbol_entry_vector> argument_list
%type <symbol_entry> argument
%type <ast_vector> expression_list


%start program

%%

program:
	declaration_statement_list
	{
		if($1 != NULL)
			program_object.set_global_table(*$1);
	}
	procedure_list{
		if ($1)
			$1->global_list_in_proc_map_check(get_line_number());
		delete $1;
	}
|
	procedure_list
;

procedure_list:
	procedure_name procedure_body
|
	procedure_list procedure_name procedure_body
;
procedure_name:
	NAME '(' argument_list ')'
	{
		current_procedure = program_object.get_procedure(*$1);
		current_procedure->check_arguments(*$3, get_line_number());
	}
|
	NAME '(' ')'
	{
		if(*$1=="main"){
			current_procedure = new Procedure(void_data_type, *$1, vector<Symbol_Table_Entry*>());
			program_object.set_procedure_map(*current_procedure);
		}
		else{
			current_procedure = program_object.get_procedure(*$1);
			current_procedure->check_arguments(vector<Symbol_Table_Entry*>(), get_line_number());
		}
	}

;

procedure_name_decl:
	NAME '(' argument_list ')' ';'
	{
		$$ = new Procedure(void_data_type, *$1, *$3);
	}
|
	NAME '(' ')' ';'
	{
		$$ = new Procedure(void_data_type, *$1, vector<Symbol_Table_Entry*>());
	}
;

function_declaration:
	INTEGER procedure_name_decl
	{
		$2->set_return_type(int_data_type);
		program_object.set_procedure_map(*$2);
	}
|
	FLOAT procedure_name_decl
	{
		$2->set_return_type(float_data_type);
		program_object.set_procedure_map(*$2);
	}
|
	DOUBLE procedure_name_decl
	{
		$2->set_return_type(float_data_type);
		program_object.set_procedure_map(*$2);
	}
|
	VOID procedure_name_decl
	{
		$2->set_return_type(void_data_type);
		program_object.set_procedure_map(*$2);
	}
;

procedure_body:
	'{' declaration_statement_list
	{
		current_procedure->set_local_list(*$2);
		delete $2;
	}
	basic_block_list '}'
	{

		current_procedure->set_basic_block_list(*$4);
		current_procedure->bb_check_goto_number_exist(get_line_number());

		delete $4;
	}
|
	'{' basic_block_list '}'
	{
		current_procedure->set_basic_block_list(*$2);
		current_procedure->bb_check_goto_number_exist(get_line_number());

		delete $2;
	}
;

argument_list:
	argument_list ',' argument
	{
		$1->push_back($3);
		$$ = $1;
	}
|
	argument
	{
		$$ = new vector<Symbol_Table_Entry*>;
		$$->push_back($1);
	}
;

argument:
	INTEGER NAME
	{
		$$ = new Symbol_Table_Entry(*$2, int_data_type);
		delete $2;
	}
|
	FLOAT NAME
	{
		$$ = new Symbol_Table_Entry(*$2, float_data_type);
		delete $2;
	}
|
	DOUBLE NAME
	{
		$$ = new Symbol_Table_Entry(*$2, float_data_type);
		delete $2;
	}
;

declaration_statement_list:
	declaration_statement
	{
		int line = get_line_number();
		program_object.variable_in_proc_map_check($1->get_variable_name(), line);

		string var_name = $1->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		$$ = new Symbol_Table();
		$$->push_symbol($1);
	}
|
	declaration_statement_list declaration_statement
	{

		int line = get_line_number();
		program_object.variable_in_proc_map_check($2->get_variable_name(), line);

		string var_name = $2->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		if ($1 != NULL)
		{
			if($1->variable_in_symbol_list_check(var_name))
			{
				int line = get_line_number();
				report_error("Variable is declared twice", line);
			}

			$$ = $1;
		}

		else
			$$ = new Symbol_Table();

		$$->push_symbol($2);
	}
|
	declaration_statement_list function_declaration
	{
		$$ = $1;
	}
|
	function_declaration
	{
		$$ = NULL;
	}
;

declaration_statement:
	INTEGER NAME ';'
	{
		$$ = new Symbol_Table_Entry(*$2, int_data_type);
		delete $2;
	}
|
	FLOAT NAME ';'
	{
		$$ = new Symbol_Table_Entry(*$2, float_data_type);
		delete $2;
	}
|
	DOUBLE NAME ';'
	{
		$$ = new Symbol_Table_Entry(*$2, float_data_type);
		delete $2;
	}
;



basic_block_list:
	basic_block_list basic_block
	{
		if (!$2)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		bb_strictly_increasing_order_check($$, $2->get_bb_number());

		$$ = $1;
		$$->back()->set_successor(true);
		$$->push_back($2);
	}
|
	basic_block
	{
		if (!$1)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		$$ = new list<Basic_Block *>;
		$$->push_back($1);
	}

;

basic_block:
	basic_block_number ':' executable_statement_list
	{

		if ($3 != NULL){
			$$ = new Basic_Block($1, *$3);
			$$->set_successor(check_bb_has_successor);
		}
		else
		{
			list<Ast *> * ast_list = new list<Ast *>;
			$$ = new Basic_Block($1, *ast_list);
			$$->set_successor(false);
		}

		delete $3;
	}
;

executable_statement_list:
	assignment_statement_list
	{
		$$ = $1;
		check_bb_has_successor = false;
	}
|
	assignment_statement_list goto_statement
	{
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;
		check_bb_has_successor = true;
		$$->push_back($2);
	}
|
	assignment_statement_list if_statement
	{
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;
		check_bb_has_successor = true;
		$$->push_back($2);
	}
|
	assignment_statement_list return_statement
	{
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;
		check_bb_has_successor = true;
		$$->push_back($2);
	}
;

return_statement:
	RETURN expression ';'
	{
		$$ = new Return_Ast($2,current_procedure);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	RETURN ';'
	{
		$$ = new Return_Ast(NULL,current_procedure);
		int line = get_line_number();
		$$->check_ast(line);
	}
;

assignment_statement_list:
	{
		$$ = NULL;
	}
|
	assignment_statement_list assignment_statement
	{
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
	}
;

assignment_statement:
	variable ASSIGN_OP expression ';'
	{
		$$ = new Assignment_Ast($1, $3);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	function_call ';'
	{
		$$ = $1;
	}
;

goto_statement:
	GOTO basic_block_number ';'
	{
		$$ = new Goto_Stmt_Ast($2);
		current_procedure->goto_numbers.push_back($2);
	}
;

basic_block_number:
	BASIC_BLOCK
	{
		if ($1 < 2)
		{
			int line = get_line_number();
			report_error("Illegal basic block lable", line);
		}
		$$ = $1;
	}
;

if_statement:
	IF '(' conditional_expression ')' goto_statement ELSE goto_statement
	{
		$$ = new If_Else_Stmt_Ast($3, $5, $7);
	}
;


conditional_expression:
	expression GT expression
	{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	expression LT expression
	{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	expression GE expression
	{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	expression LE expression
	{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	expression EQ expression
	{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	expression NE expression
	{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}
;


atomic_expression:
	constant
	{
		$$ = $1;
	}
|
	variable
	{
		$$ = $1;
	}
|
	function_call
	{
		$$ = $1;
	}
|
	'(' expression ')'
	{
		$$ = $2;
	}
;

basic_expression:
	atomic_expression
	{
		$$ = $1;
	}
|
	'-' atomic_expression
	{
		$$ = new Unary_Ast($2);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	'(' FLOAT ')' atomic_expression
	{
		$$  = new TypeCast_Ast($4, float_data_type);
	}
|
	'(' INTEGER ')' atomic_expression
	{
		$$  = new TypeCast_Ast($4, int_data_type);
	}
|
	'(' DOUBLE ')' atomic_expression
	{
		$$  = new TypeCast_Ast($4, float_data_type);
	}

;

expression:
	basic_expression
	{
		$$ = $1;
	}
|
	conditional_expression
	{
		$$ = $1;
	}
|
	arithmetic_expression
	{
		$$ = $1;
	}
;

arithmetic_expression:
	expression '+' expression
	{
		$$ = new Plus_Ast($1, $3);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	expression '-' expression
	{
		$$ = new Minus_Ast($1, $3);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	expression '*' expression
	{
		$$ = new Multiplication_Ast($1, $3);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	expression '/' expression
	{
		$$ = new Division_Ast($1, $3);
		int line = get_line_number();
		$$->check_ast(line);
	}

;

function_call:
	NAME '(' expression_list ')'
	{
		$$ = new Call_Ast(program_object.get_procedure(*$1),*$3);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	NAME '(' ')'
	{
		$$ = new Call_Ast(program_object.get_procedure(*$1), vector<Ast*>());
		int line = get_line_number();
		$$->check_ast(line);
	}
;

expression_list:
	expression_list ',' expression
	{
		$$->push_back($3);
	}
|
	expression
	{
		$$ = new vector<Ast*>();
		$$->push_back($1);
	}
;
variable:
	NAME
	{
		Symbol_Table_Entry var_table_entry;

		if (current_procedure->variable_in_symbol_list_check(*$1))
			 var_table_entry = current_procedure->get_symbol_table_entry(*$1);

		else if (program_object.variable_in_symbol_list_check(*$1))
			var_table_entry = program_object.get_symbol_table_entry(*$1);

		else
		{
			int line = get_line_number();
			report_error("Variable has not been declared", line);
		}

		$$ = new Name_Ast(*$1, var_table_entry);

		delete $1;
	}
;

constant:
	INTEGER_NUMBER
	{
		$$ = new Number_Ast<int>($1, int_data_type);
	}
|
	FNUM
	{
		$$ = new Number_Ast<float>($1, float_data_type);
	}
;
