#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>
#include <pagetable.h>

enum DataType : uint8_t {FreeSpace, Char, Short, Int, Float, Long, Double, Err};

typedef struct Variable {
    std::string name;
    DataType type;
    uint32_t virtual_address;
    uint32_t size;
} Variable;

typedef struct Process {
    uint32_t pid;
    std::vector<Variable*> variables;
} Process;

class Mmu {
private:
    uint32_t _next_pid;
    uint32_t _max_size;
    std::vector<Process*> _processes;

public:
    Mmu(int memory_size);
    ~Mmu();

    void deleteProcess(uint32_t pid);
    uint32_t createProcess();
    void mergeFreeSpace(uint32_t pid, Variable *var);
    bool isVariableInOwnPage(uint32_t pid, Variable* var, int page_number, PageTable *page_table);
    Variable* getVariable(uint32_t pid, std::string name);
    Variable* findFreeSpace(uint32_t pid, uint32_t size);
    void addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address);
    void print();
    DataType stringToDataType(std::string string);
    uint32_t sizeOfType(DataType type);
};

#endif // __MMU_H_