#include "declaration_collector.h"

#include <ast/ast_fwd.h>
#include <ast/ast.h>
#include <error/error_catalog.h>
#include <compiler/compilation_pass.h>
#include <modules/module_manager.h>
#include <symbolics/symbol_table.h>
#include <representations/entities/module_entity.h>
#include <representations/entities/entity_ref.h>

void declaration_collector::visit(std::shared_ptr<function_declaration> func) {
	// functions and constructors cannot be nested
	if (current_function || current_constructor) {
		if (func->is_constructor) {
			ERROR(ERR_NESTED_CONSTRUCTOR_NOT_ALLOWED, func->position);
		}
		else {
			ERROR(ERR_NESTED_FUNCTION_NOT_ALLOWED, func->position);
		}
		return;
	}

	// overrides cant be generic
	if (func->is_generic && func->is_override) {
		ERROR(ERR_OVERRIDE_CANT_BE_GENERIC, func->position);
	}

	// TODO: this should be a type checker variable check
	// void parameters are not allowed in functions & constructors
	for (const auto& param : func->parameters) {
		if (param->type->is_primitive() && param->type->primitive == DT_VOID) {
			ERROR(ERR_PARAM_VOID_TYPE, func->position);
			break;
		}
	}

	// constructor-specific logic
	if (func->is_constructor) {
		// constructors cannot be defined outside of a type declaration
		if (!current_type) {
			ERROR(ERR_CONSTRUCTOR_OUTSIDE_TYPE, func->position);
			return;
		}
		func->return_type = current_type->type();

		// check if constructor is already defined
		for (const auto& existing_constructor : current_type->constructors) {
			if (existing_constructor != func && existing_constructor->parameters == func->parameters) {
				ERROR(ERR_CONSTRUCTOR_OVERLOAD_EXISTS, func->position);
				return;
			}
		}

		current_constructor = func;
		if (func->body) {
			func->body->accept(*this);
		}
		current_constructor = nullptr;
		return;
	}

	// check modifiers
	//if (true) {}

	// add to symbol table
	add_symbol_result res = sym_table->add_symbol(func, current_type ? type_entity::get(current_type) : nullptr);
	if (res.error != SYMBOL_OK) {
		switch (res.error) {
		case SYMBOL_CONFLICTS_WITH_FUNCTION: // in this case it means exact same signature
			ERROR(ERR_FUNCTION_ALREADY_DEFINED, func->position, func->identifier.c_str());
			break;
		case SYMBOL_CANNOT_OVERLOAD_BY_RETURN_TYPE:
			ERROR(ERR_CANNOT_OVERLOAD_BY_RETURN_TYPE, func->position, func->identifier.c_str());
			break;

		case SYMBOL_CONFLICTS_WITH_VARIABLE:
			ERROR(ERR_NAME_CONFLICT, func->position, "function", func->identifier, "variable");
			break;
		case SYMBOL_CONFLICTS_WITH_TYPE:
			ERROR(ERR_NAME_CONFLICT, func->position, "function", func->identifier, "type");
			break;
		case SYMBOL_CONFLICTS_WITH_MODULE:
			ERROR(ERR_NAME_CONFLICT, func->position, "function", func->identifier, "module");
			break;

		default:
			ERROR(ERR_INTERNAL_ERROR, func->position, "Declaration Collector", "Unknown error while adding function symbol");
			break;
		}
	}
	func->func_ref = entity_ref(res.eid);

	// set parent module (even for methods, for now anyway)
	func->parent_module = current_module;

	if (func->body) {
		current_function = func;
		func->body->accept(*this);
		current_function = nullptr;
	}
}
void declaration_collector::visit(std::shared_ptr<variable_declaration> var) {
	// NOTE: only global variables should be picked up here
	// local variables will be handled in a later pass

	// no variables should be of type void
	if (var->type->is_primitive() && var->type->primitive == DT_VOID) {
		ERROR(ERR_VARIABLE_VOID_TYPE, var->position);
		return;
	}

	// const variables must have an initializer
	if (var->is_const && !var->initializer) {
		ERROR(ERR_CONST_NO_INITIALIZER, var->position);
		return;
	}

	if (!current_function && !current_constructor && !current_type) {
		add_symbol_result res = sym_table->add_symbol(var);
		// only check for module level errors
		// as this is a module-level variable
		if (res.error != SYMBOL_OK) {
			switch (res.error) {
				case SYMBOL_CONFLICTS_WITH_VARIABLE:
					ERROR(ERR_VARIABLE_ALREADY_DECLARED_SCOPE, var->position, var->identifier.c_str());
					break;
				case SYMBOL_CONFLICTS_WITH_FUNCTION:
					ERROR(ERR_NAME_CONFLICT, var->position, "variable", var->identifier, "function");
					break;
				case SYMBOL_CONFLICTS_WITH_TYPE:
					ERROR(ERR_NAME_CONFLICT, var->position, "variable", var->identifier, "type");
					break;
				case SYMBOL_CONFLICTS_WITH_ENUM:
					ERROR(ERR_NAME_CONFLICT, var->position, "variable", var->identifier, "enum");
					break;
				case SYMBOL_CONFLICTS_WITH_MODULE:
					ERROR(ERR_NAME_CONFLICT, var->position, "variable", var->identifier, "module");
					break;

				default:
					ERROR(ERR_INTERNAL_ERROR, var->position, "Declaration Collector", "Unknown error while adding variable symbol");
					break;
			}
		}
		var->var_ref = entity_ref(res.eid);
	}

	// set parent module
	var->parent_module = current_module;
}
void declaration_collector::visit(std::shared_ptr<type_declaration> decl) {
	// check if type is already defined
	add_symbol_result res = sym_table->add_symbol(decl);
	if (res.error != SYMBOL_OK) {
		switch (res.error) {
			case SYMBOL_CONFLICTS_WITH_TYPE:
				ERROR(ERR_TYPE_ALREADY_DEFINED, decl->position, decl->name().c_str());
				break;

			case SYMBOL_CONFLICTS_WITH_VARIABLE:
				ERROR(ERR_NAME_CONFLICT, decl->position, "type", decl->name(), "variable");
				break;
			case SYMBOL_CONFLICTS_WITH_FUNCTION:
				ERROR(ERR_NAME_CONFLICT, decl->position, "type", decl->name(), "function");
				break;
			case SYMBOL_CONFLICTS_WITH_ENUM:
				ERROR(ERR_NAME_CONFLICT, decl->position, "type", decl->name(), "enum");
				break;
			case SYMBOL_CONFLICTS_WITH_MODULE:
				ERROR(ERR_NAME_CONFLICT, decl->position, "type", decl->name(), "module");
				break;

			default:
				ERROR(ERR_INTERNAL_ERROR, decl->position, "Declaration Collector", "Unknown error while adding type symbol");
				break;
		}
	}

	// types cannot be nested (yet)
	if (current_type || current_function || current_constructor) {
		ERROR(ERR_NESTED_TYPE_DECLARATION_NOT_ALLOWED, decl->position);
		return;
	}

	// structs cannot contain methods
	if (decl->type_kind == CT_STRUCT && !decl->methods.empty()) {
		ERROR(ERR_STRUCT_CONTAINS_METHOD, decl->position);
		return;
	}

	// structs and interfaces cannot be derived
	if (decl->type_kind == CT_STRUCT && !decl->base_types.empty()) {
		ERROR(ERR_STRUCT_IS_DERIVED, decl->position);
		return;
	}
	else if (decl->type_kind == CT_INTERFACE && !decl->base_types.empty()) {
		ERROR(ERR_INTERFACE_IS_DERIVED, decl->position);
		return;
	}

	// interfaces cannot contain constructors or member variables
	if (decl->type_kind == CT_INTERFACE && !decl->constructors.empty()) {
		ERROR(ERR_INTERFACE_CONTAINS_CONSTRUCTOR, decl->position);
		return;
	}
	if (decl->type_kind == CT_INTERFACE && !decl->fields.empty()) {
		ERROR(ERR_INTERFACE_CONTAINS_MEMBER_VAR, decl->position);
		return;
	}

	// set parent module (may be null)
	decl->parent_module = current_module;

	current_type = decl;
	for (const auto& member : decl->fields) {
		member->accept(*this);
	}
	for (const auto& constructor : decl->constructors) {
		// this is done during parsing (whether it should or not idk but ill leave it for now)
		//constructor->is_constructor = true;
		constructor->accept(*this);
	}
	for (const auto& method : decl->methods) {
		method->is_method = true;
		method->parent_type = decl;
		method->accept(*this);
	}
	for (const auto& op : decl->operators) {
		op->accept(*this);
	}
	current_type = nullptr;
}
void declaration_collector::visit(std::shared_ptr<module_declaration> mod) {
	std::vector<std::string> module_path;

	// identify full module name
	if (!current_module->is_global()) {
		module_path = current_module->name_path();
	}
	module_path.push_back(mod->name);

	// set the parent module (of the declaration, this doesnt effect the module_info representation)
	// not sure if this will ever come in handy, but its good to have it for all declarations
	mod->parent_module = current_module;

	// lookup module
	auto module = module_manager.get_module(module_path);
	if (module != nullptr) {
		current_module = module;
	}
	else {
		// doesnt exist - create a new module
		// note: dont pass full name, its automatically handled based on the parent module passed
		std::shared_ptr<module_entity> parent = current_module;
		current_module = module_manager.add_module(mod->name, current_module);
		if (parent) {
			// add as submodule
			parent->symbols().add_symbol(current_module);
		}
	}
	sym_table = &current_module->symbols();
	mod->entity = current_module;

	for (const auto& decl : mod->declarations) {
		decl->accept(*this);
	}

	current_module = current_module->parent_module;
	sym_table = &current_module->symbols();
}
void declaration_collector::visit(std::shared_ptr<enum_declaration> enm) {
	// ensure all options reference the owner
	for (auto& option : enm->options) {
		option->declaration = enm;
	}

	// add to symbol table
	add_symbol_result res = sym_table->add_symbol(enm);
	if (res.error != SYMBOL_OK) {
		switch (res.error) {
		case SYMBOL_CONFLICTS_WITH_ENUM:
			ERROR(ERR_ENUM_ALREADY_DEFINED, enm->position, enm->identifier);
			break;

		case SYMBOL_CONFLICTS_WITH_VARIABLE:
			ERROR(ERR_NAME_CONFLICT, enm->position, "enum", enm->identifier, "variable");
			break;
		case SYMBOL_CONFLICTS_WITH_FUNCTION:
			ERROR(ERR_NAME_CONFLICT, enm->position, "enum", enm->identifier, "function");
			break;
		case SYMBOL_CONFLICTS_WITH_TYPE:
			ERROR(ERR_NAME_CONFLICT, enm->position, "enum", enm->identifier, "type");
			break;
		case SYMBOL_CONFLICTS_WITH_MODULE:
			ERROR(ERR_NAME_CONFLICT, enm->position, "enum", enm->identifier, "module");
			break;

		default:
			ERROR(ERR_INTERNAL_ERROR, enm->position, "Declaration Collector", "Unknown error while adding enum symbol");
			break;
		}
	}

	// enums cannot be nested
	if (current_type || current_function || current_constructor) {
		ERROR(ERR_NESTED_ENUM_NOT_ALLOWED, enm->position);
		return;
	}

	// cannot have more than one option with the same identifier
	std::unordered_set<std::string> option_names;
	for (const auto& option : enm->options) {
		if (option_names.find(option->identifier) != option_names.end()) {
			ERROR(ERR_ENUM_OPTION_ALREADY_DEFINED, option->position, option->identifier.c_str(), enm->name().c_str());
			return;
		}
		option_names.insert(option->identifier);
	}

	// set parent module
	enm->parent_module = current_module;
}

void declaration_collector::visit(std::shared_ptr<import_statement> import_stmt) {
	// ensure top-level
	if (!current_module->is_global() || current_type || current_function || current_constructor || !sym_table->in_global_scope()) {
		ERROR(ERR_IMPORT_BAD_POS, import_stmt->position);
		return;
	}

	// everythings OK, do nothing
	// actually resolution occurs in the import_resolver pass
	// we only check here because we have more location context
}