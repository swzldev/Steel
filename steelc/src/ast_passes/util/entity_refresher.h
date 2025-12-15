#pragma once

#include <memory>

#include <ast/ast_visitor.h>
#include <compiler/compilation_pass.h>
#include <symbolics/symbol_table.h>

class entity_refresher : public ast_visitor {
public:
	entity_refresher(const symbol_table& sym_table)
		: sym_table(sym_table) {
	}

	void visit(std::shared_ptr<identifier_expression> expr) override;
	void visit(std::shared_ptr<member_expression> expr) override;

private:
	const symbol_table& sym_table;
};