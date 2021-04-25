#include "mmu.h"
#include "pagetable.h"
#include <sstream>
#include <iomanip>
#include <math.h>

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

void Mmu::setFreeSpace(uint32_t pid, Variable *var)
{
    int i;
    int index = 0;
    Process *proc = NULL;
    Variable *var1 = NULL;
    Variable *var2 = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
            for (i = 0; i < proc->variables.size(); i++)
            {
                if (proc->variables[i]->virtual_address == var->virtual_address)
                {
                    index = i;
                    
                    if (i < proc->variables.size() - 1)
                    {
                        var1 = proc->variables[i+1];
                        std::cout << var1->name << std::endl;
                    }
                    if (i > 0)                   
                    {
                        var2 = proc->variables[i-1];
                        std::cout << var2->name << std::endl;
                    }
                    break;
                }
            }
        }
        break;
    }
    if (var1 != NULL)
    {
        if (var1->type == DataType::FreeSpace)
        {
            var->size += var1->size;
            proc->variables.erase(proc->variables.begin() + index + 1);
            
            var->virtual_address = var1->virtual_address;
        }
    }
    if (var2 != NULL)
    {
        if (var2->type == DataType::FreeSpace)
        {
            var->size += var2->size;
            proc->variables.erase(proc->variables.begin() + index - 1);
        }
    }
}

bool Mmu::isVariableInOwnPage(uint32_t pid, Variable* var, PageTable *page_table)
{
    Process *proc = NULL;
    int var_page_number = 0;
    int page_number = 0;
    int n = (int)log2(page_table->_page_size); // n = number of bits for page offset
    var_page_number = var->virtual_address >> n;

    int i;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
            for (i = 0; i < proc->variables.size(); i++)
            {
                // Get page number of variable
                page_number = proc->variables[i]->virtual_address >> n;

                if (var_page_number == page_number)
                {
                    return false;
                }
            }
        }
        break;
    }
    return true;
}

Variable* Mmu::getVariable(uint32_t pid, std::string name)
{
    Process *proc = NULL;
    Variable *var = NULL;
    int i;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
            for (i = 0; i < proc->variables.size(); i++)
            {
                if (proc->variables[i]->name.compare(name) == 0)
                {
                    var = proc->variables[i];
                    break;
                }
            }
            break;
        }
    }
    return var;
}

// Add array fit on part of page
Variable* Mmu::findFreeSpace(uint32_t pid, uint32_t size)
{
    Process *proc = NULL;
    Variable *var = NULL;
    int i;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
            for (i = 0; i < proc->variables.size(); i++)
            {
                if (proc->variables[i]->type == DataType::FreeSpace && 
                    proc->variables[i]->size >= size)
                {
                    var = proc->variables[i];
                    var->size -= size;
                }
            }
            break;
        }
    }
    return var;
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
            break;
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
            if (_processes[i]->variables[j]->type == DataType::FreeSpace)
            {
                //continue;
            }
            uint32_t pid = _processes[i]->pid;
            std::string name = _processes[i]->variables[j]->name;
            uint32_t virtual_addr = _processes[i]->variables[j]->virtual_address;
            std::stringstream sstream;
            sstream << "0x" << std::setfill('0') << std::setw(8) << std::uppercase << std::hex << virtual_addr;
            std::string result = sstream.str();
            uint32_t size = _processes[i]->variables[j]->size;
            printf("%5u | %-13s | %12s | %10u\n", pid, name.c_str(), result.c_str(), size);
        }
    }
}

DataType Mmu::stringToDataType(std::string string)
{
    if (string.compare("char") == 0) {
        return DataType::Char;
    } else if (string.compare("double") == 0) {
        return DataType::Double;
    } else if (string.compare("float") == 0) {
        return DataType::Float;
    } else if (string.compare("int") == 0) {
        return DataType::Int;
    } else if (string.compare("long") == 0) {
        return DataType::Long;
    } else if (string.compare("short") == 0) {
        return DataType::Short;
    } else {
        return DataType::FreeSpace;
    }
}

uint32_t Mmu::sizeOfType(DataType type)
{
    if (type == DataType::Char) {
        return 1;
    } else if (type == DataType::Short) {
        return 2;
    } else if (type == DataType::Int || type == DataType::Float) {
        return 4;
    }  else if (type == DataType::Double || type == DataType::Long) {
        return 8;
    } else {
        return -1;
    }
}