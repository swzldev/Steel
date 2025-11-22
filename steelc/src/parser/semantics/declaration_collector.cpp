#include "declaration_collector.h"

#include "../ast/ast.h"

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
		return;
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

	// function-specific logic
	// check modifiers
	if (true) {}

	// if were in a type it must be a method
	if (current_type) {
		func->is_method = true;
	}
	else {
		// check if function is already defined (global)
		error_code err = sym_table->add_function(func);
		if (err != ERR_SUCCESS) {
			// we know all the errors reported here only use the functions identifier
			// but this may need to change
			ERROR(err, func->position, func->identifier.c_str());
		}
	}

	// void parameters are not allowed in functions & constructors
	for (const auto& param : func->parameters) {
		if (param->type->is_primitive() && param->type->primitive == DT_VOID) {
			ERROR(ERR_PARAM_VOID_TYPE, func->position);
			break;
		}
	}

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
		auto err = sym_table->add_variable(var);
		if (err == ERR_VARIABLE_ALREADY_DECLARED_SCOPE) {
			ERROR(ERR_VARIABLE_ALREADY_DECLARED_SCOPE, var->position, var->identifier.c_str());
			return;
		}
		// shouldnt have any generic problems in the global scope
	}
}
void declaration_collector::visit(std::shared_ptr<type_declaration> decl) {
	// check if type is already defined
	auto err = sym_table->add_type(decl);
	if (err == ERR_TYPE_ALREADY_DEFINED) {
		ERROR(ERR_TYPE_ALREADY_DEFINED, decl->position, decl->name().c_str());
		return;
	}
	else if (err == ERR_ENUM_ALREADY_DEFINED) {
		ERROR(ERR_ENUM_ALREADY_DEFINED, decl->position, decl->name().c_str());
		return;
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

	// check if were exporting this function

	current_type = decl;
	for (const auto& member : decl->fields) {
		member->accept(*this);
	}
	for (const auto& constructor : decl->constructors) {
		constructor->accept(*this);
	}
	for (const auto& method : decl->methods) {
		method->accept(*this);
		method->parent_type = decl;
	}
	for (const auto& op : decl->operators) {
		op->accept(*this);
	}
	current_type = nullptr;
}
void declaration_collector::visit(std::shared_ptr<module_declaration> mod) {
	std::string module_name = mod->name;

	// identify full module name
	if (!current_module->is_global()) {
		module_name = current_module->name + "." + module_name;
	}

	// lookup module
	auto module = module_manager.get_module(module_name);
	if (module != nullptr) {
		current_module = module;
	}
	else {
		// doesnt exist - create a new module
		current_module = module_manager.add_module(module_name, current_module);
	}
	sym_table = &current_module->symbols;
	mod->module_info = current_module;

	for (const auto& decl : mod->declarations) {
		decl->accept(*this);
	}

	// returns the parent of the current module (or the global module)
	current_module = current_module->parent_module;
	sym_table = &current_module->symbols;
}
void declaration_collector::visit(std::shared_ptr<enum_declaration> enm) {
	// ensure all options reference the owner
	for (auto& option : enm->options) {
		option->declaration = enm;
	}

	// check if type is already defined
	auto err = sym_table->add_enum(enm);
	if (err == ERR_ENUM_ALREADY_DEFINED) {
		ERROR(ERR_ENUM_ALREADY_DEFINED, enm->position, enm->name().c_str());
		return;
	}
	else if (err == ERR_TYPE_ALREADY_DEFINED) {
		ERROR(ERR_TYPE_ALREADY_DEFINED, enm->position, enm->name().c_str());
		return;
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