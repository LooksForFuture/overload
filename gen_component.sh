#!/bin/sh

# Usage: ./gen_component.sh <component_name>
# Example: ./gen_component.sh collider

if [ $# -ne 1 ]; then
    echo "Usage: $0 <component_name>"
    exit 1
fi

COMPONENT_NAME="$1"
COMPONENT_NAME_UPPER=$(echo "$COMPONENT_NAME" | tr '[:lower:]' '[:upper:]')
COMPONENT_NAME_LOWER=$(echo "$COMPONENT_NAME" | tr '[:upper:]' '[:lower:]')

# Generate header file
cat > "include/${COMPONENT_NAME_LOWER}.h" << EOF
#ifndef ${COMPONENT_NAME_UPPER}_H
#define ${COMPONENT_NAME_UPPER}_H

#include <component.h>
#include <stdbool.h>

#define ${COMPONENT_NAME_UPPER}_PUBLIC_FIELDS(field) \\
	field(${COMPONENT_NAME_LOWER}, bool, enabled)

decl_component(${COMPONENT_NAME_LOWER}, ${COMPONENT_NAME_UPPER}_PUBLIC_FIELDS);

#endif /* ${COMPONENT_NAME_UPPER}_H */
EOF

# Generate source file
cat > "${COMPONENT_NAME_LOWER}.c" << EOF
#include <${COMPONENT_NAME_LOWER}.h>
#include <entity.h>
#include <component.h>

#define ${COMPONENT_NAME_UPPER}_FIELDS(field) \\
	${COMPONENT_NAME_UPPER}_PUBLIC_FIELDS(field)

static decl_component_storage(${COMPONENT_NAME_LOWER},
			      ${COMPONENT_NAME_UPPER}_PUBLIC_FIELDS,
			      ${COMPONENT_NAME_UPPER}_FIELDS)

static inline void ${COMPONENT_NAME_LOWER}_init_private(void) {}
static inline void ${COMPONENT_NAME_LOWER}_shutdown_private(void) {}
static inline void ${COMPONENT_NAME_LOWER}_add_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

static inline void ${COMPONENT_NAME_LOWER}_rem_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

static inline bool
${COMPONENT_NAME_LOWER}_enabled_impl(${COMPONENT_NAME_LOWER} handle)
{
	return ${COMPONENT_NAME_LOWER}_data.enabled[handle.id];
}

static inline void
${COMPONENT_NAME_LOWER}_set_enabled_impl(${COMPONENT_NAME_LOWER} handle, bool enabled)
{
	${COMPONENT_NAME_LOWER}_data.enabled[handle.id] = enabled;
}
EOF

echo "Generated ${COMPONENT_NAME_LOWER}.h and ${COMPONENT_NAME_LOWER}.c"
