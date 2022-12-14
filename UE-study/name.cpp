#include "name.h"

auto PrintName(std::string str) -> void {
	std::cout << str << '\n';
}

auto GetExeBase() -> uintptr_t {
	return reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
}

// <sampleeee.exe> + offset == pFNameEntryAllocator
DWORD offset = 0x439F140;

auto GetEntrySize(bool bIsWide, DWORD length) -> DWORD {
	if (bIsWide)
	{
		return ((length * 2) + alignof(FNameEntryHeader) + FNameEntryAllocator::Stride - 1) & ~(FNameEntryAllocator::Stride - 1);
	}
	else
	{
		PRINTF("%zd", alignof(FNameEntryHeader));
		PRINTF("%d", FNameEntryAllocator::Stride);
		PRINTF("%zd", (length + alignof(FNameEntryHeader) + FNameEntryAllocator::Stride - 1) & ~(FNameEntryAllocator::Stride - 1));
		return (length + alignof(FNameEntryHeader) + FNameEntryAllocator::Stride - 1) & ~(FNameEntryAllocator::Stride - 1);
	}
}

auto GetNameDump() -> void {
	uintptr_t moduleBase = GetExeBase();
	FNameEntryAllocator* fNameEntryAllocator = reinterpret_cast<FNameEntryAllocator*>(moduleBase + offset);

	char* pFNameEntry = reinterpret_cast<char*>(fNameEntryAllocator->Blocks[0]);

	// getting 50 names. By the way, 50 is just a random number. I'll eraborate it later.
	for (int i = 0; i < 50; ++i) {
		DWORD length = reinterpret_cast<FNameEntryHeader*>(pFNameEntry)->Len >> 5;
		bool bIsWide = reinterpret_cast<FNameEntryHeader*>(pFNameEntry)->bIsWide;

		if (length == 0)
			break;

		// creating a buffer
		char tmpName[NAME_SIZE] = { 0, };

		PRINTF("%d", reinterpret_cast<FNameEntryHeader*>(pFNameEntry)->bIsWide);
		PRINTF("length: %zd", sizeof(char) * length);

		// skipping 0x2 which is the sizeof Header
		strncpy(tmpName, pFNameEntry + 0x2, length);
		PrintName(std::string(tmpName));
		// clearing it out
		tmpName[length] = 0;
		// std::cout << "[*] the length to skip: " << sizeof(FNameEntryHeader) + sizeof(char) * length << std::endl;

		// moving to the next entry
		pFNameEntry += GetEntrySize(bIsWide, length);
	}
}
