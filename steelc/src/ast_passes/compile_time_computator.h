#pragma once

#include <memory>
#include <string>

#include <ast_passes/classes/computed_value.h>
#include <ast/ast_visitor.h>
#include <ast/ast_fwd.h>
#include <representations/types/data_type.h>

class compile_time_computator_visitor : ast_visitor {
	compile_time_computator_visitor()
		: result(data_type::UNKNOWN, "") {
	}

	//void visit(std::shared_ptr<function_declaration> func) override;
	//void visit(std::shared_ptr<variable_declaration> var) override;
	//void visit(std::shared_ptr<type_declaration> decl);
	//void visit(std::shared_ptr<module_declaration> mod);
	//void visit(std::shared_ptr<conversion_declaration> conv);
	//void visit(std::shared_ptr<operator_declaration> op);
	//void visit(std::shared_ptr<enum_declaration> enum_decl);
	//void visit(std::shared_ptr<enum_option> option);
	//void visit(std::shared_ptr<expression_statement> expr);
	void visit(std::shared_ptr<binary_expression> expr);
	//void visit(std::shared_ptr<assignment_expression> expr);
	//void visit(std::shared_ptr<member_expression> expr);
	//void visit(std::shared_ptr<address_of_expression> expr);
	//void visit(std::shared_ptr<deref_expression> expr);
	//void visit(std::shared_ptr<unary_expression> expr);
	//void visit(std::shared_ptr<index_expression> expr);
	//void visit(std::shared_ptr<identifier_expression> id) override;
	//void visit(std::shared_ptr<this_expression> expr);
	//void visit(std::shared_ptr<cast_expression> expr);
	//void visit(std::shared_ptr<initializer_list> init);
	//void visit(std::shared_ptr<function_call> func_call);
	void visit(std::shared_ptr<literal> literal) override;
	//void visit(std::shared_ptr<import_statement> import_stmt);
	//void visit(std::shared_ptr<code_block> block) override;
	//void visit(std::shared_ptr<if_statement> if_stmt) override;
	//void visit(std::shared_ptr<inline_if> inline_if);
	//void visit(std::shared_ptr<for_loop> for_loop);
	//void visit(std::shared_ptr<while_loop> while_loop);
	//void visit(std::shared_ptr<return_statement> ret_stmt) override;
	//void visit(std::shared_ptr<break_statement> brk_stmt);

private:
	computed_value result;

	template<typename Nty>
	computed_value accept(std::shared_ptr<Nty> node) {
		result = computed_value(data_type::UNKNOWN, "");
		node->accept(*this);
		return result;
	}

	void not_constant_expr() const;
};