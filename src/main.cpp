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

std::vector<int> pids;
int count = 0;

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

    while (1) {
        // Prompt input
        std::string command;
        std::cout << "> ";
        std::getline (std::cin, command);
        // Handle command
        // TODO: implement this!
        std::vector<const char*> command_list;
        const char *token;
        token = std::strtok((char *)command.c_str(), " ");

        if (command == "" || strcmp(token, "") == 0) 
        {
            continue;
        }
        else if (command == "exit")
        {
            break;
        }
        else if (strcmp(token, "create") == 0)
        {
            while (token != NULL) {
                command_list.push_back(token);
                token = strtok(NULL, " "); 
            }
            if (command_list.size() <= 1) {
                // error
                continue;
            }
            int text_size = std::stoi(command_list[1]);
            int data_size = std::stoi(command_list[2]);
            createProcess(text_size, data_size, mmu, page_table);
            
        }
        else if (strcmp(token, "allocate") == 0)
        {
            while (token != NULL) {
                command_list.push_back(token);
                token = strtok(NULL, " ");
            }
            if (command_list.size() <= 1) {
                // error
                continue;
            }
            if (!stringToIntTest(command_list[1]))
            {
                continue;
            }
            int pid = std::stoi(command_list[1]);
            std::string var_name = command_list[2];
            //std::string type1 = command_list[3];
            DataType type = mmu->stringToDataType(command_list[3]);
            uint32_t num_elements = (uint32_t)std::stoi(command_list[4]);
            //printf("pid = %d var_name = %s type = %s num_elements = %d\n", pid, var_name.c_str(), type1.c_str(), num_elements);
            allocateVariable(pid, var_name, type, num_elements, mmu, page_table);
        }
        else if (strcmp(token, "set") == 0)
        {
            
        }
        else if (strcmp(token, "print") == 0)
        {
            while (token != NULL) {
                command_list.push_back(token);
                token = strtok(NULL, " ");
            }
            if (command_list.size() <= 1) {
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
                    for (int k=0; k < pids.size(); k++)
                    {
                        std::cout << pids[k] << std::endl;
                    }
                }
            } 
            else 
            {
                // Add error checking here for "print <PID>:<var_name>" 
                size_t sep = print_str.find(":");
                if (sep == std::string::npos)
                {
                    continue;
                }
                uint32_t pid = std::stoi(print_str.substr(0, sep));
                std::string var_name = print_str.substr(sep + 1);
                std::cout << pid << " " << var_name << std::endl;
                
            }
        }
    }

    // Cean up
    free(memory);
    delete mmu;
    delete page_table;

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
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
    uint32_t size_bytes = (uint32_t) mmu->sizeOfType(type) * num_elements;
    int page_size = page_table->_page_size;
    //   - find first <free space> in virtual memory (mmu) of size within a page already allocated to this process that is large enough to fit the new variable
    Variable *var = mmu->findFreeSpace(pid, size_bytes);
    if (var != NULL)
    {
        //   - insert variable into MMU
        mmu->addVariableToProcess(pid, var_name, type, size_bytes, var->virtual_address);

        // find page number for virtual address and virtual address + size
        uint32_t next_virtual_address = var->virtual_address + size_bytes - 1;
        int n = (int)log2(page_size); // n = number of bits for page offset
        int page_number = 0;
        int next_page_number = 0;

        page_number = var->virtual_address >> n;
        next_page_number = next_virtual_address >> n;
        
        std::cout << "Page number = " << page_number << std::endl;
        std::cout << "Next page number = " << next_page_number << std::endl;
        // If the page number for the current virtual address is not equal to page number of the next
        // virtual address, this means the size is too big for the current page, allocate new page.
        while (page_number <= next_page_number)
        {
            // If frame is already in the page_table, check the next page number
            if (page_table->getFrame(pid, page_number) != -1)
            {
                page_number++;
                continue;
            }
            page_table->addEntry(pid, page_number);
            page_number++;
        }
    }
    else
    {
        // Error not enough memory
         fprintf(stderr, "Error, not enough memory");
    }
    
    //   - print virtual memory address
    if (var_name.compare("<TEXT>") != 0 && var_name.compare("<GLOBALS>") != 0 && var_name.compare("<STACK>") != 0)
    {
        std::cout << var->virtual_address << std::endl;
    }
    var->virtual_address += size_bytes;
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    int physical_address;
    int n = (int)log2(page_table->_page_size);
    Variable *var = mmu->getVariable(pid, var_name);
    if (var != NULL)
    {   
        uint32_t type_size = mmu->sizeOfType(var->type);
        //   - look up physical address for variable based on its virtual address / offset
        physical_address = page_table->getPhysicalAddress(pid, (var->virtual_address + offset));
        //   - insert `value` into `memory` at physical address
        memcpy((uint8_t*)memory + physical_address, value, type_size);
    }
    else
    {
        // Error, variable not found
        fprintf(stderr, "Error, variable not found");
    }
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array)
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process
}

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