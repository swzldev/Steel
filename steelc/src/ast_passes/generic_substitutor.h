#pragma once

#include <string>
#include <map>

#include <ast/ast_visitor.h>
#include <ast/ast_fwd.h>
#include <compiler/compilation_pass.h>
#include <representations/types/types_fwd.h>

class generic_substitutor : public ast_visitor, public compilation_pass {
public:
	generic_substitutor(std::shared_ptr<compilation_unit> unit, std::vector<type_ptr> substitution_list)
		: compilation_pass(unit), substitution_list(substitution_list) {
	}

	void visit(std::shared_ptr<function_declaration> func) override;
	void visit(std::shared_ptr<variable_declaration> var) override;
	void visit(std::shared_ptr<type_declaration> decl) override;
	void visit(std::shared_ptr<binary_expression> expr) override;
	void visit(std::shared_ptr<assignment_expression> expr) override;
	void visit(std::shared_ptr<address_of_expression> expr) override;
	void visit(std::shared_ptr<deref_expression> expr) override;
	void visit(std::shared_ptr<unary_expression> expr) override;
	void visit(std::shared_ptr<index_expression> expr) override;
	void visit(std::shared_ptr<cast_expression> expr) override;
	void visit(std::shared_ptr<member_expression> expr) override;
	void visit(std::shared_ptr<initializer_list> init) override;
	void visit(std::shared_ptr<function_call> func_call) override;
	void visit(std::shared_ptr<if_statement> if_stmt) override;
	void visit(std::shared_ptr<inline_if> inline_if) override;
	void visit(std::shared_ptr<for_loop> for_loop) override;
	void visit(std::shared_ptr<while_loop> while_loop) override;
	void visit(std::shared_ptr<return_statement> ret_stmt) override;

private:
	std::vector<type_ptr> substitution_list;

	// this function is much more safe for getting
	// substitutions as it WONT throw if index is OOB
	type_ptr get_substitution(size_t parameter_index) const;
	void try_substitute(type_ptr& type_ref) const;
};