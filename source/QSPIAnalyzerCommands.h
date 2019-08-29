#include <LogicPublicTypes.h>

struct CommandAttr {
	bool AcceptsAddr;
	bool UsesDummyCycles;
	bool HasData;
	bool isWrite;
	int AddressLineMask;
	int DataLineMask;
	char CommandName[128];
};

const CommandAttr& GetQSPICommandAttr(U64 id);
bool IsCommandValid(U64 id);
U64 GetQSPICommand(U64 index);
