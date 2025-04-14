# Makefile示例框架

# 编译器设置
CC = /usr/bin/g++
CFLAGS = -Wall -Wextra
LLVM_BIN = /server17/ic/llvm-bin/bin
#LLVM_BIN=/home/xusiyi/sys_llvm/llvm-project-cmake-build-debug/bin
VNS_INC_DIR = /server17/ic/venus-llvm-project/venus_test_1
VINS_RESULT_DIR = ./Debug/emulator_vins_result/

# 目标DAG
-include dsl/config.mk
TARGET_DAG ?= PDSCH
LITE_BRANCH = develop
DSL_BRANCH = v4.0

# 目标文件名
TARGET = ./Debug/Emulator

# 源文件目录
SRC_DIR = ./source
# 头文件目录
INC_DIR = ./include
# 编译生成的目标文件目录
OBJ_DIR = ./Debug

SVTB_DIR ?= ../../sim

TASK_SRC_DIR = ./task_src
TASK_BUILD_DIR = ./task_build
TASK_NAME?=task14_polar_encode

# 源文件和目标文件列表
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# 编译规则
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -I$(INC_DIR) -c -o $@ $<

# # 主目标（可根据需要修改）
$(TARGET): $(OBJ_FILES)
	$(CC) $^ -o $@

$(TASK_BUILD_DIR)/firmware.c: $(TASK_SRC_DIR)/Task_template.c
	cp $< $@
	$(MAKE) -C $(TASK_BUILD_DIR) begin
	$(MAKE) -C $(TASK_BUILD_DIR) all TASK_NAME=$(TASK_NAME)

.PHONY: Emulator
Emulator: $(SRC_FILES) 
	/usr/bin/g++ -fdiagnostics-color=always -g $^ -DVENUSROW=${VENUSROW} -DVENUSLANE=${VENUSLANE} -I$(INC_DIR) -o ./Debug/$@

new_folder:
	if [ ! -d $(OBJ_DIR) ]; then \
		mkdir -p $(OBJ_DIR); \
	fi
	if [ -d $(VINS_RESULT_DIR) ]; then \
		rm -r $(VINS_RESULT_DIR); \
	fi
	mkdir -p $(VINS_RESULT_DIR) 

new_build: $(TASK_SRC_DIR)/Task_template.c
	cp -r $(VNS_INC_DIR)/* $(OBJ_DIR)
	cp $< $(OBJ_DIR)/test.c
	$(MAKE) -C $(OBJ_DIR) h2asm PATH=$(LLVM_BIN) INC_DIR=../task_utils
	$(MAKE) -C $(OBJ_DIR) all PATH=$(LLVM_BIN) INC_DIR=../task_utils
	# cp $(OBJ_DIR)/test.bin ../benchmark/c/$(TASK_NAME)/firmware.bin

# 默认目标
# For the legacy pass
# all: new_folder $(TASK_BUILD_DIR)/firmware.c $(TARGET)
# For the new compiler and single task emulation
all: new_folder new_build $(TARGET)
# For DSL-level emulation
emudsl: new_folder $(TARGET)
	./Debug/Emulator -w -j${SVTB_DIR}/model/l2_scheduler/dsl/final_spec.json \
						-b${SVTB_DIR}/model/l2_scheduler/dsl/combined.bin	\
						-c${SVTB_DIR}/build/L2_DMA_trx.log

# 清理规则
clean:
	rm -rf $(OBJ_DIR)/* $(TARGET)
	rm -f $(TASK_BUILD_DIR)/firmware.c
	$(MAKE) -C $(TASK_BUILD_DIR) clean
.PHONY: all clean

.PHONY: repo
REPO_IP=$(shell git remote -v | grep "fetch" | tr "/" "\\n" | grep ":6655")

repo:
	# # Check DSL repo
	# if [ -d "dsl" ]; then \
	# 	cd dsl && git pull --rebase && git checkout $(DSL_BRANCH); \
	# else \
	# 	git clone -b $(DSL_BRANCH) http://$(REPO_IP)/TACEYEON/dsl.git; \
	# fi

	# # Check 5GLITE repo
	# if [ -d "5g_lite" ]; then \
	# 	cd 5g_lite && git pull --rebase && git checkout $(LITE_BRANCH); \
	# else \
	# 	git clone -b $(LITE_BRANCH) http://$(REPO_IP)/FengYuan/5g_lite.git; \
	# fi

	# Init Debug directory for VEMU
	if [ ! -d "Debug" ]; then mkdir -p Debug; fi

.PHONY: rm_repo
rm_repo:
	rm -rf dsl AceEcho

# .PHONY: emu_init
# emu_init: repo
# 	cd dsl/venus_test && for file in ../../AceEcho/tasks/include/*.h; do filename=$(basename "$file"); [ -e "$filename" ] && rm -f "$filename"; ln -sf $$file ./; done
# 	cd dsl/venus_test && rm -f Task* && for file in ../../AceEcho/tasks/${TARGET_DAG}/*.c; do ln -sf $$file ./ ; done
# 	cd dsl/ && rm -f *.bas && for file in ../AceEcho/tasks/${TARGET_DAG}/*.bas; do ln -sf $$file ./ ; done

.PHONY: emu_init
emu_init: repo
	cd dsl/venus_test && for file in ../../AceEcho/tasks/include/*.h; do filename=$$(basename "$$file"); echo "./$$filename"; [ -e "$$filename" ] && rm -f "./$$filename"; echo "Creating symlink for $$file"; ln -sf "$$file" ./; done
	cd dsl/venus_test && rm -f Task* *.c && for file in ../../AceEcho/tasks/${TARGET_DAG}/*.c; do echo "Creating symlink for $$file"; ln -sf "$$file" ./ ; done
	cd dsl/ && rm -f *.bas && for file in ../AceEcho/tasks/${TARGET_DAG}/*.bas; do echo "Creating symlink for $$file"; ln -sf "$$file" ./ ; done


# .PHONY: rm_repo
# rm_repo:
# 	rm -rf dsl 5g_lite

# .PHONY: emu_init
# emu_init: repo
# 	cd dsl/venus_test && for file in ../../5g_lite/include/*.h; do filename=$$(basename "$$file"); echo "./$$filename"; [ -e "$$filename" ] && rm -f "./$$filename"; echo "Creating symlink for $$file"; ln -sf "$$file" ./; done
# 	cd dsl/venus_test && rm -f Task* *.c && for file in ../../5g_lite/tasks/${TARGET_DAG}/*.c; do echo "Creating symlink for $$file"; ln -sf "$$file" ./ ; done
# 	cd dsl/ && rm -f *.bas && for file in ../5g_lite/tasks/${TARGET_DAG}/*.bas; do echo "Creating symlink for $$file"; ln -sf "$$file" ./ ; done