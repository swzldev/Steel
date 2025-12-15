#include "entity_refresher.h"

#include <memory>

#include <ast/ast.h>

void entity_refresher::visit(std::shared_ptr<identifier_expression> expr) {
	expr->entity_ref.resolve(sym_table);
}
void entity_refresher::visit(std::shared_ptr<member_expression> expr) {
	expr->entity_ref.resolve(sym_table);
}
