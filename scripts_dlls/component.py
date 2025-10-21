import os, sys

name = input("Enter component name (e.g. Bullet): ").strip()

if not name:
    print("Component name cannot be empty.")
    sys.exit(1)

class_name = f"{name}Component"
file_name = f"{class_name}.hpp"
file_path = os.path.join("src", "components", file_name)

os.makedirs(os.path.dirname(file_path), exist_ok=True)

template = f"""#pragma once
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"

class {class_name} : public Flag::IComponent {{
public:
  ~{class_name}() = default;

}};
"""

with open(file_path, "w", encoding="utf-8") as f:
    f.write(template)

print(file_path)
