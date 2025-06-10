import os
import sys

def generate_S_file(task, mode):
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

lui sp, 0x24
jal ra, {}
/* break */
ebreak //系统自陷
"""

    special_in_release_content = """.section .text
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

lui	a1, 524799
addi	s5, zero, 1
sw	s5, 4(a1)
mv a1, x0
mv s5, x0
lui sp, 0x140
jal ra, {}
/* break */
ebreak //系统自陷
"""

    special_tasks = {"Task_getPolarInfo", "Task_nrPDSCHIndices", "Task_nrPDSCHDMRSIndices"}
    use_special = mode == "release" and task in special_tasks
    content = special_in_release_content if use_special else template_content

    folder_path = "asm"
    os.makedirs(folder_path, exist_ok=True)
    file_path = os.path.join(folder_path, f"{task}.S")

    with open(file_path, "w") as file:
        file.write(content.format(task, task))

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python asm.py [debug|release] task1 [task2 ...]")
        sys.exit(1)

    mode = sys.argv[1]
    tasks = sys.argv[2:]
    if mode not in ("debug", "release"):
        print("Error: Mode must be 'debug' or 'release'.")
        sys.exit(1)
    for task in tasks:
        generate_S_file(task, mode)
