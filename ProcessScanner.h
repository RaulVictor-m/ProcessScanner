#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>

struct MemoryBlock { UINT64 addr, size; MemoryBlock* next; };


class ProcessScanner
{
private:
	HANDLE				m_process_Handler;
	DWORD				m_process_ID;

	MemoryBlock*		m_memory_blocks;

	
	

public:
//constructor & destructor
				ProcessScanner	();
				~ProcessScanner	();


	bool		Init(const char* processName);
	bool		Init(const DWORD process_ID);


//get atributes
	const	HANDLE	Get_process_handler	()const { return m_process_Handler; }
	const	DWORD	Get_processID		()const	{ return m_process_ID; }
	


//funcitions
	bool		Read_memory_address	(UINT64 address, void* output, size_t size) const;
	bool		Write_memory_address(UINT64 address, void* input, size_t size)	const;


	size_t		Find_all_patterns(UINT64* addresses, const size_t addresses_size,const UCHAR* pattern, const size_t pattern_size, 
								const UINT64 MAX_RANGE = MAXULONG64, const UINT64 MIN_RANGE = 0) const;

	UINT64		Find_pattern(const UCHAR* pattern,const size_t pattern_size, 
								const UINT64 MAX_RANGE = MAXULONG64, const UINT64 MIN_RANGE = 0) const;



private:
	UINT64		match_pattern_in_address(const UCHAR* pattern, const size_t pattern_size, const UINT64 addr, const size_t read_size) const;

	bool		set_processHandler	();
	void		set_blocks			();


	void		del_blocks			();
};