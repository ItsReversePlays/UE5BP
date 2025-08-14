import unreal
import inspect

def get_property_type(prop):
    """Helper function to get a readable type for a UProperty."""
    try:
        if isinstance(prop, unreal.BoolProperty):
            return "bool"
        if isinstance(prop, unreal.IntProperty):
            return "int"
        if isinstance(prop, unreal.FloatProperty):
            return "float"
        if isinstance(prop, unreal.NameProperty):
            return "Name"
        if isinstance(prop, unreal.StrProperty):
            return "string"
        if isinstance(prop, unreal.TextProperty):
            return "Text"
        if isinstance(prop, unreal.ObjectProperty):
            return f"Object ({prop.get_property_class().get_name()})"
        if isinstance(prop, unreal.StructProperty):
            return f"Struct ({prop.get_struct().get_name()})"
        if isinstance(prop, unreal.EnumProperty):
            return f"Enum ({prop.get_enum().get_name()})"
        if isinstance(prop, unreal.ArrayProperty):
            return f"Array<{get_property_type(prop.get_inner())}>"
        if isinstance(prop, unreal.MapProperty):
            key = get_property_type(prop.get_key_prop())
            value = get_property_type(prop.get_value_prop())
            return f"Map<{key}, {value}>"
        if isinstance(prop, unreal.SetProperty):
            return f"Set<{get_property_type(prop.get_element_prop())}>"
        if isinstance(prop, unreal.DelegateProperty):
            return "Delegate"
        # Add more types as needed
        return prop.get_class().get_name()
    except Exception as e:
        return f"UnknownType ({e})"


def get_blueprint_callable_functions():
    """
    Iterates through all loaded classes and functions to find those
    that are exposed to Blueprints and prints their information.
    """
    unreal.log("Starting scan for Blueprint-callable functions...")

    # Get all loaded objects and filter for classes.
    # This is a robust way to get all UClass instances, even if it's a bit slow.
    all_objects = unreal.get_all_loaded_objects()
    all_classes = [obj for obj in all_objects if isinstance(obj, unreal.Class)]

    for cls in all_classes:
        # We only care about classes that can have callable functions.
        if not cls.has_any_class_flags(unreal.ClassFlags.FUNCTION):
            continue

        # Filter out classes that are not relevant to typical Blueprint usage
        # This is an optional step to reduce noise.
        path_name = cls.get_path_name()
        if "/Script/Editor" in path_name or "/Script/AlembicImporter" in path_name:
            continue

        try:
            functions = cls.get_functions()
        except Exception:
            # Some classes might not support get_functions()
            continue

        for func in functions:
            # Check if the function is callable from Blueprints
            if func.has_any_function_flags(unreal.FunctionFlags.BLUEPRINT_CALLABLE):

                # --- Basic Info ---
                class_name = cls.get_name()
                func_name = func.get_name()

                # --- Function Metadata ---
                category = func.get_meta_data("Category")
                is_pure = func.has_any_function_flags(unreal.FunctionFlags.BLUEPRINT_PURE)

                print(f"--- Function: {func_name} ---")
                print(f"  Class: {class_name}")
                if category:
                    print(f"  Category: {category}")
                print(f"  Is Pure: {is_pure}")

                # --- Parameters & Return Value ---
                properties = func.get_properties()
                return_prop = None
                params = []

                for prop in properties:
                    is_return_param = prop.has_any_property_flags(unreal.PropertyFlags.RETURN_PARAMETER)
                    is_out_param = prop.has_any_property_flags(unreal.PropertyFlags.OUT_PARAMETER)

                    param_info = {
                        "name": prop.get_name(),
                        "type": get_property_type(prop),
                        "direction": "Return" if is_return_param else ("Out" if is_out_param else "In")
                    }

                    if is_return_param:
                        return_prop = param_info
                    else:
                        params.append(param_info)

                if return_prop:
                    print(f"  Return Value: {return_prop['type']}")

                if params:
                    print("  Parameters:")
                    for param in params:
                        print(f"    - {param['name']} ({param['type']}) [{param['direction']}]")

                print("\n" + "="*40 + "\n")


# Execute the function
get_blueprint_callable_functions()

unreal.log("Finished scanning for Blueprint-callable functions.")
