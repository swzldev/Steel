#include <memory>

// type forward declarations
class data_type;
class custom_type;
class array_type;
class pointer_type;
class enum_type;
class enum_option_type;
class generic_type;

using type_ptr = std::shared_ptr<data_type>;