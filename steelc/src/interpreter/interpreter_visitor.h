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

	void begin(std::shared_ptr<class function_declaration> entry_point);

	virtual void visit(std::shared_ptr<class function_declaration> func) override;
	virtual void visit(std::shared_ptr<class variable_declaration> var) override;
	virtual void visit(std::shared_ptr<class type_declaration> decl) override;
	virtual void visit(std::shared_ptr<class expression_statement> expr) override;
	virtual void visit(std::shared_ptr<class binary_expression> expr) override;
	virtual void visit(std::shared_ptr<class assignment_expression> expr) override;
	virtual void visit(std::shared_ptr<class member_expression> expr) override;
	virtual void visit(std::shared_ptr<class unary_expression> expr) override;
	virtual void visit(std::shared_ptr<class index_expression> expr) override;
	virtual void visit(std::shared_ptr<class identifier_expression> expr) override;
	virtual void visit(std::shared_ptr<class this_expression> expr) override;
	virtual void visit(std::shared_ptr<class initializer_list> init) override;
	virtual void visit(std::shared_ptr<class function_call> func_call) override;
	virtual void visit(std::shared_ptr<class constructor_call> constructor_call) override;
	virtual void visit(std::shared_ptr<class literal> literal) override;
	virtual void visit(std::shared_ptr<class block_statement> block) override;
	virtual void visit(std::shared_ptr<class if_statement> if_stmt) override;
	virtual void visit(std::shared_ptr<class for_loop> for_loop) override;
	virtual void visit(std::shared_ptr<class while_loop> while_loop) override;
	virtual void visit(std::shared_ptr<class return_statement> ret_stmt) override;

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