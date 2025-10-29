#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "classes/function_return.h"
#include "classes/runtime_value.h"
#include "../parser/ast/ast_fwd.h"
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
	virtual void visit(std::shared_ptr<address_of_expression> expr) override;
	virtual void visit(std::shared_ptr<deref_expression> expr) override;
	virtual void visit(std::shared_ptr<unary_expression> expr) override;
	virtual void visit(std::shared_ptr<index_expression> expr) override;
	virtual void visit(std::shared_ptr<identifier_expression> expr) override;
	virtual void visit(std::shared_ptr<this_expression> expr) override;
	virtual void visit(std::shared_ptr<cast_expression> expr) override;
	virtual void visit(std::shared_ptr<initializer_list> init) override;
	virtual void visit(std::shared_ptr<function_call> func_call) override;
	virtual void visit(std::shared_ptr<literal> literal) override;
	virtual void visit(std::shared_ptr<code_block> block) override;
	virtual void visit(std::shared_ptr<if_statement> if_stmt) override;
	virtual void visit(std::shared_ptr<inline_if> if_stmt) override;
	virtual void visit(std::shared_ptr<for_loop> for_loop) override;
	virtual void visit(std::shared_ptr<while_loop> while_loop) override;
	virtual void visit(std::shared_ptr<return_statement> ret_stmt) override;
	virtual void visit(std::shared_ptr<break_statement> brk_stmt) override;

private:
	std::vector<std::map<std::string, std::shared_ptr<runtime_value>>> variables;
	void push_scope();
	void pop_scope();
	void add_var(const std::string& identifier, std::shared_ptr<runtime_value> value);
	std::shared_ptr<runtime_value> get_var(const std::string& identifier);
	void set_var(const std::string& identifier, std::shared_ptr<runtime_value> value);

	std::shared_ptr<runtime_value> expression_result;
	std::shared_ptr<runtime_value> this_object;
	std::vector<std::shared_ptr<runtime_value>> this_stack;

	val_ptr new_val(const type_ptr type); // initializes to default value of type
	val_ptr new_val(const type_ptr type, const std::string& value);
	val_ptr new_val(val_ptr other);
	val_ptr new_pointer(std::shared_ptr<runtime_value> pointee);
	val_ptr new_array(const type_ptr elem_type, const std::vector<std::shared_ptr<runtime_value>>& elements);

	void build_vftable(std::shared_ptr<runtime_value> obj, std::shared_ptr<custom_type> type);
	void set_this(std::shared_ptr<runtime_value> obj);
	void remove_this();

	std::vector<std::shared_ptr<function_declaration>> function_stack;
	void enter_function(std::shared_ptr<function_declaration> func, std::vector<std::shared_ptr<expression>> args);
	void enter_method(std::shared_ptr<function_declaration> func, std::vector<std::shared_ptr<expression>> args, std::shared_ptr<runtime_value> object);

	void throw_exception(std::string message, position pos);
};