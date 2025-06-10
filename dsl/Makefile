include ./config.mk

all: clean main_target run_variable

main_target: 
	if [ ! -d "venus_test/asm" ]; then \
		mkdir -p venus_test/asm; \
	fi
	
	if [ -d "./IJ" ]; then \
	    rm -rf ./IJ/*; \
	else \
	    mkdir -p ./IJ; \
	fi
	if [ ! -d "./heft_new/json" ]; then \
		git clone https://gitclone.com/github.com/nlohmann/json ./heft_new/json; \
	fi
	if [ -d "./bin" ]; then \
	    rm -rf ./bin/*; \
	else \
	    mkdir -p ./bin; \
	fi
	if [ -d "./final_output" ]; then \
	    rm -rf ./final_output/*; \
	else \
	    mkdir -p ./final_output; \
	fi
	if [ ! -d "./variable/map" ]; then \
	    mkdir -p ./variable/map; \
	fi
	if [ ! -d "./heft_new/DAG" ]; then \
	    mkdir -p ./heft_new/DAG; \
	fi	
	if [ -e "./variable/global.c" ]; then \
	    > ./variable/global.c; \
	else \
	    touch ./variable/global.c; \
	fi
	if [ -e "./variable/parameter.c" ]; then \
	    > ./variable/parameter.c; \
	else \
	    touch ./variable/parameter.c; \
	fi
	if [ -e "./variable/dfedata.c" ]; then \
	    > ./variable/dfedata.c; \
	else \
	    touch ./variable/dfedata.c; \
	fi
	if [ -e "./variable/dag_input.c" ]; then \
	    > ./variable/dag_input.c; \
	else \
	    touch ./variable/dag_input.c; \
	fi	
	
	> ./variable/map/global.json
	> ./variable/map/parameter.json
	> ./variable/map/return_value.json
	> ./variable/map/dfedata.json
	> ./variable/map/dag_input.json
	> ./variable/map/all_data.json

	mkdir -p ./venus_test/ir/analysis
	mkdir -p ./variable/ir
	$(MAKE) -C ./venus_test all
	python3 ./ply/basic.py $(TARGET_DAG).bas > debug.txt
	
run_variable:
	$(MAKE) -C ./variable dag_variable_pass
	$(MAKE) -C ./variable asm
	$(MAKE) -C ./variable link_all
	$(MAKE) -C ./venus_test map_task
	$(MAKE) -C ./venus_test single_task_test
	python3 ./script/combine_bin_without_padding.py $(VenusInputStructAddr)
	$(MAKE) -C ./variable map_variable
	$(MAKE) -C ./venus_test input_dest_addr
	
	python3 ./script/task_info.py
	python3 ./script/input_spec_dest_addr.py
	python3 ./script/input_spec_attribute.py
	python3 ./script/slice.py
	$(MAKE) -C ./heft_new all
	python3 ./script/final_json_global_para.py
	python3 ./script/final_json_return.py
	$(MAKE) -C ./venus_test OUTBINHEX

clean:
	$(MAKE) -C ./venus_test clean
	$(MAKE) -C ./variable clean
	$(MAKE) -C ./heft_new clean

.PHONY: all main_target run_variable clean
