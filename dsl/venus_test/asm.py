import os
import sys

def generate_S_file(task):
    template_content = """.section .text
.global {}

mv x1, x0
mv x2, x0
mv x3, x0
mv x4, x0
mv x5, x0
mv x6, x0
mv x7, x0
mv x8, x0
mv x9, x0
mv x10, x0
mv x11, x0
mv x12, x0
mv x13, x0
mv x14, x0
mv x15, x0
mv x16, x0
mv x17, x0
mv x18, x0
mv x19, x0
mv x20, x0
mv x21, x0
mv x22, x0
mv x23, x0
mv x24, x0
mv x25, x0
mv x26, x0
mv x27, x0
mv x28, x0
mv x29, x0
mv x30, x0
mv x31, x0

lui sp, 0x100
jal ra, {}
/* break */
ebreak //系统自陷
"""

    folder_path = "asm"
    os.makedirs(folder_path, exist_ok=True)
    file_path = os.path.join(folder_path, f"{task}.S")

    with open(file_path, "w") as file:
        file.write(template_content.format(task, task))

if __name__ == "__main__":
    for task in sys.argv[1:]:
        generate_S_file(task)
