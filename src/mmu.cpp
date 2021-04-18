#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->type = DataType::FreeSpace;
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

void Mmu::addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
{
    int i;
    Process *proc = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }

    Variable *var = new Variable();
    var->name = var_name;
    var->type = type;
    var->virtual_address = address;
    var->size = size;
    if (proc != NULL)
    {
        proc->variables.push_back(var);
    }
}

void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            // TODO: print all variables (excluding <FREE_SPACE> entries)
            if (_processes[i]->variables[j]->type != DataType::FreeSpace)
            {
                uint32_t pid = _processes[i]->pid;
                std::string name = _processes[i]->variables[j]->name;
                uint32_t virtual_addr = _processes[i]->variables[j]->virtual_address;
                uint32_t size = _processes[i]->variables[j]->size;
                printf("%5u | %-9s | %8u | %8u\n", pid, name.c_str(), virtual_addr, size);
            }
        }
    }
}

DataType Mmu::stringToDataType(std::string string)
{
    if (string.compare("Char")) {
        return DataType::Char;
    } else if (string.compare("Double")) {
        return DataType::Double;
    } else if (string.compare("Float")) {
        return DataType::Float;
    } else if (string.compare("Int")) {
        return DataType::Int;
    } else if (string.compare("Long")) {
        return DataType::Long;
    } else if (string.compare("Short")) {
        return DataType::Short;
    } else {
        return DataType::FreeSpace;
    }
}
