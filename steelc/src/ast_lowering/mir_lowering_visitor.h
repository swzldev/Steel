#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include <ast/ast_fwd.h>
#include <ast/ast_visitor.h>
#include <mir/mir_fwd.h>
#include <mir/mir_function.h>
#include <mir/mir_operand.h>
#include <mir/builders/mir_builder.h>

// mir_lowering_visitor
// 
// responsible for lowering AST nodes within a function to MIR
// typically these are statements and expressions
//
// this class is used internally by mir_lowerer
// it is not intended to be used directly

class mir_lowering_visitor : public ast_visitor {
public:
	mir_lowering_visitor(mir_function& current_func)
		: current_func(current_func) {
	}

	void visit(std::shared_ptr<variable_declaration> var) override;
	void visit(std::shared_ptr<function_declaration> func) override;
	//void visit(std::shared_ptr<enum_option> option);
	//void visit(std::shared_ptr<expression_statement> expr);
	//void visit(std::shared_ptr<binary_expression> expr);
	//void visit(std::shared_ptr<assignment_expression> expr);
	void visit(std::shared_ptr<member_expression> expr);
	//void visit(std::shared_ptr<address_of_expression> expr);
	//void visit(std::shared_ptr<deref_expression> expr);
	//void visit(std::shared_ptr<unary_expression> expr);
	//void visit(std::shared_ptr<index_expression> expr);
	void visit(std::shared_ptr<identifier_expression> id) override;
	//void visit(std::shared_ptr<this_expression> expr);
	//void visit(std::shared_ptr<cast_expression> expr);
	//void visit(std::shared_ptr<initializer_list> init);
	void visit(std::shared_ptr<function_call> func_call);
	void visit(std::shared_ptr<literal> literal) override;
	//void visit(std::shared_ptr<import_statement> import_stmt);
	void visit(std::shared_ptr<code_block> block) override;
	//void visit(std::shared_ptr<if_statement> if_stmt) override;
	//void visit(std::shared_ptr<inline_if> inline_if);
	//void visit(std::shared_ptr<for_loop> for_loop);
	//void visit(std::shared_ptr<while_loop> while_loop);
	void visit(std::shared_ptr<return_statement> ret_stmt) override;
	//void visit(std::shared_ptr<break_statement> brk_stmt);

private:
	mir_function& current_func;
	mir_builder builder;

	std::vector<std::unordered_map<std::string, mir_operand>> locals_stack;

	mir_operand result;

	template<typename Nty>
	mir_operand accept(std::shared_ptr<Nty> node) {
		result = {};
		node->accept(*this);
		return result;
	}

	// local management
	inline void push_scope() {
		locals_stack.push_back({});
	}
	inline void pop_scope() {
		if (!locals_stack.empty()) {
			locals_stack.pop_back();
		}
	}
	mir_operand get_local(const std::string& name);
};