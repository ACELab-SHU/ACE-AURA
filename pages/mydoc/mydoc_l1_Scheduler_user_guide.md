---
title: L1 Scheduler User Guide
audience: writer, designer
#tags: [navigation]
last_updated: June 10, 2025
keywords: l1, scheduler, user, guide, language
summary: "L1 scheduler"
sidebar: mydoc_sidebar
permalink: mydoc_l1_scheduler_user_guide.html
folder: mydoc
---


# Step 1: Prepare Your DSL DAG Projec

**Make sure your DSL DAG project has been properly generated. The following files are required for integration with the L1 simulator:**

* The compiled DAG binary image: `<project_path>/dsl/bin/<dag_name>.bin`
* The DAG metadata file: `<project_path>/dsl/final_output/<dag_name>.json`
* The individual task binary images: `<project_path>/dsl/venus_test/ir/<task_name>.hex`

These files will be used by the L1 simulator for joint debugging and integration.

# Step 2: Write the L1 Runtime Code

**2.1 Acquire the input and output sequence of the DAG by running dagInfoPaser script:**

```cd <project_path>/l1_main && python ./dagInfoPaser.py <dag_name>```

**2.2 Retrieve the order of input and output variables from the file:**

`<project_path>/l1_main/daginfo/<dag_name>.txt`

**2.3 Update `<dag_name>/source/l1.cpp` to declare the input and output variables for your DAG.** Ensure that the variable types and lengths strictly match those defined in the associated `.bas` file.

Then, establish the DAG-to-DAG dependencies to define execution order and data flow.

**For example, suppose the following input and output variables are declared in your .bas file:**

    ' input parameter
      dfedata char dfe_input[4388]
      dag_input short nCellid[1]
    
    ' output parameter
      return_value short subFrameNum[1]

**Suppose your DAG’s input and output variable names are defined as follows:**

    # Input Names
    nCellid
    dfe_input
    # Return Output Names
    subFrameNum

**You can then invoke the DAG in `l1.cpp` as follows:**

* `fire_dag` API Parameters:

| Parameter Index | Description | Source | Example Value |
| --- | --- | --- | --- |
| 0   | Name of the DAG | Defined in your DSL project | `dag_name` |
| 1   | Number of input parameters | Defined in your `.bas` file | `12` |
| 2   | Number of output parameters | Defined in your `.bas` file | `6` |

You can use the `fire_dag` function to instruct L1 to deploy and run the specified DAG for computation.

    #include "l1_api.h"
    char dfe_input[4388] = {...};
    short nCellid[1] = {2};
    short subFrameNum[1];
    
    void l1_process() {
      fire_dag(dag_name, 2, 1, &dfe_input, &nCellid, &subFrameNum);
    
      // You can connect the output of this DAG as the input
      // to the next DAG to represent data flow and dependencies between DAGs.
    }


{% include links.html %}
