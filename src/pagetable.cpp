#include "pagetable.h"
#include <cmath>

int frame = 0;

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
}

PageTable::~PageTable()
{
}

std::vector<std::string> PageTable::sortedKeys()
{
    std::vector<std::string> keys;

    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        keys.push_back(it->first);
    }

    std::sort(keys.begin(), keys.end(), PageTableKeyComparator());

    return keys;
}

// Frees all pages associated with given process
void PageTable::freeProcessPages(uint32_t pid)
{
    std::map<std::string, int>::iterator it;
    std::string entry;
    int i;
    int size = _table.size();
    for (i=0; i < size; i++)
    {
        entry = std::to_string(pid) + "|" + std::to_string(i);
        it = _table.find(entry);
        if (it == _table.end())
        {
            break;
        }
        _table.erase(it);
    }
}

// Free a frame in the page table
void PageTable::freeFrame(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    // Free frame
    _table[entry] == 0;
}

// Get a specified frame in the page table
int PageTable::getFrame(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    std::map<std::string, int>::iterator it = _table.find(entry);

    if (it == _table.end())
    {
        return -1;
    }
    else
    {
        return _table[entry];
    }
    
}

void PageTable::addEntry(uint32_t pid, int page_number)
{   
    
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    // Find free frame
    // TODO: implement this!
    _table[entry] = frame;
    frame++;
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{
    // Convert virtual address to page_number and page_offset
    // TODO: implement this!
    int page_number = 0;
    int page_offset = 0;

    int n = (int)log2(_page_size); // n = number of bits for page offset
    page_number = virtual_address >> n;
    page_offset = (_page_size - 1) & virtual_address;

    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    int frame_number = 0;
    if (_table.count(entry) > 0)
    {
        // TODO: implement this!
        frame_number = _table[entry];
        address = (_page_size * frame_number) + page_offset;
    }

    return address;
}

void PageTable::print()
{
    int i;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    std::vector<std::string> keys = sortedKeys();

    for (i = 0; i < keys.size(); i++)
    {
        // TODO: print all pages
        size_t sep = keys[i].find("|");
        uint32_t pid = std::stoi(keys[i].substr(0, sep));
        int page = std::stoi(keys[i].substr(sep + 1));
        int frame = _table[keys[i]];

        printf(" %4u | %11d | %12d\n", pid, page, frame);
    }
}