#pragma once

#include <map>
#include <string>
#include <memory>

#include "ast_fwd.h"

class ast_visitor {
public:
	virtual void visit(std::shared_ptr<compilation_unit> program);
	virtual void visit(std::shared_ptr<function_declaration> func);
	virtual void visit(std::shared_ptr<variable_declaration> var);
	virtual void visit(std::shared_ptr<type_declaration> decl);
	virtual void visit(std::shared_ptr<module_declaration> mod);
	virtual void visit(std::shared_ptr<conversion_declaration> conv);
	virtual void visit(std::shared_ptr<operator_declaration> op);
	virtual void visit(std::shared_ptr<enum_declaration> enum_decl);
	virtual void visit(std::shared_ptr<expression_statement> expr);
	virtual void visit(std::shared_ptr<binary_expression> expr);
	virtual void visit(std::shared_ptr<assignment_expression> expr);
	virtual void visit(std::shared_ptr<member_expression> expr);
	virtual void visit(std::shared_ptr<address_of_expression> expr);
	virtual void visit(std::shared_ptr<deref_expression> expr);
	virtual void visit(std::shared_ptr<unary_expression> expr);
	virtual void visit(std::shared_ptr<index_expression> expr);
	virtual void visit(std::shared_ptr<identifier_expression> expr);
	virtual void visit(std::shared_ptr<this_expression> expr);
	virtual void visit(std::shared_ptr<cast_expression> expr);
	virtual void visit(std::shared_ptr<initializer_list> init);
	virtual void visit(std::shared_ptr<function_call> func_call);
	virtual void visit(std::shared_ptr<literal> literal);
	virtual void visit(std::shared_ptr<import_statement> import_stmt);
	virtual void visit(std::shared_ptr<code_block> block);
	virtual void visit(std::shared_ptr<if_statement> if_stmt);
	virtual void visit(std::shared_ptr<inline_if> inline_if);
	virtual void visit(std::shared_ptr<for_loop> for_loop);
	virtual void visit(std::shared_ptr<while_loop> while_loop);
	virtual void visit(std::shared_ptr<return_statement> ret_stmt);
	virtual void visit(std::shared_ptr<break_statement> brk_stmt);
	virtual void visit(std::shared_ptr<generic_parameter> param);
};