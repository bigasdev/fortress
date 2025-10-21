import os, sys

name = input("Enter system name (e.g. Combat): ").strip()
interfaces = input("Enter interfaces (comma-separated, e.g. IUpdate,IFixedUpdate) or leave empty: ").strip()
interfaces_code = []
interfaces_include_str = ""
interfaces_functions_str = ""
interfaces_cpp_functions_str = ""

interfaces_map = {
    "fixed": "IFixedUpdateSystem",
    "pause": "IPauseSystem",
    "popup": "IPopupSystem",
    "reactive": "IReactiveSystem",
    "render": "IRenderSystem",
    "start": "IStartSystem",
    "update": "IUpdateSystem"
}

interfaces_include = {
    "fixed" : '#include "systems/IFixedUpdateSystem.hpp"',
    "pause" : '#include "systems/IPauseSystem.hpp"',
    "popup" : '#include "systems/IPopupSystem.hpp"',
    "reactive" : '#include "systems/IReactiveSystem.hpp"',
    "render" : '#include "systems/IRenderSystem.hpp"',
    "start" : '#include "systems/IStartSystem.hpp"',
    "update" : '#include "systems/IUpdateSystem.hpp"'
}

interfaces_functions = {
    "fixed": "void fixed_update() override;",
    "pause": "void pause() override;",
    "popup": "void popup() override;",
    "reactive": "void on_message(const Flag::IMessage &message) override;",
    "render": "void render() override;",
    "start": "void start() override;",
    "update": "void update() override;"
}

interfaces_cpp_functions = {
    "fixed": "void {class_name}::fixed_update() {{\n    // TODO: Implement fixed update logic here\n}}\n",
    "pause": "void {class_name}::pause() {{\n    // TODO: Implement pause logic here\n}}\n",
    "popup": "void {class_name}::popup() {{\n    // TODO: Implement popup logic here\n}}\n",
    "reactive": "void {class_name}::on_message(const Flag::IMessage &message) {{\n    // TODO: Implement message handling logic here\n}}\n",
    "render": "void {class_name}::render() {{\n    // TODO: Implement render logic here\n}}\n",
    "start": "void {class_name}::start() {{\n    // TODO: Implement start logic here\n}}\n",
    "update": "void {class_name}::update() {{\n    // TODO: Implement update logic here\n}}\n"
}

if interfaces:
    interfaces_list = [iface.strip().lower() for iface in interfaces.split(",")]
    interfaces_code = [interfaces_map.get(iface, iface) for iface in interfaces_list]
    print(f"Using interfaces: {', '.join(interfaces_code)}")

    for iface in interfaces_list:
        if iface in interfaces_include:
            interfaces_include_str += f"\n{interfaces_include[iface]}"
        if iface in interfaces_functions:
            interfaces_functions_str += f"{interfaces_functions[iface]}\n"
        if iface in interfaces_cpp_functions:
            interfaces_cpp_functions_str += interfaces_cpp_functions[iface].format(class_name=f"{name}System") + "\n"

    print(f"Using includes: {interfaces_include_str}")

if not name:
    print("System name cannot be empty.")
    sys.exit(1)


class_name = f"{name}System"
file_header_name = f"{class_name}.hpp"
file_cpp_name = f"{class_name}.cpp"
file_header_path = os.path.join("src", "systems", file_header_name)
file_cpp_path = os.path.join("src", "systems", file_cpp_name)

os.makedirs(os.path.dirname(file_header_path), exist_ok=True)
os.makedirs(os.path.dirname(file_cpp_path), exist_ok=True)

template_header = f"""#pragma once
#ifndef {class_name.upper()}_HPP
#define {class_name.upper()}_HPP
{interfaces_include_str}
class {class_name} : {'public Flag::' + ', public Flag::'.join(interfaces_code) if interfaces_code else ''}
{{
public:
    {class_name}() = default;
    ~{class_name}() = default;

    {interfaces_functions_str}
}};
#endif // {class_name.upper()}_HPP
"""

template_cpp = f"""#include "{file_header_name}"
{interfaces_cpp_functions_str}
"""



with open(file_header_path, "w", encoding="utf-8") as f:
    f.write(template_header)

with open(file_cpp_path, "w", encoding="utf-8") as f:
    f.write(template_cpp)

