#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "classes/runtime_value.h"
#include "classes/function_return.h"
#include "../parser/ast/ast_visitor.h"
#include "../parser/symbolics/symbol_table.h"

class interpreter_visitor : public ast_visitor {
public:
	interpreter_visitor()
		: this_object(nullptr) {
	}

	void begin(std::shared_ptr<function_declaration> entry_point);

	virtual void visit(std::shared_ptr<function_declaration> func) override;
	virtual void visit(std::shared_ptr<variable_declaration> var) override;
	virtual void visit(std::shared_ptr<type_declaration> decl) override;
	virtual void visit(std::shared_ptr<expression_statement> expr) override;
	virtual void visit(std::shared_ptr<binary_expression> expr) override;
	virtual void visit(std::shared_ptr<assignment_expression> expr) override;
	virtual void visit(std::shared_ptr<member_expression> expr) override;
	virtual void visit(std::shared_ptr<unary_expression> expr) override;
	virtual void visit(std::shared_ptr<index_expression> expr) override;
	virtual void visit(std::shared_ptr<identifier_expression> expr) override;
	virtual void visit(std::shared_ptr<this_expression> expr) override;
	virtual void visit(std::shared_ptr<initializer_list> init) override;
	virtual void visit(std::shared_ptr<function_call> func_call) override;
	virtual void visit(std::shared_ptr<constructor_call> constructor_call) override;
	virtual void visit(std::shared_ptr<literal> literal) override;
	virtual void visit(std::shared_ptr<block_statement> block) override;
	virtual void visit(std::shared_ptr<if_statement> if_stmt) override;
	virtual void visit(std::shared_ptr<inline_if> if_stmt) override;
	virtual void visit(std::shared_ptr<for_loop> for_loop) override;
	virtual void visit(std::shared_ptr<while_loop> while_loop) override;
	virtual void visit(std::shared_ptr<return_statement> ret_stmt) override;
	virtual void visit(std::shared_ptr<return_if> ret_stmt) override;

private:
	std::vector<std::map<std::string, std::shared_ptr<runtime_value>>> variables;
	void push_scope();
	void pop_scope();
	void add_var(const std::string& identifier, std::shared_ptr<runtime_value> value);
	std::shared_ptr<runtime_value> get_var(const std::string& identifier);
	void set_var(const std::string& identifier, std::shared_ptr<runtime_value> value);

	std::shared_ptr<runtime_value> expression_result;
	std::shared_ptr<runtime_value> this_object;

	void set_this(std::shared_ptr<runtime_value> obj);
	void remove_this();

	std::vector<std::shared_ptr<function_declaration>> function_stack;
	void enter_function(std::shared_ptr<function_declaration> func, std::vector<std::shared_ptr<expression>> args);

	void throw_exception(std::string message, position pos);
};