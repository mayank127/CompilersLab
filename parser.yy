
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
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;
	Relation_Op relation_op;
};

%token <integer_value> INTEGER_NUMBER BASIC_BLOCK
%token <float_value> FNUM
%token <string_value> NAME

%token RETURN INTEGER FLOAT DOUBLE IF ELSE GOTO ASSIGN_OP

%left <relation_op> NE EQ
%left <relation_op> LT LE GT GE

%left '+' '-'
%left '/' '*'

/*
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
%type <ast> conditional_expression
%type <ast> variable
%type <ast> constant
%type <integer_value> basic_block_number
*/


%start program

%%

program:
	declaration_statement_list procedure_name
	/*{
		program_object.set_global_table(*$1);
	}*/
	procedure_body
	/*{
		program_object.set_procedure_map(*current_procedure);

		if ($1)
			$1->global_list_in_proc_map_check(get_line_number());

		delete $1;
	}*/
|
	procedure_name
/*	{
	}
*/	procedure_body
/*	{
		program_object.set_procedure_map(*current_procedure);
	}
*/;

procedure_name:
	NAME '(' ')'
	/*{
		current_procedure = new Procedure(void_data_type, *$1);
	}*/
;

procedure_body:
	'{' declaration_statement_list
	/*{
		current_procedure->set_local_list(*$2);
		delete $2;
	}*/
	basic_block_list '}'
	/*{

		current_procedure->set_basic_block_list(*$4);

		bb_check_goto_number_exist($4);

		delete $4;
	}*/
|
	'{' basic_block_list '}'
	/*{

		current_procedure->set_basic_block_list(*$2);
		bb_check_goto_number_exist($2);

		delete $2;
	}*/
;

declaration_statement_list:
	declaration_statement
	/*{
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
	}*/
|
	declaration_statement_list declaration_statement
	/*{
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list

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
	}*/
;

declaration_statement:
	INTEGER NAME ';'
	/*{
		$$ = new Symbol_Table_Entry(*$2, int_data_type);

		delete $2;
	}*/
|
	FLOAT NAME ';'
|
	DOUBLE NAME ';'

;

basic_block_list:
	basic_block_list basic_block
	/*{
		if (!$2)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		bb_strictly_increasing_order_check($$, $2->get_bb_number());

		$$ = $1;
		$$->back()->set_successor(true);
		$$->push_back($2);
	}*/
|
	basic_block
	/*{
		if (!$1)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		$$ = new list<Basic_Block *>;
		$$->push_back($1);
	}*/

;

basic_block:
	basic_block_number ':' executable_statement_list
	/*{

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
	}*/
;

executable_statement_list:
	assignment_statement_list
	/*{
		$$ = $1;
		check_bb_has_successor = false;
	}*/
|
	assignment_statement_list goto_statement
	/*{
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;
		check_bb_has_successor = true;
		$$->push_back($2);
	}*/
|
	assignment_statement_list if_statement
	/*{
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;
		check_bb_has_successor = true;
		$$->push_back($2);
	}*/
|
	assignment_statement_list RETURN ';'
	/*{
		Ast * ret = new Return_Ast();


		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;
		check_bb_has_successor = true;
		$$->push_back(ret);
	}*/
;

assignment_statement_list:
	/*{
		$$ = NULL;
	}*/
|
	assignment_statement_list assignment_statement
	/*{
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
	}*/
;

assignment_statement:
	variable ASSIGN_OP expression ';'
	/*{
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
	}*/
;

goto_statement:
	GOTO basic_block_number ';'
	/*{
		$$ = new Goto_Stmt_Ast($2);
		goto_numbers.push_back($2);
	}*/
;

basic_block_number:
	BASIC_BLOCK
	/*{
		if ($1 < 2)
		{
			int line = get_line_number();
			report_error("Illegal basic block lable", line);
		}
		$$ = $1;
	}*/
;

if_statement:
	IF '(' conditional_expression ')' goto_statement ELSE goto_statement
	/*{
		$$ = new If_Else_Stmt_Ast($3, $5, $7);
	}*/
;


conditional_expression:
	expression GT expression
	/*{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}*/
|
	expression LT expression
	/*{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}*/
|
	expression GE expression
	/*{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}*/
|
	expression LE expression
	/*{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}*/
|
	expression EQ expression
	/*{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}*/
|
	expression NE expression
	/*{
		$$ = new Relational_Expr_Ast($1, $3, $2);
		int line = get_line_number();
		$$->check_ast(line);
	}*/
;

basic_expression:
	constant
	/*{
		$$ = $1;
	}*/
|
	variable
	/*{
		$$ = $1;
	}*/
|

	arithmetic_expression
|
	'(' arithmetic_expression ')'
;

expression:

	basic_expression
|
	conditional_expression
	/*{
		$$ = $1;
	}*/

;

arithmetic_expression:
	basic_expression '+' basic_expression
|
	basic_expression '-' basic_expression
|
	basic_expression '*' basic_expression
|
	basic_expression '/' basic_expression
|
	'-' basic_expression
|
	'(' FLOAT ')' basic_expression
|
	'(' INTEGER ')' basic_expression
|
	'(' DOUBLE ')' basic_expression
;
variable:
	NAME
	/*{
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
	}*/
;

constant:
	INTEGER_NUMBER
	/*{
		$$ = new Number_Ast<int>($1, int_data_type);
	}*/
|
	FNUM
;
