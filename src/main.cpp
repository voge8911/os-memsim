#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);
bool stringToIntTest(std::string input);
bool pidExists(int pid);

std::vector<int> pids;

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint32_t mem_size = 67108864;
    void *memory = malloc(mem_size); // 64 MB (64 * 1024 * 1024)

    // Create MMU and Page Table
    Mmu *mmu = new Mmu(mem_size);
    PageTable *page_table = new PageTable(page_size);

    while (1)
    {
        // Prompt input
        std::string command;
        std::cout << "> ";
        std::getline(std::cin, command);
        // Handle command
        // TODO: implement this!
        std::vector<const char *> command_list;
        const char *token;
        token = std::strtok((char *)command.c_str(), " ");

        // Check for whitespace
        if (command == "" || strcmp(token, "") == 0)
        {
            continue;
        }
        // exit loop if command is "exit"
        else if (command == "exit")
        {
            break;
        }
        // Check if input is a command, if so, run command
        else if (strcmp(token, "create") == 0)
        {
            while (token != NULL)
            {
                command_list.push_back(token);
                token = strtok(NULL, " ");
            }
            if (command_list.size() <= 1)
            {
                fprintf(stderr, "error: not enough arguments\n");
                continue;
            }
            if (!stringToIntTest(command_list[1]) || !stringToIntTest(command_list[2]))
            {
                fprintf(stderr, "error: bad arguments\n");
                continue;
            }
            int text_size = std::stoi(command_list[1]);
            int data_size = std::stoi(command_list[2]);
            createProcess(text_size, data_size, mmu, page_table);
        }
        else if (strcmp(token, "allocate") == 0)
        {
            while (token != NULL)
            {
                command_list.push_back(token);
                token = strtok(NULL, " ");
            }
            if (command_list.size() <= 4)
            {
                fprintf(stderr, "error: not enought arguments\n");
                continue;
            }
            if (!stringToIntTest(command_list[1]) || !stringToIntTest(command_list[4]))
            {
                fprintf(stderr, "error: bad arguments\n");
                continue;
            }
            int pid = std::stoi(command_list[1]);
            std::string var_name = command_list[2];

            if (pidExists(pid) == false)
            {
                fprintf(stderr, "error: process not found\n");
                continue;
            }
            if (mmu->getVariable(pid, var_name) != NULL)
            {
                fprintf(stderr, "error: variable already exists\n");
                continue;
            }

            DataType type = mmu->stringToDataType(command_list[3]);
            uint32_t num_elements = (uint32_t)std::stoi(command_list[4]);
            allocateVariable(pid, var_name, type, num_elements, mmu, page_table);
        }
        else if (strcmp(token, "set") == 0)
        {
            while (token != NULL)
            {
                command_list.push_back(token);
                token = strtok(NULL, " ");
            }
            if (command_list.size() <= 4)
            {
                fprintf(stderr, "error: not enought arguments\n");
                continue;
            }
            if (!stringToIntTest(command_list[1]) || !stringToIntTest(command_list[3]))
            {
                fprintf(stderr, "error: bad arguments\n");
                continue;
            }
            int pid = std::stoi(command_list[1]);
            std::string var_name = command_list[2];
            if (pidExists(pid) == false)
            {
                fprintf(stderr, "error: process not found\n");
                continue;
            }
            if (mmu->getVariable(pid, var_name) == NULL)
            {
                fprintf(stderr, "error: variable not found\n");
                continue;
            }
            Variable *var = mmu->getVariable(pid, var_name);
            uint32_t offset = std::stoi(command_list[3]);
            int i;
            if (var->type == DataType::Char)
            {
                for (i = 4; i < command_list.size(); i++)
                {
                    char x = *command_list[i];
                    void *value = (void *)x;
                    setVariable(pid, var_name, offset, value, mmu, page_table, memory);
                    offset++;
                }
            }
            else if (var->type == DataType::Short)
            {
                for (i = 4; i < command_list.size(); i++)
                {
                    short x = (short)std::stoi(command_list[i]);
                    void *value = (void *)x;
                    setVariable(pid, var_name, offset, value, mmu, page_table, memory);
                    offset++;
                }
            }
            else if (var->type == DataType::Int)
            {
                for (i = 4; i < command_list.size(); i++)
                {
                    int x = std::stoi(command_list[i]);
                    void *value = (void *)x;
                    setVariable(pid, var_name, offset, value, mmu, page_table, memory);
                    offset++;
                }
            }
            else if (var->type == DataType::Float)
            {
                for (i = 4; i < command_list.size(); i++)
                {
                    float x = std::stof(command_list[i]);
                    float *p = &x;
                    void *value = (void *)p;
                    setVariable(pid, var_name, offset, value, mmu, page_table, memory);
                    offset++;
                }
            }
            else if (var->type == DataType::Double)
            {
                for (i = 4; i < command_list.size(); i++)
                {
                    double x = std::stod(command_list[i]);
                    double *p = &x;
                    void *value = (void *)p;
                    setVariable(pid, var_name, offset, value, mmu, page_table, memory);
                    offset++;
                }
            }
            else if (var->type == DataType::Long)
            {
                for (i = 4; i < command_list.size(); i++)
                {
                    long x = std::stol(command_list[i]);
                    void *value = (void *)x;
                    setVariable(pid, var_name, offset, value, mmu, page_table, memory);
                    offset++;
                }
            }
            else
            {
                fprintf(stderr, "error: wrong data type\n");
                continue;
            }
        }
        else if (strcmp(token, "print") == 0)
        {
            while (token != NULL)
            {
                command_list.push_back(token);
                token = strtok(NULL, " ");
            }
            if (command_list.size() <= 1)
            {
                // print error
                continue;
            }
            std::string print_str = command_list[1];

            if (print_str.compare("mmu") == 0)
            {
                mmu->print();
            }
            else if (print_str.compare("page") == 0)
            {
                page_table->print();
            }
            else if (print_str.compare("processes") == 0)
            {
                // if pids are not empty, then print the pids
                if (!pids.empty())
                {
                    for (int k = 0; k < pids.size(); k++)
                    {
                        std::cout << pids[k] << std::endl;
                    }
                }
            }
            else
            {
                // Check for "print <PID>:<var_name>"
                size_t sep = print_str.find(":");
                std::string pid_string = print_str.substr(0, sep);

                if (sep == std::string::npos || !stringToIntTest(pid_string) || pid_string == "")
                {
                    continue;
                }
                uint32_t pid = std::stoi(pid_string);
                std::string var_name = print_str.substr(sep + 1);
                Variable *var = mmu->getVariable(pid, var_name);
                if (pidExists(pid) == false)
                {
                    fprintf(stderr, "error: process not found\n");
                    continue;
                }
                else if (var == NULL)
                {
                    fprintf(stderr, "error: variable not found\n");
                    continue;
                }
                // Now print PID:var_name
                int physical_address = 0;
                int type_size = mmu->sizeOfType(var->type);
                int num_elements = var->size / type_size;
                int offset = 0;
                if (var->type == DataType::FreeSpace)
                {
                    fprintf(stderr, "error: can't print Free Space\n");
                    continue;
                }
                // Print elements of variable
                int i;
                if (var->type == DataType::Char)
                {
                    for (i = 0; i < num_elements; i++)
                    {   
                        offset = i * type_size;
                        physical_address = page_table->getPhysicalAddress(pid, var->virtual_address + offset);
                        char x;
                        memcpy(&x, (uint8_t *)memory + physical_address, type_size);
                        if (i != (num_elements - 1))
                            std::cout << x << ", ";
                        else
                            std::cout << x << std::endl;
                        if (i == 3)
                            break;
                    }
                }
                else if (var->type == DataType::Short)
                {
                    for (i = 0; i < num_elements; i++)
                    {   
                        offset = i * type_size;
                        physical_address = page_table->getPhysicalAddress(pid, var->virtual_address + offset);
                        short x;
                        memcpy(&x, (uint8_t *)memory + physical_address, type_size);
                        if (i != (num_elements - 1))
                            std::cout << x << ", ";
                        else
                            std::cout << x << std::endl;
                        if (i == 3)
                            break;
                    }
                }
                else if (var->type == DataType::Int)
                {
                    for (i = 0; i < num_elements; i++)
                    {
                        offset = i * type_size;
                        physical_address = page_table->getPhysicalAddress(pid, var->virtual_address + offset);
                        int x;
                        memcpy(&x, (uint8_t *)memory + physical_address, type_size);
                        if (i != (num_elements - 1))
                            std::cout << x << ", ";
                        else
                            std::cout << x << std::endl;
                        if (i == 3)
                            break;
                    }
                }
                else if (var->type == DataType::Float)
                {
                    for (i = 0; i < num_elements; i++)
                    {
                        offset = i * type_size;
                        physical_address = page_table->getPhysicalAddress(pid, var->virtual_address + offset);
                        float x;
                        memcpy(&x, (uint8_t *)memory + physical_address, type_size);
                        if (i != (num_elements - 1))
                            std::cout << x << ", ";
                        else
                            std::cout << x << std::endl;
                        if (i == 3)
                            break;
                    }
                }
                else if (var->type == DataType::Double)
                {
                    for (i = 0; i < num_elements; i++)
                    {
                        offset = i * type_size;
                        physical_address = page_table->getPhysicalAddress(pid, var->virtual_address + offset);
                        double x;
                        memcpy(&x, (uint8_t *)memory + physical_address, type_size);
                        if (i != (num_elements - 1))
                            std::cout << x << ", ";
                        else
                            std::cout << x << std::endl;
                        if (i == 3)
                            break;
                    }
                }
                else if (var->type == DataType::Long)
                {
                    for (i = 0; i < num_elements; i++)
                    {
                        offset = i * type_size;
                        physical_address = page_table->getPhysicalAddress(pid, var->virtual_address + offset);
                        long x;
                        memcpy(&x, (uint8_t *)memory + physical_address, type_size);
                        if (i != (num_elements - 1))
                            std::cout << x << ", ";
                        else
                            std::cout << x << std::endl;
                        if (i == 3)
                            break;
                    }
                }
                else
                {
                    fprintf(stderr, "error: wrong data type");
                    continue;
                }
                // If variable has more than 4 elements, just print the first 4 followed by "... [N items]"
                // (where N is the number of elements)
                if (num_elements > 4)
                {
                    printf("... [%d items]\n", num_elements);
                }
            }
        }
        else if (strcmp(token, "free") == 0)
        {
            while (token != NULL)
            {
                command_list.push_back(token);
                token = strtok(NULL, " ");
            }
            if (command_list.size() <= 2)
            { // not enough arguments
                continue;
            }
            if (!stringToIntTest(command_list[1]))
            { // bad pid
                continue;
            }
            uint32_t pid = std::stoi(command_list[1]);
            std::string var_name = command_list[2];
            freeVariable(pid, var_name, mmu, page_table);
        }
        else if (strcmp(token, "terminate") == 0)
        {
            while (token != NULL)
            {
                command_list.push_back(token);
                token = strtok(NULL, " ");
            }
            if (command_list.size() < 2)
            { // not enough arguments
                continue;
            }
            if (!stringToIntTest(command_list[1]))
            { // bad pid
                continue;
            }
            uint32_t pid = std::stoi(command_list[1]);
            terminateProcess(pid, mmu, page_table);
        }
        else
        {
            printf("error: command not recognized\n");
        }
    }

    // Clean up
    free(memory);
    delete mmu;
    delete page_table;

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std::endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std::endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std::endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std::endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std::endl;
    std::cout << "  * print <object> (prints data)" << std::endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std::endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std::endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std::endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std::endl;
    std::cout << std::endl;
}

void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - create new process in the MMU
    uint32_t pid = mmu->createProcess();
    pids.push_back(pid);
    //   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    //   - DataType is Char because `n` Chars is `n` bytes
    allocateVariable(pid, "<TEXT>"   , DataType::Char, text_size, mmu, page_table);
    allocateVariable(pid, "<GLOBALS>", DataType::Char, data_size, mmu, page_table);
    allocateVariable(pid, "<STACK>"  , DataType::Char, 65536    , mmu, page_table);
    //   - print pid
    printf("%d\n", pid);
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    uint32_t size_bytes = (uint32_t)mmu->sizeOfType(type) * num_elements;
    int page_size = page_table->_page_size;
    //   - find first <free space> in virtual memory (mmu) of size within a page already allocated to this process that is large enough to fit the new variable
    Variable *var = mmu->findFreeSpace(pid, size_bytes);
    if (var != NULL)
    {
        //   - insert variable into MMU
        mmu->addVariableToProcess(pid, var_name, type, size_bytes, var->virtual_address);

        // find page number for virtual address and virtual address + size
        int n = (int)log2(page_size); // n = number of bits for page offset
        int page_number = var->virtual_address >> n;
        int next_page_number = var->virtual_address + size_bytes - 1 >> n;

        // If the page number for the current virtual address is not equal to page number of the next
        // virtual address, this means the size is too big for the current page, allocate new page.
        while (page_number <= next_page_number)
        {
            // If frame is not already in the page_table, add an entry for that page
            if (page_table->getFrame(pid, page_number) == -1)
            {
                page_table->addEntry(pid, page_number);
            }
            page_number++;
        }
        //   - print virtual memory address
        if (var_name.compare("<TEXT>") != 0 && var_name.compare("<GLOBALS>") != 0 && var_name.compare("<STACK>") != 0)
        {
            std::cout << var->virtual_address << std::endl;
        }
        var->virtual_address += size_bytes;
    }
    else
    {
        if (pidExists(pid) == false)
        {
            fprintf(stderr, "error: process not found\n");
        }
        else
        {
            fprintf(stderr, "error: not enough memory\n");
        }
    }
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    Variable *var = mmu->getVariable(pid, var_name);
    offset = offset * mmu->sizeOfType(var->type);

    if (var != NULL)
    {
        uint32_t type_size = mmu->sizeOfType(var->type);
        //   - look up physical address for variable based on its virtual address / offset
        int physical_address = page_table->getPhysicalAddress(pid, (var->virtual_address + offset));
        //   - insert `value` into `memory` at physical address
        if (var->type == DataType::Double || var->type == DataType::Float)
        {
            memcpy((uint8_t *)memory + physical_address, value, type_size);
        }
        else
        {
            memcpy((uint8_t *)memory + physical_address, &value, type_size);
        }
    }
    else
    {
        if (pidExists(pid) == false)
        {
            // Error, process not found
            fprintf(stderr, "error: process not found\n");
        }
        else
        {
            // Error, variable not found
            fprintf(stderr, "error: variable not found\n");
        }
    }
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array)
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    int page_size = page_table->_page_size;
    int page_number = 0;
    int next_page_number = 0;
    
    int n = (int)log2(page_size); // n = number of bits for page offset
    Variable *var = mmu->getVariable(pid, var_name);

    if (var != NULL)
    {
        page_number = var->virtual_address >> n;
        next_page_number = var->virtual_address + var->size - 1 >> n;
        //   - free page if this variable was the only one on a given page
        while (page_number <= next_page_number)
        {
            if (mmu->isVariableInOwnPage(pid, var, page_number, page_table))
            {
                page_table->freeFrame(pid, page_number);
            }
            page_number++;
        }

        //   - remove entry from MMU
        var->type = DataType::FreeSpace;
        var->name = "<FREE_SPACE>";

        mmu->mergeFreeSpace(pid, var);
    }
    else
    {
        if (pidExists(pid) == false)
        {
            fprintf(stderr, "error: process not found\n");
        }
        else
        {
            fprintf(stderr, "error: variable not found\n");
        }
    }
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    //   - remove process from MMU
    mmu->deleteProcess(pid);
    //   - free all pages associated with given process
    page_table->freeProcessPages(pid);
    //   - remove pid from list of pids
    std::vector<int>::iterator it = pids.begin() + (pid-1024);
    pids.erase(it);
}

// Returns: true if input can be converted to an integer, false otherwise
bool stringToIntTest(std::string input)
{
    bool is_integer = true;
    // go through each character of input, checking for data type errors
    for (int i = 0; i < input.length(); i++)
    {
        // try input to integer conversion
        try
        {
            std::stoi(input.substr(i, i + 1));
        }
        // if conversion fails, input must be wrong data type
        catch (const std::invalid_argument e)
        {
            is_integer = false;
            break;
        }
    }
    return is_integer;
}

// Returns: true if pid exists, false if pid does not exist
bool pidExists(int pid)
{
    int i;
    for (i = 0; i < pids.size(); i++)
    {
        if (pid == pids[i])
        {
            return true;
        }
    }
    return false;
}