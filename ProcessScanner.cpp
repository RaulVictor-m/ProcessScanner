#include "ProcessScanner.h"
#define MEGABYTES(x)    1024 * (1024 * x)
#define BufferSize 4096


//constructor & destructor
ProcessScanner::ProcessScanner()
{
    m_process_Handler = NULL;
    m_process_ID = NULL;
    m_memory_blocks = NULL;
}

ProcessScanner::~ProcessScanner()
{
    del_blocks();

    CloseHandle(m_process_Handler);
}

bool ProcessScanner::Init(const char* processName)
{
    HWND hWnd = FindWindowA(nullptr, processName);
    GetWindowThreadProcessId(hWnd, &m_process_ID);

    if (set_processHandler())
    {
        set_blocks();
        return true;
    }

    return false;
}

bool ProcessScanner::Init(const DWORD process_ID)
{
    m_process_ID = process_ID;

    if (set_processHandler())
    {
        set_blocks();
        return true;
    }

    return false;
}



bool ProcessScanner::Read_memory_address(UINT64 address, void* output, size_t size) const
{
    return ReadProcessMemory(m_process_Handler, (LPVOID)address, output, size, nullptr);
}

bool ProcessScanner::Write_memory_address(UINT64 address, void* input, size_t size) const
{
    return WriteProcessMemory(m_process_Handler, (LPVOID)address, input, size, nullptr);
}




size_t ProcessScanner::Find_all_patterns(UINT64* addresses, const size_t addresses_size,const UCHAR* pattern,const size_t pattern_size, const UINT64 MAX_RANGE, const UINT64 MIN_RANGE) const
{
    UINT64 MIN_ADDR = MIN_RANGE;
    for (size_t i = 0; i < addresses_size; i++)
    {
        addresses[i] = Find_pattern(pattern, pattern_size,MAX_RANGE , MIN_ADDR);

        if (!addresses[i])
        {
            return i + 1;
        }
            
        MIN_ADDR = addresses[i]+1;
    }
    return addresses_size;
}

UINT64 ProcessScanner::Find_pattern(const UCHAR* pattern, const size_t pattern_size,const UINT64 MAX_RANGE,const UINT64 MIN_RANGE) const
{

    const MemoryBlock *block = m_memory_blocks;

    size_t read_size = 0;
    UINT64 addr = 0;
    size_t size = 0;
    int cout = 0;

    UINT64 return_address;
    while (block)
    {
        addr = block->addr;
        size = block->size;
        cout++;
                            //DEFINING THE RANGE
        if (block->addr < MIN_RANGE)
            if (block->addr + block->size < MIN_RANGE)
            {
                goto Next_Memory_block;
            }
            else 
            {
                addr = MIN_RANGE;
                size = block->size - (addr - block->addr);
            }
        
        if ((block->addr + block->size) > MAX_RANGE)
            if (block->addr > MAX_RANGE)
            {
                goto Next_Memory_block;
            }
            else
            {
                size = MAX_RANGE - block->addr;
            }
        


        
                        //SETING UP THE READSIZE TO FIT THE BUFFER
        while (size)
        {
            if (size > BufferSize)
            {
                read_size = BufferSize;
                size -= (BufferSize - pattern_size);
            }
            else
            {
                read_size = size;
                size = 0;
            }


                        //READING IN THE DEFINED AREA
            return_address = Match_pattern_in_address(pattern, pattern_size, addr, read_size);
            if (return_address) 
                return return_address;
            
            

                        //SETING THE NEXT STATING ADDR TO LOAD IF NEEDED
            addr += (read_size - pattern_size);
        }

        Next_Memory_block:
        block = block->next;
    }

    

    return 0;
}




UINT64 ProcessScanner::Match_pattern_in_address(const UCHAR* pattern, const size_t pattern_size, const UINT64 addr, const size_t read_size) const
{
    UCHAR buffer[BufferSize];
    if (Read_memory_address(addr, buffer, read_size))
    {

        //buffer search
        for (int i = 0; i <= (read_size - pattern_size); i++)
        {

            //compere to pattern
            for (size_t ip = 0; ip < pattern_size; ip++)
            {

                if (buffer[i + ip] == pattern[ip])
                {

                    if ((ip + 1) == pattern_size)
                    {
                        return addr + i;
                    }

                }
                else break;

            }

        }
        return 0;
    }
    return 0;
}



















bool ProcessScanner::set_processHandler()
{
    m_process_Handler = OpenProcess(PROCESS_ALL_ACCESS, false, m_process_ID);
    if (m_process_Handler) return 1;
    return 0;
}

void ProcessScanner::set_blocks()
{
    
    MemoryBlock head = { 0,0,NULL };
    m_memory_blocks = &head;





    MemoryBlock* block = m_memory_blocks;


    MEMORY_BASIC_INFORMATION meminfo;
    UCHAR *addr = 0;


    while (true)
    {
        if (!(VirtualQueryEx(m_process_Handler, addr, &meminfo, sizeof(meminfo)))) break;




#define WRITABLE (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)
        if ((meminfo.State & MEM_COMMIT) || (meminfo.Protect & WRITABLE))
        {

            


            block->next = new MemoryBlock{ (UINT64)meminfo.BaseAddress, meminfo.RegionSize, NULL };
            block = block->next;




        }

        addr += meminfo.RegionSize;
    }


    m_memory_blocks = m_memory_blocks->next;


}

void ProcessScanner::del_blocks()
{
    while (m_memory_blocks)
    {
        MemoryBlock* block = m_memory_blocks;
        m_memory_blocks = m_memory_blocks->next;
        delete block;
    }

}
