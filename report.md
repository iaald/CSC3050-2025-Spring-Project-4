# CSC3050 Project 4: Cache Simulator Report

Group Members: Runyuan He, Mingshi Deng

Student ID: 123090163, 123090081

Contribution: 50%, 50%

Note: Only leave sections blank if they are not yet implemented. We will verify that your code matches the data in your report. Any inconsistencies will result in double point deductions.


## 1. Part 1


### 1.1 Split Cache Design

Split cache is a way of organizing the cache in computer, where cache is split into ICache and DCache. Instruction Cache (ICache) is dedicated to storing instructions, and Data Cache (DCache) is dedicated to storing data. Unified cache does not have such split.

While unified cache has one access path, split cache allows the acquisition of instructions and data to be carried out in parallel, accelerating memory access by increasing throughput.

### 1.2 Implementation Details

ICache is similar to DCache, except that since instructions are usually unmodifiable, ICache is read-only, which means it should not consider/support the $\texttt{setByte()}$ function.

To simulate parallel access, we assume start fetching both caches at same time, the overall consumption should be:

$$
\text{Total Cycle} = \max (TotalCycle_{I}, TotalCycle_{D})
$$

That is, the total time consumption depends on the value that takes longer in the two caches.

### 1.3 Result Analysis

| Trace File | Cache Type    | Miss Count | Expected Miss Count | Total Cycles | Expected Total Cycles |
| ---------- | ------------- | ---------- | ------------------- | ------------ | --------------------- |
| I.trace    | Split Cache   |     512    |         512         |    41088     |    41088              |
| I.trace    | Unified Cache |     384    |         384         |    54912     |    54912              |
| D.trace    | Split Cache   |     634    |         634         |    67584     |    67584              |
| D.trace    | Unified Cache |     507    |         507         |    68141     |    68141              |

By analyzing the data, we can discover the following several phenomena:

* Using Split cache makes the miss count of test results of different trace files increase.

  > Instructions are relatively continuous in memory, so the cache hit rate of instructions is higher. After using split cache, the size of i-cache limits the amount of instruction cache, and reducing the hit rate.

* The rate of growth of miss count for I_Trace is higher than that of D_Trace.

  > Therefore, if the proportion of instructions in the total access is higher, this impact will be correspondingly greater.

* Using Split cache makes the total cycles of test results of different trace files decrease.

  > By increasing the throughput, split cache reduces the total cycles.


## 2. Part 2

### Correct results after bug fixed:

| Cache Level | Read Count | Write Count | Hit Count | Miss Count | Miss Rate | Total Cycles |
|-------|-----------|------------|----------|------------|-----------|--------------|
| L1    | 181,708   | 50,903     | 177,911  | 54,700     | 23.5%     | 717,519      |
| L2    | 47,667    | 7,033      | 25,574   | 29,126     | 53.1%     | 888,292      |
| L3    | 26,984    | 2,142      | 21,596   | 7,530      | 25.9%     | 1,184,920    |

### My Results after bug fixed:

| Cache Level | Read Count | Write Count | Hit Count | Miss Count | Miss Rate | Total Cycles |
| ----------- | ---------- | ----------- | --------- | ---------- | --------- | ------------ |
| L1          |            |             | 177,911   | 54,700     | 23.5%     | 717,519      |
| L2          |            |             | 25,608    | 29,092     | 53.2%     | 886,984      |
| L3          |            |             | 21,562    | 7,530      | 25.9%     | 1,184,240    |


## 3. Part 3

### Optimization Techniques Application and Results
(Choose one optimization technique for each trace file)

*No Optimization* means, just use the initial code to run simulation.

| Trace       | Optimization    | Miss Count | Miss Rate | Expected Miss Rate |
| ----------- | --------------- | ---------- | --------- | ------------------ |
| test1.trace | No Optimization |            |           |       None         |
| test1.trace |                 |            |           |     0.371%         |
| test2.trace | No Optimization |   100013   |   98.532% |       None         |
| test2.trace | Pre-fetching    |     1272   |    1.253% |    5.803%          |
| test3.trace | No Optimization |            |           |       None         |
| test3.trace |                 |            |           |   0.372%           |


## 4. Part 4

### 4.1 Performance Comparison (L1 Level)

| Implementation | Read Count | Write Count | Hit Count | Miss Count | Miss Rate | Total Cycles |
|----------------|------------|-------------|-----------|------------|-----------|--------------|
| matmul0 |         |         |         |           |         |         |
| matmul1 |         |         |         |           |         |         |
| matmul2 |         |         |         |           |         |         |
| matmul3 |         |         |         |           |         |         |
| matmul4 |         |         |         |           |         |         |

### 4.2 Analysis of Performance Differences between matmul1, 2, 3
