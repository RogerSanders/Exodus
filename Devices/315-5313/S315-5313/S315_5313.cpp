#include "S315_5313.h"
#include "DebugMenuHandler.h"
//##DEBUG##
#include <iostream>

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const S315_5313::RenderOp S315_5313::operationsH32[] = {
	//End of left scrolled 2 cells
	{S315_5313::RenderOp::PATTERN_B, 1},

	//Cell block 1
	{S315_5313::RenderOp::MAPPING_A, 2}, {S315_5313::RenderOp::ACC_SLOT,  2}, {S315_5313::RenderOp::PATTERN_A, 2}, {S315_5313::RenderOp::PATTERN_A, 3}, {S315_5313::RenderOp::MAPPING_B, 2}, {S315_5313::RenderOp::MAPPING_S, 0}, {S315_5313::RenderOp::PATTERN_B, 2}, {S315_5313::RenderOp::PATTERN_B, 3},
	{S315_5313::RenderOp::MAPPING_A, 4}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A, 4}, {S315_5313::RenderOp::PATTERN_A, 5}, {S315_5313::RenderOp::MAPPING_B, 4}, {S315_5313::RenderOp::MAPPING_S, 1}, {S315_5313::RenderOp::PATTERN_B, 4}, {S315_5313::RenderOp::PATTERN_B, 5},
	{S315_5313::RenderOp::MAPPING_A, 6}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A, 6}, {S315_5313::RenderOp::PATTERN_A, 7}, {S315_5313::RenderOp::MAPPING_B, 6}, {S315_5313::RenderOp::MAPPING_S, 2}, {S315_5313::RenderOp::PATTERN_B, 6}, {S315_5313::RenderOp::PATTERN_B, 7},
	{S315_5313::RenderOp::MAPPING_A, 8}, {S315_5313::RenderOp::REFRESH,   0}, {S315_5313::RenderOp::PATTERN_A, 8}, {S315_5313::RenderOp::PATTERN_A, 9}, {S315_5313::RenderOp::MAPPING_B, 8}, {S315_5313::RenderOp::MAPPING_S, 3}, {S315_5313::RenderOp::PATTERN_B, 8}, {S315_5313::RenderOp::PATTERN_B, 9},

	//Cell block 2
	{S315_5313::RenderOp::MAPPING_A,10}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,10}, {S315_5313::RenderOp::PATTERN_A,11}, {S315_5313::RenderOp::MAPPING_B,10}, {S315_5313::RenderOp::MAPPING_S, 4}, {S315_5313::RenderOp::PATTERN_B,10}, {S315_5313::RenderOp::PATTERN_B,11},
	{S315_5313::RenderOp::MAPPING_A,12}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,12}, {S315_5313::RenderOp::PATTERN_A,13}, {S315_5313::RenderOp::MAPPING_B,12}, {S315_5313::RenderOp::MAPPING_S, 5}, {S315_5313::RenderOp::PATTERN_B,12}, {S315_5313::RenderOp::PATTERN_B,13},
	{S315_5313::RenderOp::MAPPING_A,14}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,14}, {S315_5313::RenderOp::PATTERN_A,15}, {S315_5313::RenderOp::MAPPING_B,14}, {S315_5313::RenderOp::MAPPING_S, 6}, {S315_5313::RenderOp::PATTERN_B,14}, {S315_5313::RenderOp::PATTERN_B,15},
	{S315_5313::RenderOp::MAPPING_A,16}, {S315_5313::RenderOp::REFRESH,   0}, {S315_5313::RenderOp::PATTERN_A,16}, {S315_5313::RenderOp::PATTERN_A,17}, {S315_5313::RenderOp::MAPPING_B,16}, {S315_5313::RenderOp::MAPPING_S, 7}, {S315_5313::RenderOp::PATTERN_B,16}, {S315_5313::RenderOp::PATTERN_B,17},

	//Cell block 3
	{S315_5313::RenderOp::MAPPING_A,18}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,18}, {S315_5313::RenderOp::PATTERN_A,19}, {S315_5313::RenderOp::MAPPING_B,18}, {S315_5313::RenderOp::MAPPING_S, 8}, {S315_5313::RenderOp::PATTERN_B,18}, {S315_5313::RenderOp::PATTERN_B,19},
	{S315_5313::RenderOp::MAPPING_A,20}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,20}, {S315_5313::RenderOp::PATTERN_A,21}, {S315_5313::RenderOp::MAPPING_B,20}, {S315_5313::RenderOp::MAPPING_S, 9}, {S315_5313::RenderOp::PATTERN_B,20}, {S315_5313::RenderOp::PATTERN_B,21},
	{S315_5313::RenderOp::MAPPING_A,22}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,22}, {S315_5313::RenderOp::PATTERN_A,23}, {S315_5313::RenderOp::MAPPING_B,22}, {S315_5313::RenderOp::MAPPING_S,10}, {S315_5313::RenderOp::PATTERN_B,22}, {S315_5313::RenderOp::PATTERN_B,23},
	{S315_5313::RenderOp::MAPPING_A,24}, {S315_5313::RenderOp::REFRESH,   0}, {S315_5313::RenderOp::PATTERN_A,24}, {S315_5313::RenderOp::PATTERN_A,25}, {S315_5313::RenderOp::MAPPING_B,24}, {S315_5313::RenderOp::MAPPING_S,11}, {S315_5313::RenderOp::PATTERN_B,24}, {S315_5313::RenderOp::PATTERN_B,25},

	//Cell block 4
	{S315_5313::RenderOp::MAPPING_A,26}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,26}, {S315_5313::RenderOp::PATTERN_A,27}, {S315_5313::RenderOp::MAPPING_B,26}, {S315_5313::RenderOp::MAPPING_S,12}, {S315_5313::RenderOp::PATTERN_B,26}, {S315_5313::RenderOp::PATTERN_B,27},
	{S315_5313::RenderOp::MAPPING_A,28}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,28}, {S315_5313::RenderOp::PATTERN_A,29}, {S315_5313::RenderOp::MAPPING_B,28}, {S315_5313::RenderOp::MAPPING_S,13}, {S315_5313::RenderOp::PATTERN_B,28}, {S315_5313::RenderOp::PATTERN_B,29},
	{S315_5313::RenderOp::MAPPING_A,30}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,30}, {S315_5313::RenderOp::PATTERN_A,31}, {S315_5313::RenderOp::MAPPING_B,30}, {S315_5313::RenderOp::MAPPING_S,14}, {S315_5313::RenderOp::PATTERN_B,30}, {S315_5313::RenderOp::PATTERN_B,31},
	{S315_5313::RenderOp::MAPPING_A,32}, {S315_5313::RenderOp::REFRESH,   0}, {S315_5313::RenderOp::PATTERN_A,32}, {S315_5313::RenderOp::PATTERN_A,33}, {S315_5313::RenderOp::MAPPING_B,32}, {S315_5313::RenderOp::MAPPING_S,15}, {S315_5313::RenderOp::PATTERN_B,32}, {S315_5313::RenderOp::PATTERN_B,33},

	//End of line
	{S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::ACC_SLOT,  1},
	{S315_5313::RenderOp::PATTERN_S, 0}, {S315_5313::RenderOp::PATTERN_S, 1}, {S315_5313::RenderOp::PATTERN_S, 2}, {S315_5313::RenderOp::PATTERN_S, 3}, {S315_5313::RenderOp::PATTERN_S, 4},
	{S315_5313::RenderOp::PATTERN_S, 5}, {S315_5313::RenderOp::PATTERN_S, 6}, {S315_5313::RenderOp::PATTERN_S, 7}, {S315_5313::RenderOp::PATTERN_S, 8}, {S315_5313::RenderOp::PATTERN_S, 9},
	{S315_5313::RenderOp::PATTERN_S,10}, {S315_5313::RenderOp::PATTERN_S,11}, {S315_5313::RenderOp::PATTERN_S,12},
	{S315_5313::RenderOp::ACC_SLOT,  0},
	{S315_5313::RenderOp::PATTERN_S,13}, {S315_5313::RenderOp::PATTERN_S,14},

	//HBlank start
	{S315_5313::RenderOp::PATTERN_S,15}, {S315_5313::RenderOp::PATTERN_S,16}, {S315_5313::RenderOp::PATTERN_S,17}, {S315_5313::RenderOp::PATTERN_S,18}, {S315_5313::RenderOp::PATTERN_S,19},
	{S315_5313::RenderOp::PATTERN_S,20}, {S315_5313::RenderOp::PATTERN_S,21}, {S315_5313::RenderOp::PATTERN_S,22}, {S315_5313::RenderOp::PATTERN_S,23}, {S315_5313::RenderOp::PATTERN_S,24},
	{S315_5313::RenderOp::PATTERN_S,25},
	{S315_5313::RenderOp::ACC_SLOT,  0},

	//New line
	{S315_5313::RenderOp::HSCROLL,   0}, {S315_5313::RenderOp::PATTERN_S,26}, {S315_5313::RenderOp::PATTERN_S,27}, {S315_5313::RenderOp::PATTERN_S,28}, {S315_5313::RenderOp::PATTERN_S,29},

	//Start of left scrolled 2 cells
	{S315_5313::RenderOp::MAPPING_A, 0}, {S315_5313::RenderOp::PATTERN_S,30}, {S315_5313::RenderOp::PATTERN_A, 0}, {S315_5313::RenderOp::PATTERN_A, 1}, {S315_5313::RenderOp::MAPPING_B, 0}, {S315_5313::RenderOp::PATTERN_S,31}, {S315_5313::RenderOp::PATTERN_B, 0}
};

//----------------------------------------------------------------------------------------
const S315_5313::RenderOp S315_5313::operationsH40[] = {
	//Cell block 1
	{S315_5313::RenderOp::MAPPING_A, 2}, {S315_5313::RenderOp::ACC_SLOT,  2}, {S315_5313::RenderOp::PATTERN_A, 2}, {S315_5313::RenderOp::PATTERN_A, 3}, {S315_5313::RenderOp::MAPPING_B, 2}, {S315_5313::RenderOp::MAPPING_S, 0}, {S315_5313::RenderOp::PATTERN_B, 2}, {S315_5313::RenderOp::PATTERN_B, 3},
	{S315_5313::RenderOp::MAPPING_A, 4}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A, 4}, {S315_5313::RenderOp::PATTERN_A, 5}, {S315_5313::RenderOp::MAPPING_B, 4}, {S315_5313::RenderOp::MAPPING_S, 1}, {S315_5313::RenderOp::PATTERN_B, 4}, {S315_5313::RenderOp::PATTERN_B, 5},
	{S315_5313::RenderOp::MAPPING_A, 6}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A, 6}, {S315_5313::RenderOp::PATTERN_A, 7}, {S315_5313::RenderOp::MAPPING_B, 6}, {S315_5313::RenderOp::MAPPING_S, 2}, {S315_5313::RenderOp::PATTERN_B, 6}, {S315_5313::RenderOp::PATTERN_B, 7},
	{S315_5313::RenderOp::MAPPING_A, 8}, {S315_5313::RenderOp::REFRESH,   0}, {S315_5313::RenderOp::PATTERN_A, 8}, {S315_5313::RenderOp::PATTERN_A, 9}, {S315_5313::RenderOp::MAPPING_B, 8}, {S315_5313::RenderOp::MAPPING_S, 3}, {S315_5313::RenderOp::PATTERN_B, 8}, {S315_5313::RenderOp::PATTERN_B, 9},

	//Cell block 2
	{S315_5313::RenderOp::MAPPING_A,10}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,10}, {S315_5313::RenderOp::PATTERN_A,11}, {S315_5313::RenderOp::MAPPING_B,10}, {S315_5313::RenderOp::MAPPING_S, 4}, {S315_5313::RenderOp::PATTERN_B,10}, {S315_5313::RenderOp::PATTERN_B,11},
	{S315_5313::RenderOp::MAPPING_A,12}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,12}, {S315_5313::RenderOp::PATTERN_A,13}, {S315_5313::RenderOp::MAPPING_B,12}, {S315_5313::RenderOp::MAPPING_S, 5}, {S315_5313::RenderOp::PATTERN_B,12}, {S315_5313::RenderOp::PATTERN_B,13},
	{S315_5313::RenderOp::MAPPING_A,14}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,14}, {S315_5313::RenderOp::PATTERN_A,15}, {S315_5313::RenderOp::MAPPING_B,14}, {S315_5313::RenderOp::MAPPING_S, 6}, {S315_5313::RenderOp::PATTERN_B,14}, {S315_5313::RenderOp::PATTERN_B,15},
	{S315_5313::RenderOp::MAPPING_A,16}, {S315_5313::RenderOp::REFRESH,   0}, {S315_5313::RenderOp::PATTERN_A,16}, {S315_5313::RenderOp::PATTERN_A,17}, {S315_5313::RenderOp::MAPPING_B,16}, {S315_5313::RenderOp::MAPPING_S, 7}, {S315_5313::RenderOp::PATTERN_B,16}, {S315_5313::RenderOp::PATTERN_B,17},

	//Cell block 3
	{S315_5313::RenderOp::MAPPING_A,18}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,18}, {S315_5313::RenderOp::PATTERN_A,19}, {S315_5313::RenderOp::MAPPING_B,18}, {S315_5313::RenderOp::MAPPING_S, 8}, {S315_5313::RenderOp::PATTERN_B,18}, {S315_5313::RenderOp::PATTERN_B,19},
	{S315_5313::RenderOp::MAPPING_A,20}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,20}, {S315_5313::RenderOp::PATTERN_A,21}, {S315_5313::RenderOp::MAPPING_B,20}, {S315_5313::RenderOp::MAPPING_S, 9}, {S315_5313::RenderOp::PATTERN_B,20}, {S315_5313::RenderOp::PATTERN_B,21},
	{S315_5313::RenderOp::MAPPING_A,22}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,22}, {S315_5313::RenderOp::PATTERN_A,23}, {S315_5313::RenderOp::MAPPING_B,22}, {S315_5313::RenderOp::MAPPING_S,10}, {S315_5313::RenderOp::PATTERN_B,22}, {S315_5313::RenderOp::PATTERN_B,23},
	{S315_5313::RenderOp::MAPPING_A,24}, {S315_5313::RenderOp::REFRESH,   0}, {S315_5313::RenderOp::PATTERN_A,24}, {S315_5313::RenderOp::PATTERN_A,25}, {S315_5313::RenderOp::MAPPING_B,24}, {S315_5313::RenderOp::MAPPING_S,11}, {S315_5313::RenderOp::PATTERN_B,24}, {S315_5313::RenderOp::PATTERN_B,25},

	//Cell block 4
	{S315_5313::RenderOp::MAPPING_A,26}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,26}, {S315_5313::RenderOp::PATTERN_A,27}, {S315_5313::RenderOp::MAPPING_B,26}, {S315_5313::RenderOp::MAPPING_S,12}, {S315_5313::RenderOp::PATTERN_B,26}, {S315_5313::RenderOp::PATTERN_B,27},
	{S315_5313::RenderOp::MAPPING_A,28}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,28}, {S315_5313::RenderOp::PATTERN_A,29}, {S315_5313::RenderOp::MAPPING_B,28}, {S315_5313::RenderOp::MAPPING_S,13}, {S315_5313::RenderOp::PATTERN_B,28}, {S315_5313::RenderOp::PATTERN_B,29},
	{S315_5313::RenderOp::MAPPING_A,30}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,30}, {S315_5313::RenderOp::PATTERN_A,31}, {S315_5313::RenderOp::MAPPING_B,30}, {S315_5313::RenderOp::MAPPING_S,14}, {S315_5313::RenderOp::PATTERN_B,30}, {S315_5313::RenderOp::PATTERN_B,31},
	{S315_5313::RenderOp::MAPPING_A,32}, {S315_5313::RenderOp::REFRESH,   0}, {S315_5313::RenderOp::PATTERN_A,32}, {S315_5313::RenderOp::PATTERN_A,33}, {S315_5313::RenderOp::MAPPING_B,32}, {S315_5313::RenderOp::MAPPING_S,15}, {S315_5313::RenderOp::PATTERN_B,32}, {S315_5313::RenderOp::PATTERN_B,33},

	//Cell block 5
	{S315_5313::RenderOp::MAPPING_A,34}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,34}, {S315_5313::RenderOp::PATTERN_A,35}, {S315_5313::RenderOp::MAPPING_B,34}, {S315_5313::RenderOp::MAPPING_S,16}, {S315_5313::RenderOp::PATTERN_B,34}, {S315_5313::RenderOp::PATTERN_B,35},
	{S315_5313::RenderOp::MAPPING_A,36}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,36}, {S315_5313::RenderOp::PATTERN_A,37}, {S315_5313::RenderOp::MAPPING_B,36}, {S315_5313::RenderOp::MAPPING_S,17}, {S315_5313::RenderOp::PATTERN_B,36}, {S315_5313::RenderOp::PATTERN_B,37},
	{S315_5313::RenderOp::MAPPING_A,38}, {S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::PATTERN_A,38}, {S315_5313::RenderOp::PATTERN_A,39}, {S315_5313::RenderOp::MAPPING_B,38}, {S315_5313::RenderOp::MAPPING_S,18}, {S315_5313::RenderOp::PATTERN_B,38}, {S315_5313::RenderOp::PATTERN_B,39},
	{S315_5313::RenderOp::MAPPING_A,40}, {S315_5313::RenderOp::REFRESH,   0}, {S315_5313::RenderOp::PATTERN_A,40}, {S315_5313::RenderOp::PATTERN_A,41}, {S315_5313::RenderOp::MAPPING_B,40}, {S315_5313::RenderOp::MAPPING_S,19}, {S315_5313::RenderOp::PATTERN_B,40}, {S315_5313::RenderOp::PATTERN_B,41},

	//End of line
	{S315_5313::RenderOp::ACC_SLOT,  0}, {S315_5313::RenderOp::ACC_SLOT,  1},
	{S315_5313::RenderOp::PATTERN_S, 0}, {S315_5313::RenderOp::PATTERN_S, 1}, {S315_5313::RenderOp::PATTERN_S, 2}, {S315_5313::RenderOp::PATTERN_S, 3}, {S315_5313::RenderOp::PATTERN_S, 4},
	{S315_5313::RenderOp::PATTERN_S, 5}, {S315_5313::RenderOp::PATTERN_S, 6}, {S315_5313::RenderOp::PATTERN_S, 7}, {S315_5313::RenderOp::PATTERN_S, 8}, {S315_5313::RenderOp::PATTERN_S, 9},
	{S315_5313::RenderOp::PATTERN_S,10}, {S315_5313::RenderOp::PATTERN_S,11}, {S315_5313::RenderOp::PATTERN_S,12}, {S315_5313::RenderOp::PATTERN_S,13}, {S315_5313::RenderOp::PATTERN_S,14},
	{S315_5313::RenderOp::PATTERN_S,15}, {S315_5313::RenderOp::PATTERN_S,16},

	//HBlank start
	{S315_5313::RenderOp::PATTERN_S,17}, {S315_5313::RenderOp::PATTERN_S,18}, {S315_5313::RenderOp::PATTERN_S,19}, {S315_5313::RenderOp::PATTERN_S,20}, {S315_5313::RenderOp::PATTERN_S,21},
	{S315_5313::RenderOp::PATTERN_S,22},
	{S315_5313::RenderOp::ACC_SLOT,  0},
	{S315_5313::RenderOp::PATTERN_S,23}, {S315_5313::RenderOp::PATTERN_S,24}, {S315_5313::RenderOp::PATTERN_S,25}, {S315_5313::RenderOp::PATTERN_S,26}, {S315_5313::RenderOp::PATTERN_S,27},
	{S315_5313::RenderOp::PATTERN_S,28}, {S315_5313::RenderOp::PATTERN_S,29}, {S315_5313::RenderOp::PATTERN_S,30}, {S315_5313::RenderOp::PATTERN_S,31}, {S315_5313::RenderOp::PATTERN_S,32},
	{S315_5313::RenderOp::PATTERN_S,33},

	//New line
	{S315_5313::RenderOp::HSCROLL,   0}, {S315_5313::RenderOp::PATTERN_S,34}, {S315_5313::RenderOp::PATTERN_S,35}, {S315_5313::RenderOp::PATTERN_S,36}, {S315_5313::RenderOp::PATTERN_S,37},

	//Left scrolled 2 cells
	{S315_5313::RenderOp::MAPPING_A, 0}, {S315_5313::RenderOp::PATTERN_S,38}, {S315_5313::RenderOp::PATTERN_A, 0}, {S315_5313::RenderOp::PATTERN_A, 1}, {S315_5313::RenderOp::MAPPING_B, 0}, {S315_5313::RenderOp::PATTERN_S,39}, {S315_5313::RenderOp::PATTERN_B, 0}, {S315_5313::RenderOp::PATTERN_B, 1}
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::S315_5313(const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(L"315-5313", ainstanceName, amoduleID),
reg(registerCount, false, Data(8)),
status(10),
bstatus(10),
hcounter(9),
bhcounter(9),
vcounter(9),
bvcounter(9),
hcounterLatchedData(9),
bhcounterLatchedData(9),
vcounterLatchedData(9),
bvcounterLatchedData(9),
readBuffer(16),
breadBuffer(16),
renderVSRAMCachedRead(16),
originalCommandAddress(16),
boriginalCommandAddress(16),
commandAddress(16),
bcommandAddress(16),
commandCode(6),
bcommandCode(6),
dmaTransferReadCache(16),
bdmaTransferReadCache(16),
dmaTransferInvalidPortWriteDataCache(16),
bdmaTransferInvalidDataCache(16),
regSession(Data(8)),
vramSession(0),
cramSession(0),
vsramSession(0),
spriteCacheSession(0),
renderWindowActiveCache(maxCellsPerRow / cellsPerColumn),
renderMappingDataCacheLayerA(maxCellsPerRow, Data(16)),
renderMappingDataCacheLayerB(maxCellsPerRow, Data(16)),
renderMappingDataCacheSprite(maxCellsPerRow, Data(16)),
renderPatternDataCacheLayerA(maxCellsPerRow, Data(32)),
renderPatternDataCacheLayerB(maxCellsPerRow, Data(32)),
renderPatternDataCacheSprite(maxCellsPerRow, Data(32)),
renderSpriteDisplayCache(maxSpriteDisplayCacheSize),
renderSpriteDisplayCellCache(maxSpriteDisplayCellCacheSize)
{
	//Create the menu handler
	menuHandler = new DebugMenuHandler(this);
	menuHandler->LoadMenuItems();

	fifoBuffer.resize(fifoBufferSize);
	bfifoBuffer.resize(fifoBufferSize);

	memoryBus = 0;
	vram = 0;
	cram = 0;
	vsram = 0;
	spriteCache = 0;
	psg = 0;
	clockSourceCLK0 = 0;
	clockSourceCLK1 = 0;

	//Initialize our CE line state
	ceLineMaskLowerDataStrobeInput = 0;
	ceLineMaskUpperDataStrobeInput = 0;
	ceLineMaskReadHighWriteLowInput = 0;
	ceLineMaskAddressStrobeInput = 0;
	ceLineMaskRMWCycleInProgress = 0;
	ceLineMaskRMWCycleFirstOperation = 0;
	ceLineMaskLowerDataStrobeOutput = 0;
	ceLineMaskUpperDataStrobeOutput = 0;
	ceLineMaskReadHighWriteLowOutput = 0;
	ceLineMaskAddressStrobeOutput = 0;
	ceLineMaskLWR = 0;
	ceLineMaskUWR = 0;
	ceLineMaskCAS0 = 0;
	ceLineMaskRAS0 = 0;
	ceLineMaskOE0 = 0;

	//We need to initialize these variables here since a commit is triggered before
	//initialization the first time the system is booted.
	pendingRenderOperation = false;
	renderThreadActive = false;
	renderTimeslicePending = false;
	drawingImageBufferPlane = 0;
	imageBuffer.resize(imageBufferHeight * imageBufferWidth);

	//Initialize the sprite pixel buffer
	for(unsigned int i = 0; i < renderSpritePixelBufferPlaneCount; ++i)
	{
		spritePixelBuffer[i].resize(spritePixelBufferSize);
	}
	renderSpritePixelBufferAnalogRenderPlane = 0;
	renderSpritePixelBufferDigitalRenderPlane = (renderSpritePixelBufferAnalogRenderPlane + 1) % renderSpritePixelBufferPlaneCount;

	busGranted = false;
	palModeLineState = false;

	//##DEBUG##
	outputTestDebugMessages = false;
	//outputTimingDebugMessages = true;
	outputTimingDebugMessages = false;
}

//----------------------------------------------------------------------------------------
S315_5313::~S315_5313()
{
	//Delete the menu handler
	menuHandler->ClearMenuItems();
	delete menuHandler;
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetLineID(const wchar_t* lineName) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"IPL")		//O
	{
		return LINE_IPL;
	}
	else if(lineNameString == L"INT")	//O
	{
		return LINE_INT;
	}
	else if(lineNameString == L"INTAK")	//O
	{
		return LINE_INTAK;
	}
	else if(lineNameString == L"BR")	//O
	{
		return LINE_BR;
	}
	else if(lineNameString == L"BG")	//I
	{
		return LINE_BG;
	}
	else if(lineNameString == L"PAL")	//I
	{
		return LINE_PAL;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
const wchar_t* S315_5313::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_IPL:
		return L"IPL";
	case LINE_INT:
		return L"INT";
	case LINE_INTAK:
		return L"INTAK";
	case LINE_BR:
		return L"BR";
	case LINE_BG:
		return L"BG";
	case LINE_PAL:
		return L"PAL";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetLineWidth(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_IPL:
		return 3;
	case LINE_INT:
		return 1;
	case LINE_INTAK:
		return 1;
	case LINE_BR:
		return 1;
	case LINE_BG:
		return 1;
	case LINE_PAL:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(accessMutex);
	switch(targetLine)
	{
	case LINE_INTAK:{
		//##DEBUG##
		//		std::wcout << "SetLineState - VDP_LINE_INTAK:\t" << lineData.LSB() << '\n';

		memoryBus->SetLineState(LINE_INTAK, Data(GetLineWidth(LINE_INTAK), 1), GetDeviceContext(), caller, accessTime, accessContext);
		memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), 0), GetDeviceContext(), caller, accessTime, accessContext);

		//Since the interrupt has been acknowledged, clear the VINT occurrence flag.
		SetStatusFlagF(false);
		break;}
	case LINE_BG:{
		//##DEBUG##
		//		std::wcout << "SetLineState - VDP_LINE_BG:\t" << lineData.LSB() << '\n';

		//Convert the access time into a cycle count relative to MCLK, rounding up the
		//result to the nearest MCLK cycle.
		unsigned int accessMclkCycle = ConvertAccessTimeToMclkCount(accessTime + lastTimesliceMclkCyclesRemainingTime);
		double accessMclkCycleInAccessTime = ConvertMclkCountToAccessTime(accessMclkCycle);
		if(accessMclkCycleInAccessTime < accessTime)
		{
			accessMclkCycle += 1;
		}

		boost::mutex::scoped_lock lock(workerThreadMutex);
		if(dmaTransferActive || busGranted)
		{
			busGranted = lineData.LSB();
			dmaTransferNextReadMclk = accessMclkCycle;
			dmaTransferLastTimesliceUsedReadDelay = 0;

			//If the bus has just been granted in response to a DMA transfer operation
			//initiating a bus request, set the current timeslice progress for the VDP
			//core to match the time at which the bus was granted. This allows us to set
			//the device which controls bus ownership as a dependent device on the VDP
			//core, as the timeslice progress will be valid during a DMA transfer.
			if(busGranted)
			{
				GetDeviceContext()->SetCurrentTimesliceProgress(accessTime);
			}

			GetDeviceContext()->SetTransientExecutionActive(busGranted);
			workerThreadUpdate.notify_all();
		}
		break;}
	case LINE_PAL:{
		//##DEBUG##
		//		std::wcout << "SetLineState - VDP_LINE_PAL:\t" << lineData.LSB() << '\n';

		palModeLineState = lineData.LSB();
		break;}
	}
}

//----------------------------------------------------------------------------------------
bool S315_5313::AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(accessMutex);
	switch(targetLine)
	{
	case LINE_BR:{
		boost::mutex::scoped_lock workerLock(workerThreadMutex);
		bool targetLineState = lineData.GetBit(0);
		if(dmaTransferActive == targetLineState)
		{
			//If the current state of the target line matches the target state, we have
			//nothing to do.
			return true;
		}
		else if(busGranted && !targetLineState && workerThreadPaused)
		{
			//If we currently have the bus, and the caller is requesting that we advance
			//until the bus request line is negated, we can accurately perform that
			//operation. In this case, we instruct the DMA worker thread to run the DMA
			//operation to completion.
			dmaAdvanceUntilDMAComplete = true;
			workerThreadUpdate.notify_all();
			dmaAdvanceUntilDMAComplete = false;
			workerThreadIdle.wait(workerLock);
			return true;
		}
		break;}
	case LINE_INT:
		//##TODO## We don't require this for the Mega Drive, but implement it anyway.
		break;
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Clock source functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetClockSourceID(const wchar_t* clockSourceName) const
{
	std::wstring lineNameString = clockSourceName;
	if(lineNameString == L"MCLK")
	{
		return CLOCK_MCLK;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
const wchar_t* S315_5313::GetClockSourceName(unsigned int clockSourceID) const
{
	switch(clockSourceID)
	{
	case CLOCK_MCLK:
		return L"MCLK";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
void S315_5313::SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Apply the input clock rate change
	if(clockInput == CLOCK_MCLK)
	{
		clockMclkCurrent = clockRate;
	}

	//Since a clock rate change will affect our timing point calculations, trigger a
	//rollback.
	GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------
void S315_5313::TransparentSetClockSourceRate(unsigned int clockInput, double clockRate)
{
	//Apply the input clock rate change
	if(clockInput == CLOCK_MCLK)
	{
		clockMclkCurrent = clockRate;
	}
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool S315_5313::BuildDevice()
{
	//Initialize the layer priority lookup table. We use this table to speed up layer
	//priority selection during rendering.
	layerPriorityLookupTable.resize(layerPriorityLookupTableSize);
	for(unsigned int i = 0; i < layerPriorityLookupTableSize; ++i)
	{
		//Determine the input layer settings for this table index value
		bool shadowHighlightEnabled = (i & (1 << 8)) != 0;
		bool spriteIsShadowOperator = (i & (1 << 7)) != 0;
		bool spriteIsHighlightOperator = (i & (1 << 6)) != 0;
		bool foundSpritePixel = (i & (1 << 5)) != 0;
		bool foundLayerAPixel = (i & (1 << 4)) != 0;
		bool foundLayerBPixel = (i & (1 << 3)) != 0;
		bool prioritySprite = (i & (1 << 2)) != 0;
		bool priorityLayerA = (i & (1 << 1)) != 0;
		bool priorityLayerB = (i & 1) != 0;

		//Resolve the layer priority for this combination of layer settings
		unsigned int layerIndex;
		bool shadow;
		bool highlight;
		CalculateLayerPriorityIndex(layerIndex, shadow, highlight, shadowHighlightEnabled, spriteIsShadowOperator, spriteIsHighlightOperator, foundSpritePixel, foundLayerAPixel, foundLayerBPixel, prioritySprite, priorityLayerA, priorityLayerB);

		//Incorporate the shadow and highlight bits into the layer index value
		layerIndex |= shadow? 1 << 3: 0;
		layerIndex |= highlight? 1 << 2: 0;

		//Write the combined value to the layer priority lookup table
		layerPriorityLookupTable[i] = layerIndex;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool S315_5313::ValidateDevice()
{
	return (memoryBus != 0) && (vram != 0) && (cram != 0) && (vsram != 0) && (spriteCache != 0);
}

//----------------------------------------------------------------------------------------
void S315_5313::Initialize()
{
	reg.Initialize();
	Reset();

	//Initialize the default external clock divider settings
	//##TODO## Make the clock dividers configurable through the VDP debugger
	const unsigned int initialClockDividerCLK0 = 15;
	const unsigned int initialClockDividerCLK1 = 7;
	if(clockSourceCLK0 != 0)
	{
		clockSourceCLK0->TransparentSetClockDivider((double)initialClockDividerCLK0);
	}
	if(clockSourceCLK1 != 0)
	{
		clockSourceCLK1->TransparentSetClockDivider((double)initialClockDividerCLK1);
	}

	//Note that further testing has also shown that the VRAM is NOT initialized to all 0.
	//It appears the VRAM is initialized with a logical, repeating pattern, but further
	//tests must be carried out to determine what this pattern is. I would suggest reading
	//the entire contents of VRAM, CRAM, and VSRAM back into RAM for analysis.
	//##TODO## Confirm the default pattern for the VRAM
	if(vram != 0)
	{
		vram->Initialize();
		for(unsigned int i = 0; i < vramSize; i += 2)
		{
			//##FIX## This is just a random value, not what the system actually has.
			//		vram->WriteLatest(i, 0xDE);
			//		vram->WriteLatest(i+1, 0xAD);
			//##FIX## Reverted to just zero-filling VRAM, due to some test roms which
			//don't correctly initialize used VRAM areas. Perform some hardware tests to
			//determine an appropriate fill pattern for VRAM.
			vram->WriteLatest(i, 0x00);
			vram->WriteLatest(i+1, 0x00);
		}
	}

	//The CRAM is initialized to 0x0EEE in all slots, except for the second entry in the
	//second palette line, which is initialized to 0x0CEE. This means byte 0x22 in the
	//CRAM is 0x0C instead of 0x0E.
	//##TODO## Confirm the default pattern for the CRAM
	if(cram != 0)
	{
		cram->Initialize();
		for(unsigned int i = 0; i < cramSize; i += 2)
		{
			cram->WriteLatest(i, 0x0E);
			cram->WriteLatest(i+1, 0xEE);
		}
		cram->WriteLatest(0x22, 0x0C);
		cram->WriteLatest(0x23, 0xEE);
	}

	//The VSRAM is initialized to 0x07FF, except for the entry at 0x00, which is 0x07DF,
	//and the entry at 0x22, which is 0x07FB.
	//##TODO## Confirm the default pattern for the VSRAM
	if(vsram != 0)
	{
		vsram->Initialize();
		for(unsigned int i = 0; i < vsramSize; i += 2)
		{
			vsram->WriteLatest(i, 0x07);
			vsram->WriteLatest(i+1, 0xFF);
		}
		vsram->WriteLatest(0x00, 0x07);
		vsram->WriteLatest(0x01, 0xDF);
		vsram->WriteLatest(0x22, 0x07);
		vsram->WriteLatest(0x23, 0xFB);
	}

	//We have no idea how the sprite cache should be initialized. Most likely, it is not
	//initialized to 0 on the real hardware, but that's what we do here currently.
	//##TODO## Perform hardware tests to determine how the sprite cache is initialized
	//after power-on.
	if(spriteCache != 0)
	{
		spriteCache->Initialize();
		for(unsigned int i = 0; i < spriteCacheSize; i += 2)
		{
			spriteCache->WriteLatest(i, 0x00);
		}
	}

	//Clear all the uncommitted render timeslice buffers. We don't need to clear the
	//committed buffers, since the render thread always fully processes all committed
	//pending timeslices before entering the suspended state, but we need to clear the
	//uncommitted buffers here to ensure bad data isn't passed across to the render thread
	//if the system is initialized while there are uncommitted 
	//##FIX## Actually, I don't think we should have to clear these here. The system is
	//never allowed to be suspended until it reaches a consistent state, and the
	//initialize function is only called when the system is at a consistent state, so it
	//seems like these buffers should always be empty at this point anyway.
	//timesliceRenderInfoListUncommitted.clear();
	//regTimesliceListUncommitted.clear();
	//vramTimesliceListUncommitted.clear();
	//cramTimesliceListUncommitted.clear();
	//vsramTimesliceListUncommitted.clear();
	//spriteCacheTimesliceListUncommitted.clear();

	currentTimesliceLength = 0;
	currentTimesliceMclkCyclesRemainingTime = 0;
	lastTimesliceMclkCyclesOverrun = 0;
	stateLastUpdateTime = 0;
	stateLastUpdateMclk = 0;
	stateLastUpdateMclkUnused = 0;
	stateLastUpdateMclkUnusedFromLastTimeslice = 0;

	externalInterruptPending = false;

	//##TODO## Initialize the actual data in the FIFO buffer. We can determine the default
	//values for many bits in the FIFO on power-up by immediately setting up a read target
	//from VSRAM and CRAM, and stepping through each entry in the FIFO, saving the values
	//we read out of the initial bits. Most likely, the FIFO is initialized to 0, but we
	//should perform a test to be certain.
	fifoNextReadEntry = 0;
	fifoNextWriteEntry = 0;
	commandWritePending = false;
	for(unsigned int i = 0; i < fifoBufferSize; ++i)
	{
		fifoBuffer[i].codeRegData = 0;
		fifoBuffer[i].addressRegData = 0;
		fifoBuffer[i].dataPortWriteData = 0;
		fifoBuffer[i].dataWriteHalfWritten = false;
		fifoBuffer[i].pendingDataWrite = false;
	}
	renderVSRAMCachedRead = 0;
	readDataAvailable = false;
	readDataHalfCached = false;
	dmaFillOperationRunning = false;

	workerThreadPaused = false;
	dmaTransferActive = false;
	dmaTransferInvalidPortWriteCached = false;
	dmaAdvanceUntilDMAComplete = false;
	busGranted = false;
	palModeLineState = true;
	interlaceEnabled = false;
	interlaceDouble = false;
	screenModeRS0 = false;
	screenModeRS1 = false;
	screenModeV30 = false;
	palMode = true;
	eventSnapshotSaved = false;
}

//----------------------------------------------------------------------------------------
void S315_5313::Reset()
{
	//After numerous tests performed on the actual system, it appears that all VDP
	//registers are initialized to 0 on a reset. This is supported by Addendum 4 from
	//SEGA in the Genesis Software Manual. Note that since Mode 4 graphics are enabled
	//when bit 2 of reg 1 is 0, the VDP starts in Mode 4. This has been confirmed on the
	//real hardware.
	for(unsigned int i = 0; i < registerCount; ++i)
	{
		SetRegisterData(0, AccessTarget().AccessLatest(), Data(8, 0));
	}
	vintEnabled = false;
	hintEnabled = false;
	exintEnabled = false;
	hintCounterReloadValue = 0;
	dmaEnabled = false;
	dmd0 = false;
	dmd1 = false;
	dmaLengthCounter = 0;
	dmaSourceAddressByte1 = 0;
	dmaSourceAddressByte2 = 0;
	dmaSourceAddressByte3 = 0;
	autoIncrementData = 0;
	interlaceEnabledCached = false;
	interlaceDoubleCached = false;
	screenModeRS0Cached = false;
	screenModeRS1Cached = false;
	screenModeV30Cached = false;
	displayEnabledCached = false;
	spriteAttributeTableBaseAddressDecoded = 0;
	verticalScrollModeCached = false;
	cachedDMASettingsChanged = false;

	//##TODO## Check if we need to clear these here
	status = 0;
	//##FIX## We know for a fact that real VDP powers on with what is essentially a random
	//hcounter and vcounter value. Some Mega Drive games are known to rely on this
	//behaviour, as they use the hcounter as a random seed. Examples given include "X-Men
	//2 Clone Wars" for character assignment, and Eternal Champions and Bonkers for the
	//Sega logo.
	hcounter = 0;
	vcounter = 0;
	hvCounterLatched = false;
	hintCounter = 0;
	hintCounterReloadValue = 0;
	hintEnabled = false;
	exintEnabled = false;

	renderDigitalHCounterPos = 0;
	renderDigitalVCounterPos = 0;
	renderDigitalVCounterPosPreviousLine = 0;
	renderDigitalRemainingMclkCycles = 0;
	renderDigitalScreenModeRS0Active = false;
	renderDigitalScreenModeRS1Active = false;
	renderDigitalScreenModeV30Active = false;
	renderDigitalInterlaceEnabledActive = false;
	renderDigitalInterlaceDoubleActive = false;
	renderDigitalPalModeActive = false;
	renderDigitalOddFlagSet = false;
	renderDigitalMclkCycleProgress = 0;
	renderLayerAHscrollPatternDisplacement = 0;
	renderLayerBHscrollPatternDisplacement = 0;
	renderLayerAHscrollMappingDisplacement = 0;
	renderLayerBHscrollMappingDisplacement = 0;
	renderLayerAVscrollPatternDisplacement = 0;
	renderLayerBVscrollPatternDisplacement = 0;
	renderLayerAVscrollMappingDisplacement = 0;
	renderLayerBVscrollMappingDisplacement = 0;

	//Initialize the command port data
	//##TODO## Test to see if these values are actually cleared on a reset.
	fifoNextReadEntry = 0;
	fifoNextWriteEntry = 0;
	codeAndAddressRegistersModifiedSinceLastWrite = true;
	commandWritePending = false;
	originalCommandAddress = 0;
	commandAddress = 0;
	commandCode = 0;
	for(unsigned int i = 0; i < fifoBufferSize; ++i)
	{
		fifoBuffer[i].codeRegData = 0;
		fifoBuffer[i].addressRegData = 0;
		fifoBuffer[i].dataPortWriteData = 0;
		fifoBuffer[i].dataWriteHalfWritten = false;
		fifoBuffer[i].pendingDataWrite = false;
	}
	renderVSRAMCachedRead = 0;
	readDataAvailable = false;
	readDataHalfCached = false;
	dmaFillOperationRunning = false;

	lineLWRSavedStateRMW = false;
	lineUWRSavedStateRMW = false;
	lineCAS0SavedStateRMW = false;
	lineRAS0SavedStateRMW = false;
	lineOE0SavedStateRMW = false;
}

//----------------------------------------------------------------------------------------
void S315_5313::BeginExecution()
{
	//Initialize the render worker thread state
	pendingRenderOperationCount = 0;
	renderThreadLagging = false;
	regTimesliceList.clear();
	vramTimesliceList.clear();
	cramTimesliceList.clear();
	vsramTimesliceList.clear();
	spriteCacheTimesliceList.clear();

	//Start the render worker thread
	renderThreadActive = true;
	boost::thread renderThread(boost::bind(boost::mem_fn(&S315_5313::RenderThreadNew), this));

	//Start the DMA worker thread
	workerThreadActive = true;
	boost::thread workerThread(boost::bind(boost::mem_fn(&S315_5313::DMAWorkerThread), this));
}

//----------------------------------------------------------------------------------------
void S315_5313::SuspendExecution()
{
	//Suspend the render worker thread
	boost::mutex::scoped_lock renderLock(renderThreadMutex);
	if(renderThreadActive)
	{
		renderThreadActive = false;
		renderThreadUpdate.notify_all();
		renderThreadStopped.wait(renderLock);
	}

	//Suspend the DMA worker thread
	boost::mutex::scoped_lock workerLock(workerThreadMutex);
	if(workerThreadActive)
	{
		workerThreadActive = false;
		workerThreadUpdate.notify_all();
		workerThreadStopped.wait(workerLock);
	}
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool S315_5313::AddReference(const wchar_t* referenceName, IDevice* target)
{
	std::wstring referenceNameString = referenceName;
	if(referenceNameString == L"VRAM")
	{
		ITimedBufferIntDevice* device = dynamic_cast<ITimedBufferIntDevice*>(target);
		if(device != 0)
		{
			vram = device->GetTimedBuffer();
		}
	}
	else if(referenceNameString == L"CRAM")
	{
		ITimedBufferIntDevice* device = dynamic_cast<ITimedBufferIntDevice*>(target);
		if(device != 0)
		{
			cram = device->GetTimedBuffer();
		}
	}
	else if(referenceNameString == L"VSRAM")
	{
		ITimedBufferIntDevice* device = dynamic_cast<ITimedBufferIntDevice*>(target);
		if(device != 0)
		{
			vsram = device->GetTimedBuffer();
		}
	}
	else if(referenceNameString == L"SpriteCache")
	{
		ITimedBufferIntDevice* device = dynamic_cast<ITimedBufferIntDevice*>(target);
		if(device != 0)
		{
			spriteCache = device->GetTimedBuffer();
		}
	}
	else if(referenceNameString == L"PSG")
	{
		psg = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool S315_5313::AddReference(const wchar_t* referenceName, IBusInterface* target)
{
	std::wstring referenceNameString = referenceName;
	if(referenceNameString == L"BusInterface")
	{
		memoryBus = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool S315_5313::AddReference(const wchar_t* referenceName, IClockSource* target)
{
	bool result = false;
	std::wstring referenceNameString = referenceName;
	if(referenceNameString == L"CLK0")
	{
		if(target->GetClockType() == IClockSource::CLOCKTYPE_DIVIDER)
		{
			clockSourceCLK0 = target;
			result = true;
		}
	}
	else if(referenceNameString == L"CLK1")
	{
		if(target->GetClockType() == IClockSource::CLOCKTYPE_DIVIDER)
		{
			clockSourceCLK1 = target;
			result = true;
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool S315_5313::RemoveReference(IDevice* target)
{
	if((IDevice*)vram == target)
	{
		vram = 0;
	}
	else if((IDevice*)cram == target)
	{
		cram = 0;
	}
	else if((IDevice*)vsram == target)
	{
		vsram = 0;
	}
	else if((IDevice*)spriteCache == target)
	{
		spriteCache = 0;
	}
	else if(psg == target)
	{
		psg = 0;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool S315_5313::RemoveReference(IBusInterface* target)
{
	if(memoryBus == target)
	{
		memoryBus = 0;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool S315_5313::RemoveReference(IClockSource* target)
{
	if(clockSourceCLK0 == target)
	{
		clockSourceCLK0 = 0;
	}
	else if(clockSourceCLK1 == target)
	{
		clockSourceCLK1 = 0;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Suspend functions
//----------------------------------------------------------------------------------------
bool S315_5313::UsesTransientExecution() const
{
	return true;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
S315_5313::UpdateMethod S315_5313::GetUpdateMethod() const
{
	return UPDATEMETHOD_TIMESLICE;
}

//----------------------------------------------------------------------------------------
bool S315_5313::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void S315_5313::NotifyUpcomingTimeslice(double nanoseconds)
{
	stateLastUpdateMclk = lastTimesliceMclkCyclesOverrun;
	stateLastUpdateMclkUnused = 0;
	stateLastUpdateTime = ConvertMclkCountToAccessTime(lastTimesliceMclkCyclesOverrun);
	//##DEBUG## Restore old timing behaviour for testing
//	stateLastUpdateTime = 0;
//	stateLastUpdateMclk = 0;
//	stateLastUpdateMclkUnused = 0;

	currentTimesliceLength = nanoseconds;
	eventsProcessedForTimeslice = (nanoseconds <= 0.0);

	lastAccessTime = 0;

	lastTimesliceMclkCyclesRemainingTime = currentTimesliceMclkCyclesRemainingTime;
	currentTimesliceTotalMclkCycles = ConvertAccessTimeToMclkCount(currentTimesliceLength + lastTimesliceMclkCyclesRemainingTime);
	double mclkCyclesToAddInAccessTime = ConvertMclkCountToAccessTime(currentTimesliceTotalMclkCycles);
	currentTimesliceMclkCyclesRemainingTime = (currentTimesliceLength + lastTimesliceMclkCyclesRemainingTime) - mclkCyclesToAddInAccessTime;

	//Round off error adjustment code in ConvertAccessTimeToMclkCount can result in
	//currentTimesliceTotalMclkCycles being rounded up. In this case, our
	//currentTimesliceMclkCyclesRemainingTime variable would end up with a negative
	//result, the presence of which would bias us towards over-estimating the number of
	//mclk cycles for the next timeslice too, resulting in a compounding error, with the
	//numbers getting further and further off with each successive timeslice. We
	//compensate for the negative error here when it occurs, by removing one mclk cycle to
	//force a positive result.
	while(currentTimesliceMclkCyclesRemainingTime < 0)
	{
		//##DEBUG##
//		std::wcout << "######################################################\n";
//		std::wcout << "VDP NotifyUpcomingTimeslice resulted in negative mclkCyclesRemainingTime!\n";
//		std::wcout << "nanoseconds:\t" << nanoseconds << "\n";
//		std::wcout << "currentTimesliceTotalMclkCycles:\t" << currentTimesliceTotalMclkCycles << "\n";
//		std::wcout << "mclkCyclesToAddInAccessTime:\t" << mclkCyclesToAddInAccessTime << "\n";
//		std::wcout << "currentTimesliceMclkCyclesRemainingTime:\t" << currentTimesliceMclkCyclesRemainingTime << "\n";
//		std::wcout << "lastTimesliceMclkCyclesRemainingTime:\t" << lastTimesliceMclkCyclesRemainingTime << "\n";
//		std::wcout << "######################################################\n";
		if(currentTimesliceTotalMclkCycles > 0)
		{
			currentTimesliceTotalMclkCycles -= 1;
			mclkCyclesToAddInAccessTime = ConvertMclkCountToAccessTime(currentTimesliceTotalMclkCycles);
		}
		currentTimesliceMclkCyclesRemainingTime = (currentTimesliceLength + lastTimesliceMclkCyclesRemainingTime) - mclkCyclesToAddInAccessTime;
	}

	//##DEBUG##
	if(outputTimingDebugMessages)
	{
		std::wcout << "VDPNotifyUpcomingTimeslice:\t" << currentTimesliceLength << '\t' << currentTimesliceTotalMclkCycles << '\t' << mclkCyclesToAddInAccessTime << '\t' << currentTimesliceMclkCyclesRemainingTime << '\n';
	}

	//Flag a new render operation to start at the next commit
	pendingRenderOperation = true;

	//Add the new timeslice to all our timed buffers
	reg.AddTimeslice(currentTimesliceTotalMclkCycles);
	vram->AddTimeslice(currentTimesliceTotalMclkCycles);
	cram->AddTimeslice(currentTimesliceTotalMclkCycles);
	vsram->AddTimeslice(currentTimesliceTotalMclkCycles);
	spriteCache->AddTimeslice(currentTimesliceTotalMclkCycles);

	//Add references to the new timeslice entry from our timed buffers to the uncommitted
	//timeslice lists for the buffers
	regTimesliceListUncommitted.push_back(reg.GetLatestTimeslice());
	vramTimesliceListUncommitted.push_back(vram->GetLatestTimesliceReference());
	cramTimesliceListUncommitted.push_back(cram->GetLatestTimesliceReference());
	vsramTimesliceListUncommitted.push_back(vsram->GetLatestTimesliceReference());
	spriteCacheTimesliceListUncommitted.push_back(spriteCache->GetLatestTimesliceReference());
	timesliceRenderInfoListUncommitted.push_back(TimesliceRenderInfo(lastTimesliceMclkCyclesOverrun));
}

//----------------------------------------------------------------------------------------
bool S315_5313::SendNotifyBeforeExecuteCalled() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void S315_5313::NotifyBeforeExecuteCalled()
{
	//If the DMA worker thread is currently active but paused, resume it here.
	boost::mutex::scoped_lock lock(workerThreadMutex);
	if(workerThreadActive && workerThreadPaused)
	{
		//##DEBUG##
//		std::wcout << L"NotifyBeforeExecuteCalled is resuming DMA worker thread\n";

		workerThreadUpdate.notify_all();
	}
}

//----------------------------------------------------------------------------------------
bool S315_5313::SendNotifyAfterExecuteCalled() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void S315_5313::NotifyAfterExecuteCalled()
{
	//Ensure that the DMA worker thread has finished executing
	boost::mutex::scoped_lock lock(workerThreadMutex);
	if(workerThreadActive && !workerThreadPaused && busGranted)
	{
		//##DEBUG##
//		std::wcout << L"NotifyAfterExecuteCalled is waiting for DMA worker thread to pause\n";
//		std::wcout << '\t' << workerThreadActive << '\t' << workerThreadPaused << '\t' << busGranted << '\n';

		workerThreadIdle.wait(lock);
	}

	//##DEBUG##
	if(outputTimingDebugMessages)
	{
		std::wcout << "VDP - NotifyAfterExecuteCalled(Before): " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << stateLastUpdateMclk << '\t' << stateLastUpdateMclkUnused << '\t' << std::setprecision(16) << stateLastUpdateTime << '\n';
	}

	//Ensure the VDP is advanced up to the end of its timeslice
	if((currentTimesliceTotalMclkCycles > 0) && (currentTimesliceTotalMclkCycles > GetProcessorStateMclkCurrent()))
	{
		UpdateInternalState(currentTimesliceTotalMclkCycles, false, false, false, false, false, false, false);
	}

	//Calculate the number of mclk cycles we ran over the end of the last timeslice
	lastTimesliceMclkCyclesOverrun = 0;
	if(GetProcessorStateMclkCurrent() > currentTimesliceTotalMclkCycles)
	{
		lastTimesliceMclkCyclesOverrun = GetProcessorStateMclkCurrent() - currentTimesliceTotalMclkCycles;
	}

	//##DEBUG##
	if(outputTimingDebugMessages)
	{
		std::wcout << "VDP - NotifyAfterExecuteCalled(After): " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << stateLastUpdateMclk << '\t' << stateLastUpdateMclkUnused << '\t' << std::setprecision(16) << stateLastUpdateTime << '\n';
	}

	//##FIX## We just moved the following block from the end of NotifyAfterExecuteCalled()
	//to fix a problem with an external device calling into the VDP while it is stepping
	//through a timing point, but this has changed the order of how things are done when
	//performing a commit. Previously, this code was always run before ExecuteCommit() was
	//called, but now ExecuteCommit() is always called before this is run. I believe this
	//will break things, but we need to check the code to see. We also know this has
	//introduced issues with savestate support. Perhaps we should issue a zero-sized
	//timeslice to each device before stepping through a device? Or maybe we should
	//reverse the order of the device step and the call to NotifyAfterExecuteCalled() when
	//a device needs to be stepped through.
	//-I would recommend reversing the order of NotifyAfterExecuteCalled() and the device
	//step. The idea of a device step is just to step that device once more at the end of
	//a timeslice, so it's not really a new timeslice, and the execute phase isn't really
	//over until that step has occurred. This code should be moved back into the
	//NotifyAfterExecuteCalled() function, and the system execution function modified to
	//ensure that the device step is always called before this function.
	//-Note that we just moved this code back, and fixed the system code.
	//####################################################################################

	//Record any unused mclk cycles from this timeslice, so we can pass them over into the
	//next timeslice.
	stateLastUpdateMclkUnusedFromLastTimeslice += stateLastUpdateMclkUnused;

	//If a DMA transfer is in progress, calculate the number of mclk cycles which have
	//already been consumed out of the delay time before the next external bus read
	//operation, and reset the next read time to 0. This will allow the result of the DMA
	//read operation to be processed at the correct mclk cycle in the next timeslice.
	if(dmaTransferActive)
	{
		//Calculate the number of mclk wait cycles that have already been served out of
		//the delay before the next read operation is complete.
		unsigned int dmaTransferNextReadCompleteTime = (dmaTransferNextReadMclk + (dmaTransferReadTimeInMclkCycles - dmaTransferLastTimesliceUsedReadDelay));
		if(dmaTransferNextReadCompleteTime > GetProcessorStateMclkCurrent())
		{
			//Calculate the number of mclk cycles that have already been advanced from the
			//point at which the DMA read operation began, to the point we're up to now.
			//Note that this is safe, as dmaTransferNextReadMclk is never greater than the
			//current state time. Also note that we add the calculated value together with
			//any existing value in this variable, so that extremely small timeslices that
			//don't consume all the used read delay from the last timeslice will roll any
			//remaining used delay into the next timeslice.
			dmaTransferLastTimesliceUsedReadDelay += GetProcessorStateMclkCurrent() - dmaTransferNextReadMclk;
		}
		else
		{
			//Since we've already advanced up to or past the point at which the next DMA
			//read operation would have completed, record that the entire read delay time
			//has been consumed.
			dmaTransferLastTimesliceUsedReadDelay = dmaTransferReadTimeInMclkCycles;
		}

		//Reset the next DMA transfer read time to 0. The used read delay, calculated
		//above, will be used to preserve the relative displacement of this value from the
		//current processor state time into the next timeslice.
		dmaTransferNextReadMclk = 0;
	}

	//Record the final position we advanced to in this timeslice. Note that this may be
	//past the end of the timeslice itself, as other devices may access the VDP before or
	//at the end of the timeslice, but may be stalled until a later point in time after
	//the end of the timeslice. This is compensated for by beginning the next timeslice
	//with an offset into that timeslice period.
	timesliceRenderInfoListUncommitted.rbegin()->timesliceEndPosition = GetProcessorStateMclkCurrent();
}

//----------------------------------------------------------------------------------------
void S315_5313::ExecuteTimeslice(double nanoseconds)
{
	//Obtain a lock on the accessMutex, to prevent port access interfering with this
	//operation.
	boost::mutex::scoped_lock lock(accessMutex);

	//##DEBUG##
	if(outputTimingDebugMessages)
	{
		std::wcout << "VDP - ExecuteTimeslice: " << nanoseconds << '\t' << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << stateLastUpdateMclkUnused << '\t' << nextEventToExecute.event << '\t' << nextEventToExecute.hcounter << '\t' << nextEventToExecute.vcounter << '\t' << nextEventToExecute.mclkCycleCounter << '\t' << std::setprecision(16) << nextEventToExecuteTime << '\n';
	}

	//Load the initial screen mode settings for the event search
	bool eventOddFlagSet = GetStatusFlagOddInterlaceFrame();
	bool eventInterlaceEnabled = interlaceEnabled;
	bool eventInterlaceDouble = interlaceDouble;
	bool eventPalMode = palMode;
	bool eventScreenModeV30 = screenModeV30;
	bool eventScreenModeRS0 = screenModeRS0;
	bool eventScreenModeRS1 = screenModeRS1;

	//While an event is pending from this timeslice, process it.
	EventProperties nextEvent;
	GetNextEvent(lastTimesliceMclkCyclesOverrun, false, hintCounter, hcounter.GetData(), vcounter.GetData(), nextEvent, eventOddFlagSet, eventInterlaceEnabled, eventInterlaceDouble, eventPalMode, eventScreenModeV30, eventScreenModeRS0, eventScreenModeRS1, interlaceEnabledCached, interlaceDoubleCached, palModeLineState, screenModeV30Cached, screenModeRS0Cached, screenModeRS1Cached);
	while((nextEvent.mclkCycleCounter <= currentTimesliceTotalMclkCycles) && (nextEvent.mclkCycleCounter < nextEventToExecute.mclkCycleCounter))
	{
		//Calculate the time of this event. Note that we subtract the
		//lastTimesliceMclkCyclesRemainingTime variable here, while we add this value on
		//port access. This is because here we want to convert an internal time value
		//which is relative to the device, into an external time relative to the system,
		//which is the opposite of what we do on port access.
		double eventTime = ConvertMclkCountToAccessTime(nextEvent.mclkCycleCounter) - lastTimesliceMclkCyclesRemainingTime;

		//##DEBUG##
		//std::wcout << "NextEvent: " << eventMclkCounter << '\t' << nextEvent.event << '\t' << nextEvent.mclkCycleCounter << '\t' << std::setprecision(16) << eventTime << '\t' << std::setprecision(16) << nanoseconds << '\n';

		//Execute the event
		ExecuteEvent(nextEvent, eventTime, nextEvent.hcounter, nextEvent.vcounter, screenModeRS0, screenModeRS1, screenModeV30, palMode, interlaceEnabled);

		//Find the next event
		GetNextEvent(nextEvent.mclkCycleCounter, false, hintCounter, nextEvent.hcounter, nextEvent.vcounter, nextEvent, eventOddFlagSet, eventInterlaceEnabled, eventInterlaceDouble, eventPalMode, eventScreenModeV30, eventScreenModeRS0, eventScreenModeRS1, interlaceEnabledCached, interlaceDoubleCached, palModeLineState, screenModeV30Cached, screenModeRS0Cached, screenModeRS1Cached);
	}

	//Release the block on port access now that events have been processed. This locking
	//mechanism is in place because we need to make sure that the device state remains
	//fixed at the beginning of the timeslice until all events have been processed, in
	//order to ensure that we can accurately calculate the time when the events occur.
	eventsProcessedForTimeslice = true;
	eventProcessingCompleted.notify_all();

	//If the render thread is lagging, pause here until it has caught up, so we don't
	//leave the render thread behind with an ever-increasing workload it will never be
	//able to complete.
	if(renderThreadLagging)
	{
		boost::mutex::scoped_lock lock(timesliceMutex);
		while(renderThreadLagging)
		{
			renderThreadLaggingStateChange.wait(lock);
		}
	}
}

//----------------------------------------------------------------------------------------
//##FIX## A timing point can occur when the VDP accesses other devices, in particular,
//when the VDP asserts the BR line. Currently, this function is called when this occurs,
//however we have no way of determining whether we got here because we triggered a
//rollback from external access, or if we actually reached the target timeslice. We need
//to implement a system where we can handle unexpected timing points generated by this
//core, even when the timing points are raised by another device.
void S315_5313::ExecuteTimesliceTimingPointStep(unsigned int accessContext)
{
	//Ensure that the DMA worker thread has finished executing
	boost::mutex::scoped_lock lock(workerThreadMutex);
	if(workerThreadActive && !workerThreadPaused && busGranted)
	{
		//##DEBUG##
		//			std::wcout << L"ExecuteTimeslice is on a timing point waiting for DMA worker thread to pause\n";
		//			std::wcout << '\t' << workerThreadActive << '\t' << workerThreadPaused << '\t' << busGranted << '\n';

		workerThreadIdle.wait(lock);
	}

	//Ensure the VDP is advanced up to the end of its timeslice
	if((currentTimesliceTotalMclkCycles > 0) && (currentTimesliceTotalMclkCycles > GetProcessorStateMclkCurrent()))
	{
		UpdateInternalState(currentTimesliceTotalMclkCycles, false, false, false, false, false, false, false);
	}

	//##DEBUG##
	if(accessContext == ACCESSCONTEXT_TIMINGPOINT)
	{
		if(outputTimingDebugMessages)
		{
			std::wcout << "VDP - ExecuteOnTimingPoint: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << stateLastUpdateMclkUnused << '\t' << nextEventToExecute.event << '\t' << nextEventToExecute.hcounter << '\t' << nextEventToExecute.vcounter << '\t' << nextEventToExecute.mclkCycleCounter << '\t' << std::setprecision(16) << nextEventToExecuteTime << '\n';
		}
	}

	//Set the time at which this event is occurring. With recent changes to the way the
	//system handles timeslices, this is simply 0, since every device will receive a new
	//timeslice to handle this timing point.
	double eventTime = 0.0;

	//Execute the event
	if(eventSnapshotSaved)
	{
		ExecuteEvent(nextEventToExecute, eventTime, nextEventToExecute.hcounter, nextEventToExecute.vcounter, eventSnapshotScreenModeRS0, eventSnapshotScreenModeRS1, eventSnapshotScreenModeV30, eventSnapshotPalMode, eventSnapshotInterlaceEnabled);
		eventSnapshotSaved = false;
	}
	else if((hcounter.GetData() == nextEventToExecute.hcounter) && (vcounter.GetData() == nextEventToExecute.vcounter))
	{
		ExecuteEvent(nextEventToExecute, eventTime, nextEventToExecute.hcounter, nextEventToExecute.vcounter, screenModeRS0, screenModeRS1, screenModeV30, palMode, interlaceEnabled);
	}
	else if(accessContext == ACCESSCONTEXT_TIMINGPOINT)
	{
		//##FIX## We have a major problem with our timing system. Timing points can be
		//missed, because a calling device may access the VDP just before the end of
		//the current timeslice, with the VDP having a timing point at the end of the
		//timeslice, and the call itself may need to be blocked for a period of time
		//that moves past the timing point. In our current execution model, we will
		//allow the call to proceed, and advance the VDP to the end of that call, even
		//if the call passes the end of the current timeslice. This works fine for
		//devices which can only be accessed by a single device at a time, however, it
		//creates a problem with our timing points, because the VDP is essentially
		//multi-threaded, and while the calling device would be stalled at the time
		//the timing point occurs, that timing point should still execute on time.
		//##TODO## What we need to do to fix our update dilemma, is detect the case
		//where we would skip a timing point due to an access call, and during the
		//update process, stop at that timing point, and snapshot all the state data
		//we need to capture at that point in order to execute the timing point. With
		//that state data captured, we then proceed as normal, but when the timing
		//point is executed here, we should retrieve the saved state information we
		//need for that event, and process it using the saved state information. This
		//will solve our threading problem perfectly. Note that we should still keep a
		//debug check like the one below for the case where no event information was
		//latched and saved. This could occur if we actually failed to reach a timing
		//point, rather than advancing past it.
		//##DEBUG##
		std::wcout << "######################################################\n";
		std::wcout << "VDP Didn't reach timing point!!:\n";
		std::wcout << "           Actual\tTarget\n";
		std::wcout << "HCounter   " << hcounter.GetData() << "\t\t" << nextEventToExecute.hcounter << '\n';
		std::wcout << "VCounter   " << vcounter.GetData() << "\t\t" << nextEventToExecute.vcounter << '\n';
		std::wcout << currentTimesliceTotalMclkCycles << "\t" << stateLastUpdateMclk << "\t" << stateLastUpdateMclkUnused << "\t" << stateLastUpdateMclkUnusedFromLastTimeslice << "\n";
		std::wcout << "######################################################\n";
	}
}

////----------------------------------------------------------------------------------------
//void S315_5313::GetNextEvent(unsigned int currentMclkCycleCounter, bool timingPointsOnly, unsigned int currentHIntCounter, unsigned int currentPosHCounter, unsigned int currentPosVCounter, EventProperties& nextEvent) const
//{
//	//Adjust the cycle count to factor in unused state mclk update cycles
//	//##NOTE## We've shifted responsibility for this to the caller
//	currentMclkCycleCounter += stateLastUpdateMclkUnusedFromLastTimeslice + stateLastUpdateMclkUnused;
//
//	//Obtain information on the current processor state
//	const HScanSettings& hscanSettings = GetHScanSettings(screenModeRS0, screenModeRS1);
//	const VScanSettings& vscanSettings = GetVScanSettings(screenModeV30, palMode, interlaceEnabled);
//
//	//Start a search for the next event
//	bool skippedEvent = false;
//	unsigned int skippedMclkCycles = 0;
//	do
//	{
//		Event nextEventToOccur;
//		unsigned int eventPosHCounter;
//		unsigned int eventPosVCounter;
//		bool eventGeneratesTimingPoint;
//
//		bool oddFlagSet = GetStatusFlagOddInterlaceFrame();
//
//		//Start with hint counter advance as the next event
//		unsigned int hintCounterAdvanceEventPosHCounter = hscanSettings.hintTriggerPoint;
//		unsigned int hintCounterAdvanceEventPosVCounter = currentPosVCounter;
//		if((currentPosHCounter >= hscanSettings.hintTriggerPoint) && (currentPosHCounter < hscanSettings.vcounterIncrementPoint))
//		{
//			//The hint trigger point is before the vcounter increment point on a line. If
//			//the current hcounter position indicates that the hint trigger point will not
//			//be hit until the next vcounter value, calculate the incremented vcounter
//			//value where the next hint trigger point will occur.
//			hintCounterAdvanceEventPosVCounter = AddStepsToVCounter(hscanSettings, currentPosHCounter, vscanSettings, interlaceEnabled, oddFlagSet, currentPosVCounter, 1);
//		}
//		nextEventToOccur = NEXTUPDATESTEP_HINTCOUNTERADVANCE;
//		eventPosHCounter = hintCounterAdvanceEventPosHCounter;
//		eventPosVCounter = hintCounterAdvanceEventPosVCounter;
//
//		//Determine if we should generate a timing point for this hint counter advance
//		//step
//		if(hintCounterAdvanceEventPosVCounter == vscanSettings.vcounterMaxValue)
//		{
//			//Generate a timing point if the hcounter is about to be reloaded for the last
//			//time before the beginning of active scan, when the hint counter will start
//			//being decremented.
//			eventGeneratesTimingPoint = true;
//		}
//		else if(hintEnabled && (currentHIntCounter == 0) && (hintCounterAdvanceEventPosVCounter < vscanSettings.vblankSetPoint))
//		{
//			//Generate a timing point here if hint is enabled, the counter is going to
//			//expire if it is advanced one more slot, and the line on which the update
//			//will occur is in the active scan area.
//			eventGeneratesTimingPoint = true;
//		}
//		else
//		{
//			eventGeneratesTimingPoint = false;
//		}
//
//		//Check if VINT is going to occur before the earliest event found
//		unsigned int vintEventPosHCounter = hscanSettings.fflagSetPoint;
//		unsigned int vintEventPosVCounter = vscanSettings.vblankSetPoint;
//		if(((vintEventPosHCounter != currentPosHCounter) || (vintEventPosVCounter != currentPosVCounter))
//		&& EventOccursWithinCounterRange(hscanSettings, currentPosHCounter, currentPosVCounter, eventPosHCounter, eventPosVCounter, vintEventPosHCounter, vintEventPosVCounter))
//		{
//			//VINT is the next event
//			nextEventToOccur = NEXTUPDATESTEP_VINT;
//			eventPosHCounter = vintEventPosHCounter;
//			eventPosVCounter = vintEventPosVCounter;
//			eventGeneratesTimingPoint = true;
//		}
//
//		////Check if vsync is going to occur before the earliest event found
//		//unsigned int vsyncEventPosHCounter = hscanSettings.vcounterIncrementPoint;
//		//unsigned int vsyncEventPosVCounter = vscanSettings.vsyncAssertedPoint;
//		//if(((vsyncEventPosHCounter != currentPosHCounter) || (vsyncEventPosVCounter != currentPosVCounter))
//		//&& EventOccursWithinCounterRange(hscanSettings, currentPosHCounter, currentPosVCounter, eventPosHCounter, eventPosVCounter, vsyncEventPosHCounter, vsyncEventPosVCounter))
//		//{
//		//	//VSYNC is the next event
//		//	nextEventToOccur = NEXTUPDATESTEP_VSYNC;
//		//	eventPosHCounter = vsyncEventPosHCounter;
//		//	eventPosVCounter = vsyncEventPosVCounter;
//		//	eventGeneratesTimingPoint = true;
//		//}
//
//		//Calculate the number of pixel clock steps between now and the target event
//		unsigned int pixelClockStepsUntilNextEvent = 0;
//		pixelClockStepsUntilNextEvent = GetPixelClockStepsBetweenHVCounterValues(hscanSettings, currentPosHCounter, eventPosHCounter, vscanSettings, interlaceEnabled, oddFlagSet, currentPosVCounter, eventPosVCounter);
//
//		//Calculate the number of mclk cycles which correspond to the number of pixel
//		//clock cycles until the next event.
//		unsigned int mclkStepsUntilNextEvent = GetMclkTicksForPixelClockTicks(hscanSettings, pixelClockStepsUntilNextEvent, currentPosHCounter, screenModeRS0, screenModeRS1);
//
//		//Calculate the final mclk cycle counter value when the target event is reached
//		unsigned int mclkCounterAtNextEvent = currentMclkCycleCounter + mclkStepsUntilNextEvent + skippedMclkCycles;
//
//		//If the next event we've found occurs before the start position for the event
//		//search, or we're looking for timing point events and the next event doesn't
//		//generate a timing point, skip it and look for the next event after this one.
//		if((mclkCounterAtNextEvent < currentMclkCycleCounter) || (timingPointsOnly && !eventGeneratesTimingPoint))
//		{
//			//If we're about to skip a hintcounter advance step, advance the hint counter
//			//first, so we can correctly determine when the counter expires.
//			if((nextEventToOccur == NEXTUPDATESTEP_HINTCOUNTERADVANCE) && (mclkCounterAtNextEvent > currentMclkCycleCounter))
//			{
//				if((eventPosVCounter >= vscanSettings.vblankSetPoint) || (currentHIntCounter == 0))
//				{
//					currentHIntCounter = hintCounterReloadValue;
//				}
//				else
//				{
//					--currentHIntCounter;
//				}
//			}
//			skippedEvent = true;
//			currentPosHCounter = eventPosHCounter;
//			currentPosVCounter = eventPosVCounter;
//			skippedMclkCycles += mclkStepsUntilNextEvent;
//			continue;
//		}
//
//		//Return the event info
//		skippedEvent = false;
//		nextEvent.event = nextEventToOccur;
//		nextEvent.mclkCycleCounter = mclkCounterAtNextEvent;
//		nextEvent.hcounter = eventPosHCounter;
//		nextEvent.vcounter = eventPosVCounter;
//
//		//Adjust the cycle count to factor in unused state mclk update cycles
//		nextEvent.mclkCycleCounter -= stateLastUpdateMclkUnusedFromLastTimeslice + stateLastUpdateMclkUnused;
//
//		//##DEBUG##
////		if(timingPointsOnly && !skippedEvent)
////		{
////			std::wcout << "VDP - GetNextEvent: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << stateLastUpdateMclkUnused << '\t' << eventPosHCounter << '\t' << eventPosVCounter << '\n';
////		}
//	}
//	while(skippedEvent);
//}

//----------------------------------------------------------------------------------------
//##FIX## We need to toggle eventOddFlagSet at the appropriate time. Right now, this value
//is never modified.
void S315_5313::GetNextEvent(unsigned int currentMclkCycleCounter, bool timingPointsOnly, unsigned int currentHIntCounter, unsigned int currentPosHCounter, unsigned int currentPosVCounter, EventProperties& nextEvent, bool& eventOddFlagSet, bool& eventInterlaceEnabled, bool& eventInterlaceDouble, bool& eventPalMode, bool& eventScreenModeV30, bool& eventScreenModeRS0, bool& eventScreenModeRS1, bool eventInterlaceEnabledNew, bool eventInterlaceDoubleNew, bool eventPalModeNew, bool eventScreenModeV30New, bool eventScreenModeRS0New, bool eventScreenModeRS1New) const
{
	//Adjust the cycle count to factor in unused state mclk update cycles
	//##TODO## Update the comment below
	//##NOTE## We've shifted responsibility for this to the caller
	currentMclkCycleCounter += stateLastUpdateMclkUnusedFromLastTimeslice + stateLastUpdateMclkUnused;

	//Check whether any of the relevant video mode settings have changed since they were
	//latched.
	bool hscanSettingsChanged = (eventScreenModeRS0 != eventScreenModeRS0New) || (eventScreenModeRS1 != eventScreenModeRS1New);
	bool vscanSettingsChanged = (eventScreenModeV30 != eventScreenModeV30New) || (eventPalMode != eventPalModeNew) || (eventInterlaceEnabled != eventInterlaceEnabledNew);

	//Start a search for the next event
	bool skippedEvent = false;
	unsigned int skippedMclkCycles = 0;
	do
	{
		//Obtain the current hscan and vscan settings
		//##TODO## Only latch these at the start of the search, and when they change, to
		//improve performance.
		const HScanSettings& hscanSettings = GetHScanSettings(eventScreenModeRS0, eventScreenModeRS1);
		const VScanSettings& vscanSettings = GetVScanSettings(eventScreenModeV30, eventPalMode, eventInterlaceEnabled);

		Event nextEventToOccur;
		unsigned int eventPosHCounter;
		unsigned int eventPosVCounter;
		bool eventGeneratesTimingPoint;
		bool forceSkipEvent;

		//Start with hint counter advance as the next event
		unsigned int hintCounterAdvanceEventPosHCounter = hscanSettings.hintTriggerPoint;
		unsigned int hintCounterAdvanceEventPosVCounter = currentPosVCounter;
		if((currentPosHCounter >= hscanSettings.hintTriggerPoint) && (currentPosHCounter < hscanSettings.vcounterIncrementPoint))
		{
			//The hint trigger point is before the vcounter increment point on a line. If
			//the current hcounter position indicates that the hint trigger point will not
			//be hit until the next vcounter value, calculate the incremented vcounter
			//value where the next hint trigger point will occur.
			hintCounterAdvanceEventPosVCounter = AddStepsToVCounter(hscanSettings, currentPosHCounter, vscanSettings, eventInterlaceEnabled, eventOddFlagSet, currentPosVCounter, 1);
		}
		nextEventToOccur = NEXTUPDATESTEP_HINTCOUNTERADVANCE;
		eventPosHCounter = hintCounterAdvanceEventPosHCounter;
		eventPosVCounter = hintCounterAdvanceEventPosVCounter;
		forceSkipEvent = false;

		//Determine if we should generate a timing point for this hint counter advance
		//step
		if(hintCounterAdvanceEventPosVCounter == vscanSettings.vcounterMaxValue)
		{
			//Generate a timing point if the hcounter is about to be reloaded for the last
			//time before the beginning of active scan, when the hint counter will start
			//being decremented.
			eventGeneratesTimingPoint = true;
		}
		else if(hintEnabled && (currentHIntCounter == 0) && (hintCounterAdvanceEventPosVCounter < vscanSettings.vblankSetPoint))
		{
			//Generate a timing point here if hint is enabled, the counter is going to
			//expire if it is advanced one more slot, and the line on which the update
			//will occur is in the active scan area.
			eventGeneratesTimingPoint = true;
		}
		else
		{
			eventGeneratesTimingPoint = false;
		}

		//##TODO## Comment this
		if(hscanSettingsChanged)
		{
			unsigned int hblankEventPosHCounter = hscanSettings.hblankSetPoint;
			unsigned int hblankEventPosVCounter = currentPosVCounter;
			if(currentPosHCounter >= hscanSettings.hblankSetPoint)
			{
				//The hint trigger point is after the vcounter increment point on a line.
				//If the current hcounter position indicates that the hint trigger point
				//will not be hit until the next vcounter value, calculate the incremented
				//vcounter value where the next hint trigger point will occur.
				hblankEventPosVCounter = AddStepsToVCounter(hscanSettings, currentPosHCounter, vscanSettings, eventInterlaceEnabled, eventOddFlagSet, currentPosVCounter, 1);
			}

			if(((hblankEventPosHCounter != currentPosHCounter) || (hblankEventPosVCounter != currentPosVCounter))
			   && EventOccursWithinCounterRange(hscanSettings, currentPosHCounter, currentPosVCounter, eventPosHCounter, eventPosVCounter, hblankEventPosHCounter, hblankEventPosVCounter))
			{
				nextEventToOccur = NEXTUPDATESTEP_HBLANK;
				eventPosHCounter = hblankEventPosHCounter;
				eventPosVCounter = hblankEventPosVCounter;
				eventGeneratesTimingPoint = false;
				forceSkipEvent = true;
			}
		}

		//##TODO## Comment this
		if(vscanSettingsChanged)
		{
			unsigned int vblankEventPosHCounter = hscanSettings.vcounterIncrementPoint;
			unsigned int vblankEventPosVCounter = vscanSettings.vblankSetPoint;
			if(((vblankEventPosHCounter != currentPosHCounter) || (vblankEventPosVCounter != currentPosVCounter))
			   && EventOccursWithinCounterRange(hscanSettings, currentPosHCounter, currentPosVCounter, eventPosHCounter, eventPosVCounter, vblankEventPosHCounter, vblankEventPosVCounter))
			{
				nextEventToOccur = NEXTUPDATESTEP_VBLANK;
				eventPosHCounter = vblankEventPosHCounter;
				eventPosVCounter = vblankEventPosVCounter;
				eventGeneratesTimingPoint = false;
				forceSkipEvent = true;
			}
		}

		//Check if VINT is going to occur before the earliest event found
		unsigned int vintEventPosHCounter = hscanSettings.fflagSetPoint;
		unsigned int vintEventPosVCounter = vscanSettings.vblankSetPoint;
		if(((vintEventPosHCounter != currentPosHCounter) || (vintEventPosVCounter != currentPosVCounter))
		   && EventOccursWithinCounterRange(hscanSettings, currentPosHCounter, currentPosVCounter, eventPosHCounter, eventPosVCounter, vintEventPosHCounter, vintEventPosVCounter))
		{
			//VINT is the next event
			nextEventToOccur = NEXTUPDATESTEP_VINT;
			eventPosHCounter = vintEventPosHCounter;
			eventPosVCounter = vintEventPosVCounter;
			eventGeneratesTimingPoint = true;
			forceSkipEvent = false;
		}

		////Check if vsync is going to occur before the earliest event found
		//unsigned int vsyncEventPosHCounter = hscanSettings.vcounterIncrementPoint;
		//unsigned int vsyncEventPosVCounter = vscanSettings.vsyncAssertedPoint;
		//if(((vsyncEventPosHCounter != currentPosHCounter) || (vsyncEventPosVCounter != currentPosVCounter))
		//&& EventOccursWithinCounterRange(hscanSettings, currentPosHCounter, currentPosVCounter, eventPosHCounter, eventPosVCounter, vsyncEventPosHCounter, vsyncEventPosVCounter))
		//{
		//	//VSYNC is the next event
		//	nextEventToOccur = NEXTUPDATESTEP_VSYNC;
		//	eventPosHCounter = vsyncEventPosHCounter;
		//	eventPosVCounter = vsyncEventPosVCounter;
		//	eventGeneratesTimingPoint = true;
		//	forceSkipEvent = false;
		//}

		//Calculate the number of pixel clock steps between now and the target event
		unsigned int pixelClockStepsUntilNextEvent = 0;
		pixelClockStepsUntilNextEvent = GetPixelClockStepsBetweenHVCounterValues(hscanSettings, currentPosHCounter, eventPosHCounter, vscanSettings, eventInterlaceEnabled, eventOddFlagSet, currentPosVCounter, eventPosVCounter);

		//Calculate the number of mclk cycles which correspond to the number of pixel
		//clock cycles until the next event.
		unsigned int mclkStepsUntilNextEvent = GetMclkTicksForPixelClockTicks(hscanSettings, pixelClockStepsUntilNextEvent, currentPosHCounter, eventScreenModeRS0, eventScreenModeRS1);

		//Calculate the final mclk cycle counter value when the target event is reached
		unsigned int mclkCounterAtNextEvent = currentMclkCycleCounter + mclkStepsUntilNextEvent + skippedMclkCycles;

		//If the next event we've found occurs before the start position for the event
		//search, or we're looking for timing point events and the next event doesn't
		//generate a timing point, or this event is marked to always be skipped, skip
		//it and look for the next event after this one.
		if((mclkCounterAtNextEvent < currentMclkCycleCounter) || (timingPointsOnly && !eventGeneratesTimingPoint) || forceSkipEvent)
		{
			//Determine if any special processing is required for this skipped event
			if((nextEventToOccur == NEXTUPDATESTEP_HINTCOUNTERADVANCE) && (mclkCounterAtNextEvent > currentMclkCycleCounter))
			{
				//If we're about to skip a hintcounter advance step, advance the hint counter
				//first, so we can correctly determine when the counter expires.
				if((eventPosVCounter >= vscanSettings.vblankSetPoint) || (currentHIntCounter == 0))
				{
					currentHIntCounter = hintCounterReloadValue;
				}
				else
				{
					--currentHIntCounter;
				}
			}
			else if(hscanSettingsChanged && (nextEventToOccur == NEXTUPDATESTEP_HBLANK))
			{
				//If vertical scan information has changed, and we've just advanced to vblank,
				//latch the new screen mode settings.

				//If the interlace mode has changed, the new setting is latched when the
				//vblank set event occurs. This has been verified in all video modes through
				//hardware tests.
				eventInterlaceEnabled = eventInterlaceEnabledNew;
				eventInterlaceDouble = eventInterlaceDoubleNew;
				//##TODO## Verify that changes to the PAL line state are latched at vblank
				eventPalMode = eventPalModeNew;
				//##TODO## Verify that the V28/V30 mode change is latched at vblank
				eventScreenModeV30 = eventScreenModeV30New;

				//Now that we've processed this screen mode settings change, flag that no
				//settings change is required.
				vscanSettingsChanged = false;
			}
			else if(vscanSettingsChanged && (nextEventToOccur == NEXTUPDATESTEP_VBLANK))
			{
				//If horizontal scan information has changed, and we've just advanced to hblank,
				//latch the new screen mode settings.

				//##FIX## These settings changes are supposed to take effect immediately
				eventScreenModeRS0 = eventScreenModeRS0New;
				eventScreenModeRS1 = eventScreenModeRS1New;

				//Now that we've processed this screen mode settings change, flag that no
				//settings change is required.
				hscanSettingsChanged = false;
			}

			//Flag that we've skipped the next event, and add the number of mclk cycles
			//consumed by this event to the skipped mclk cycle count.
			skippedEvent = true;
			skippedMclkCycles += mclkStepsUntilNextEvent;

			//Advance the current HV counters to the event position. Note that we use the
			//function below to perform this task rather than simply assigning the new
			//HV counter position, as this function handles the odd flag toggle and extra
			//scanline that occurs when interlacing is active.
			AdvanceHVCounters(hscanSettings, currentPosHCounter, vscanSettings, eventInterlaceEnabled, eventOddFlagSet, currentPosVCounter, pixelClockStepsUntilNextEvent);

			//Since this event has been skipped, begin the event search again.
			continue;
		}

		//Return the event info
		skippedEvent = false;
		nextEvent.event = nextEventToOccur;
		nextEvent.mclkCycleCounter = mclkCounterAtNextEvent;
		nextEvent.hcounter = eventPosHCounter;
		nextEvent.vcounter = eventPosVCounter;

		//Adjust the cycle count to factor in unused state mclk update cycles
		nextEvent.mclkCycleCounter -= stateLastUpdateMclkUnusedFromLastTimeslice + stateLastUpdateMclkUnused;

//##DEBUG##
//		if(timingPointsOnly && !skippedEvent)
//		{
//			std::wcout << "VDP - GetNextEvent: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << stateLastUpdateMclkUnused << '\t' << eventPosHCounter << '\t' << eventPosVCounter << '\n';
//		}
	}
	while(skippedEvent);
}

//----------------------------------------------------------------------------------------
//##TODO## Refactor this to make it more readable, and shift it into the general timing
//calculation code block, separating it from event processing.
bool S315_5313::EventOccursWithinCounterRange(const HScanSettings& hscanSettings, unsigned int hcounterStart, unsigned int vcounterStart, unsigned int hcounterEnd, unsigned int vcounterEnd, unsigned int hcounterEventPos, unsigned int vcounterEventPos)
{
	return (((vcounterStart < vcounterEventPos)
		  || ((vcounterStart == vcounterEventPos)
		   && (((hcounterStart < hscanSettings.vcounterIncrementPoint)
		     && (hcounterEventPos < hscanSettings.vcounterIncrementPoint)
			 && (hcounterStart <= hcounterEventPos))
			|| ((hcounterStart >= hscanSettings.vcounterIncrementPoint)
			 && (hcounterEventPos >= hscanSettings.vcounterIncrementPoint)
			 && (hcounterStart <= hcounterEventPos))
			|| ((hcounterStart >= hscanSettings.vcounterIncrementPoint)
			 && (hcounterEventPos < hscanSettings.vcounterIncrementPoint)
			)))) //The target event occurs at or after the start position
		 && ((vcounterEnd > vcounterEventPos)
		  || ((vcounterEnd == vcounterEventPos)
		   && (((hcounterEnd < hscanSettings.vcounterIncrementPoint)
		     && (hcounterEventPos < hscanSettings.vcounterIncrementPoint))
			 && (hcounterEnd >= hcounterEventPos)
		    || ((hcounterEnd >= hscanSettings.vcounterIncrementPoint)
			 && (hcounterEventPos >= hscanSettings.vcounterIncrementPoint))
			 && (hcounterEnd >= hcounterEventPos)
			|| ((hcounterEventPos >= hscanSettings.vcounterIncrementPoint)
			 && (hcounterEnd < hscanSettings.vcounterIncrementPoint)
		   ))))); //The target event occurs at or after the end position
}

//----------------------------------------------------------------------------------------
void S315_5313::ExecuteEvent(EventProperties eventProperties, double accessTime, unsigned int ahcounter, unsigned int avcounter, bool ascreenModeRS0, bool ascreenModeRS1, bool ascreenModeV30, bool apalMode, bool ainterlaceEnabled)
{
	//Assumed constants
	//##TODO## Shift this into HScanSettings or the like
	//##FIX## We no longer want the number of mclk cycles per line to be assumed. We know
	//that it's possible for the number of mclk cycles per line to differ to the 3420
	//count required for PAL/NTSC video encoding, in particular, when H40 mode is enabled
	//without enabling the external EDCLK input. We should estimate when an event will
	//occur, and predict the timing event occurring at that point, assuming the screen
	//mode settings don't change, and if they do change, generate a rollback.
	//##FIX## ...And looking some more, we only use this number to determine how long to
	//assert the Z80 INT line for.
	const unsigned int mclkCyclesPerLine = 3420;

	//Obtain information on the current processor state
//	const HScanSettings& hscanSettings = GetHScanSettings(ascreenModeRS0, ascreenModeRS1);
	const VScanSettings& vscanSettings = GetVScanSettings(ascreenModeV30, apalMode, ainterlaceEnabled);

	//Perform the next update step
	switch(eventProperties.event)
	{
	//Vertical interrupt
	case NEXTUPDATESTEP_VINT:{
		//Set the VINT occurrence flag for the status register. Note that this flag is set
		//even if vertical interrupts are disabled. If vertical interrupts are enabled,
		//this bit is cleared when the interrupt is acknowledged by the M68000, otherwise
		//this bit remains set from this point on, until an actual vertical interrupt is
		//generated and acknowledged by the M68000. This behaviour has been confirmed
		//through hardware tests.
		SetStatusFlagF(true);

		//Trigger the vertical interrupt for the M68000 if vertical interrupts are enabled
		vintPending = true;
		if(vintEnabled && vintPending)
		{
			//##DEBUG##
			//std::wcout << "VDP - VINT: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << eventProperties.hcounter << '\t' << eventProperties.vcounter << '\n';

			//Trigger VInt for M68000
			memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), vintIPLLineState), GetDeviceContext(), GetDeviceContext(), accessTime, ACCESSCONTEXT_VINT);
			vintPending = false;
		}

		//Trigger the vertical interrupt line for the Z80. Note that this line is always
		//asserted, regardless of the state of the vertical interrupt enable flag.
		//##NOTE## According to Eke, the Z80 INT line is asserted for one scanline, and is
		//cleared at the same point in the following line.
		double intLineAssertedLength = ConvertMclkCountToAccessTime(mclkCyclesPerLine);
		memoryBus->SetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 1), GetDeviceContext(), GetDeviceContext(), accessTime, ACCESSCONTEXT_INT_ASSERT);
		memoryBus->SetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 0), GetDeviceContext(), GetDeviceContext(), accessTime + intLineAssertedLength, ACCESSCONTEXT_INT_RELEASE);
		break;}
	//External interrupt
	case NEXTUPDATESTEP_EXINT:{
		//Latch the current hcounter and vcounter settings, if HV counter latching has
		//been enabled.
		//##TODO## Determine when the hvCounterLatched bit should be cleared. Is it
		//cleared after the HV counter is read?
		//##TODO## Confirm what happens with the latched HV data when the interlace mode
		//changes. Is it the internal value of the hcounter which is latched, or the
		//external value?
		//##TODO## Read the info from Eke at http://gendev.spritesmind.net/forum/viewtopic.php?t=787
		//It says that setting the HV counter latch bit will always cause the HV counter
		//to return a static value. It will be initialized to an unknown default value,
		//and will be updated when HL is asserted. Confirm whether toggling HV counter
		//latch from disabled to enabled latches the current counter value at the time it
		//is enabled. Also confirm whether latching a value, disabling latching, then
		//re-enabling latching returns the same previously latched value.
		//##TODO## Confirm whether the VDP always latches the HV counter when the HL input
		//line is asserted, but that value is only reported when the latch enable bit is
		//set. Try triggering a HL latch for a known value when HV counter latch is
		//disabled, then enable the HV counter latch, and see if the initial latch value
		//is the same as the masked time when the HL trigger occurred.
		if(hvCounterLatchEnabled)
		{
			hvCounterLatched = true;
			hcounterLatchedData = ahcounter;
			vcounterLatchedData = avcounter;
		}

		//Trigger the external interrupt for the M68000 if external interrupts are
		//enabled.
		exintPending = true;
		if(exintEnabled && exintPending)
		{
			//##DEBUG##
//			std::wcout << "VDP - EXINT: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\n';

			//Trigger EXInt for M68000
			memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), exintIPLLineState), GetDeviceContext(), GetDeviceContext(), accessTime, ACCESSCONTEXT_EXINT);
			exintPending = false;
		}
		break;}
	//VSync
	case NEXTUPDATESTEP_VSYNC:{
		//##TODO## This is old code, back when we rendered a frame at a time. In reality,
		//we probably do want to do something here still though, although in this case, it
		//should be performing a buffer swap or the like to support double-buffered
		//rendering. This should be optional however, and in particular, we should support
		//rendering to the front buffer when partway through a line when emulation is
		//paused, IE, sitting on a breakpoint.
		//Since we've reached vsync, we should now have a complete frame worth of data to
		//render. As such, flag a pending render operation to trigger rendering of the
		//completed frame.
		//pendingRenderOperation = true;
		break;}
	//Hint counter advance
	case NEXTUPDATESTEP_HINTCOUNTERADVANCE:{
		//##DEBUG##
		if(eventProperties.vcounter == 0x1FF)
		{
			//##DEBUG##
			//std::wcout << "VDP - Latching HCounter data for new frame: " << hintCounterReloadValue << '\n';
		}

		if(eventProperties.vcounter >= vscanSettings.vblankSetPoint)
		{
			//Latch the initial hintCounter value for the frame
			hintCounter = hintCounterReloadValue;
		}
		else if(hintCounter == 0)
		{
			//Reload the hint counter now that it has expired
			hintCounter = hintCounterReloadValue;

			//Trigger the horizontal interrupt for the M68000 if horizontal interrupts are
			//enabled.
			//##TODO## Clean up the old comment below
			//##FIX## According to tests performed by Eke, this implementation is wrong. What
			//actually happens is that an internal hint pending flag is set, which remains set
			//until an interrupt is both triggered and acknowledged. The interrupt is only
			//triggered when the interrupt enable bit is set, so in other words, as soon as
			//we enable horizontal interrupts on the VDP, a HINT is probably going to be
			//triggered. Note that the VINT implementation is identical, with a vint pending
			//flag held. We would need to test and confirm the behaviour of EXINT, but it most
			//likely works the same way. Also note that changes to the interrupt enable bits,
			//IE, setting it as cleared again when it is already cleared. What actually will
			//be happening in hardware is the pending interrupt flags always get set, and only
			//cleared when an interrupt is acknowledged. The VDP decides whether to assert an
			//interrupt line by taking the pending interrupt flag, and ANDing it with the
			//interrupt enable bit. We should follow this same implementation in our core.
			hintPending = true;
			if(hintEnabled && hintPending)
			{
				//##DEBUG##
				//std::wcout << "VDP - HINT: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << eventProperties.hcounter << '\t' << eventProperties.vcounter << '\t' << hintCounterReloadValue << '\n';

				//Trigger HInt for M68000
				memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), hintIPLLineState), GetDeviceContext(), GetDeviceContext(), accessTime, ACCESSCONTEXT_HINT);
				hintPending = false;
			}
		}
		else
		{
			--hintCounter;
		}
		break;}
	default:{
		//##TODO## Raise an assert here
		break;}
	}
}

//----------------------------------------------------------------------------------------
double S315_5313::GetNextTimingPointInDeviceTime(unsigned int& accessContext) const
{
	//Set the access context data for this timing point to a special value, so we can
	//differentiate between rollback events and timing point events.
	accessContext = ACCESSCONTEXT_TIMINGPOINT;

	//Load the initial screen mode settings for the event search
	bool eventOddFlagSet = GetStatusFlagOddInterlaceFrame();
	bool eventInterlaceEnabled = interlaceEnabled;
	bool eventInterlaceDouble = interlaceDouble;
	bool eventPalMode = palMode;
	bool eventScreenModeV30 = screenModeV30;
	bool eventScreenModeRS0 = screenModeRS0;
	bool eventScreenModeRS1 = screenModeRS1;

	//Determine the details of the next timing point event
	GetNextEvent(0, true, hintCounter, hcounter.GetData(), vcounter.GetData(), nextEventToExecute, eventOddFlagSet, eventInterlaceEnabled, eventInterlaceDouble, eventPalMode, eventScreenModeV30, eventScreenModeRS0, eventScreenModeRS1, interlaceEnabledCached, interlaceDoubleCached, palModeLineState, screenModeV30Cached, screenModeRS0Cached, screenModeRS1Cached);

	//This timing code is rather hard to understand from the code alone. The best way to
	//visualize it is with a diagram. Basically, all we're doing here is converting the
	//time of the next event from a time which is relative to the current VDP state, which
	//may be either before or after the end of the last timeslice, to a time which is
	//relative to the end of the last timeslice, which is what the system needs here.
	unsigned int mclkCyclesUntilNextEventInDeviceTime = nextEventToExecute.mclkCycleCounter;
	unsigned int mclkCyclesUntilNextEventFromLastTimesliceMclkCycleEnd = lastTimesliceMclkCyclesOverrun + mclkCyclesUntilNextEventInDeviceTime;
	double extraTimeBetweenLastTimesliceMclkCycleEndAndActualTimesliceEnd = currentTimesliceMclkCyclesRemainingTime;
	double timeUntilNextEventFromLastTimesliceMclkCycleEnd = ConvertMclkCountToAccessTime(mclkCyclesUntilNextEventFromLastTimesliceMclkCycleEnd);
	double timeFromEndOfLastTimesliceToNextEventInSystemTime = timeUntilNextEventFromLastTimesliceMclkCycleEnd - extraTimeBetweenLastTimesliceMclkCycleEndAndActualTimesliceEnd;
	nextEventToExecuteTime = timeFromEndOfLastTimesliceToNextEventInSystemTime;

	//##DEBUG##
	if(outputTimingDebugMessages)
	{
		std::wcout << "VDP - GetTimingPoint: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << lastTimesliceMclkCyclesOverrun << '\t' << currentTimesliceMclkCyclesRemainingTime << '\t' << nextEventToExecute.event << '\t' << nextEventToExecute.hcounter << '\t' << nextEventToExecute.vcounter << '\t' << nextEventToExecute.mclkCycleCounter << '\t' << std::setprecision(16) << nextEventToExecuteTime << '\n';
	}

	return nextEventToExecuteTime;
}

//----------------------------------------------------------------------------------------
void S315_5313::ExecuteRollback()
{
	//Update state
	currentTimesliceMclkCyclesRemainingTime = bcurrentTimesliceMclkCyclesRemainingTime;
	lastTimesliceMclkCyclesOverrun = blastTimesliceMclkCyclesOverrun;
	stateLastUpdateMclkUnusedFromLastTimeslice = bstateLastUpdateMclkUnusedFromLastTimeslice;

	//Bus interface
	busGranted = bbusGranted;
	palModeLineState = bpalModeLineState;

	//Clock sources
	clockMclkCurrent = bclockMclkCurrent;

	//Saved CE line state for Read-Modify-Write cycles
	lineLWRSavedStateRMW = blineLWRSavedStateRMW;
	lineUWRSavedStateRMW = blineUWRSavedStateRMW;
	lineCAS0SavedStateRMW = blineCAS0SavedStateRMW;
	lineRAS0SavedStateRMW = blineRAS0SavedStateRMW;
	lineOE0SavedStateRMW = blineOE0SavedStateRMW;

	//Physical registers and memory buffers
	reg.Rollback();
	vram->Rollback();
	cram->Rollback();
	vsram->Rollback();
	spriteCache->Rollback();
	status = bstatus;
	hcounter = bhcounter;
	vcounter = bvcounter;
	hcounterLatchedData = bhcounterLatchedData;
	vcounterLatchedData = bvcounterLatchedData;
	hvCounterLatched = bhvCounterLatched;
	hintCounter = bhintCounter;
	vintPending = bvintPending;
	hintPending = bhintPending;
	exintPending = bexintPending;
	readDataAvailable = breadDataAvailable;
	readDataHalfCached = breadDataHalfCached;
	dmaFillOperationRunning = bdmaFillOperationRunning;

	//Cached register settings
	hvCounterLatched = bhvCounterLatchEnabled;
	interlaceEnabled = binterlaceEnabled;
	interlaceDouble = binterlaceDouble;
	screenModeRS0 = bscreenModeRS0;
	screenModeRS1 = bscreenModeRS1;
	screenModeV30 = bscreenModeV30;
	palMode = bpalMode;
	vintEnabled = bvintEnabled;
	hintEnabled = bhintEnabled;
	exintEnabled = bexintEnabled;
	hintCounterReloadValue = bhintCounterReloadValue;
	dmaEnabled = bdmaEnabled;
	dmd0 = bdmd0;
	dmd1 = bdmd1;
	dmaLengthCounter = bdmaLengthCounter;
	dmaSourceAddressByte1 = bdmaSourceAddressByte1;
	dmaSourceAddressByte2 = bdmaSourceAddressByte2;
	dmaSourceAddressByte3 = bdmaSourceAddressByte3;
	autoIncrementData = bautoIncrementData;
	interlaceEnabledCached = binterlaceEnabledCached;
	interlaceDoubleCached = binterlaceDoubleCached;
	screenModeRS0Cached = bscreenModeRS0Cached;
	screenModeRS1Cached = bscreenModeRS1Cached;
	screenModeV30Cached = bscreenModeV30Cached;
	displayEnabledCached = bdisplayEnabledCached;
	spriteAttributeTableBaseAddressDecoded = bspriteAttributeTableBaseAddressDecoded;
	verticalScrollModeCached = bverticalScrollModeCached;

	//FIFO buffer registers
	for(unsigned int i = 0; i < fifoBufferSize; ++i)
	{
		fifoBuffer[i] = bfifoBuffer[i];
	}
	readBuffer = breadBuffer;
	fifoNextReadEntry = bfifoNextReadEntry;
	fifoNextWriteEntry = bfifoNextWriteEntry;

	//Control port registers
	codeAndAddressRegistersModifiedSinceLastWrite = bcodeAndAddressRegistersModifiedSinceLastWrite;
	commandWritePending = bcommandWritePending;
	originalCommandAddress = boriginalCommandAddress;
	commandAddress = bcommandAddress;
	commandCode = bcommandCode;

	//DMA worker thread properties
	workerThreadPaused = bworkerThreadPaused;

	//DMA transfer registers
	dmaTransferActive = bdmaTransferActive;
	dmaTransferReadDataCached = bdmaTransferReadDataCached;
	dmaTransferReadCache = bdmaTransferReadCache;
	dmaTransferNextReadMclk = bdmaTransferNextReadMclk;
	dmaTransferLastTimesliceUsedReadDelay = bdmaTransferLastTimesliceUsedReadDelay;
	dmaTransferInvalidPortWriteCached = bdmaTransferInvalidPortWriteCached;
	dmaTransferInvalidPortWriteAddressCache = bdmaTransferInvalidAddressCache;
	dmaTransferInvalidPortWriteDataCache = bdmaTransferInvalidDataCache;

	//Event handling
	nextEventToExecute = bnextEventToExecute;
	nextEventToExecuteTime = bnextEventToExecuteTime;
	eventSnapshotSaved = beventSnapshotSaved;
	eventSnapshotScreenModeRS0 = beventSnapshotScreenModeRS0;
	eventSnapshotScreenModeRS1 = beventSnapshotScreenModeRS1;
	eventSnapshotScreenModeV30 = beventSnapshotScreenModeV30;
	eventSnapshotPalMode = beventSnapshotPalMode;
	eventSnapshotInterlaceEnabled = beventSnapshotInterlaceEnabled;

	//Clear any uncommitted timeslices from our render timeslice buffers
	timesliceRenderInfoListUncommitted.clear();
	regTimesliceListUncommitted.clear();
	for(std::list<ITimedBufferInt::Timeslice*>::const_iterator i = vramTimesliceListUncommitted.begin(); i != vramTimesliceListUncommitted.end(); ++i)
	{
		vram->FreeTimesliceReference(*i);
	}
	vramTimesliceListUncommitted.clear();
	for(std::list<ITimedBufferInt::Timeslice*>::const_iterator i = cramTimesliceListUncommitted.begin(); i != cramTimesliceListUncommitted.end(); ++i)
	{
		cram->FreeTimesliceReference(*i);
	}
	cramTimesliceListUncommitted.clear();
	for(std::list<ITimedBufferInt::Timeslice*>::const_iterator i = vsramTimesliceListUncommitted.begin(); i != vsramTimesliceListUncommitted.end(); ++i)
	{
		vsram->FreeTimesliceReference(*i);
	}
	vsramTimesliceListUncommitted.clear();
	for(std::list<ITimedBufferInt::Timeslice*>::const_iterator i = spriteCacheTimesliceListUncommitted.begin(); i != spriteCacheTimesliceListUncommitted.end(); ++i)
	{
		spriteCache->FreeTimesliceReference(*i);
	}
	spriteCacheTimesliceListUncommitted.clear();

	//##DEBUG##
	if(outputTimingDebugMessages)
	{
		//Wait for the render thread to complete its work
		boost::mutex::scoped_lock lock(renderThreadMutex);
		while(pendingRenderOperationCount > 0)
		{
			renderThreadLaggingStateChange.wait(lock);
		}

		//Print out render thread synchronization info
		std::wcout << "VDP Synchronization - Rollback:\n"
		           << "-Digital:\t" << hcounter.GetData() << '\t' << vcounter.GetData() << '\n'
		           << "-Render:\t" << renderDigitalHCounterPos << '\t' << renderDigitalVCounterPos << '\n';
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::ExecuteCommit()
{
	//Update state
	bcurrentTimesliceMclkCyclesRemainingTime = currentTimesliceMclkCyclesRemainingTime;
	blastTimesliceMclkCyclesOverrun = lastTimesliceMclkCyclesOverrun;
	bstateLastUpdateMclkUnusedFromLastTimeslice = stateLastUpdateMclkUnusedFromLastTimeslice;

	//Bus interface
	bbusGranted = busGranted;
	bpalModeLineState = palModeLineState;

	//Clock sources
	bclockMclkCurrent = clockMclkCurrent;

	//Saved CE line state for Read-Modify-Write cycles
	blineLWRSavedStateRMW = lineLWRSavedStateRMW;
	blineUWRSavedStateRMW = lineUWRSavedStateRMW;
	blineCAS0SavedStateRMW = lineCAS0SavedStateRMW;
	blineRAS0SavedStateRMW = lineRAS0SavedStateRMW;
	blineOE0SavedStateRMW = lineOE0SavedStateRMW;

	//Physical registers and memory buffers
	reg.Commit();
	vram->Commit();
	cram->Commit();
	vsram->Commit();
	spriteCache->Commit();
	bstatus = status;
	bhcounter = hcounter;
	bvcounter = vcounter;
	bhcounterLatchedData = hcounterLatchedData;
	bvcounterLatchedData = vcounterLatchedData;
	bhvCounterLatched = hvCounterLatched;
	bhintCounter = hintCounter;
	bvintPending = vintPending;
	bhintPending = hintPending;
	bexintPending = exintPending;
	breadDataAvailable = readDataAvailable;
	breadDataHalfCached = readDataHalfCached;
	bdmaFillOperationRunning = dmaFillOperationRunning;

	//Cached register settings
	bhvCounterLatched = hvCounterLatchEnabled;
	binterlaceEnabled = interlaceEnabled;
	binterlaceDouble = interlaceDouble;
	bscreenModeRS0 = screenModeRS0;
	bscreenModeRS1 = screenModeRS1;
	bscreenModeV30 = screenModeV30;
	bpalMode = palMode;
	bvintEnabled = vintEnabled;
	bhintEnabled = hintEnabled;
	bexintEnabled = exintEnabled;
	bhintCounterReloadValue = hintCounterReloadValue;
	bdmaEnabled = dmaEnabled;
	bdmd0 = dmd0;
	bdmd1 = dmd1;
	bdmaLengthCounter = dmaLengthCounter;
	bdmaSourceAddressByte1 = dmaSourceAddressByte1;
	bdmaSourceAddressByte2 = dmaSourceAddressByte2;
	bdmaSourceAddressByte3 = dmaSourceAddressByte3;
	bautoIncrementData = autoIncrementData;
	binterlaceEnabledCached = interlaceEnabledCached;
	binterlaceDoubleCached = interlaceDoubleCached;
	bscreenModeRS0Cached = screenModeRS0Cached;
	bscreenModeRS1Cached = screenModeRS1Cached;
	bscreenModeV30Cached = screenModeV30Cached;
	bdisplayEnabledCached = displayEnabledCached;
	bspriteAttributeTableBaseAddressDecoded = spriteAttributeTableBaseAddressDecoded;
	bverticalScrollModeCached = verticalScrollModeCached;

	//Propagate any changes to the cached DMA settings back into the reg buffer. This
	//makes changes to DMA settings visible to the debugger.
	//##FIX## If changes are made to the DMA settings through the debugger, the settings
	//cache won't be updated currently.
	if(cachedDMASettingsChanged)
	{
		AccessTarget accessTarget;
		accessTarget.AccessLatest();
		M5SetDMALengthCounter(accessTarget, dmaLengthCounter);
		M5SetDMASourceAddressByte1(accessTarget, dmaSourceAddressByte1);
		M5SetDMASourceAddressByte2(accessTarget, dmaSourceAddressByte2);
		cachedDMASettingsChanged = false;
	}

	//FIFO buffer registers
	for(unsigned int i = 0; i < fifoBufferSize; ++i)
	{
		bfifoBuffer[i] = fifoBuffer[i];
	}
	breadBuffer = readBuffer;
	bfifoNextReadEntry = fifoNextReadEntry;
	bfifoNextWriteEntry = fifoNextWriteEntry;

	//Control port registers
	bcodeAndAddressRegistersModifiedSinceLastWrite = codeAndAddressRegistersModifiedSinceLastWrite;
	bcommandWritePending = commandWritePending;
	boriginalCommandAddress = originalCommandAddress;
	bcommandAddress = commandAddress;
	bcommandCode = commandCode;

	//DMA worker thread properties
	bworkerThreadPaused = workerThreadPaused;

	//DMA transfer registers
	bdmaTransferActive = dmaTransferActive;
	bdmaTransferReadDataCached = dmaTransferReadDataCached;
	bdmaTransferReadCache = dmaTransferReadCache;
	bdmaTransferNextReadMclk = dmaTransferNextReadMclk;
	bdmaTransferLastTimesliceUsedReadDelay = dmaTransferLastTimesliceUsedReadDelay;
	dmaTransferInvalidPortWriteCached = bdmaTransferInvalidPortWriteCached;
	dmaTransferInvalidPortWriteAddressCache = bdmaTransferInvalidAddressCache;
	dmaTransferInvalidPortWriteDataCache = bdmaTransferInvalidDataCache;

	//Event handling
	bnextEventToExecute = nextEventToExecute;
	bnextEventToExecuteTime = nextEventToExecuteTime;
	beventSnapshotSaved = eventSnapshotSaved;
	beventSnapshotScreenModeRS0 = eventSnapshotScreenModeRS0;
	beventSnapshotScreenModeRS1 = eventSnapshotScreenModeRS1;
	beventSnapshotScreenModeV30 = eventSnapshotScreenModeV30;
	beventSnapshotPalMode = eventSnapshotPalMode;
	beventSnapshotInterlaceEnabled = eventSnapshotInterlaceEnabled;

	//Ensure that a valid latest timeslice exists in all our buffers. We need this check
	//here, because commits can be triggered by the system at potentially any point in
	//time, whether a timeslice has been issued or not.
	if(!regTimesliceListUncommitted.empty() && !vramTimesliceListUncommitted.empty() && !cramTimesliceListUncommitted.empty() && !vsramTimesliceListUncommitted.empty() && !spriteCacheTimesliceListUncommitted.empty())
	{
		//Obtain a timeslice lock so we can update the data we feed to the render thread
		boost::mutex::scoped_lock lock(timesliceMutex);

		//Add the number of timeslices we are about to commit to the count of pending
		//render operations. This is used to track if the render thread is lagging.
		pendingRenderOperationCount += (unsigned int)regTimesliceListUncommitted.size();

		//Move all timeslices in our uncommitted timeslice lists over to the committed
		//timeslice lists, for processing by the render thread.
		timesliceRenderInfoList.splice(timesliceRenderInfoList.end(), timesliceRenderInfoListUncommitted);
		regTimesliceList.splice(regTimesliceList.end(), regTimesliceListUncommitted);
		vramTimesliceList.splice(vramTimesliceList.end(), vramTimesliceListUncommitted);
		cramTimesliceList.splice(cramTimesliceList.end(), cramTimesliceListUncommitted);
		vsramTimesliceList.splice(vsramTimesliceList.end(), vsramTimesliceListUncommitted);
		spriteCacheTimesliceList.splice(spriteCacheTimesliceList.end(), spriteCacheTimesliceListUncommitted);

		//Notify the render thread that it's got more work to do
		renderThreadUpdate.notify_all();
	}

	//##DEBUG##
	if(outputTimingDebugMessages)
	{
		//Wait for the render thread to complete its work
		boost::mutex::scoped_lock lock(renderThreadMutex);
		while(pendingRenderOperationCount > 0)
		{
			renderThreadLaggingStateChange.wait(lock);
		}

		//Print out render thread synchronization info
		std::wcout << "VDP Synchronization - Commit:\n"
		           << "-Digital:\t" << hcounter.GetData() << '\t' << vcounter.GetData() << '\n'
		           << "-Render:\t" << renderDigitalHCounterPos << '\t' << renderDigitalVCounterPos << '\n';
	}
}

//----------------------------------------------------------------------------------------
//void S315_5313::RenderThread()
//{
//	boost::mutex::scoped_lock lock(renderThreadMutex);
//
//	//Start the render loop
//	while(renderThreadActive)
//	{
//		//Obtain a copy of the latest completed timeslice period
//		bool renderTimesliceObtained = false;
//		{
//			boost::mutex::scoped_lock lock(timesliceMutex);
//			if(renderTimeslicePending)
//			{
//				regTimesliceCopy = regTimeslice;
//				vramTimesliceCopy = vramTimeslice;
//				cramTimesliceCopy = cramTimeslice;
//				vsramTimesliceCopy = vsramTimeslice;
//				renderTimeslicePending = false;
//
//				//Flag that we managed to obtain a render timeslice
//				renderTimesliceObtained = true;
//			}
//		}
//
//		//If no render timeslice was available, suspend this thread until a timeslice
//		//becomes available, or this thread is instructed to stop, then begin the loop
//		//again.
//		if(!renderTimesliceObtained)
//		{
//			renderThreadUpdate.wait(lock);
//			continue;
//		}
//
//		//Render a complete frame to the framebuffer
//		//##TODO## Rather than rendering frame by frame, render line by line. What I mean
//		//by that is, our render thread should receive updates in 3420 MCLK cycle blocks,
//		//and should track which line it is up to in active scan, so that it knows which
//		//line to render next. If double buffering is not being used, partially rendered
//		//frames will be displayed on the screen. If double buffering is being used, our
//		//render thread will switch the buffers after rendering the last line before
//		//vblank.
//		RenderFrame();
//
//		//##TODO## Re-evaluate this. We want to re-implement the frameskip behaviour we
//		//had in the previous VDP core, where only the latest frame which is ready is
//		//rendered. We need to consider how we'll handle interlacing with this system
//		//though.
//		//Advance to the latest target timeslice
//		{
//			boost::mutex::scoped_lock lock(timesliceMutex);
//			reg.AdvancePastTimeslice(regTimesliceCopy);
//			vram->AdvancePastTimeslice(vramTimesliceCopy);
//			cram->AdvancePastTimeslice(cramTimesliceCopy);
//			vsram->AdvancePastTimeslice(vsramTimesliceCopy);
//			vram->FreeTimesliceReference(vramTimesliceCopy);
//			cram->FreeTimesliceReference(cramTimesliceCopy);
//			vsram->FreeTimesliceReference(vsramTimesliceCopy);
//		}
//	}
//	renderThreadStopped.notify_all();
//}

//----------------------------------------------------------------------------------------
void S315_5313::RenderThreadNew()
{
	boost::mutex::scoped_lock lock(renderThreadMutex);

	//Start the render loop
	bool done = false;
	while(!done)
	{
		//Obtain a copy of the latest completed timeslice period
		bool renderTimesliceObtained = false;
		TimesliceRenderInfo timesliceRenderInfo;
		{
			boost::mutex::scoped_lock timesliceLock(timesliceMutex);

			//If there is at least one render timeslice pending, grab it from the queue.
			//##FIX## Our pendingRenderOperationCount doesn't really work the way we want.
			//Currently, here in the render thread, we decrement it once for each
			//timeslice received, but we don't increment it once for each timeslice.
			//##FIX## This problem with pendingRenderOperationCount is now breaking our
			//PSG and YM2612 cores, since the render thread marks itself as lagging when
			//it's not, and the execute thread waits forever for it to fix itself. We need
			//to get this working, and replicate it in the PSG and YM2612 cores.
//			if((pendingRenderOperationCount > 0) && !regTimesliceList.empty() && !vramTimesliceList.empty() && !cramTimesliceList.empty() && !vsramTimesliceList.empty())
			if(!regTimesliceList.empty() && !vramTimesliceList.empty() && !cramTimesliceList.empty() && !vsramTimesliceList.empty() && !spriteCacheTimesliceList.empty())
			{
				//Update the lagging state for the render thread
				--pendingRenderOperationCount;
				renderThreadLagging = (pendingRenderOperationCount > maxPendingRenderOperationCount);
				renderThreadLaggingStateChange.notify_all();

				//Grab the next completed timeslice from the timeslice list
				timesliceRenderInfo = *timesliceRenderInfoList.begin();
				regTimesliceCopy = *regTimesliceList.begin();
				vramTimesliceCopy = *vramTimesliceList.begin();
				cramTimesliceCopy = *cramTimesliceList.begin();
				vsramTimesliceCopy = *vsramTimesliceList.begin();
				spriteCacheTimesliceCopy = *spriteCacheTimesliceList.begin();
				timesliceRenderInfoList.pop_front();
				regTimesliceList.pop_front();
				vramTimesliceList.pop_front();
				cramTimesliceList.pop_front();
				vsramTimesliceList.pop_front();
				spriteCacheTimesliceList.pop_front();

				//Flag that we managed to obtain a render timeslice
				renderTimesliceObtained = true;
			}
		}

		//If no render timeslice was available, we need to wait for a thread suspension
		//request or a new timeslice to be received, then begin the loop again.
		if(!renderTimesliceObtained)
		{
			//If the render thread has not already been instructed to stop, suspend this
			//thread until a timeslice becomes available or this thread is instructed to
			//stop.
			if(renderThreadActive)
			{
				renderThreadUpdate.wait(lock);
			}

			//If the render thread has been suspended, flag that we need to exit this
			//render loop.
			done = !renderThreadActive;

			//Begin the loop again
			continue;
		}

		//Begin advance sessions for each of our timed buffers
		reg.BeginAdvanceSession(regSession, regTimesliceCopy, false);
		vram->BeginAdvanceSession(vramSession, vramTimesliceCopy, false);
		cram->BeginAdvanceSession(cramSession, cramTimesliceCopy, true);
		vsram->BeginAdvanceSession(vsramSession, vsramTimesliceCopy, false);
		spriteCache->BeginAdvanceSession(spriteCacheSession, spriteCacheTimesliceCopy, false);

		//Calculate the number of cycles to advance in this update step, and reset the
		//current advance progress through this timeslice.
		unsigned int mclkCyclesToAdvance = timesliceRenderInfo.timesliceEndPosition - timesliceRenderInfo.timesliceStartPosition;
		renderDigitalMclkCycleProgress = timesliceRenderInfo.timesliceStartPosition;
		//##DEBUG## Restore old timing behaviour for testing
//		unsigned int mclkCyclesToAdvance = regTimesliceCopy->timesliceLength;
//		renderDigitalMclkCycleProgress = 0;

		//Advance the digital render process
		AdvanceRenderProcess(mclkCyclesToAdvance);

		//Advance past the timeslice we've just rendered from
		{
			//##TODO## I don't think we need this lock here anymore. Confirm that we can
			//remove it.
			boost::mutex::scoped_lock lock(timesliceMutex);
			reg.AdvancePastTimeslice(regTimesliceCopy);
			vram->AdvancePastTimeslice(vramTimesliceCopy);
			cram->AdvancePastTimeslice(cramTimesliceCopy);
			vsram->AdvancePastTimeslice(vsramTimesliceCopy);
			spriteCache->AdvancePastTimeslice(spriteCacheTimesliceCopy);
			vram->FreeTimesliceReference(vramTimesliceCopy);
			cram->FreeTimesliceReference(cramTimesliceCopy);
			vsram->FreeTimesliceReference(vsramTimesliceCopy);
			spriteCache->FreeTimesliceReference(spriteCacheTimesliceCopy);
		}
	}
	renderThreadStopped.notify_all();
}

////----------------------------------------------------------------------------------------
////##FIX## We're calling this when vsync is asserted in the VDP currently, but the logic of
////this function doesn't consider what time the frame actually starts. We need to ensure
////that the actual register and VRAM buffers are in sync with the correct time for the
////rendering process. Basically, we need to advance the VDP state from the vsync initial
////point up to the point where the first pixel output starts, which is when the vcounter is
////incremented to the start of a certain line. Check the analog output notes for more info.
//void S315_5313::RenderFrame()
//{
//	//Create our reg buffer access target. Since we're working with the committed
//	//state, access will be very fast, so there's no need to worry about caching
//	//settings.
//	AccessTarget accessTarget;
//	accessTarget.AccessCommitted();
//
//	//##TODO## We need to decide when we consider a "frame" to start, and when we
//	//consider a "line" to start. The VDP doesn't have the concept of a start and end
//	//for these things, but we need to make one in order to divide rendering into
//	//logical segments.
//
//	//##TODO##
//	mclkCycleRenderProgress = 0;
//	reg.BeginAdvanceSession(regSession, regTimesliceCopy, true);
//	vram->BeginAdvanceSession(vramSession, vramTimesliceCopy, false);
//	cram->BeginAdvanceSession(cramSession, cramTimesliceCopy, true);
//	vsram->BeginAdvanceSession(vsramSession, vsramTimesliceCopy, false);
//
//	//Obtain the VScan settings for the currently active display mode
//	//##TODO## Implement palMode properly. We need to know what the palMode flag was
//	//set to when the frame began, not what the current live state of the line is.
//	//##FIX## We need to cache the state of the V30 cell mode at vsync
//	bool v30CellModeEnabled = M5GetV30CellModeEnabled(accessTarget);
//	const VScanSettings& vscanSettings = GetVScanSettings(v30CellModeEnabled, palMode, M5GetLSM0(accessTarget));
//
//	//Calculate the number of lines to render for this frame in the various sections of
//	//the display.
//	//##TODO## Calculate these somewhere else, perhaps in GetVScanSettings().
//	//##TODO## Accurately calculate the video regions of the display
//	unsigned int activeScanLinesToRender = v30CellModeEnabled? 30*8: 28*8;
//	unsigned int topBorderLinesToRender = (vscanSettings.linesPerFrame - activeScanLinesToRender) / 2;
//	unsigned int bottomBorderLinesToRender = ((vscanSettings.linesPerFrame - activeScanLinesToRender) / 2) + ((vscanSettings.linesPerFrame - activeScanLinesToRender) % 2);
//
//	//Perform the VDP render operations
//	//##TODO## Render the top and bottom border regions correctly
//	//Render the top border region
//	for(unsigned int i = 0; i < topBorderLinesToRender; ++i)
//	{
//		RenderBorderLine(accessTarget, i, i);
//	}
//	//Render the active display lines
//	for(unsigned int i = 0; i < activeScanLinesToRender; ++i)
//	{
//		RenderActiveLine(accessTarget, topBorderLinesToRender+i, i);
//	}
//	//Render the bottom border region
//	for(unsigned int i = 0; i < bottomBorderLinesToRender; ++i)
//	{
//		RenderBorderLine(accessTarget, topBorderLinesToRender+activeScanLinesToRender+i, i);
//	}
//
//	//Here's what we DO know about the way data is dealt with internally:
//	//-Colour values are only decoded when a final pixel is being displayed. We know
//	//this because the VDP colour bus outputs the palette line and entry information.
//	//-Highlight and shadow flags are output separately on the colour bus.
//	//-A flag is output indicating whether the pixel is "transparent". Not clear from
//	//documentation exactly what this means, but most likely a pixel is flagged as
//	//transparent if it comes from the backdrop.
//	//-A flag is output indicating whether the pixel originated from a sprite or a
//	//layer.
//	//-What all of this tells us is that the VDP almost certainly has each layer fill
//	//its own 16-pixel buffer plane, and the layer selection from each plane only
//	//occurs as that pixel is being displayed.
//
//	//Pattern processing notes:
//	//Patent 4243984, Column 9, row 25:
//	//-A pattern name is latched into the "name latch" register, by combining the name
//	//table base address, with the current vcounter and hcounter, and loading the
//	//mapping entry it references.
//	//-Using the current mapping stored in the "name latch" register, a pair of "video
//	//colour codes" are latched from the "pattern colour table", and stored in a pair
//	//of "pattern colour registers". These are a pair of 2-byte block mappings from
//	//the layer mapping table.
//	//-Using the video colour codes, the overlay control will load 2 sets of 4-byte
//	//pattern data, corresponding with two visible lines of pixels, one from each
//	//tile, into "pattern shift registers". These pattern shift registers then provide
//	//the pixels for that plane to the output circuit.
//
//	//##NOTE## The VISUAL operation of the VDP runs behind the DIGITAL operation of
//	//the VDP. The digital VDP starts rendering as soon as the left border begins. The
//	//first pixel is "drawn" when the first pixel of the left border is displayed. The
//	//last pixel is drawn before the right border is displayed. The drawn pixels are
//	//buffered before they are drawn to the screen, but as far as the digital
//	//operation of the VDP is concerned, the status register and hv counter will
//	//report that the VDP is ahead of what has actually been drawn to the screen. This
//	//is important, since the CRAM flicker bug affects the LIVE video output, not the
//	//buffered video output. In other words, if you perform a CRAM write when the
//	//digital operation (with active scan being at the start of the line, and the left
//	//border being at the end of the line) reports that the right border is starting,
//	//the actual flicker on the screen will occur before the right border begins.
//}
//
////----------------------------------------------------------------------------------------
////This handles the single line which runs immediately before the first "active" line which
////shows plane data. This line has the normal access restrictions as an active line, but is
////only used to capture sprite data for the following line.
////----------------------------------------------------------------------------------------
//void S315_5313::RenderPrefetchLineForSprites(const AccessTarget& accessTarget)
//{
//	//##TODO##
//}
//
////----------------------------------------------------------------------------------------
//void S315_5313::RenderBorderLine(const AccessTarget& accessTarget, unsigned int renderBufferLineNo, unsigned int sectionLineNo)
//{
//	const unsigned int mclkCyclesPerLine = 3420;
//
//	//Note:
//	//-A line starts at hscanSettings.vcounterIncrementPoint
//	//-A frame starts at hscanSettings.vcounterIncrementPoint on line vscanSettings.vsyncAssertedPoint
//	//-This is accurate in the sense that the first and last visible scanlines which
//	//appear on the screen definitely start and stop rendering the border colour at the
//	//vcounter increment point. This causes us a little problem for rendering purposes
//	//however, since there are still pixels from active scan to be output by the analog
//	//output of the chip, while the digital operation of the chip has completed rendering
//	//at the increment point.
//	//-I would suggest it is impractical to attempt to "pass on" rendered data from one
//	//line to the next. Maybe we should have special case handling for the first and last
//	//border lines, and the first and last active lines, and make the start of a line the
//	//beginning of hsync for rendering purposes?
//	//-Another alternative is to completely separate the digital and analog portions of
//	//the rendering process. In this model, we would collect all digital rendering
//	//information across all lines, then pass that information on to the analog output
//	//phase, which would consume as much digital data as is available and use it to render
//	//the final image. This seems like a much better option. Not only will the
//	//implementation be easier, it will probably be faster and more flexible also, and
//	//should open us up to be able to do mid-line drawing updates if we implement it
//	//with that in mind.
//
//	//##TODO## Implement this properly
//	const HScanSettings& hscanSettings = GetHScanSettings(M5GetRS0(accessTarget), M5GetRS1(accessTarget));
//
//	//Render the right border
//	unsigned int totalLinePixelCount = hscanSettings.leftBorderPixelCount + hscanSettings.activeDisplayPixelCount + hscanSettings.rightBorderPixelCount;
//	for(unsigned int i = 0; i < totalLinePixelCount; ++i)
//	{
//		unsigned int borderColorPaletteLine = M5GetBackgroundColorPalette(accessTarget);
//		unsigned int borderColorPaletteEntry = M5GetBackgroundColorIndex(accessTarget);
//		WriteColorValueToImageBuffer(borderColorPaletteLine, borderColorPaletteEntry, false, false, i, renderBufferLineNo);
//	}
//
//	mclkCycleRenderProgress += mclkCyclesPerLine;
//	reg.AdvanceBySession(mclkCycleRenderProgress, regSession, regTimesliceCopy);
//	vram->AdvanceBySession(mclkCycleRenderProgress, vramSession, vramTimesliceCopy);
//	vsram->AdvanceBySession(mclkCycleRenderProgress, vsramSession, vsramTimesliceCopy);
//	cram->AdvanceBySession(mclkCycleRenderProgress, cramSession, cramTimesliceCopy);
//}
//
////----------------------------------------------------------------------------------------
////This function performs the actual task of rendering graphical data to the VDP
////framebuffer. We have performed precise hardware tests on the physical hardware to
////determine how and when various data is latched for the VDP rendering process. This
////render function maintains cycle accuracy for the rendering process, and reads data from
////the VRAM buffers and register settings at the same time as the real hardware.
////
////Testing notes:
////-We have confirmed that the various base address registers, IE, for layer A mapping
////data, are not cached. Changes to these registers take effect the next time mapping data
////is read.
////-We have confirmed that the field size registers are not cached, and changes take effect
////the next time mapping data is read.
////----------------------------------------------------------------------------------------
//void S315_5313::RenderActiveLine(const AccessTarget& accessTarget, unsigned int renderBufferLineNo, unsigned int sectionLineNo)
//{
//	//##TODO## Implement mode 4 support
//	//##FIX## The active interlace mode is latched during vblank. It needs to be cached,
//	//and the cached value retrieved here.
//	//##FIX## Although on the real hardware H40 mode can be enabled and disabled mid-line,
//	//we do not support this for performance reasons, and it is instead latched at hblank.
//	//We need to cache the H40 mode setting for each line, and use that cached value here.
//	LineRenderSettings renderSettings;
//	renderSettings.interlaceMode2Active = M5GetLSM0(accessTarget) && M5GetLSM1(accessTarget);
//	renderSettings.renderWithH40Mode = M5GetRS1(accessTarget);
//	const HScanSettings& hscanSettings = GetHScanSettings(M5GetRS0(accessTarget), renderSettings.renderWithH40Mode);
//
//	//Calculate various screen mode settings for this line
//	renderSettings.blockPatternSizeY = renderSettings.interlaceMode2Active? 16: 8;
//	renderSettings.blockPatternByteSize = renderSettings.blockPatternRowByteSize * renderSettings.blockPatternSizeY;
//	renderSettings.cellBlockSetsToRead = renderSettings.renderWithH40Mode? 5: 4;
//	renderSettings.mclkCyclesPerSCCycle = renderSettings.renderWithH40Mode? 4: 5;
//	renderSettings.mclkCyclesPerLogicStep = renderSettings.mclkCyclesPerSCCycle * renderSettings.scCyclesPerLogicStep;
//
//	//Determine which line of active scan we're rendering
//	renderSettings.activePixelY = sectionLineNo;
//	renderSettings.activeBlockY = sectionLineNo / renderSettings.blockPixelSizeY;
//
//	//##DEBUG##
//	const unsigned int mclkCyclesPerLine = 3420;
//	unsigned int initialMclkCycleRenderProgress = mclkCycleRenderProgress;
//
//	//Calculate the address of the hscroll data to read for this line
//	unsigned int hscrollDataBase = M5GetHScrollDataBase(accessTarget);
//	bool hscrState = M5GetHSCR(accessTarget);
//	bool lscrState = M5GetLSCR(accessTarget);
//	unsigned int hscrollDataAddress = hscrollDataBase;
//	//##TODO## Based on the EA logo for Populous, it appears that the state of LSCR is
//	//ignored when HSCR is not set. We should confirm this on hardware.
//	if(hscrState)
//	{
//		hscrollDataAddress += lscrState? (sectionLineNo * 4): ((renderSettings.activeBlockY * renderSettings.blockPixelSizeY) * 4);
//	}
//
//	//Read the hscroll data for this line
//	//##TODO## Confirm the way scrolling data is interpreted through hardware tests. Eg,
//	//does -1 actually scroll to the left by one place, or are 0 and -1 equivalent?
//	//##TODO## According to the official documentation, the upper 6 bits of the hscroll
//	//data are unused, and are allowed to be used by software to store whatever values
//	//they want. Confirm this on hardware.
//	unsigned int layerAHscrollOffset = ((unsigned int)vram->ReadCommitted(hscrollDataAddress+0) << 8) | (unsigned int)vram->ReadCommitted(hscrollDataAddress+1);
//	unsigned int layerBHscrollOffset = ((unsigned int)vram->ReadCommitted(hscrollDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(hscrollDataAddress+3);
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//	//##TODO## Read the sprite pattern data
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//	//Break the hscroll data into its two component parts. The lower 4 bits represent a
//	//displacement into the 2-cell column, or in other words, the displacement of the
//	//starting pixel within each column, while the upper 6 bits represent an offset for
//	//the column mapping data itself.
//	//-----------------------------------------
//	//| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	//|---------------------------------------|
//	//|  Column Shift Value   | Displacement  |
//	//-----------------------------------------
//	unsigned int scrollValueDisplacementLayerA = layerAHscrollOffset & 0x00F;
//	unsigned int columnShiftValueLayerA = (layerAHscrollOffset & 0x3F0) >> 4;
//	unsigned int scrollValueDisplacementLayerB = layerBHscrollOffset & 0x00F;
//	unsigned int columnShiftValueLayerB = (layerBHscrollOffset & 0x3F0) >> 4;
//
//	//Create the buffers for our mapping and pattern data
//	unsigned int currentRenderPixelLayerA = 0;
//	unsigned int currentRenderPixelLayerB = 0;
//	std::vector<Data> mappingDataCacheLayerA(2, Data(16));
//	std::vector<Data> mappingDataCacheLayerB(2, Data(16));
//	std::vector<Data> patternDataCacheLayerA(2, Data(32));
//	std::vector<Data> patternDataCacheLayerB(2, Data(32));
//	std::vector<unsigned int> paletteIndexSprite(hscanSettings.activeDisplayPixelCount+16);
//	std::vector<unsigned int> paletteIndexLayerA(hscanSettings.activeDisplayPixelCount+16);
//	std::vector<unsigned int> paletteIndexLayerB(hscanSettings.activeDisplayPixelCount+16);
//	std::vector<unsigned int> paletteLineSprite(hscanSettings.activeDisplayPixelCount+16);
//	std::vector<unsigned int> paletteLineLayerA(hscanSettings.activeDisplayPixelCount+16);
//	std::vector<unsigned int> paletteLineLayerB(hscanSettings.activeDisplayPixelCount+16);
//	std::vector<bool> prioritySprite(hscanSettings.activeDisplayPixelCount+16);
//	std::vector<bool> priorityLayerA(hscanSettings.activeDisplayPixelCount+16);
//	std::vector<bool> priorityLayerB(hscanSettings.activeDisplayPixelCount+16);
//
//	//Determine the playfield dimensions
//	//##TODO## Determine when playfield dimensions are actually calculated, either for the
//	//entire line like this, or whether these registers are checked mid-line and can be
//	//modified mid-line. Most likely, mid-line modifications are possible.
//	unsigned int hszState = M5GetHSZ(accessTarget);
//	unsigned int vszState = M5GetVSZ(accessTarget);
//
//	//##TODO## Comment this
//	//##TODO## If we're in interlace mode 2, and the odd flag is set, we need to add 1 to
//	//the virtual vertical screen position when calculating the pattern and mapping row
//	//numbers.
//	unsigned int columnNumber = 0x3F;
//	unsigned int displayRowNumber = (renderSettings.interlaceMode2Active? (sectionLineNo * 2): sectionLineNo);
//
//	//Calculate the address of the vscroll data to read for this block
//	//##TODO## Determine how hscroll interacts with vscroll when both are applied at the
//	//same time. Read the test notes from Eke: Reportedly, if a 2-cell column is partially
//	//visible on the left, that column has vscroll fixed to 0 for H32 mode, however for
//	//H40 mode, it's some strange ANDed value of several VSRAM addresses. Check the
//	//following thread from SpritesMind for more info:
//	//http://gendev.spritesmind.net/forum/viewtopic.php?t=737&start=30
//	//This only occurs when column-based vscroll is applied. Overall vscroll values work
//	//correctly. This all needs major testing on hardware.
//	//##TODO## Test how overall vertical scrolling interacts with line-based and overall
//	//horizontal scrolling.
//	bool vscrState = M5GetVSCR(accessTarget);
//	unsigned int vscrollDataAddress = vscrState? (columnNumber * 4): 0;
//
//	//Read the vscroll data for this line. Note only the lower 10 bits are
//	//effective, or the lower 11 bits in the case of interlace mode 2, due to the
//	//scrolled address being wrapped to lie within the total field boundaries,
//	//which never exceed 128 blocks.
//	unsigned int layerAVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+0) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+1);
//	unsigned int layerBVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+2) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+3);
//
//	//Calculate the target block mapping pair and selected row for layer A
//	//##TODO## Confirm through VRAM snooping which actual address is read from for the
//	//left scrolled cell block when the horizontal scroll value is aligned to a 2-cell
//	//boundary. Even though the data is never displayed in this case, we can and should
//	//determine where the VDP reads this data from.
//	//##TODO## Confirm through VRAM snooping which actual address is read from when a
//	//left-aligned window is present for layer A, both with and without horizontal
//	//scrolling. We suspect that the normal block which is read for layer A, without a
//	//window being active, may be read in this case.
//	//##TODO## Replace the division and modulus operators here with bitshift and bitmask
//	//operations.
//	unsigned int mappingColumnNumberLayerA = columnNumber - columnShiftValueLayerA;
//	unsigned int mappingRowNumberLayerA = (displayRowNumber + layerAVscrollOffset) / renderSettings.blockPatternSizeY;
//	unsigned int patternRowNumberLayerA = (displayRowNumber + layerAVscrollOffset) % renderSettings.blockPatternSizeY;
//
//	//##TODO## Implement support for the window layer
////	unsigned int nameTableBaseA = M5GetNameTableBaseWindow(accessTarget);
////	ReadMappingDataPair(mappingDataCacheLayerA[0], mappingDataCacheLayerA[1], nameTableBaseA, displayRowNumber / renderSettings.blockPatternSizeY, columnNumber, 0x0, 0x0);
//
//	//Read layer A left scrolled cell block mapping data
//	unsigned int nameTableBaseA = M5GetNameTableBaseScrollA(accessTarget);
//	ReadMappingDataPair(mappingDataCacheLayerA[0], mappingDataCacheLayerA[1], nameTableBaseA, mappingRowNumberLayerA, mappingColumnNumberLayerA, hszState, vszState);
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//	//##TODO## Read the sprite pattern data
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//	//Read layer A left scrolled cell block pattern row 1
//	ReadPatternDataRow(renderSettings, patternDataCacheLayerA[0], mappingDataCacheLayerA[0], patternRowNumberLayerA);
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//	//Read layer A left scrolled cell block pattern row 2
//	ReadPatternDataRow(renderSettings, patternDataCacheLayerA[1], mappingDataCacheLayerA[1], patternRowNumberLayerA);
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//	//Render the layer A column
//	RenderColumnBlockPair(columnNumber, scrollValueDisplacementLayerA, mappingDataCacheLayerA[0], mappingDataCacheLayerA[1], patternDataCacheLayerA[0], patternDataCacheLayerA[1], paletteLineLayerA, paletteIndexLayerA, currentRenderPixelLayerA);
//
//	//Calculate the target block mapping pair and selected row for layer B
//	unsigned int mappingColumnNumberLayerB = columnNumber - columnShiftValueLayerB;
//	unsigned int mappingRowNumberLayerB = (displayRowNumber + layerBVscrollOffset) / renderSettings.blockPatternSizeY;
//	unsigned int patternRowNumberLayerB = (displayRowNumber + layerBVscrollOffset) % renderSettings.blockPatternSizeY;
//
//	//Read layer B left scrolled cell block mapping data
//	unsigned int nameTableBaseB = M5GetNameTableBaseScrollB(accessTarget);
//	ReadMappingDataPair(mappingDataCacheLayerB[0], mappingDataCacheLayerB[1], nameTableBaseB, mappingRowNumberLayerB, mappingColumnNumberLayerB, hszState, vszState);
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//	//##TODO## Read sprite mapping data
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//	//Read layer B left scrolled cell block pattern row 1
//	ReadPatternDataRow(renderSettings, patternDataCacheLayerB[0], mappingDataCacheLayerB[0], patternRowNumberLayerB);
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//	//Read layer B left scrolled cell block pattern row 2
//	ReadPatternDataRow(renderSettings, patternDataCacheLayerB[1], mappingDataCacheLayerB[1], patternRowNumberLayerB);
//	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//	//Render the layer B column
//	RenderColumnBlockPair(columnNumber, scrollValueDisplacementLayerB, mappingDataCacheLayerB[0], mappingDataCacheLayerB[1], patternDataCacheLayerB[0], patternDataCacheLayerB[1], paletteLineLayerB, paletteIndexLayerB, currentRenderPixelLayerB);
//
//	//Read the mapping and pattern data for all layers
//	for(unsigned int cellBlockSet = 0; cellBlockSet < renderSettings.cellBlockSetsToRead; ++cellBlockSet)
//	{
//		for(unsigned int i = 0; i < 4; ++i)
//		{
//			columnNumber = (cellBlockSet * 4) + i;
//
//			//Calculate the address of the vscroll data to read for this block
//			//##TODO## Determine if the vscroll data is read at the start of each 2-cell
//			//block, even for overall scrolling. We can determine this by changing the
//			//vscroll data mid-line.
//			//##TODO## Determine if mid-line changes to VSCR take effect immediately
//			bool vscrState = M5GetVSCR(accessTarget);
//			unsigned int vscrollDataAddress = vscrState? (columnNumber * 4): 0;
//
//			//Read the vscroll data for this line. Note only the lower 10 bits are
//			//effective, or the lower 11 bits in the case of interlace mode 2, due to the
//			//scrolled address being wrapped to lie within the total field boundaries,
//			//which never exceed 128 blocks.
//			unsigned int layerAVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+0) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+1);
//			unsigned int layerBVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+2) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+3);
//
//			//Calculate the target block mapping pair and selected row for layer A
//			unsigned int mappingColumnNumberLayerA = columnNumber - columnShiftValueLayerA;
//			unsigned int mappingRowNumberLayerA = (displayRowNumber + layerAVscrollOffset) / renderSettings.blockPatternSizeY;
//			unsigned int patternRowNumberLayerA = (displayRowNumber + layerAVscrollOffset) % renderSettings.blockPatternSizeY;
//
//			//Read layer A mapping data
//			unsigned int nameTableBaseA = M5GetNameTableBaseScrollA(accessTarget);
//			ReadMappingDataPair(mappingDataCacheLayerA[0], mappingDataCacheLayerA[1], nameTableBaseA, mappingRowNumberLayerA, mappingColumnNumberLayerA, hszState, vszState);
//			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//			//External access slot, or refresh cycle on 4th block in set.
//			if(i < 3)
//			{
//				reg.AdvanceBySession(mclkCycleRenderProgress, regSession, regTimesliceCopy);
//				vram->AdvanceBySession(mclkCycleRenderProgress, vramSession, vramTimesliceCopy);
//				vsram->AdvanceBySession(mclkCycleRenderProgress, vsramSession, vsramTimesliceCopy);
//			}
//			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//			//Read layer A pattern row 1
//			ReadPatternDataRow(renderSettings, patternDataCacheLayerA[0], mappingDataCacheLayerA[0], patternRowNumberLayerA);
//			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//			//Read layer A pattern row 2
//			ReadPatternDataRow(renderSettings, patternDataCacheLayerA[1], mappingDataCacheLayerA[1], patternRowNumberLayerA);
//			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//			//Render the layer A column
//			RenderColumnBlockPair(columnNumber, scrollValueDisplacementLayerA, mappingDataCacheLayerA[0], mappingDataCacheLayerA[1], patternDataCacheLayerA[0], patternDataCacheLayerA[1], paletteLineLayerA, paletteIndexLayerA, currentRenderPixelLayerA);
//
//			//Calculate the target block mapping pair and selected row for layer B
//			unsigned int mappingColumnNumberLayerB = columnNumber - columnShiftValueLayerB;
//			unsigned int mappingRowNumberLayerB = (displayRowNumber + layerBVscrollOffset) / renderSettings.blockPatternSizeY;
//			unsigned int patternRowNumberLayerB = (displayRowNumber + layerBVscrollOffset) % renderSettings.blockPatternSizeY;
//
//			//Read layer B mapping data
//			unsigned int nameTableBaseB = M5GetNameTableBaseScrollB(accessTarget);
//			ReadMappingDataPair(mappingDataCacheLayerB[0], mappingDataCacheLayerB[1], nameTableBaseB, mappingRowNumberLayerB, mappingColumnNumberLayerB, hszState, vszState);
//			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//			//##TODO## Read sprite mapping data
//			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//			//Read layer B pattern row 1
//			ReadPatternDataRow(renderSettings, patternDataCacheLayerB[0], mappingDataCacheLayerB[0], patternRowNumberLayerB);
//			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//			//Read layer B pattern row 2
//			ReadPatternDataRow(renderSettings, patternDataCacheLayerB[1], mappingDataCacheLayerB[1], patternRowNumberLayerB);
//			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
//
//			//Render the layer A column
//			RenderColumnBlockPair(columnNumber, scrollValueDisplacementLayerB, mappingDataCacheLayerB[0], mappingDataCacheLayerB[1], patternDataCacheLayerB[0], patternDataCacheLayerB[1], paletteLineLayerB, paletteIndexLayerB, currentRenderPixelLayerB);
//		}
//	}
//
//	//##TODO## Note that we only ever need to read or advance the CRAM data when we're
//	//actually rendering, after the line has been built. This means we should be advancing
//	//the CRAM data state separately from the rest of the buffers, and it is when we
//	//perform this advance that we take CRAM write flicker into account.
//	//##TODO## Clean up this old code.
//	unsigned int cramWriteFlickerBufferSize = hscanSettings.leftBorderPixelCount + hscanSettings.activeDisplayPixelCount + hscanSettings.rightBorderPixelCount;
//	std::vector<unsigned int> cramWriteFlickerPaletteIndex(cramWriteFlickerBufferSize);
//	std::vector<unsigned int> cramWriteFlickerPaletteLine(cramWriteFlickerBufferSize);
//	std::vector<bool> cramWriteFlickerOccurred(cramWriteFlickerBufferSize, false);
//
//	//##TODO## Don't do this at all. Store CRAM writes into a buffer, like we will for
//	//rendered sprite pixels.
//	//##TODO## Consider enhancing the session stepping for timed buffers, to allow the
//	//next write time to be extracted from the session info. This will avoid the need to
//	//call BufferGetWriteInfo here unnecessarily, as we can use the saved information in
//	//the session to determine if there is a write in range.
//	//unsigned int cramWriteIndex = 0;
//	//IManagedBufferInt::WriteInfo cramWriteInfo = cram->BufferGetWriteInfo(cramWriteIndex++, cramTimesliceCopy);
//	//while(cramWriteInfo.exists && (cramWriteInfo.writeTime < mclkCyclesPerLine))
//	//{
//	//	unsigned int pixelIndex = (cramWriteInfo.writeTime / renderSettings.mclkCyclesPerSCCycle) / 2;
//	//	unsigned int paletteLine = cramWriteInfo.writeAddress / 0x20;
//	//	unsigned int paletteIndex = (cramWriteInfo.writeAddress % 0x20) / 2;
//	//	cramWriteFlickerOccurred[pixelIndex] = true;
//	//	cramWriteFlickerPaletteLine[pixelIndex] = paletteLine;
//	//	cramWriteFlickerPaletteIndex[pixelIndex] = paletteIndex;
//	//	cramWriteInfo = cram->BufferGetWriteInfo(cramWriteIndex++, cramTimesliceCopy);
//	//}
//
//	//##FIX## The background colour palette and index settings have been advanced to the
//	//end of the line by now. We need the live register values. In fact, this issue of
//	//live register values affects a lot more than just background changes. It affects the
//	//disable video flag for example.
//	//Render the left border
//	for(unsigned int i = 0; i < hscanSettings.leftBorderPixelCount; ++i)
//	{
//		unsigned int borderColorPaletteLine = M5GetBackgroundColorPalette(accessTarget);
//		unsigned int borderColorPaletteEntry = M5GetBackgroundColorIndex(accessTarget);
//		if(cramWriteFlickerOccurred[i])
//		{
//			borderColorPaletteLine = cramWriteFlickerPaletteLine[i];
//			borderColorPaletteEntry = cramWriteFlickerPaletteIndex[i];
//		}
//		WriteColorValueToImageBuffer(borderColorPaletteLine, borderColorPaletteEntry, false, false, i, renderBufferLineNo);
//	}
//
//	//Render the active display
//	for(unsigned int i = 0; i < hscanSettings.activeDisplayPixelCount; ++i)
//	{
//		//Read the background palette settings
//		unsigned int paletteLineBackground = M5GetBackgroundColorPalette(accessTarget);
//		unsigned int paletteIndexBackground = M5GetBackgroundColorIndex(accessTarget);
//
//		//Load the palette line and index values for each layer into our data table
//		unsigned int paletteLineData[4];
//		unsigned int paletteIndexData[4];
//		paletteLineData[LAYER_SPRITE] = paletteLineSprite[i];
//		paletteLineData[LAYER_LAYERA] = paletteLineLayerA[i];
//		paletteLineData[LAYER_LAYERB] = paletteLineLayerB[i];
//		paletteLineData[LAYER_BACKGROUND] = paletteLineBackground;
//		paletteIndexData[LAYER_SPRITE] = paletteIndexSprite[i];
//		paletteIndexData[LAYER_LAYERA] = paletteIndexLayerA[i];
//		paletteIndexData[LAYER_LAYERB] = paletteIndexLayerB[i];
//		paletteIndexData[LAYER_BACKGROUND] = paletteIndexBackground;
//
//		//Determine if any of the palette index values for any of the layers indicate a
//		//transparent pixel.
//		//##TODO## Consider renaming and reversing the logic of these flags to match the
//		//comment above. The name of "found pixel" isn't very descriptive, and in the case
//		//of the sprite layer "isPixelOpaque" could be misleading when the sprite pixel is
//		//being used as an operator in shadow/highlight mode. A flag with a name like
//		//isPixelTransparent would be much more descriptive.
//		bool foundSpritePixel = (paletteIndexData[LAYER_SPRITE] != 0);
//		bool foundLayerAPixel = (paletteIndexData[LAYER_LAYERA] != 0);
//		bool foundLayerBPixel = (paletteIndexData[LAYER_LAYERB] != 0);
//
//		//Read the shadow/highlight mode settings
//		//##TODO## Check when changes to the shadow/highlight register take effect. Can
//		//this register be toggled mid-line?
//		bool shadowHighlightEnabled = M5GetShadowHighlightEnabled(accessTarget);
//		bool spriteIsShadowOperator = (paletteLineData[LAYER_SPRITE] == 3) && (paletteIndexData[LAYER_SPRITE] == 15);
//		bool spriteIsHighlightOperator = (paletteLineData[LAYER_SPRITE] == 3) && (paletteIndexData[LAYER_SPRITE] == 14);
//
//		//Perform layer priority calculations, and determine the layer to use, as well as
//		//the resulting state of the shadow and highlight bits.
//		//unsigned int layerIndex;
//		//bool shadow;
//		//bool highlight;
//		//CalculateLayerPriorityIndex(layerIndex, shadow, highlight, shadowHighlightEnabled, spriteIsShadowOperator, spriteIsHighlightOperator, foundSpritePixel, foundLayerAPixel, foundLayerBPixel, prioritySprite, priorityLayerA, priorityLayerB);
//
//		//Encode the parameters for the layer priority calculation into an index value for
//		//the priority lookup table.
//		unsigned int priorityIndex = 0;
//		priorityIndex |= (unsigned int)shadowHighlightEnabled << 8;
//		priorityIndex |= (unsigned int)spriteIsShadowOperator << 7;
//		priorityIndex |= (unsigned int)spriteIsHighlightOperator << 6;
//		priorityIndex |= (unsigned int)foundSpritePixel << 5;
//		priorityIndex |= (unsigned int)foundLayerAPixel << 4;
//		priorityIndex |= (unsigned int)foundLayerBPixel << 3;
//		priorityIndex |= (unsigned int)prioritySprite[i] << 2;
//		priorityIndex |= (unsigned int)priorityLayerA[i] << 1;
//		priorityIndex |= (unsigned int)priorityLayerB[i];
//
//		//Lookup the pre-calculated layer priority from the lookup table. We use a lookup
//		//table to eliminate branching, which should yield a significant performance
//		//boost.
//		unsigned int layerSelectionResult = layerPriorityLookupTable[priorityIndex];
//
//		//Extract the layer index, shadow, and highlight data from the combined result
//		//returned from the layer priority lookup table.
//		unsigned int layerIndex = layerSelectionResult & 0x03;
//		bool shadow = (layerSelectionResult & 0x08) != 0;
//		bool highlight = (layerSelectionResult & 0x04) != 0;
//
//		//Read the palette line and index to use for the selected layer
//		unsigned int paletteLine = paletteLineData[layerIndex];
//		unsigned int paletteIndex = paletteIndexData[layerIndex];
//
//		//Write the final colour values to the image buffer
//		if(cramWriteFlickerOccurred[hscanSettings.leftBorderPixelCount+i])
//		{
//			paletteLine = cramWriteFlickerPaletteLine[hscanSettings.leftBorderPixelCount+i];
//			paletteIndex = cramWriteFlickerPaletteIndex[hscanSettings.leftBorderPixelCount+i];
//		}
//		WriteColorValueToImageBuffer(paletteLine, paletteIndex, shadow, highlight, hscanSettings.leftBorderPixelCount + i, renderBufferLineNo);
//	}
//
//	//Render the right border
//	for(unsigned int i = 0; i < hscanSettings.rightBorderPixelCount; ++i)
//	{
//		unsigned int borderColorPaletteLine = M5GetBackgroundColorPalette(accessTarget);
//		unsigned int borderColorPaletteEntry = M5GetBackgroundColorIndex(accessTarget);
//		if(cramWriteFlickerOccurred[hscanSettings.leftBorderPixelCount+hscanSettings.activeDisplayPixelCount+i])
//		{
//			borderColorPaletteLine = cramWriteFlickerPaletteLine[hscanSettings.leftBorderPixelCount+hscanSettings.activeDisplayPixelCount+i];
//			borderColorPaletteEntry = cramWriteFlickerPaletteIndex[hscanSettings.leftBorderPixelCount+hscanSettings.activeDisplayPixelCount+i];
//		}
//		WriteColorValueToImageBuffer(borderColorPaletteLine, borderColorPaletteEntry, false, false, hscanSettings.leftBorderPixelCount+hscanSettings.activeDisplayPixelCount+i, renderBufferLineNo);
//	}
//
//	//##DEBUG##
//	unsigned int remainingMclkCyclesInLine = mclkCyclesPerLine - (mclkCycleRenderProgress - initialMclkCycleRenderProgress);
//	mclkCycleRenderProgress += remainingMclkCyclesInLine;
//	reg.AdvanceBySession(mclkCycleRenderProgress, regSession, regTimesliceCopy);
//	vram->AdvanceBySession(mclkCycleRenderProgress, vramSession, vramTimesliceCopy);
//	cram->AdvanceBySession(mclkCycleRenderProgress, cramSession, cramTimesliceCopy);
//	vsram->AdvanceBySession(mclkCycleRenderProgress, vsramSession, vsramTimesliceCopy);
//}

//----------------------------------------------------------------------------------------
void S315_5313::AdvanceRenderProcess(unsigned int mclkCyclesRemainingToAdvance)
{
	//Create our reg buffer access target. Since we're working with the committed state,
	//access will be very fast, so there's no need to worry about caching settings.
	AccessTarget accessTarget;
	accessTarget.AccessCommitted();

 	//Get the hscan and vscan settings for this scanline
	const HScanSettings* hscanSettings = &GetHScanSettings(renderDigitalScreenModeRS0Active, renderDigitalScreenModeRS1Active);
	const VScanSettings* vscanSettings = &GetVScanSettings(renderDigitalScreenModeV30Active, renderDigitalPalModeActive, renderDigitalInterlaceEnabledActive);

	//Combine any remaining mclk cycles from the previous render update step into this
	//update step.
	mclkCyclesRemainingToAdvance += renderDigitalRemainingMclkCycles;

	//Calculate the number of mclk cycles required to advance the render process one
	//pixel clock step
	unsigned int mclkTicksForNextPixelClockTick;
	mclkTicksForNextPixelClockTick = GetMclkTicksForPixelClockTicks(*hscanSettings, 1, renderDigitalHCounterPos, renderDigitalScreenModeRS0Active, renderDigitalScreenModeRS1Active);

	//Advance each complete pixel clock step we are able to complete in this update step
	while(mclkCyclesRemainingToAdvance >= mclkTicksForNextPixelClockTick)
	{
		//Advance the register buffer up to the current time. Register changes can occur
		//at any time, so we need to ensure this buffer is always current.
		reg.AdvanceBySession(renderDigitalMclkCycleProgress, regSession, regTimesliceCopy);

		//If we've reached a point where horizontal screen mode settings need to be
		//latched, cache the new settings now.
		if(renderDigitalHCounterPos == hscanSettings->hblankSetPoint)
		{
			//Cache the new settings
			renderDigitalScreenModeRS0Active = M5GetRS0(accessTarget);
			renderDigitalScreenModeRS1Active = M5GetRS1(accessTarget);

			//Latch updated screen mode settings
			hscanSettings = &GetHScanSettings(renderDigitalScreenModeRS0Active, renderDigitalScreenModeRS1Active);
		}

		//If we've reached a point where vertical screen mode settings need to be latched,
		//cache the new settings now.
		if((renderDigitalVCounterPos == vscanSettings->vblankSetPoint) && (renderDigitalHCounterPos == hscanSettings->vcounterIncrementPoint))
		{
			//Cache the new settings
			renderDigitalScreenModeV30Active = M5GetV30CellModeEnabled(accessTarget);
			renderDigitalInterlaceEnabledActive = M5GetLSM0(accessTarget);
			renderDigitalInterlaceDoubleActive = M5GetLSM1(accessTarget);
			//##FIX## This is incorrect. This is retrieving the current live state of the
			//line. We need to store history information for this line, so that the
			//correct line state can be set here.
			renderDigitalPalModeActive = palMode;

			//Latch updated screen mode settings
			vscanSettings = &GetVScanSettings(renderDigitalScreenModeV30Active, renderDigitalPalModeActive, renderDigitalInterlaceEnabledActive);
		}

		//Perform any digital render operations which need to occur on this cycle. Note
		//that operation steps only occur once every 4 SC cycles, since it takes the VRAM
		//4 SC cycles for each 32-bit serial memory read, which is performed by the VDP
		//itself to read VRAM data for the rendering process, or 4 SC cycles for an 8-bit
		//direct memory read or write, which can occur at an access slot. Every 2 SC
		//cycles however, a pixel is output to the analog output circuit to perform layer
		//priority selection and video output. Interestingly, the synchronization of the
		//memory times with the hcounter update process is different, depending on whether
		//H40 mode is active. Where a H32 mode is selected, memory access occurs on odd
		//hcounter values. Where H40 mode is selected, memory access occurs on even
		//hcounter values.
		//##TODO## Perform more hardware tests on this behaviour, to confirm the
		//synchronization differences, and determine whether it is the memory access
		//timing or the hcounter progression which changes at the time the H40 screen mode
		//setting is toggled.
		bool hcounterLowerBit = (renderDigitalHCounterPos & 0x1) != 0;
		if(renderDigitalScreenModeRS1Active != hcounterLowerBit)
		{
			UpdateDigitalRenderProcess(accessTarget, *hscanSettings, *vscanSettings);
		}

		//Perform any analog render operations which need to occur on this cycle
		UpdateAnalogRenderProcess(accessTarget, *hscanSettings, *vscanSettings);

		//If we're about to increment the vcounter, save the current value of it before
		//the increment, so that the analog render process can use it to calculate the
		//current analog output line.
		if((renderDigitalHCounterPos + 1) == hscanSettings->vcounterIncrementPoint)
		{
			renderDigitalVCounterPosPreviousLine = renderDigitalVCounterPos;
		}

		//Advance the HV counters for the digital render process
		AdvanceHVCounters(*hscanSettings, renderDigitalHCounterPos, *vscanSettings, renderDigitalInterlaceEnabledActive, renderDigitalOddFlagSet, renderDigitalVCounterPos, 1);

		//Advance the mclk cycle progress of the current render timeslice
		mclkCyclesRemainingToAdvance -= mclkTicksForNextPixelClockTick;
		renderDigitalMclkCycleProgress += mclkTicksForNextPixelClockTick;

		//Calculate the number of mclk cycles required to advance the render process one
		//pixel clock step
		mclkTicksForNextPixelClockTick = GetMclkTicksForPixelClockTicks(*hscanSettings, 1, renderDigitalHCounterPos, renderDigitalScreenModeRS0Active, renderDigitalScreenModeRS1Active);
	}

	//Save any remaining mclk cycles from this update step
	renderDigitalRemainingMclkCycles = mclkCyclesRemainingToAdvance;
}

//----------------------------------------------------------------------------------------
void S315_5313::UpdateDigitalRenderProcess(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings)
{
	bool interlaceMode2Active = renderDigitalInterlaceEnabledActive && renderDigitalInterlaceDoubleActive;

	//Obtain the set of update steps for the current screen mode settings
	unsigned int operationArraySize = 0;
	const RenderOp* operationArray = 0;
	if(renderDigitalScreenModeRS1Active)
	{
		operationArray = &operationsH40[0];
		operationArraySize = sizeof(operationsH40) / sizeof(operationsH40[0]);
	}
	else
	{
		operationArray = &operationsH32[0];
		operationArraySize = sizeof(operationsH32) / sizeof(operationsH32[0]);
	}

	//##TODO## Restructure the following code. We shouldn't manually manipulate the next
	//operation outside of the section of code which originally determined it. What we
	//should do is gather required state, and then calculate the next operation in one
	//step at the end.

	//Determine what operation to perform at the current scanline position
	unsigned int hcounterLinear = HCounterValueFromVDPInternalToLinear(hscanSettings, renderDigitalHCounterPos);
	RenderOp nextOperation = operationArray[(hcounterLinear >> 1) % operationArraySize];

	//Use the current VCounter data to determine whether we are rendering an active line
	//of the display, which active line number we're up to, based on the current screen
	//mode settings.
	bool insideActiveScanRow = false;
	unsigned int renderDigitalCurrentRow = 0;
	unsigned int renderDigitalRowCount = 0;
	if((renderDigitalVCounterPos >= vscanSettings.activeDisplayVCounterFirstValue) && (renderDigitalVCounterPos <= vscanSettings.activeDisplayVCounterLastValue))
	{
		//We're inside the active display region. Calculate the playfield row number for
		//the current line, and the total number of rows in the active display region.
		insideActiveScanRow = true;
		renderDigitalCurrentRow = renderDigitalVCounterPos - vscanSettings.activeDisplayVCounterFirstValue;
		renderDigitalRowCount = vscanSettings.activeDisplayVCounterLastValue - vscanSettings.activeDisplayVCounterFirstValue;
	}

	//Read the display enable register. If this register is cleared, the output for this
	//update step is forced to the background colour, and free access to VRAM is
	//permitted. Any read operations that would have been performed from VRAM are not
	//performed in this case. If this occurs during the active region of the display, it
	//corrupts the output, due to mapping and pattern data not being read at the
	//appropriate time. It is safe to disable the display during the portion of a scanline
	//that lies outside the active display area, however, this reduces the number of
	//sprite pattern blocks which can be read, creating a lower sprite limit on the
	//following scanline. A sprite overflow is flagged if not all the sprite pixels could
	//be read due to the display being disabled, even if there would normally be enough
	//time to read all the pattern data had the display not been disabled. The only game
	//which is known to rely on this is "Mickey Mania" for the Mega Drive. See
	//http://gendev.spritesmind.net/forum/viewtopic.php?t=541
	//##TODO## Determine if the display enable bit is effective when the VDP test
	//register has been set to one of the modes that disables the blanking of the
	//display in the border regions.
	bool displayEnabled = M5GetDisplayEnabled(accessTarget);

	//If this line is outside the active display area, IE, in the top or bottom border
	//areas or in vblank, or if the display is currently disabled, free access is
	//permitted to VRAM, except during the memory refresh slots. To emulate this, we force
	//the next operation to an external access slot under these conditions, unless we're
	//up to a memory refresh slot.
	if(!insideActiveScanRow || !displayEnabled)
	{
		if((nextOperation.operation != RenderOp::REFRESH) && (nextOperation.operation != RenderOp::ACC_SLOT))
		{
			nextOperation.operation = RenderOp::ACC_SLOT;
			nextOperation.index = 0;
		}
	}

	//Perform the next operation
	switch(nextOperation.operation)
	{
	case RenderOp::HSCROLL:{
		//Read register settings which affect which hscroll data is loaded
		unsigned int hscrollDataBase = M5GetHScrollDataBase(accessTarget);
		bool hscrState = M5GetHSCR(accessTarget);
		bool lscrState = M5GetLSCR(accessTarget);

		//Load the hscroll data into the hscroll data cache
		DigitalRenderReadHscrollData(renderDigitalCurrentRow, hscrollDataBase, hscrState, lscrState, renderLayerAHscrollPatternDisplacement, renderLayerBHscrollPatternDisplacement, renderLayerAHscrollMappingDisplacement, renderLayerBHscrollMappingDisplacement);
		break;}
	case RenderOp::MAPPING_A:{
		//Calculate the index number of the current 2-cell column that's being rendered.
		//Note that we subtract 1 from the index number specified in the operation table.
		//The first mapping data we read is for the left-scrolled 2-cell column, which
		//reads its mapping data from cell block -1.
		unsigned int renderDigitalCurrentColumn = (nextOperation.index - 1) / cellsPerColumn;

		//Read registers which affect the read of vscroll data
		bool vscrState = M5GetVSCR(accessTarget);

		////Read vscroll data for layers A and B
		//DigitalRenderReadVscrollData(renderDigitalCurrentColumn, vscrState, interlaceMode2Active, renderLayerAVscrollPatternDisplacement, renderLayerBVscrollPatternDisplacement, renderLayerAVscrollMappingDisplacement, renderLayerBVscrollMappingDisplacement);

		//Read vscroll data for layer A
		DigitalRenderReadVscrollDataNew(renderDigitalCurrentColumn, 0, vscrState, interlaceMode2Active, renderLayerAVscrollPatternDisplacement, renderLayerAVscrollMappingDisplacement, renderVSRAMCachedRead);

		//Read the current window register settings
		bool windowAlignedRight = M5GetWindowRightAligned(accessTarget);
		bool windowAlignedBottom = M5GetWindowBottomAligned(accessTarget);
		unsigned int windowBasePointX = M5GetWindowBasePointX(accessTarget);
		unsigned int windowBasePointY = M5GetWindowBasePointY(accessTarget);

		//Calculate the screen position of the window layer
		unsigned int windowStartCellX = (windowAlignedRight)? windowBasePointX: 0;
		unsigned int windowEndCellX = (windowAlignedRight)? 9999: windowBasePointX;
		unsigned int windowStartCellY = (windowAlignedBottom)? windowBasePointY: 0;
		unsigned int windowEndCellY = (windowAlignedBottom)? 9999: windowBasePointY;

		//Determine if the window is active in the current column and row
		//##TODO## Perform hardware tests on changing the window register state mid-line.
		//We need to determine how the VDP responds, in particular, how the window
		//distortion bug behaves when the window is enabled or disabled mid-line.
		const unsigned int rowsToDisplayPerCell = 8;
		unsigned int currentCellRow = renderDigitalCurrentRow / rowsToDisplayPerCell;
		bool windowActiveInThisColumn = false;
		if(nextOperation.index > 0)
		{
			windowActiveInThisColumn = ((renderDigitalCurrentColumn >= windowStartCellX) && (renderDigitalCurrentColumn < windowEndCellX))
			                        || ((currentCellRow >= windowStartCellY) && (currentCellRow < windowEndCellY));
			renderWindowActiveCache[renderDigitalCurrentColumn] = windowActiveInThisColumn;
		}

		//Calculate the effective screen row number to use when calculating the mapping
		//data row for this line. Note that the screen row number is just used directly
		//under normal circumstances, but when interlace mode 2 is active, there are
		//effectively double the number of vertical screen rows, with all the odd rows in
		//one frame, and all the even rows in another frame, so if interlace mode 2 is
		//active we double the screen row number and select either odd or even rows based
		//on the current state of the odd flag.
		unsigned int screenRowIndex = renderDigitalCurrentRow;
		if(interlaceMode2Active)
		{
			screenRowIndex *= 2;
			if(renderDigitalOddFlagSet)
			{
				++screenRowIndex;
			}
		}

		//Read the correct mapping data for this column, taking into account whether that
		//mapping data should be read from the window mapping table or the layer A mapping
		//table.
		if(windowActiveInThisColumn)
		{
			//Read register settings which affect which mapping data is loaded
			unsigned int nameTableBase = M5GetNameTableBaseWindow(accessTarget);

			//The window mapping table dimensions are determined based on the H40 screen
			//mode state. If H40 mode is active, the window mapping table is 64 cells
			//wide, otherwise, it is 32 cells wide. We emulate that here by calculating
			//the mapping table dimensions ourselves and passing this data in when reading
			//the window layer mapping data.
			unsigned int hszState = (renderDigitalScreenModeRS1Active)? 0x1: 0x0;
			unsigned int vszState = 0x0;

			//Read window mapping data
			DigitalRenderReadMappingDataPair(screenRowIndex, renderDigitalCurrentColumn, interlaceMode2Active, nameTableBase, 0, 0, 0, hszState, vszState, renderMappingDataCacheLayerA[nextOperation.index], renderMappingDataCacheLayerA[nextOperation.index+1]);
		}
		else
		{
			//Read register settings which affect which mapping data is loaded
			unsigned int nameTableBase = M5GetNameTableBaseScrollA(accessTarget);
			unsigned int hszState = M5GetHSZ(accessTarget);
			unsigned int vszState = M5GetVSZ(accessTarget);

			//Read layer A mapping data
			DigitalRenderReadMappingDataPair(screenRowIndex, renderDigitalCurrentColumn, interlaceMode2Active, nameTableBase, renderLayerAHscrollMappingDisplacement, renderLayerAVscrollMappingDisplacement, renderLayerAVscrollPatternDisplacement, hszState, vszState, renderMappingDataCacheLayerA[nextOperation.index], renderMappingDataCacheLayerA[nextOperation.index+1]);
		}

		break;}
	case RenderOp::PATTERN_A:{
		//Calculate the vertical pattern displacement value to use to calculate the row of
		//the pattern data to read. The only purpose of this code is to handle window
		//layer support. In regions where the window layer is active, we need to disable
		//vertical scrolling, so if this cell number is within the window region of the
		//display, we force the vertical pattern displacement to 0, otherwise, we use the
		//calculated vertical pattern displacement based on the layer A vscroll data.
		unsigned int verticalPatternDisplacement = renderLayerAVscrollPatternDisplacement;
		if(nextOperation.index >= 2)
		{
			unsigned int renderDigitalCurrentColumn = (nextOperation.index - 2) / cellsPerColumn;
			if(renderWindowActiveCache[renderDigitalCurrentColumn])
			{
				verticalPatternDisplacement = (interlaceMode2Active && renderDigitalOddFlagSet)? 1: 0;
			}
		}

		//Calculate the pattern row number to read for the selected pattern, based on the
		//current row being drawn on the screen, and the vertical pattern displacement due
		//to vscroll. We also factor in interlace mode 2 support here by doubling the
		//screen row number where interlace mode 2 is active. The state of the odd flag is
		//factored in when the vscroll data is read.
		const unsigned int rowsToDisplayPerTile = 8;
		unsigned int screenPatternRowIndex = renderDigitalCurrentRow % rowsToDisplayPerTile;
		if(interlaceMode2Active)
		{
			screenPatternRowIndex *= 2;
			if(renderDigitalOddFlagSet)
			{
				++screenPatternRowIndex;
			}
		}
		const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
		unsigned int patternRowNumberNoFlip = (screenPatternRowIndex + verticalPatternDisplacement) % rowsPerTile;

		//Read layer A pattern row
		DigitalRenderReadPatternDataRow(patternRowNumberNoFlip, 0, interlaceMode2Active, renderMappingDataCacheLayerA[nextOperation.index], renderPatternDataCacheLayerA[nextOperation.index]);
		break;}
	case RenderOp::MAPPING_B:{
		//Calculate the index number of the current 2-cell column that's being rendered.
		//Note that we subtract 1 from the index number specified in the operation table.
		//The first mapping data we read is for the left-scrolled 2-cell column, which
		//reads its mapping data from cell block -1.
		unsigned int renderDigitalCurrentColumn = (nextOperation.index - 1) / cellsPerColumn;

		//Read registers which affect the read of vscroll data
		bool vscrState = M5GetVSCR(accessTarget);

		//Read vscroll data for layer B
		DigitalRenderReadVscrollDataNew(renderDigitalCurrentColumn, 1, vscrState, interlaceMode2Active, renderLayerBVscrollPatternDisplacement, renderLayerBVscrollMappingDisplacement, renderVSRAMCachedRead);

		//Calculate the effective screen row number to use when calculating the mapping
		//data row for this line. Note that the screen row number is just used directly
		//under normal circumstances, but when interlace mode 2 is active, there are
		//effectively double the number of vertical screen rows, with all the odd rows in
		//one frame, and all the even rows in another frame, so if interlace mode 2 is
		//active we double the screen row number and select either odd or even rows based
		//on the current state of the odd flag.
		unsigned int screenRowIndex = renderDigitalCurrentRow;
		if(interlaceMode2Active)
		{
			screenRowIndex *= 2;
			if(renderDigitalOddFlagSet)
			{
				++screenRowIndex;
			}
		}

		//Read register settings which affect which mapping data is loaded
		unsigned int nameTableBase = M5GetNameTableBaseScrollB(accessTarget);
		unsigned int hszState = M5GetHSZ(accessTarget);
		unsigned int vszState = M5GetVSZ(accessTarget);

		//Read layer B mapping data
		DigitalRenderReadMappingDataPair(screenRowIndex, renderDigitalCurrentColumn, interlaceMode2Active, nameTableBase, renderLayerBHscrollMappingDisplacement, renderLayerBVscrollMappingDisplacement, renderLayerBVscrollPatternDisplacement, hszState, vszState, renderMappingDataCacheLayerB[nextOperation.index], renderMappingDataCacheLayerB[nextOperation.index+1]);
		break;}
	case RenderOp::PATTERN_B:{
		//Calculate the pattern row number to read for the selected pattern, based on the
		//current row being drawn on the screen, and the vertical pattern displacement due
		//to vscroll. We also factor in interlace mode 2 support here by doubling the
		//screen row number where interlace mode 2 is active. The state of the odd flag is
		//factored in when the vscroll data is read.
		const unsigned int rowsToDisplayPerTile = 8;
		unsigned int screenPatternRowIndex = renderDigitalCurrentRow % rowsToDisplayPerTile;
		if(interlaceMode2Active)
		{
			screenPatternRowIndex *= 2;
			if(renderDigitalOddFlagSet)
			{
				++screenPatternRowIndex;
			}
		}
		const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
		unsigned int patternRowNumberNoFlip = (screenPatternRowIndex + renderLayerBVscrollPatternDisplacement) % rowsPerTile;

		//Read layer B pattern row
		DigitalRenderReadPatternDataRow(patternRowNumberNoFlip, 0, interlaceMode2Active, renderMappingDataCacheLayerB[nextOperation.index], renderPatternDataCacheLayerB[nextOperation.index]);
		break;}
	case RenderOp::MAPPING_S:{
		//Read sprite mapping data and decode sprite cells for the next sprite to display
		//on the current scanline.
		if(renderSpriteDisplayCacheCurrentIndex < renderSpriteDisplayCacheEntryCount)
		{
			//Read the sprite table base address register
			unsigned int spriteTableBaseAddress = M5GetNameTableBaseSprite(accessTarget);

			//Obtain a reference to the corresponding sprite display cache entry for this
			//sprite.
			SpriteDisplayCacheEntry& spriteDisplayCacheEntry = renderSpriteDisplayCache[renderSpriteDisplayCacheCurrentIndex];

			//Build a sprite cell list for the current sprite
			DigitalRenderBuildSpriteCellList(renderSpriteDisplayCacheCurrentIndex, spriteTableBaseAddress, interlaceMode2Active, renderDigitalScreenModeRS1Active, renderSpriteDotOverflow, spriteDisplayCacheEntry, renderSpriteDisplayCellCacheEntryCount, renderSpriteDisplayCellCache);

			//Advance to the next sprite mapping entry
			++renderSpriteDisplayCacheCurrentIndex;
		}
		break;}
	case RenderOp::PATTERN_S:{
		//##TODO## We theorize that the sprite cache is traversed here, during the sprite
		//pattern read steps, to generate the list of sprite numbers that are active on
		//the next line. This makes sense for a number of reasons, namely, that it seems
		//that the sprite list must have been fully traversed before the active cells are
		//rendered, otherwise the sprite mapping reads may begin before a single sprite is
		//encountered in the sprite cache, eg, consider the case where all sprites for a
		//given line appear at the very end of the sprite list. In order for the VDP to
		//reliably know the first sprite for a given line, it must have parsed the full
		//list of sprites previously.
		//##FIX## Note that if the sprite cache is traversed here, two entries from the
		//cache must be processed for every one sprite pattern block read. This means one
		//value is read for each pixel clock cycle, while we currently only advance the
		//digital render process once every two pixel clock cycles. We process two sprite
		//cache entries per pattern read slot to make up for that here.
		DigitalRenderBuildSpriteList(renderSpriteNextSpriteRow, interlaceMode2Active, renderDigitalScreenModeRS1Active, renderSpriteNextAttributeTableEntryToRead, renderSpriteSearchComplete, renderSpriteOverflow, renderSpriteDisplayCacheEntryCount, renderSpriteDisplayCache);
		DigitalRenderBuildSpriteList(renderSpriteNextSpriteRow, interlaceMode2Active, renderDigitalScreenModeRS1Active, renderSpriteNextAttributeTableEntryToRead, renderSpriteSearchComplete, renderSpriteOverflow, renderSpriteDisplayCacheEntryCount, renderSpriteDisplayCache);

		//Read sprite pattern data for the next sprite cell to display on the current
		//scanline.
		//##TODO## Check if the sprite collision flag is still set for sprite pixels that
		//are masked.
		//##TODO## We know that if the display is disabled during active scan, sprite
		//pattern data which was supposed to be read during the disabled region isn't
		//skipped, but rather, the entire sprite pattern data load queue remains in its
		//current state, and the next sprite pattern data block to read is retrieved from
		//the queue when a slot becomes available. This means we can't use a simple index
		//number to determine which sprite data to read, but rather, we need to keep a
		//running index of the sprite pattern data we're up to.
		if(renderSpriteDisplayCellCacheCurrentIndex < renderSpriteDisplayCellCacheEntryCount)
		{
			//Obtain a reference to the corresponding sprite cell and sprite display cache
			//entries for this sprite cell.
			SpriteCellDisplayCacheEntry& spriteCellDisplayCacheEntry = renderSpriteDisplayCellCache[renderSpriteDisplayCellCacheCurrentIndex];
			SpriteDisplayCacheEntry& spriteDisplayCacheEntry = renderSpriteDisplayCache[spriteCellDisplayCacheEntry.spriteDisplayCacheIndex];

			//Read pattern data for the sprite cell
			DigitalRenderReadPatternDataRow(spriteCellDisplayCacheEntry.patternRowOffset, spriteCellDisplayCacheEntry.patternCellOffset, interlaceMode2Active, spriteDisplayCacheEntry.mappingData, spriteCellDisplayCacheEntry.patternData);

			//Read all used bits of the horizontal position data for this sprite
			Data spritePosH(9);
			spritePosH = spriteDisplayCacheEntry.hpos;

			//Take into account sprite masking
			//##TODO## Add a much longer comment here
			if(spritePosH == 0)
			{
				if(nonSpriteMaskCellEncountered || renderSpriteDotOverflowPreviousLine)
				{
					renderSpriteMaskActive = true;
				}
			}
			else
			{
				//Since we've now encountered at least one sprite pattern cell which was
				//not a mask sprite cell, set a flag indicating that that we've
				//encountered a non mask cell. This is required in order to correctly
				//support sprite masking.
				nonSpriteMaskCellEncountered = true;
			}

			//If this sprite cell was not masked, load visible pixel information into the
			//sprite pixel buffer.
			//##TODO## Greatly improve commenting here.
			if(!renderSpriteMaskActive)
			{
				const unsigned int spritePosScreenStartH = 0x80;
				const unsigned int cellPixelWidth = 8;
				for(unsigned int cellPixelIndex = 0; cellPixelIndex < cellPixelWidth; ++cellPixelIndex)
				{
					//Check that this sprite pixel is within the visible screen boundaries
					unsigned int spritePixelH = spritePosH.GetData() + (spriteCellDisplayCacheEntry.spriteCellColumnNo * cellPixelWidth) + cellPixelIndex;
					if((spritePixelH >= spritePosScreenStartH) && ((spritePixelH - spritePosScreenStartH) < spritePixelBufferSize))
					{
						//Check that a non-transparent sprite pixel has not already been
						//written to this pixel location. Higher priority sprites are
						//listed first, with lower priority sprites appearing under higher
						//priority sprites, so if a non-transparent sprite pixel has
						//already been output to this location, the lower priority sprite
						//pixel is discarded, and the sprite collision flag is set in the
						//status register.
						unsigned int spritePixelBufferIndex = (spritePixelH - spritePosScreenStartH);
						SpritePixelBufferEntry& spritePixelBufferEntry = spritePixelBuffer[renderSpritePixelBufferDigitalRenderPlane][spritePixelBufferIndex];
						if(!spritePixelBufferEntry.entryWritten || (spritePixelBufferEntry.paletteIndex == 0))
						{
							//Mapping (Pattern Name) data format:
							//-----------------------------------------------------------------
							//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
							//|---------------------------------------------------------------|
							//|Pri|PalRow |VF |HF |              Pattern Number               |
							//-----------------------------------------------------------------
							//Pri:    Priority Bit
							//PalRow: The palette row number to use when displaying the pattern data
							//VF:     Vertical Flip
							//HF:     Horizontal Flip
							spritePixelBufferEntry.entryWritten = true;
							spritePixelBufferEntry.layerPriority = spriteDisplayCacheEntry.mappingData.GetBit(15);
							spritePixelBufferEntry.paletteLine = spriteDisplayCacheEntry.mappingData.GetDataSegment(13, 2);
							spritePixelBufferEntry.paletteIndex = DigitalRenderReadPixelIndex(spriteCellDisplayCacheEntry.patternData, spriteDisplayCacheEntry.mappingData.GetBit(11), cellPixelIndex);
						}
						else
						{
							renderSpriteCollision = true;
						}
					}
				}
			}

			//Advance to the next sprite cell entry
			++renderSpriteDisplayCellCacheCurrentIndex;
		}
		break;}
	case RenderOp::ACC_SLOT:
		//Since we've reached an external access slot, changes may now be made to VRAM or
		//VSRAM, so we need to advance the VRAM and VSRAM buffers up to this time so any
		//changes which occur at this access slot can take effect.
		vram->AdvanceBySession(renderDigitalMclkCycleProgress, vramSession, vramTimesliceCopy);
		vsram->AdvanceBySession(renderDigitalMclkCycleProgress, vsramSession, vsramTimesliceCopy);
		spriteCache->AdvanceBySession(renderDigitalMclkCycleProgress, spriteCacheSession, spriteCacheTimesliceCopy);

		//We use some special flags in the external access slots to indicate the point at
		//which to clear the internal state data for sprites, and initialize the
		//structures to begin reading sprite data for the next line.
		//##TODO## Ensure that on a reset, this data is initialized correctly
		if(nextOperation.index == 1)
		{
			renderSpriteSearchComplete = false;
			renderSpriteOverflow = false;
			renderSpriteNextAttributeTableEntryToRead = 0;
			renderSpriteDisplayCacheEntryCount = 0;
			renderSpriteDisplayCellCacheCurrentIndex = 0;
			renderSpriteDisplayCacheCurrentIndex = 0;
			//Calculate the row number of the next line of sprite data to fetch. Note that
			//we calculate the row number by adding 2 to the current row. We add a value
			//of 1 to the current row number because sprites are first parsed one line
			//before the line on which they are displayed, since there are three separate
			//rendering phases for sprites, the first of which is a full traversal of the
			//cached sprite attribute table data, which occurs on the row before the row
			//at which we're searching for sprites to render for. We add another 1 to the
			//row number because at the point where this access slot occurs, the vcounter
			//is about to be incremented in just a few cycles.
			if(!insideActiveScanRow)
			{
				renderSpriteNextSpriteRow = 1;
			}
			else
			{
				renderSpriteNextSpriteRow = renderDigitalCurrentRow + 2;
				if(renderSpriteNextSpriteRow >= renderDigitalRowCount)
				{
					renderSpriteNextSpriteRow = 0;
				}
			}
		}
		else if(nextOperation.index == 2)
		{
			//##TODO## Test if the first line of the display always acts like a dot
			//overflow didn't occur on the previous line, or if the VDP remembers if a dot
			//overflow occurred on the very last line of the display when loading sprites
			//for the first line of the display.
			renderSpriteDotOverflowPreviousLine = renderSpriteDotOverflow;
			renderSpriteDotOverflow = false;
			renderSpriteDisplayCellCacheEntryCount = 0;
			nonSpriteMaskCellEncountered = false;
			renderSpriteMaskActive = false;
			renderSpriteCollision = false;

			//Advance the sprite pixel buffer plane
			renderSpritePixelBufferAnalogRenderPlane = (renderSpritePixelBufferAnalogRenderPlane + 1) % renderSpritePixelBufferPlaneCount;
			renderSpritePixelBufferDigitalRenderPlane = (renderSpritePixelBufferDigitalRenderPlane + 1) % renderSpritePixelBufferPlaneCount;
		}
		break;
	case RenderOp::REFRESH:
		//Nothing to do on a memory refresh cycle
		break;
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::UpdateAnalogRenderProcess(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings)
{
	bool outputNothing = false;
	bool forceOutputBackgroundPixel = false;

	//If the digital vcounter has already been incremented, but we haven't reached the end
	//of the analog line yet, move the digital vcounter back one step so we can calculate
	//the analog line number.
	unsigned int renderDigitalVCounterPosIncrementAtHBlank = renderDigitalVCounterPos;
	if((renderDigitalHCounterPos >= hscanSettings.vcounterIncrementPoint) && (renderDigitalHCounterPos < hscanSettings.hblankSetPoint))
	{
		renderDigitalVCounterPosIncrementAtHBlank = renderDigitalVCounterPosPreviousLine;
	}

	//Use the current VCounter data to determine which data is being displayed on this
	//row, based on the current screen mode settings.
	bool insidePixelBufferRegion = true;
	bool insideActiveScanVertically = false;
	unsigned int renderAnalogCurrentRow = 0;

	if((renderDigitalVCounterPosIncrementAtHBlank >= vscanSettings.activeDisplayVCounterFirstValue) && (renderDigitalVCounterPosIncrementAtHBlank <= vscanSettings.activeDisplayVCounterLastValue))
	{
		//We're inside the active display region
		renderAnalogCurrentRow = vscanSettings.topBorderLineCount + (renderDigitalVCounterPosIncrementAtHBlank - vscanSettings.activeDisplayVCounterFirstValue);
		insideActiveScanVertically = true;
	}
	else
	{
		//Check if we're in a border region, or in the blanking region
		if((renderDigitalVCounterPosIncrementAtHBlank >= vscanSettings.topBorderVCounterFirstValue) && (renderDigitalVCounterPosIncrementAtHBlank <= vscanSettings.topBorderVCounterLastValue))
		{
			//We're in the top border. In this case, we need to force the pixel output to
			//the current backdrop colour.
			renderAnalogCurrentRow = renderDigitalVCounterPosIncrementAtHBlank - vscanSettings.topBorderVCounterFirstValue;
			forceOutputBackgroundPixel = true;
		}
		else if((renderDigitalVCounterPosIncrementAtHBlank >= vscanSettings.bottomBorderVCounterFirstValue) && (renderDigitalVCounterPosIncrementAtHBlank <= vscanSettings.bottomBorderVCounterLastValue))
		{
			//We're in the bottom border. In this case, we need to force the pixel output
			//to the current backdrop colour.
			renderAnalogCurrentRow = vscanSettings.topBorderLineCount + vscanSettings.activeDisplayLineCount + (renderDigitalVCounterPosIncrementAtHBlank - vscanSettings.bottomBorderVCounterFirstValue);
			forceOutputBackgroundPixel = true;
		}
		else
		{
			//We're in a blanking region. In this case, we need to force the pixel output
			//to black.
			insidePixelBufferRegion = false;
			outputNothing = true;
		}
	}

	//Use the current HCounter data to determine which data is next to be displayed on
	//this line, based on the current screen mode settings.
	bool insideActiveScanHorizontally = false;
	unsigned int renderAnalogCurrentPixel = 0;
	unsigned int activeScanPixelIndex = 0;
	if((renderDigitalHCounterPos >= hscanSettings.activeDisplayHCounterFirstValue) && (renderDigitalHCounterPos <= hscanSettings.activeDisplayHCounterLastValue))
	{
		//We're inside the active display region. Calculate the pixel number of the
		//current pixel to output on this update cycle.
		renderAnalogCurrentPixel = hscanSettings.leftBorderPixelCount + (renderDigitalHCounterPos - hscanSettings.activeDisplayHCounterFirstValue);
		activeScanPixelIndex = (renderDigitalHCounterPos - hscanSettings.activeDisplayHCounterFirstValue);
		insideActiveScanHorizontally = true;
	}
	else
	{
		//Check if we're in a border region, or in the blanking region
		if((renderDigitalHCounterPos >= hscanSettings.leftBorderHCounterFirstValue) && (renderDigitalHCounterPos <= hscanSettings.leftBorderHCounterLastValue))
		{
			//We're in the left border. In this case, we need to force the pixel output to
			//the current backdrop colour.
			renderAnalogCurrentPixel = renderDigitalHCounterPos - hscanSettings.leftBorderHCounterFirstValue;
			forceOutputBackgroundPixel = true;
		}
		else if((renderDigitalHCounterPos >= hscanSettings.rightBorderHCounterFirstValue) && (renderDigitalHCounterPos <= hscanSettings.rightBorderHCounterLastValue))
		{
			//We're in the right border. In this case, we need to force the pixel output
			//to the current backdrop colour.
			renderAnalogCurrentPixel = hscanSettings.leftBorderPixelCount + hscanSettings.activeDisplayPixelCount + (renderDigitalHCounterPos - hscanSettings.rightBorderHCounterFirstValue);
			forceOutputBackgroundPixel = true;
		}
		else
		{
			//We're in a blanking region. In this case, we need to force the pixel output
			//to black.
			insidePixelBufferRegion = false;
			outputNothing = true;
		}
	}

	//Read the display enable register. If this register is cleared, the output for this
	//update step is forced to the background colour, and free access to VRAM is
	//permitted.
	bool displayEnabled = M5GetDisplayEnabled(accessTarget);
	if(!displayEnabled)
	{
		forceOutputBackgroundPixel = true;
	}

	//##TODO## Handle reg 0, bit 0, which completely disables video output while it is
	//set. In our case here, we should force the output colour to black.
	//##TODO## Test on the hardware if we should disable the actual rendering process
	//and allow free access to VRAM if reg 0 bit 0 is set, or if this bit only
	//disables the analog video output.

	//Determine the palette line and index numbers and the shadow/highlight state for this
	//pixel.
	bool shadow = false;
	bool highlight = false;
	unsigned int paletteLine = 0;
	unsigned int paletteIndex = 0;
	if(outputNothing)
	{
		//If a pixel is being forced to black, we currently don't have anything to do
		//here.
	}
	else if(forceOutputBackgroundPixel)
	{
		//If this pixel is being forced to the background colour, read the current
		//background palette index and line data.
		paletteLine = M5GetBackgroundColorPalette(accessTarget);
		paletteIndex = M5GetBackgroundColorIndex(accessTarget);
	}
	else if(insideActiveScanVertically && insideActiveScanHorizontally)
	{
		//If we're displaying a pixel in the active display region, determine the correct
		//palette index for this pixel.

		//Collect the pattern and priority data for this pixel from each of the various
		//layers.
		//Mapping (Pattern Name) data format:
		//-----------------------------------------------------------------
		//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//|---------------------------------------------------------------|
		//|Pri|PalRow |VF |HF |              Pattern Number               |
		//-----------------------------------------------------------------
		//Pri:    Priority Bit
		//PalRow: The palette row number to use when displaying the pattern data
		//VF:     Vertical Flip
		//HF:     Horizontal Flip
		unsigned int paletteLineData[4];
		unsigned int paletteIndexData[4];
		bool layerPriority[4];

		//Decode the sprite mapping and pattern data
		layerPriority[LAYER_SPRITE] = false;
		paletteIndexData[LAYER_SPRITE] = 0;
		if(spritePixelBuffer[renderSpritePixelBufferAnalogRenderPlane][activeScanPixelIndex].entryWritten)
		{
			const SpritePixelBufferEntry& spritePixelBufferEntry = spritePixelBuffer[renderSpritePixelBufferAnalogRenderPlane][activeScanPixelIndex];
			layerPriority[LAYER_SPRITE] = spritePixelBufferEntry.layerPriority;
			paletteLineData[LAYER_SPRITE] = spritePixelBufferEntry.paletteLine;
			paletteIndexData[LAYER_SPRITE] = spritePixelBufferEntry.paletteIndex;
		}

		//Decode the layer A mapping and pattern data
		unsigned int screenCellNo = activeScanPixelIndex / (cellBlockSizeH);
		unsigned int screenColumnNo = screenCellNo / cellsPerColumn;
		bool windowEnabledAtCell = renderWindowActiveCache[screenColumnNo];
		if(windowEnabledAtCell)
		{
			//Read the pixel data from the window plane
			unsigned int mappingNumberWindow = ((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) / cellBlockSizeH;
			unsigned int pixelNumberWindow = ((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) % cellBlockSizeH;
			const Data& windowMappingData = renderMappingDataCacheLayerA[mappingNumberWindow];
			layerPriority[LAYER_LAYERA] = windowMappingData.GetBit(15);
			paletteLineData[LAYER_LAYERA] = windowMappingData.GetDataSegment(13, 2);
			paletteIndexData[LAYER_LAYERA] = DigitalRenderReadPixelIndex(renderPatternDataCacheLayerA[mappingNumberWindow], windowMappingData.GetBit(11), pixelNumberWindow);
		}
		else
		{
			//Calculate the mapping number and pixel number within the layer
			unsigned int scrolledMappingNumberLayerA = (((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) - renderLayerAHscrollPatternDisplacement) / cellBlockSizeH;
			unsigned int scrolledPixelNumberLayerA = (((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) - renderLayerAHscrollPatternDisplacement) % cellBlockSizeH;

			//Take the window distortion bug into account. Due to an implementation quirk,
			//the real VDP apparently reads the window mapping and pattern data at the
			//start of the normal cell block reads, and doesn't use the left scrolled
			//2-cell block to read window data. As a result, the VDP doesn't have enough
			//VRAM access slots left to handle a scrolled playfield when transitioning
			//from a left-aligned window to the scrolled layer A plane. As a result of
			//their implementation, the partially visible 2-cell region immediately
			//following the end of the window uses the mapping and pattern data of the
			//column that follows it. Our implementation here has a similar effect, except
			//that we would fetch the mapping and pattern data for the 2-cell block
			//immediately before it, IE from the last column of the window region. We
			//adjust the mapping number here to correctly fetch the next column mapping
			//data instead when a partially visible column follows a left-aligned window,
			//as would the real hardware.
			//##TODO## Confirm through hardware tests that window mapping and pattern data
			//is read after the left scrolled 2-cell block.
			unsigned int currentScreenColumnPixelIndex = activeScanPixelIndex - (cellBlockSizeH * cellsPerColumn * screenColumnNo);
			unsigned int distortedPixelCount = renderLayerAHscrollPatternDisplacement + ((scrolledMappingNumberLayerA & 0x1) * cellBlockSizeH);
			if((screenColumnNo > 0) && renderWindowActiveCache[screenColumnNo-1] && (currentScreenColumnPixelIndex < distortedPixelCount))
			{
				scrolledMappingNumberLayerA += cellsPerColumn;
			}

			//Read the pixel data from the layer A plane
			const Data& layerAMappingData = renderMappingDataCacheLayerA[scrolledMappingNumberLayerA];
			layerPriority[LAYER_LAYERA] = layerAMappingData.GetBit(15);
			paletteLineData[LAYER_LAYERA] = layerAMappingData.GetDataSegment(13, 2);
			paletteIndexData[LAYER_LAYERA] = DigitalRenderReadPixelIndex(renderPatternDataCacheLayerA[scrolledMappingNumberLayerA], layerAMappingData.GetBit(11), scrolledPixelNumberLayerA);
		}

		//Decode the layer B mapping and pattern data
		unsigned int scrolledMappingNumberLayerB = (((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) - renderLayerBHscrollPatternDisplacement) / cellBlockSizeH;
		unsigned int scrolledPixelNumberLayerB = (((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) - renderLayerBHscrollPatternDisplacement) % cellBlockSizeH;
		const Data& layerBMappingData = renderMappingDataCacheLayerB[scrolledMappingNumberLayerB];
		layerPriority[LAYER_LAYERB] = layerBMappingData.GetBit(15);
		paletteLineData[LAYER_LAYERB] = layerBMappingData.GetDataSegment(13, 2);
		paletteIndexData[LAYER_LAYERB] = DigitalRenderReadPixelIndex(renderPatternDataCacheLayerB[scrolledMappingNumberLayerB], layerBMappingData.GetBit(11), scrolledPixelNumberLayerB);

		//Read the background palette settings
		layerPriority[LAYER_BACKGROUND] = false;
		paletteLineData[LAYER_BACKGROUND] = M5GetBackgroundColorPalette(accessTarget);
		paletteIndexData[LAYER_BACKGROUND] = M5GetBackgroundColorIndex(accessTarget);

		//Determine if any of the palette index values for any of the layers indicate a
		//transparent pixel.
		//##TODO## Consider renaming and reversing the logic of these flags to match the
		//comment above. The name of "found pixel" isn't very descriptive, and in the case
		//of the sprite layer "isPixelOpaque" could be misleading when the sprite pixel is
		//being used as an operator in shadow/highlight mode. A flag with a name like
		//isPixelTransparent would be much more descriptive.
		bool foundSpritePixel = (paletteIndexData[LAYER_SPRITE] != 0);
		bool foundLayerAPixel = (paletteIndexData[LAYER_LAYERA] != 0);
		bool foundLayerBPixel = (paletteIndexData[LAYER_LAYERB] != 0);

		//Read the shadow/highlight mode settings. Note that hardware tests have confirmed
		//that changes to this register take effect immediately, at any point in a line.
		//##TODO## Confirm whether shadow highlight is active in border areas
		//##TODO## Confirm whether shadow highlight is active when the display is disabled
		bool shadowHighlightEnabled = M5GetShadowHighlightEnabled(accessTarget);
		bool spriteIsShadowOperator = (paletteLineData[LAYER_SPRITE] == 3) && (paletteIndexData[LAYER_SPRITE] == 15);
		bool spriteIsHighlightOperator = (paletteLineData[LAYER_SPRITE] == 3) && (paletteIndexData[LAYER_SPRITE] == 14);

		//##NOTE## The following code is disabled, because we use a lookup table to cache
		//the result of layer priority calculations. This gives us a significant
		//performance boost. The code below is provided for future reference and debugging
		//purposes. This code should, in all instances, produce the same result as the
		//table lookup below.
		//Perform layer priority calculations, and determine the layer to use, as well as
		//the resulting state of the shadow and highlight bits.
		//unsigned int layerIndex;
		//bool shadow;
		//bool highlight;
		//CalculateLayerPriorityIndex(layerIndex, shadow, highlight, shadowHighlightEnabled, spriteIsShadowOperator, spriteIsHighlightOperator, foundSpritePixel, foundLayerAPixel, foundLayerBPixel, prioritySprite, priorityLayerA, priorityLayerB);

		//Encode the parameters for the layer priority calculation into an index value for
		//the priority lookup table.
		unsigned int priorityIndex = 0;
		priorityIndex |= (unsigned int)shadowHighlightEnabled << 8;
		priorityIndex |= (unsigned int)spriteIsShadowOperator << 7;
		priorityIndex |= (unsigned int)spriteIsHighlightOperator << 6;
		priorityIndex |= (unsigned int)foundSpritePixel << 5;
		priorityIndex |= (unsigned int)foundLayerAPixel << 4;
		priorityIndex |= (unsigned int)foundLayerBPixel << 3;
		priorityIndex |= (unsigned int)layerPriority[LAYER_SPRITE] << 2;
		priorityIndex |= (unsigned int)layerPriority[LAYER_LAYERA] << 1;
		priorityIndex |= (unsigned int)layerPriority[LAYER_LAYERB];

		//Lookup the pre-calculated layer priority from the lookup table. We use a lookup
		//table to eliminate branching, which should yield a significant performance
		//boost.
		unsigned int layerSelectionResult = layerPriorityLookupTable[priorityIndex];

		//Extract the layer index, shadow, and highlight data from the combined result
		//returned from the layer priority lookup table.
		unsigned int layerIndex = layerSelectionResult & 0x03;
		shadow = (layerSelectionResult & 0x08) != 0;
		highlight = (layerSelectionResult & 0x04) != 0;

		//Read the palette line and index to use for the selected layer
		paletteLine = paletteLineData[layerIndex];
		paletteIndex = paletteIndexData[layerIndex];
	}

	//If we just determined the palette line and index for a pixel within the active scan
	//region of the screen, initialize the sprite pixel buffer at the corresponding pixel
	//location, so that it is clear and ready to receive new sprite data on the next line.
	if(insideActiveScanVertically && insideActiveScanHorizontally)
	{
		spritePixelBuffer[renderSpritePixelBufferAnalogRenderPlane][activeScanPixelIndex].entryWritten = false;
	}

	//##TODO## Write a much longer comment here
	//##FIX## This comment doesn't actually reflect what we do right now
	//If a CRAM write has occurred at the same time as we're outputting this next
	//pixel, retrieve the value written to CRAM and output that value instead.
	//##TODO## Consider only advancing the CRAM buffer if we're passing the next write
	//time in this step. If we're not, there's no point doing the advance, since it
	//contains the same test we're doing here to decide if any work needs to be done.
	//##FIX## Correct a timing problem with all our buffers. Currently, writes to our
	//buffers are performed relative to the state update time, which factors in extra time
	//we may have rendered past the end of the timeslice in the last step. If we move past
	//the end of a timeslice, the following timeslice is shortened, and writes in that
	//timeslice are offset by the amount we ran over the last timeslice. We need to ensure
	//that the digital renderer knows about, and uses, these "stateLastUpdateMclk" values
	//to advance each timeslice, not the indicated timeslice length. This will ensure that
	//during the render process, values in the buffer will be committed at the same
	//relative time at which they were written.
	//##TODO## As part of the above, consider solving this issue more permanently, with an
	//upgrade to our timed buffers to roll writes past the end of a timeslice into the
	//next timeslice.
	if(cramSession.writeInfo.exists && (cramSession.nextWriteTime <= renderDigitalMclkCycleProgress))
	{
		const unsigned int paletteEntriesPerLine = 16;
		const unsigned int paletteEntrySize = 2;
		unsigned int cramWriteAddress = cramSession.writeInfo.writeAddress;
		paletteLine = (cramWriteAddress / paletteEntrySize) / paletteEntriesPerLine;
		paletteIndex = (cramWriteAddress / paletteEntrySize) % paletteEntriesPerLine;
	}

	//Now that we've advanced the analog render cycle and handled CRAM write flicker,
	//advance the committed state of the CRAM buffer. If a write occurred to CRAM at
	//the same time as this pixel was being drawn, it will now have been committed to
	//CRAM.
	cram->AdvanceBySession(renderDigitalMclkCycleProgress, cramSession, cramTimesliceCopy);

	//If we're drawing a pixel which is within the area of the screen we're rendering
	//pixel data for, output the pixel data to the image buffer.
	if(insidePixelBufferRegion)
	{
		//##TODO## Our new colour values are basically correct, assuming what is suspected
		//after analysis posted on SpritesMind, that the Mega Drive VDP never actually outputs
		//at full intensity. We haven't taken the apparent "ladder effect" into account
		//however. It is highly recommended that we perform our own tests on the hardware, and
		//make some comparisons between captured video output from the real system, and the
		//output from our emulator, when playing back on the same physical screen. If the
		//ladder effect is real and does have an effect on the way the intensity is perceived
		//on the screen, we should emulate it. We also need to confirm the maximum intensity
		//output by the VDP. A step size of 18 for example would get a max value of 252, which
		//would be more logical.
		//const unsigned char paletteEntryTo8Bit[8] = {0, 36, 73, 109, 146, 182, 219, 255};
		//const unsigned char paletteEntryTo8BitShadow[8] = {0, 18, 37, 55, 73, 91, 110, 128};
		//const unsigned char paletteEntryTo8BitHighlight[8] = {128, 146, 165, 183, 201, 219, 238, 255};
		const unsigned char paletteEntryTo8Bit[8] = {0, 34, 68, 102, 136, 170, 204, 238};
		const unsigned char paletteEntryTo8BitShadow[8] = {0, 17, 34, 51, 68, 85, 102, 119};
		const unsigned char paletteEntryTo8BitHighlight[8] = {119, 136, 153, 170, 187, 204, 221, 238};
		const unsigned int paletteEntriesPerLine = 16;
		const unsigned int paletteEntrySize = 2;

		//Calculate the address of the colour value to read from the palette
		unsigned int paletteEntryAddress = (paletteIndex + (paletteLine * paletteEntriesPerLine)) * paletteEntrySize;

		//Read the target palette entry
		Data paletteData(16);
		paletteData = (unsigned int)(cram->ReadCommitted(paletteEntryAddress+0) << 8) | (unsigned int)cram->ReadCommitted(paletteEntryAddress+1);

		//Decode the target palette entry, and extract the individual 7-bit R, G, and B
		//intensity values.
		//-----------------------------------------------------------------
		//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//|---------------------------------------------------------------|
		//| /   /   /   / |   Blue    | / |   Green   | / |    Red    | / |
		//-----------------------------------------------------------------
		unsigned int colorIntensityR = paletteData.GetDataSegment(1, 3);
		unsigned int colorIntensityG = paletteData.GetDataSegment(5, 3);
		unsigned int colorIntensityB = paletteData.GetDataSegment(9, 3);

		//If a reduced palette is in effect, due to bit 2 of register 1 being cleared,
		//only the lowest bit of each intensity value has any effect, and it selects
		//between half intensity and minimum intensity. Note that hardware tests have
		//shown that changes to this register take effect immediately, at any point in a
		//line.
		//##TODO## Confirm the interaction of shadow highlight mode with the palette
		//select bit.
		//##TODO## Confirm the mapping of intensity values when the palette select bit is
		//cleared.
		if(!M5GetPSEnabled(accessTarget))
		{
			colorIntensityR = (colorIntensityR & 0x01) << 2;
			colorIntensityG = (colorIntensityG & 0x01) << 2;
			colorIntensityB = (colorIntensityB & 0x01) << 2;
		}

		//Convert the palette data to a 24-bit RGB triple and write it to the image buffer
		//##TODO## As an optimization, use a combined lookup table for colour value
		//decoding, and eliminate the branching logic here.
		//##TODO## Add bounds checking to the array here
//		ImageBufferColorEntry& imageBufferEntry = imageBuffer[renderAnalogCurrentPixel + (renderAnalogCurrentRow * imageBufferWidth)];
		//##FIX## We're grafting the old image buffer into our new render code here for
		//temporary testing purposes. Once the new render code is tested and online,
		//modify the image window to use the new buffer.
		unsigned int imageBufferPixelIndex = (renderAnalogCurrentPixel + ((312 - renderAnalogCurrentRow) * imageBufferWidth)) * 4;
		//##DEBUG##
		if(imageBufferPixelIndex >= (imageWidth * imageHeight * 4))
		{
			std::wcout << "ERROR! Buffer overflow on image buffer\n";
			std::wcout << renderAnalogCurrentPixel << ' ' << renderAnalogCurrentRow << '\n';
		}
		else
		{
			ImageBufferColorEntry& imageBufferEntry = *((ImageBufferColorEntry*)&image[drawingImageBufferPlane][imageBufferPixelIndex]);
			if(outputNothing)
			{
				//If this pixel was forced to black, IE, if bit 0 of reg 0 is set, force the
				//output of this pixel to black.
				//##TODO## Do hardware testing to confirm exactly what effect setting this bit
				//has on all areas of VDP function. Does it affect the digital operation of
				//the VDP, or just the analog output? Does it affect hsync/vsync output lines
				//as well, or just color output?
				imageBufferEntry.r = 0;
				imageBufferEntry.g = 0;
				imageBufferEntry.b = 0;
			}
			else if(shadow && !highlight)
			{
				imageBufferEntry.r = paletteEntryTo8BitShadow[colorIntensityR];
				imageBufferEntry.g = paletteEntryTo8BitShadow[colorIntensityG];
				imageBufferEntry.b = paletteEntryTo8BitShadow[colorIntensityB];
			}
			else if(highlight && !shadow)
			{
				imageBufferEntry.r = paletteEntryTo8BitHighlight[colorIntensityR];
				imageBufferEntry.g = paletteEntryTo8BitHighlight[colorIntensityG];
				imageBufferEntry.b = paletteEntryTo8BitHighlight[colorIntensityB];
			}
			else
			{
				imageBufferEntry.r = paletteEntryTo8Bit[colorIntensityR];
				imageBufferEntry.g = paletteEntryTo8Bit[colorIntensityG];
				imageBufferEntry.b = paletteEntryTo8Bit[colorIntensityB];
			}
			imageBufferEntry.a = 0xFF;
		}
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderReadHscrollData(unsigned int screenRowNumber, unsigned int hscrollDataBase, bool hscrState, bool lscrState, unsigned int& layerAHscrollPatternDisplacement, unsigned int& layerBHscrollPatternDisplacement, unsigned int& layerAHscrollMappingDisplacement, unsigned int& layerBHscrollMappingDisplacement) const
{
	//Calculate the address of the hscroll data to read for this line
	unsigned int hscrollDataAddress = hscrollDataBase;
	//##TODO## Based on the EA logo for Populous, it appears that the state of LSCR is
	//ignored when HSCR is not set. We should confirm this on hardware.
	if(hscrState)
	{
		const unsigned int hscrollDataPairSize = 4;
		hscrollDataAddress += lscrState? (screenRowNumber * hscrollDataPairSize): (((screenRowNumber / renderDigitalBlockPixelSizeY) * renderDigitalBlockPixelSizeY) * hscrollDataPairSize);
	}

	//Read the hscroll data for this line
	//##TODO## Confirm the way scrolling data is interpreted through hardware tests. Eg,
	//does -1 actually scroll to the left by one place, or are 0 and -1 equivalent?
	//##TODO## According to the official documentation, the upper 6 bits of the hscroll
	//data are unused, and are allowed to be used by software to store whatever values
	//they want. Confirm this on hardware.
	unsigned int layerAHscrollOffset = ((unsigned int)vram->ReadCommitted(hscrollDataAddress+0) << 8) | (unsigned int)vram->ReadCommitted(hscrollDataAddress+1);
	unsigned int layerBHscrollOffset = ((unsigned int)vram->ReadCommitted(hscrollDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(hscrollDataAddress+3);

	//Break the hscroll data into its two component parts. The lower 4 bits represent a
	//displacement into the 2-cell column, or in other words, the displacement of the
	//starting pixel within each column, while the upper 6 bits represent an offset for
	//the column mapping data itself.
	//-----------------------------------------
	//| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------|
	//|  Column Shift Value   | Displacement  |
	//-----------------------------------------
	layerAHscrollPatternDisplacement = (layerAHscrollOffset & 0x00F);
	layerAHscrollMappingDisplacement = (layerAHscrollOffset & 0x3F0) >> 4;
	layerBHscrollPatternDisplacement = (layerBHscrollOffset & 0x00F);
	layerBHscrollMappingDisplacement = (layerBHscrollOffset & 0x3F0) >> 4;
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderReadVscrollData(unsigned int screenColumnNumber, bool vscrState, bool interlaceMode2Active, unsigned int& layerAVscrollPatternDisplacement, unsigned int& layerBVscrollPatternDisplacement, unsigned int& layerAVscrollMappingDisplacement, unsigned int& layerBVscrollMappingDisplacement) const
{
	//Calculate the address of the vscroll data to read for this block
	const unsigned int vscrollDataPairSize = 4;
	unsigned int vscrollDataAddress = vscrState? (screenColumnNumber * vscrollDataPairSize): 0;

	//##FIX## This is a temporary workaround for handling render reads past the end of the
	//VSRAM buffer. We need to do more hardware tests on this behaviour, and implement
	//this properly.
	unsigned int layerAVscrollOffset = 0;
	unsigned int layerBVscrollOffset = 0;
	if(vscrollDataAddress < 0x50)
	{
		//Read the vscroll data for this line. Note only the lower 10 bits are
		//effective, or the lower 11 bits in the case of interlace mode 2, due to the
		//scrolled address being wrapped to lie within the total field boundaries,
		//which never exceed 128 blocks.
		layerAVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+0) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+1);
		layerBVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+2) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+3);
	}

	//Break the vscroll data into its two component parts. The format of the vscroll data
	//varies depending on whether interlace mode 2 is active. When interlace mode 2 is not
	//active, the vscroll data is interpreted as a 10-bit value, where the lower 3 bits
	//represent a vertical shift on the pattern line for the selected block mapping, or in
	//other words, the displacement of the starting row within each pattern, while the
	//upper 7 bits represent an offset for the mapping data itself, like so:
	//------------------------------------------
	//| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0  |
	//|----------------------------------------|
	//|    Column Shift Value     |Displacement|
	//------------------------------------------
	//Where interlace mode 2 is active, pattern data is 8x16 pixels, not 8x8 pixels. In
	//this case, the vscroll data is treated as an 11-bit value, where the lower 4 bits
	//give the row offset, and the upper 7 bits give the mapping offset, like so:
	//---------------------------------------------
	//|10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|-------------------------------------------|
	//|    Column Shift Value     | Displacement  |
	//---------------------------------------------
	//Note that the unused upper bits in the vscroll data are simply discarded, since they
	//fall outside the maximum virtual playfield size for the mapping data. Since the
	//virtual playfield wraps, this means they have no effect.
	layerAVscrollPatternDisplacement = interlaceMode2Active? (layerAVscrollOffset & 0x00F):      (layerAVscrollOffset & 0x007);
	layerAVscrollMappingDisplacement = interlaceMode2Active? (layerAVscrollOffset & 0x7F0) >> 4: (layerAVscrollOffset & 0x3F8) >> 3;
	layerBVscrollPatternDisplacement = interlaceMode2Active? (layerBVscrollOffset & 0x00F):      (layerBVscrollOffset & 0x007);
	layerBVscrollMappingDisplacement = interlaceMode2Active? (layerBVscrollOffset & 0x7F0) >> 4: (layerBVscrollOffset & 0x3F8) >> 3;
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderReadVscrollDataNew(unsigned int screenColumnNumber, unsigned int layerNumber, bool vscrState, bool interlaceMode2Active, unsigned int& layerVscrollPatternDisplacement, unsigned int& layerVscrollMappingDisplacement, Data& vsramReadCache) const
{
	//Calculate the address of the vscroll data to read for this block
	const unsigned int vscrollDataLayerCount = 2;
	const unsigned int vscrollDataEntrySize = 2;
	unsigned int vscrollDataAddress = vscrState? (screenColumnNumber * vscrollDataLayerCount * vscrollDataEntrySize) + (layerNumber * vscrollDataEntrySize): (layerNumber * vscrollDataEntrySize);

	//##NOTE## This implements what appears to be the correct behaviour for handling reads
	//past the end of the VSRAM buffer during rendering. This can occur when horizontal
	//scrolling is applied along with vertical scrolling, in which case the leftmost
	//column can be reading data from a screen column of -1, wrapping around to the end of
	//the VSRAM buffer. In this case, the last successfully read value from the VSRAM
	//appears to be used as the read value. This also applies when performing manual reads
	//from VSRAM externally using the data port. See data port reads from VSRAM for more
	//info.
	//##TODO## This needs more through hardware tests, to definitively confirm the correct
	//behaviour.
	if(vscrollDataAddress < 0x50)
	{
		//Read the vscroll data for this line. Note only the lower 10 bits are
		//effective, or the lower 11 bits in the case of interlace mode 2, due to the
		//scrolled address being wrapped to lie within the total field boundaries,
		//which never exceed 128 blocks.
		vsramReadCache = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+0) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+1);
	}

	//Break the vscroll data into its two component parts. The format of the vscroll data
	//varies depending on whether interlace mode 2 is active. When interlace mode 2 is not
	//active, the vscroll data is interpreted as a 10-bit value, where the lower 3 bits
	//represent a vertical shift on the pattern line for the selected block mapping, or in
	//other words, the displacement of the starting row within each pattern, while the
	//upper 7 bits represent an offset for the mapping data itself, like so:
	//------------------------------------------
	//| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0  |
	//|----------------------------------------|
	//|    Column Shift Value     |Displacement|
	//------------------------------------------
	//Where interlace mode 2 is active, pattern data is 8x16 pixels, not 8x8 pixels. In
	//this case, the vscroll data is treated as an 11-bit value, where the lower 4 bits
	//give the row offset, and the upper 7 bits give the mapping offset, like so:
	//---------------------------------------------
	//|10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|-------------------------------------------|
	//|    Column Shift Value     | Displacement  |
	//---------------------------------------------
	//Note that the unused upper bits in the vscroll data are simply discarded, since they
	//fall outside the maximum virtual playfield size for the mapping data. Since the
	//virtual playfield wraps, this means they have no effect.
	layerVscrollPatternDisplacement = interlaceMode2Active? vsramReadCache.GetDataSegment(0, 4): vsramReadCache.GetDataSegment(0, 3);
	layerVscrollMappingDisplacement = interlaceMode2Active? vsramReadCache.GetDataSegment(4, 7): vsramReadCache.GetDataSegment(3, 7);
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderReadMappingDataPair(unsigned int screenRowNumber, unsigned int screenColumnNumber, bool interlaceMode2Active, unsigned int nameTableBaseAddress, unsigned int layerHscrollMappingDisplacement, unsigned int layerVscrollMappingDisplacement, unsigned int layerVscrollPatternDisplacement, unsigned int hszState, unsigned int vszState, Data& mappingDataEntry1, Data& mappingDataEntry2) const
{
	//The existence and contents of this table have been determined through analysis of
	//the behaviour of the VDP when invalid field size settings are selected. In
	//particular, the third table entry of 0 is used when the invalid HSZ mode of "10" is
	//selected. This causes the row and column to overlap when building the final address
	//value. The way the address is built in these circumstances can only be logically
	//explained by a lookup table being used for the row shift count, with the third entry
	//being set to 0, as we have implemented here.
	//##FIX## Hardware tests have shown this is not actually the case
	static const unsigned int rowShiftCountTable[4] = {6, 7, 0, 8};

	//The following calculation limits the vertical playfield size, based on the
	//horizontal playfield size. This calculation is quite simple in hardware, but looks
	//more complicated in code than it really is. Basically, the upper bit of the vertical
	//scroll mode is run through a NAND operation with the lower bit of the horizontal
	//scroll mode, and likewise, the lower bit of the vertical scroll mode is run through
	//a NAND operation with the upper bit of the horizontal scroll mode. This limits the
	//vertical scroll mode in the exact same way the real hardware does, including when
	//invalid scroll modes are being used.
	unsigned int screenSizeModeH = hszState;
	unsigned int screenSizeModeV = ((vszState & 0x1) & ((~hszState & 0x02) >> 1)) | ((vszState & 0x02) & ((~hszState & 0x01) << 1));

	//Build the playfield block masks. These masks ultimately determine the boundaries of
	//the playfield horizontally and vertically. Hardware testing and analysis on the
	//scrolling behaviour of the VDP indicate that these masks are built by mapping the
	//HSZ and VSZ bits to the upper bits of a 7-bit mask value, as shown below. Note that
	//the invalid mode setting of "10" results in a mask where bit 7 is set, and bit 6 is
	//unset. This has been confirmed through hardware tests.
	//##TODO## Test this on hardware.
	//##TODO## Test and confirm how the window distortion bug interacts with this block
	//mapping selection process.
	unsigned int playfieldBlockMaskX = (screenSizeModeH << 5) | 0x1F;
	unsigned int playfieldBlockMaskY = (screenSizeModeV << 5) | 0x1F;

	//##TODO## Update this comment
	//Calculate the row and column numbers for the mapping data. This is simply done by
	//converting the calculated playfield position from a pixel index into a block index,
	//then masking the X and Y coordinates by the horizontal and vertical block masks.
	//This gives us a row and column number, wrapped to the playfield boundaries.
	//##TODO## We want to keep this shift method for calculating the row and column,
	//rather than using division, but we should be using a constant, or at least
	//explaining why the magic number "3" is being used.
	//##TODO## Update these comments
	//##TODO## Document why we add the horizontal scroll value, but subtract the vertical
	//scroll value.
	const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
	unsigned int mappingDataRowNumber = (((screenRowNumber + layerVscrollPatternDisplacement) / rowsPerTile) + layerVscrollMappingDisplacement) & playfieldBlockMaskY;
	unsigned int mappingDataColumnNumber = ((screenColumnNumber - layerHscrollMappingDisplacement) * cellsPerColumn) & playfieldBlockMaskX;

	//Based on the horizontal playfield mode, lookup the row shift count to use when
	//building the final mapping data address value. The column shift count is always
	//fixed to 1.
	unsigned int rowShiftCount = rowShiftCountTable[screenSizeModeH];
	const unsigned int columnShiftCount = 1;

	//Calculate the final mapping data address. Note that the row number is masked with
	//the inverted mask for the column number, so that row data is only allowed to appear
	//where column data is not allowed to appear. This is based on the observed behaviour
	//of the system, as is critical in order to correctly emulate the scrolling behaviour
	//where an invalid horizontal scroll mode of "10" is applied. In this case, the row
	//data can be interleaved with the column data, since the row shift count under this
	//mode is 0.
	unsigned int mappingDataAddress = nameTableBaseAddress | ((mappingDataRowNumber << rowShiftCount) & (~playfieldBlockMaskX << columnShiftCount)) | (mappingDataColumnNumber << columnShiftCount);

	//Mask the lower two bits of the mapping data address, to align the mapping address
	//with a 4-byte boundary. The VDP can only read data from the VRAM in aligned 4-byte
	//blocks, so the lower two bits of the address are ineffective. We read a pair of
	//2-byte block mappings from the masked address.
	mappingDataAddress &= 0xFFFC;

	//Read target layer mapping data
	mappingDataEntry1 = ((unsigned int)vram->ReadCommitted(mappingDataAddress+0) << 8) | (unsigned int)vram->ReadCommitted(mappingDataAddress+1);
	mappingDataEntry2 = ((unsigned int)vram->ReadCommitted(mappingDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(mappingDataAddress+3);
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderReadPatternDataRow(unsigned int patternRowNumberNoFlip, unsigned int patternCellOffset, bool interlaceMode2Active, const Data& mappingData, Data& patternData) const
{
	//Calculate the final number of the pattern row to read, taking into account vertical
	//flip if it is specified in the block mapping.
	//Mapping (Pattern Name) data format:
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|Pri|PalRow |VF |HF |              Pattern Number               |
	//-----------------------------------------------------------------
	//Pri:    Priority Bit
	//PalRow: The palette row number to use when displaying the pattern data
	//VF:     Vertical Flip
	//HF:     Horizontal Flip
	const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
	unsigned int patternRowNumber = mappingData.GetBit(12)? (rowsPerTile - 1) - patternRowNumberNoFlip: patternRowNumberNoFlip;

	//The address of the pattern data to read is determined by combining the number of the
	//pattern (tile) with the row of the pattern to be read. The way the data is combined
	//is different under interlace mode 2, where patterns are 16 pixels high instead of
	//the usual 8 pixels. The format for pattern data address decoding is as follows when
	//interlace mode 2 is not active:
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|              Pattern Number               |Pattern Row| 0 | 0 |
	//-----------------------------------------------------------------
	//When interlace mode 2 is active, the pattern data address decoding is as follows:
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|            Pattern Number             |  Pattern Row  | 0 | 0 |
	//-----------------------------------------------------------------
	//Note that we grab the entire mapping data block as the block number when calculating
	//the address. This is because the resulting address is wrapped to keep it within the
	//VRAM boundaries. Due to this wrapping, in reality only the lower 11 bits of the
	//mapping data are effective when determining the block number, or the lower 10 bits
	//in the case of interlace mode 2.
	//##TODO## Test the above assertion on the TeraDrive with the larger VRAM mode active
	const unsigned int patternDataRowByteSize = 4;
	const unsigned int blockPatternByteSize = rowsPerTile * patternDataRowByteSize;
	unsigned int patternDataAddress = (((mappingData.GetData() + patternCellOffset) * blockPatternByteSize) + (patternRowNumber * patternDataRowByteSize)) % vramSize;

	//Read the target pattern row
	patternData = ((unsigned int)vram->ReadCommitted(patternDataAddress+0) << 24) | ((unsigned int)vram->ReadCommitted(patternDataAddress+1) << 16) | ((unsigned int)vram->ReadCommitted(patternDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(patternDataAddress+3);
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderBuildSpriteList(unsigned int screenRowNumber, bool interlaceMode2Active, bool screenModeRS1Active, unsigned int& nextTableEntryToRead, bool& spriteSearchComplete, bool& spriteOverflow, unsigned int& spriteDisplayCacheEntryCount, std::vector<SpriteDisplayCacheEntry>& spriteDisplayCache) const
{
	if(!spriteSearchComplete && !spriteOverflow)
	{
		const unsigned int spriteCacheEntrySize = 4;
		const unsigned int spriteAttributeTableSize = (screenModeRS1Active)? 80: 64;
		const unsigned int spritePosScreenStartH = 0x80;
		const unsigned int spritePosScreenStartV = (interlaceMode2Active)? 0x100: 0x80;
		const unsigned int spritePosBitCountV = (interlaceMode2Active)? 10: 9;
		const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
		const unsigned int renderSpriteDisplayCacheSize = (screenModeRS1Active)? 20: 16;
		const unsigned int renderSpriteCellDisplayCacheSize = (screenModeRS1Active)? 40: 32;

		//Calculate the address in the sprite cache of the next sprite to read data for
		unsigned int spriteCacheAddress = (nextTableEntryToRead * spriteCacheEntrySize);

		//Read all available data on the next sprite from the sprite cache
		Data spriteVPosData(16);
		Data spriteSizeAndLinkData(16);
		spriteVPosData = ((unsigned int)spriteCache->ReadCommitted(spriteCacheAddress+0) << 8) | (unsigned int)spriteCache->ReadCommitted(spriteCacheAddress+1);
		spriteSizeAndLinkData = ((unsigned int)spriteCache->ReadCommitted(spriteCacheAddress+2) << 8) | (unsigned int)spriteCache->ReadCommitted(spriteCacheAddress+3);

		//Calculate the width and height of this sprite in cells
		unsigned int spriteHeightInCells = spriteSizeAndLinkData.GetDataSegment(8, 2) + 1;

		//Calculate the relative position of the current active display line in sprite
		//space.
		//##TODO## Lay this code out better, and provide more comments on what's being
		//done to support interlace mode 2.
		//##TODO## Handle interlace mode 2 properly. In interlace mode 2, the current
		//screen row number is effectively doubled, with the current state of the odd flag
		//used as the LSB of the line number. We need to do that here.
		unsigned int currentScreenRowInSpriteSpace = screenRowNumber + spritePosScreenStartV;
		if(interlaceMode2Active)
		{
			currentScreenRowInSpriteSpace = (screenRowNumber * 2) + spritePosScreenStartV;
			if(renderDigitalOddFlagSet)
			{
				currentScreenRowInSpriteSpace += 1;
			}
		}

		//Calculate the vertical position of the sprite, discarding any unused bits.
		Data spriteVPos(spritePosBitCountV);
		spriteVPos = spriteVPosData;

		//If this next sprite is within the current display row, add it to the list of
		//sprites to display on this line.
		unsigned int spriteHeightInPixels = spriteHeightInCells * rowsPerTile;
		if((spriteVPos <= currentScreenRowInSpriteSpace) && ((spriteVPos + spriteHeightInPixels) > currentScreenRowInSpriteSpace))
		{
			//We perform a check for a sprite overflow here. If we exceed the maximum
			//number of sprites for this line, we set the sprite overflow flag, otherwise
			//we load all the sprite data from the sprite cache for this sprite.
			if(spriteDisplayCacheEntryCount < renderSpriteDisplayCacheSize)
			{
				spriteDisplayCache[spriteDisplayCacheEntryCount].spriteTableIndex = nextTableEntryToRead;
				spriteDisplayCache[spriteDisplayCacheEntryCount].spriteRowIndex = (currentScreenRowInSpriteSpace - spriteVPos.GetData());
				spriteDisplayCache[spriteDisplayCacheEntryCount].vpos = spriteVPosData;
				spriteDisplayCache[spriteDisplayCacheEntryCount].sizeAndLinkData = spriteSizeAndLinkData;
				++spriteDisplayCacheEntryCount;
			}
			else
			{
				spriteOverflow = true;
			}
		}

		//Use the link data to determine which sprite table entry to read data for next.
		//The sprite search is terminated if we encounter a sprite with a link data value
		//of 0, or if a link data value is specified which is outside the bounds of the
		//sprite table, based on the current screen mode settings.
		nextTableEntryToRead = spriteSizeAndLinkData.GetDataSegment(0, 7);
		if((nextTableEntryToRead == 0) || (nextTableEntryToRead >= spriteAttributeTableSize))
		{
			spriteSearchComplete = true;
		}
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderBuildSpriteCellList(unsigned int spriteDisplayCacheIndex, unsigned int spriteTableBaseAddress, bool interlaceMode2Active, bool screenModeRS1Active, bool& spriteDotOverflow, SpriteDisplayCacheEntry& spriteDisplayCacheEntry, unsigned int& spriteCellDisplayCacheEntryCount, std::vector<SpriteCellDisplayCacheEntry>& spriteCellDisplayCache) const
{
	if(!spriteDotOverflow)
	{
		const unsigned int spriteCacheEntrySize = 4;
		const unsigned int spriteTableEntrySize = 8;
		const unsigned int spriteAttributeTableSize = (screenModeRS1Active)? 80: 64;
		const unsigned int spritePosScreenStartH = 0x80;
		const unsigned int spritePosScreenStartV = (interlaceMode2Active)? 0x100: 0x80;
		const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
		const unsigned int renderSpriteDisplayCacheSize = (screenModeRS1Active)? 20: 16;
		const unsigned int renderSpriteCellDisplayCacheSize = (screenModeRS1Active)? 40: 32;

		//Calculate the address in VRAM of this sprite table entry
		unsigned int spriteTableEntryAddress = spriteTableBaseAddress + (spriteDisplayCacheEntry.spriteTableIndex * spriteTableEntrySize);

		//Read all remaining data for the sprite from the sprite attribute table in VRAM.
		//The first 4 bytes of the sprite attribute entry have already been loaded from
		//the internal sprite cache.
		//##TODO## Perform hardware tests, to confirm if the data from the sprite cache is
		//buffered after it is read the first time while parsing the sprite table to
		//determine the list of sprites on the current line, or if the data is read again
		//from the cache during the mapping decoding.
		Data spriteMappingData(16);
		Data spriteHPosData(16);
		spriteMappingData = ((unsigned int)vram->ReadCommitted(spriteTableEntryAddress+4) << 8) | (unsigned int)vram->ReadCommitted(spriteTableEntryAddress+5);
		spriteHPosData = ((unsigned int)vram->ReadCommitted(spriteTableEntryAddress+6) << 8) | (unsigned int)vram->ReadCommitted(spriteTableEntryAddress+7);

		//Load the remaining data into the sprite display cache
		spriteDisplayCacheEntry.mappingData = spriteMappingData;
		spriteDisplayCacheEntry.hpos = spriteHPosData;

		//Calculate the width and height of this sprite in cells
		unsigned int spriteWidthInCells = spriteDisplayCacheEntry.sizeAndLinkData.GetDataSegment(10, 2) + 1;
		unsigned int spriteHeightInCells = spriteDisplayCacheEntry.sizeAndLinkData.GetDataSegment(8, 2) + 1;

		//Extract the vertical flip and horizontal flip flags from the sprite mapping.
		//Unlike vertical and horizontal flip for normal pattern cells in the scroll
		//layers, when horizontal flip or vertical flip is applied to a sprite, the entire
		//sprite is flipped, so that the cell order itself is reversed, as well as the
		//order of the pixels within each cell. We extract the vertical flip and
		//horizontal flip flags here so we can emulate that behaviour.
		bool hflip = spriteMappingData.GetBit(11);
		bool vflip = spriteMappingData.GetBit(12);

		//Calculate the cell row number, and pattern row number within the cell, which
		//fall on the current display line for this sprite.
		unsigned int spriteCellRowNumber = spriteDisplayCacheEntry.spriteRowIndex / rowsPerTile;
		unsigned int spriteCellPatternRowNumber = spriteDisplayCacheEntry.spriteRowIndex % rowsPerTile;

		//Apply vertical flip to the sprite cell number if vertical flip is active. Note
		//that we do not invert the pattern row number here, just the cell row number. All
		//we're doing at this stage of sprite processing is decoding the cells within the
		//sprite that fall on the current display line. Vertical flip is applied to the
		//pattern row within a cell at a later stage of sprite processing.
		if(vflip)
		{
			spriteCellRowNumber = ((spriteHeightInCells - 1) - spriteCellRowNumber);
		}

		//Add details of each cell in this sprite on the current display line to the
		//internal sprite pattern render list.
		for(unsigned int i = 0; i < spriteWidthInCells; ++i)
		{
			//We perform a check for a sprite dot overflow here. If each sprite is 2 cells
			//wide, and the maximum number of sprites for a line are present, we generate
			//the exact maximum number of sprite dots per line. If sprite widths are 3
			//cells or wider, we can exceed the maximum sprite dot count at this point.
			if(spriteCellDisplayCacheEntryCount < renderSpriteCellDisplayCacheSize)
			{
				//Record the reference from this individual sprite cell back to the sprite
				//display cache entry that holds the mapping data for the cell.
				spriteCellDisplayCache[spriteCellDisplayCacheEntryCount].spriteDisplayCacheIndex = spriteDisplayCacheIndex;

				//Calculate the cell offset into the sprite pattern data for this cell,
				//taking into account horizontal flip. Note that pattern cells for sprites
				//are ordered in columns, not rows. The pattern data for the top left cell
				//appears first, followed by the pattern data for the second row on the
				//leftmost column, and so forth to the end of that column, followed by the
				//pattern data for the topmost cell in the second column, and so on.
				unsigned int spriteCellHorizontalOffset = (hflip)? (spriteWidthInCells - 1) - i: i;
				spriteCellDisplayCache[spriteCellDisplayCacheEntryCount].patternCellOffset = (spriteCellHorizontalOffset * spriteHeightInCells) + spriteCellRowNumber;
				spriteCellDisplayCache[spriteCellDisplayCacheEntryCount].patternRowOffset = spriteCellPatternRowNumber;
				spriteCellDisplayCache[spriteCellDisplayCacheEntryCount].spriteCellColumnNo = i;

				//Advance to the next available entry in the sprite cell cache
				++spriteCellDisplayCacheEntryCount;
			}
			else
			{
				spriteDotOverflow = true;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::DigitalRenderReadPixelIndex(const Data& patternRow, bool horizontalFlip, unsigned int pixelIndex) const
{
	const unsigned int patternDataPixelEntryBitCount = 4;

	if(!horizontalFlip)
	{
		//Pattern data row format (no horizontal flip):
		//---------------------------------------------------------------------------------------------------------------------------------
		//|31 |30 |29 |28 |27 |26 |25 |24 |23 |22 |21 |20 |19 |18 |17 |16 |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//|-------------------------------------------------------------------------------------------------------------------------------|
		//|    Pixel 1    |    Pixel 2    |    Pixel 3    |    Pixel 4    |    Pixel 5    |    Pixel 6    |    Pixel 7    |    Pixel 8    |
		//---------------------------------------------------------------------------------------------------------------------------------
		return patternRow.GetDataSegment(((cellBlockSizeH - 1) - pixelIndex) * patternDataPixelEntryBitCount, 4);
	}
	else
	{
		//Pattern data row format (with horizontal flip):
		//---------------------------------------------------------------------------------------------------------------------------------
		//|31 |30 |29 |28 |27 |26 |25 |24 |23 |22 |21 |20 |19 |18 |17 |16 |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//|-------------------------------------------------------------------------------------------------------------------------------|
		//|    Pixel 8    |    Pixel 7    |    Pixel 6    |    Pixel 5    |    Pixel 4    |    Pixel 3    |    Pixel 2    |    Pixel 1    |
		//---------------------------------------------------------------------------------------------------------------------------------
		return patternRow.GetDataSegment(pixelIndex * patternDataPixelEntryBitCount, 4);
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::CalculateLayerPriorityIndex(unsigned int& layerIndex, bool& shadow, bool& highlight, bool shadowHighlightEnabled, bool spriteIsShadowOperator, bool spriteIsHighlightOperator, bool foundSpritePixel, bool foundLayerAPixel, bool foundLayerBPixel, bool prioritySprite, bool priorityLayerA, bool priorityLayerB) const
{
	//Initialize the shadow/highlight flags
	shadow = false;
	highlight = false;

	//Perform layer priority calculations
	if(!shadowHighlightEnabled)
	{
		//Perform standard layer priority calculations
		if(foundSpritePixel && prioritySprite)
		{
			layerIndex = LAYER_SPRITE;
		}
		else if(foundLayerAPixel && priorityLayerA)
		{
			layerIndex = LAYER_LAYERA;
		}
		else if(foundLayerBPixel && priorityLayerB)
		{
			layerIndex = LAYER_LAYERB;
		}
		else if(foundSpritePixel)
		{
			layerIndex = LAYER_SPRITE;
		}
		else if(foundLayerAPixel)
		{
			layerIndex = LAYER_LAYERA;
		}
		else if(foundLayerBPixel)
		{
			layerIndex = LAYER_LAYERB;
		}
		else
		{
			layerIndex = LAYER_BACKGROUND;
		}
	}
	else
	{
		//Perform shadow/highlight mode layer priority calculations. Note that some
		//illustrations in the official documentation from Sega demonstrating the
		//behaviour of shadow/highlight mode are incorrect. In particular, the third and
		//fifth illustrations on page 64 of the "Genesis Software Manual", showing layers
		//B and A being shadowed when a shadow sprite operator is at a lower priority, are
		//incorrect. If any layer is above an operator sprite pixel, the sprite operator
		//is ignored, and the higher priority pixel is output without the sprite operator
		//being applied. This has been confirmed through hardware tests. All other
		//illustrations describing the operation of shadow/highlight mode in relation to
		//layer priority settings appear to be correct.
		if(foundSpritePixel && prioritySprite && !spriteIsShadowOperator && !spriteIsHighlightOperator)
		{
			layerIndex = LAYER_SPRITE;
		}
		else if(foundLayerAPixel && priorityLayerA)
		{
			layerIndex = LAYER_LAYERA;
			if(prioritySprite && spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(prioritySprite && spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else if(foundLayerBPixel && priorityLayerB)
		{
			layerIndex = LAYER_LAYERB;
			if(prioritySprite && spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(prioritySprite && spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else if(foundSpritePixel && !spriteIsShadowOperator && !spriteIsHighlightOperator)
		{
			layerIndex = LAYER_SPRITE;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
		}
		else if(foundLayerAPixel)
		{
			layerIndex = LAYER_LAYERA;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
			if(spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else if(foundLayerBPixel)
		{
			layerIndex = LAYER_LAYERB;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
			if(spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else
		{
			layerIndex = LAYER_BACKGROUND;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
			if(spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}

		//If shadow and highlight are both set, they cancel each other out. This is why a
		//sprite acting as a highlight operator is unable to highlight layer A, B, or the
		//background, if layers A and B both have their priority bits unset. This has been
		//confirmed on the hardware.
		if(shadow && highlight)
		{
			shadow = false;
			highlight = false;
		}
	}
}

////----------------------------------------------------------------------------------------
//void S315_5313::WriteColorValueToImageBuffer(unsigned int paletteLine, unsigned int paletteEntry, bool shadow, bool highlight, unsigned int xpos, unsigned int ypos)
//{
//	//##TODO## Emulate CRAM write flicker
//	const unsigned int paletteEntriesPerLine = 16;
//	const unsigned int paletteEntrySize = 2;
//	//##TODO## Our new colour values are basically correct, assuming what is suspected
//	//after analysis posted on SpritesMind, that the Mega Drive VDP never actually outputs
//	//at full intensity. We haven't taken the apparent "ladder effect" into account
//	//however. It is highly recommended that we perform our own tests on the hardware, and
//	//make some comparisons between captured video output from the real system, and the
//	//output from our emulator, when playing back on the same physical screen. If the
//	//ladder effect is real and does have an effect on the way the intensity is perceived
//	//on the screen, we should emulate it. We also need to confirm the maximum intensity
//	//output by the VDP. A step size of 18 for example would get a max value of 252, which
//	//would be more logical.
//	//const unsigned char paletteEntryTo8Bit[8] = {0, 36, 73, 109, 146, 182, 219, 255};
//	//const unsigned char paletteEntryTo8BitShadow[8] = {0, 18, 37, 55, 73, 91, 110, 128};
//	//const unsigned char paletteEntryTo8BitHighlight[8] = {128, 146, 165, 183, 201, 219, 238, 255};
//	const unsigned char paletteEntryTo8Bit[8] = {0, 34, 68, 102, 136, 170, 204, 238};
//	const unsigned char paletteEntryTo8BitShadow[8] = {0, 17, 34, 51, 68, 85, 102, 119};
//	const unsigned char paletteEntryTo8BitHighlight[8] = {119, 136, 153, 170, 187, 204, 221, 238};
//
//	//Calculate the address of the colour value to read from the palette
//	unsigned int paletteEntryAddress = (paletteEntry + (paletteLine * paletteEntriesPerLine)) * paletteEntrySize;
//
//	//Read the target palette entry
//	Data paletteData(16);
//	paletteData = (unsigned int)(cram->ReadCommitted(paletteEntryAddress+0) << 8) | (unsigned int)cram->ReadCommitted(paletteEntryAddress+1);
//
//	//Decode palette color
//	unsigned char r;
//	unsigned char g;
//	unsigned char b;
//	if(shadow && !highlight)
//	{
//		r = paletteEntryTo8BitShadow[(paletteData.GetData() & 0x000E) >> 1];
//		g = paletteEntryTo8BitShadow[(paletteData.GetData() & 0x00E0) >> 5];
//		b = paletteEntryTo8BitShadow[(paletteData.GetData() & 0x0E00) >> 9];
//	}
//	else if(highlight && !shadow)
//	{
//		r = paletteEntryTo8BitHighlight[(paletteData.GetData() & 0x000E) >> 1];
//		g = paletteEntryTo8BitHighlight[(paletteData.GetData() & 0x00E0) >> 5];
//		b = paletteEntryTo8BitHighlight[(paletteData.GetData() & 0x0E00) >> 9];
//	}
//	else
//	{
//		r = paletteEntryTo8Bit[(paletteData.GetData() & 0x000E) >> 1];
//		g = paletteEntryTo8Bit[(paletteData.GetData() & 0x00E0) >> 5];
//		b = paletteEntryTo8Bit[(paletteData.GetData() & 0x0E00) >> 9];
//	}
//
//	//Write the final colour value to the image buffer
//	const unsigned int imageBufferEntryByteCount = 4;
//	//##TODO## Clean up this entry
//	const unsigned int height = 313;
//	unsigned int bufferPos = ((xpos + (((height - 1) * imageWidth) - (ypos * imageWidth))) * imageBufferEntryByteCount);
////	unsigned int bufferPos = (xpos + (((imageHeight - 1) - ypos) * imageWidth)) * imageBufferEntryByteCount;
//	if(bufferPos >= (imageWidth * imageHeight * imageBufferEntryByteCount))
//	{
//		//##DEBUG##
//		std::wcout << "ERROR! Buffer overflow on image buffer\n";
//		std::wcout << ypos << ' ' << xpos << '\n';
//	}
//	image[drawingImageBufferPlane][bufferPos + 0] = r;
//	image[drawingImageBufferPlane][bufferPos + 1] = g;
//	image[drawingImageBufferPlane][bufferPos + 2] = b;
//	image[drawingImageBufferPlane][bufferPos + 3] = 0xFF;
//}
//
////----------------------------------------------------------------------------------------
////This function performs all the necessary calculations to determine which mapping data to
////read for a given playfield position, and reads the corresponding mapping data pair from
////VRAM. The calculations performed appear to produce the same result as the real VDP
////hardware under all modes and settings, including when invalid scroll size modes are
////used.
////
////The following comments are provided as a supplement to the comments within this
////function, and show how the internally calculated row and column numbers are combined
////with the mapping base address data to produce a final VRAM address for the mapping
////block. All possible combinations of screen mode settings are shown, including invalid
////modes (VSZ="10" or HSZ="10"). Note that invalid combinations of screen mode settings are
////not shown, since invalid combinations never actually occur, due to the vertical screen
////mode being adjusted based on the horizontal screen mode, as outlined in the function
////comments below.
////
////Officially supported screen mode settings. Note that the lower two bits of the resulting
////address are masked before the address is used.
////Mapping data VRAM address (HSZ=00 VSZ=00):
////-----------------------------------------------------------------
////| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
////|---------------------------------------------------------------|
////| Base Addr | 0 | 0 |        Row        |       Column      | 0 | (Row Shift Count = 6)
////-----------------------------------------------------------------
////Mapping data VRAM address (HSZ=01 VSZ=00):
////-----------------------------------------------------------------
////| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
////|---------------------------------------------------------------|
////| Base Addr | 0 |        Row        |         Column        | 0 | (Row Shift Count = 7)
////-----------------------------------------------------------------
////Mapping data VRAM address (HSZ=11 VSZ=00):
////-----------------------------------------------------------------
////| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
////|---------------------------------------------------------------|
////| Base Addr |        Row        |           Column          | 0 | (Row Shift Count = 8)
////-----------------------------------------------------------------
////Mapping data VRAM address (HSZ=00 VSZ=01):
////-----------------------------------------------------------------
////| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
////|---------------------------------------------------------------|
////| Base Addr | 0 |          Row          |       Column      | 0 | (Row Shift Count = 6)
////-----------------------------------------------------------------
////Mapping data VRAM address (HSZ=01 VSZ=01):
////-----------------------------------------------------------------
////| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
////|---------------------------------------------------------------|
////| Base Addr |          Row          |         Column        | 0 | (Row Shift Count = 7)
////-----------------------------------------------------------------
////Mapping data VRAM address (HSZ=00 VSZ=11):
////-----------------------------------------------------------------
////| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
////|---------------------------------------------------------------|
////| Base Addr |            Row            |       Column      | 0 | (Row Shift Count = 6)
////-----------------------------------------------------------------
////
////Officially unsupported screen modes. In this case, the row and column data may be
////interleaved, and the row shift count may be 0, as shown below:
////Mapping data VRAM address (HSZ=00 VSZ=10):
////-----------------------------------------------------------------
////| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
////|---------------------------------------------------------------|
////|           |Row| 0 |        Row        |                       | (Row Shift Count = 6)
////|           -------------------------------------------------   |
////|                                       |       Column      |   |
////|---------------------------------------------------------------|
////| Base Addr |Row| 0 |        Row        |       Column      | 0 |
////-----------------------------------------------------------------
////##FIX## Hardware tests have shown the two cases below to be incorrect. It appears the
////upper bit of the column data is never applied, and the row is never incremented when the
////invalid horizontal mode is active.
////Mapping data VRAM address (HSZ=10 VSZ=00):
////-----------------------------------------------------------------
////| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
////|---------------------------------------------------------------|
////|                                           |        Row        | (Row Shift Count = 0)
////|                                       ------------------------|
////|                                       |       Column      |   |
////|---------------------------------------------------------------|
////| Base Addr | 0 | 0 | 0 | 0 | 0 |Col| 0 |       Column      |Row|
////-----------------------------------------------------------------
////Mapping data VRAM address (HSZ=10 VSZ=10):
////-----------------------------------------------------------------
////| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
////|---------------------------------------------------------------|
////|                                   |Row| 0 |        Row        | (Row Shift Count = 0)
////|                               --------------------------------|
////|                               |Col| 0 |       Column      |   |
////|---------------------------------------------------------------|
////| Base Addr | 0 | 0 | 0 | 0 | 0 |Col|Row|       Column      |Row|
////-----------------------------------------------------------------
////##TODO## Implement the correct mappings, which are as follows:
////Mapping data VRAM address (HSZ=10 VSZ=00):
////-----------------------------------------------------------------
////| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
////|---------------------------------------------------------------|
////| Base Addr | 0 | 0 | 0 | 0 | 0 | 0 | 0 |       Column      | 0 |
////-----------------------------------------------------------------
////Mapping data VRAM address (HSZ=10 VSZ=10):
////-----------------------------------------------------------------
////| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
////|---------------------------------------------------------------|
////| Base Addr | 0 | 0 | 0 | 0 | 0 | 0 | 0 |       Column      | 0 |
////-----------------------------------------------------------------
////----------------------------------------------------------------------------------------
//void S315_5313::ReadMappingDataPair(Data& mappingDataEntry1, Data& mappingDataEntry2, unsigned int nameTableBaseAddress, unsigned int mappingDataRowNumber, unsigned int mappingDataColumnNumber, unsigned int screenSizeModeH, unsigned int screenSizeModeV) const
//{
//	//The existence and contents of this table have been determined through analysis of
//	//the behaviour of the VDP when invalid field size settings are selected. In
//	//particular, the third table entry of 0 is used when the invalid HSZ mode of "10" is
//	//selected. This causes the row and column to overlap when building the final address
//	//value. The way the address is built in these circumstances can only be logically
//	//explained by a lookup table being used for the row shift count, with the third entry
//	//being set to 0, as we have implemented here.
//	//##FIX## Hardware tests have shown this is not actually the case
//	static const unsigned int rowShiftCountTable[4] = {6, 7, 0, 8};
//
//	//The following calculation limits the vertical playfield size, based on the
//	//horizontal playfield size. This calculation is quite simple in hardware, but looks
//	//more complicated in code than it really is. Basically, the upper bit of the vertical
//	//scroll mode is run through a NAND operation with the lower bit of the horizontal
//	//scroll mode, and likewise, the lower bit of the vertical scroll mode is run through
//	//a NAND operation with the upper bit of the horizontal scroll mode. This limits the
//	//vertical scroll mode in the exact same way the real hardware does, including when
//	//invalid scroll modes are being used.
//	screenSizeModeV = ((screenSizeModeV & 0x1) & ((~screenSizeModeH & 0x02) >> 1)) | ((screenSizeModeV & 0x02) & ((~screenSizeModeH & 0x01) << 1));
//
//	//Build the playfield block masks. These masks ultimately determine the boundaries of
//	//the playfield horizontally and vertically. Hardware testing and analysis on the
//	//scrolling behaviour of the VDP indicate that these masks are built by mapping the
//	//HSZ and VSZ bits to the upper bits of a 7-bit mask value, as shown below. Note that
//	//the invalid mode setting of "10" results in a mask where bit 7 is set, and bit 6 is
//	//unset. This has been confirmed through hardware tests.
//	//##TODO## Test this on hardware.
//	//##TODO## Test and confirm how the window distortion bug interacts with this block
//	//mapping selection process.
//	unsigned int playfieldBlockMaskX = (screenSizeModeH << 5) | 0x1F;
//	unsigned int playfieldBlockMaskY = (screenSizeModeV << 5) | 0x1F;
//
//	//Calculate the row and column numbers for the mapping data. This is simply done by
//	//converting the calculated playfield position from a pixel index into a block index,
//	//then masking the X and Y coordinates by the horizontal and vertical block masks.
//	//This gives us a row and column number, wrapped to the playfield boundaries.
//	//##TODO## We want to keep this shift method for calculating the row and column,
//	//rather than using division, but we should be using a constant, or at least
//	//explaining why the magic number "3" is being used.
//	//##TODO## Update these comments
//	mappingDataRowNumber &= playfieldBlockMaskY;
//	mappingDataColumnNumber <<= 1;
//	mappingDataColumnNumber &= playfieldBlockMaskX;
//
//	//Based on the horizontal playfield mode, lookup the row shift count to use when
//	//building the final mapping data address value. The column shift count is always
//	//fixed to 1.
//	unsigned int rowShiftCount = rowShiftCountTable[screenSizeModeH];
//	const unsigned int columnShiftCount = 1;
//
//	//Calculate the final mapping data address. Note that the row number is masked with
//	//the inverted mask for the column number, so that row data is only allowed to appear
//	//where column data is not allowed to appear. This is based on the observed behaviour
//	//of the system, as is critical in order to correctly emulate the scrolling behaviour
//	//where an invalid horizontal scroll mode of "10" is applied. In this case, the row
//	//data can be interleaved with the column data, since the row shift count under this
//	//mode is 0.
//	unsigned int mappingDataAddress = nameTableBaseAddress | ((mappingDataRowNumber << rowShiftCount) & (~playfieldBlockMaskX << columnShiftCount)) | (mappingDataColumnNumber << columnShiftCount);
//
//	//Mask the lower two bits of the mapping data address, to align the mapping address
//	//with a 4-byte boundary. The VDP can only read data from the VRAM in aligned 4-byte
//	//blocks, so the lower two bits of the address are ineffective. We read a pair of
//	//2-byte block mappings from the masked address.
//	mappingDataAddress &= 0xFFFC;
//
//	//##TODO## Clean up the old mapping code below
//	////Calculate the target layer mapping data column pair. Note that since we read mapping
//	////blocks in pairs, we need to find the next pair of mapping entries, not the next
//	////individual mapping entry, which lies at or after the specified playfield position.
//	////##TODO## The division operator we use in this function can be avoided. Since the
//	////size of a block is always a power of 2, we should be performing a bitshift rather
//	////than a full blown division operator. This is certainly what the real hardware would
//	////do.
//	//unsigned int mappingDataColumnAddress = ((layerScrolledPlayfieldX / (renderSettings.blockPatternSizeX * 2)) * 2) & playfieldBlockMaskX;
//	//if((layerScrolledPlayfieldX % (renderSettings.blockPatternSizeX * 2)) != 0)
//	//{
//	//	//If a block pair is partially visible on the left side of the playfield,
//	//	//those blocks are already being displayed using the scrolled block data.
//	//	//In this case, advance to the next pair block. Note that this also
//	//	//emulates the window distortion bug.
//	//	//##TODO## Confirm this emulates the window distortion bug.
//	//	mappingDataColumnAddress += 2;
//	//}
//
//	////Calculate the target layer mapping data row
//	////##TODO## Avoid the division operator here, and use a shift instead.
//	//unsigned int mappingDataRowAddress = (layerScrolledPlayfieldY / renderSettings.blockPatternSizeY) & playfieldBlockMaskY;
//
//	////Calculate the final target layer mapping data address
//	////##TODO## Continue testing and analysis on the invalid playfield sizes, and determine
//	////how the row number is combined into the base address.
//	//unsigned int mappingDataAddress = nameTableBaseAddress + ((mappingDataColumnAddress + (mappingDataRowAddress * playfieldBlockSizeX)) * renderSettings.blockMappingEntryByteSize);
//
//	//Read target layer mapping data
//	mappingDataEntry1 = ((unsigned int)vram->ReadCommitted(mappingDataAddress+0) << 8) | (unsigned int)vram->ReadCommitted(mappingDataAddress+1);
//	mappingDataEntry2 = ((unsigned int)vram->ReadCommitted(mappingDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(mappingDataAddress+3);
//}
//
////----------------------------------------------------------------------------------------
//void S315_5313::ReadPatternDataRow(const LineRenderSettings& renderSettings, Data& patternData, const Data& mappingData, unsigned int patternRowNumber) const
//{
//	//Calculate the final address for the pattern row, applying vertical flip if it is
//	//specified in the block mapping. Note that we grab the entire mapping data block as
//	//the block number when calculating the address. This is because the resulting address
//	//is wrapped to keep it within the VRAM boundaries. Due to this wrapping, in reality
//	//only the lower 11 bits of the mapping data are effective when determining the block
//	//number, or the lower 10 bits in the case of interlace mode 2.
//	//##TODO## Use low-level bit operations here, and provide a comment showing the
//	//structure of the calculated address value.
//	unsigned int patternRowNumberWithFlip = mappingData.GetBit(12)? (renderSettings.blockPatternSizeY - 1) - patternRowNumber: patternRowNumber;
//	unsigned int patternDataAddress = ((mappingData.GetData() * renderSettings.blockPatternByteSize) + (patternRowNumberWithFlip * renderSettings.blockPatternRowByteSize)) % vramSize;
//
//	//Read the target pattern row
//	patternData = ((unsigned int)vram->ReadCommitted(patternDataAddress+0) << 24) | ((unsigned int)vram->ReadCommitted(patternDataAddress+1) << 16) | ((unsigned int)vram->ReadCommitted(patternDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(patternDataAddress+3);
//}
//
////----------------------------------------------------------------------------------------
//void S315_5313::RenderColumnBlockPair(unsigned int columnNumber, unsigned int scrollValueDisplacement, const Data& mappingDataCell1, const Data& mappingDataCell2, const Data& patternDataCell1, const Data& patternDataCell2, std::vector<unsigned int>& outputPaletteLines, std::vector<unsigned int>& outputPaletteEntries, unsigned int& currentRenderPixel) const
//{
//	//Extract the individual palette index values from the block pattern data, accounting
//	//for the horizontal flip flag in the block mappings.
//	std::vector<unsigned int> patternEntryArray(16);
//	if(mappingDataCell1.GetBit(11))
//	{
//		patternEntryArray[0] = patternDataCell1.GetDataSegment(0, 4);
//		patternEntryArray[1] = patternDataCell1.GetDataSegment(4, 4);
//		patternEntryArray[2] = patternDataCell1.GetDataSegment(8, 4);
//		patternEntryArray[3] = patternDataCell1.GetDataSegment(12, 4);
//		patternEntryArray[4] = patternDataCell1.GetDataSegment(16, 4);
//		patternEntryArray[5] = patternDataCell1.GetDataSegment(20, 4);
//		patternEntryArray[6] = patternDataCell1.GetDataSegment(24, 4);
//		patternEntryArray[7] = patternDataCell1.GetDataSegment(28, 4);
//	}
//	else
//	{
//		patternEntryArray[0] = patternDataCell1.GetDataSegment(28, 4);
//		patternEntryArray[1] = patternDataCell1.GetDataSegment(24, 4);
//		patternEntryArray[2] = patternDataCell1.GetDataSegment(20, 4);
//		patternEntryArray[3] = patternDataCell1.GetDataSegment(16, 4);
//		patternEntryArray[4] = patternDataCell1.GetDataSegment(12, 4);
//		patternEntryArray[5] = patternDataCell1.GetDataSegment(8, 4);
//		patternEntryArray[6] = patternDataCell1.GetDataSegment(4, 4);
//		patternEntryArray[7] = patternDataCell1.GetDataSegment(0, 4);
//	}
//	if(mappingDataCell2.GetBit(11))
//	{
//		patternEntryArray[8] = patternDataCell2.GetDataSegment(0, 4);
//		patternEntryArray[9] = patternDataCell2.GetDataSegment(4, 4);
//		patternEntryArray[10] = patternDataCell2.GetDataSegment(8, 4);
//		patternEntryArray[11] = patternDataCell2.GetDataSegment(12, 4);
//		patternEntryArray[12] = patternDataCell2.GetDataSegment(16, 4);
//		patternEntryArray[13] = patternDataCell2.GetDataSegment(20, 4);
//		patternEntryArray[14] = patternDataCell2.GetDataSegment(24, 4);
//		patternEntryArray[15] = patternDataCell2.GetDataSegment(28, 4);
//	}
//	else
//	{
//		patternEntryArray[8] = patternDataCell2.GetDataSegment(28, 4);
//		patternEntryArray[9] = patternDataCell2.GetDataSegment(24, 4);
//		patternEntryArray[10] = patternDataCell2.GetDataSegment(20, 4);
//		patternEntryArray[11] = patternDataCell2.GetDataSegment(16, 4);
//		patternEntryArray[12] = patternDataCell2.GetDataSegment(12, 4);
//		patternEntryArray[13] = patternDataCell2.GetDataSegment(8, 4);
//		patternEntryArray[14] = patternDataCell2.GetDataSegment(4, 4);
//		patternEntryArray[15] = patternDataCell2.GetDataSegment(0, 4);
//	}
//
//	//Extract the palette line numbers from the block mappings
//	unsigned int paletteLineCell1 = mappingDataCell1.GetDataSegment(13, 2);
//	unsigned int paletteLineCell2 = mappingDataCell2.GetDataSegment(13, 2);
//
//	//##TODO## Make a comment about the window distortion bug
//	//Calculate the number of pixels to output for this column, and the initial pixel
//	//entry in the pattern buffer to output.
//	unsigned int columnPixelEndPosLayerA = (((columnNumber + 1) << 4) + scrollValueDisplacement) & 0x3FF;
//	unsigned int pixelsToOutputLayerA = columnPixelEndPosLayerA - currentRenderPixel;
//	unsigned int pixelIndexStartValueLayerA = 0x10 - (pixelsToOutputLayerA & 0x0F);
//
//	//Output each pixel for this column
//	for(unsigned int i = 0; i < pixelsToOutputLayerA; ++i)
//	{
//		unsigned int patternEntryBufferIndex = (pixelIndexStartValueLayerA + i) % 0x10;
//		outputPaletteEntries[currentRenderPixel] = patternEntryArray[patternEntryBufferIndex];
//		outputPaletteLines[currentRenderPixel] = (patternEntryBufferIndex < 8)? paletteLineCell1: paletteLineCell2;
//		++currentRenderPixel;
//	}
//}

//----------------------------------------------------------------------------------------
void S315_5313::DMAWorkerThread()
{
	boost::mutex::scoped_lock lock(workerThreadMutex);

	//##DEBUG##
//	std::wcout << L"DMAWorkerThread running\n";

	//Begin the DMA work loop
	while(workerThreadActive)
	{
		if(!busGranted)
		{
			//##DEBUG##
//			std::wcout << L"DMAWorkerThread going idle\t" << GetProcessorStateTime() << '\n';
//			std::wcout << '\t' << workerThreadActive << '\t' << workerThreadPaused << '\t' << busGranted << '\n';

			//If we don't currently have the bus, go idle until a DMA work request comes
			//through.
			GetDeviceContext()->SetTransientExecutionActive(false);
			workerThreadIdle.notify_all();
			workerThreadUpdate.wait(lock);
			GetDeviceContext()->SetTransientExecutionActive(busGranted);

			//##DEBUG##
//			std::wcout << L"DMAWorkerThread going active\t" << GetProcessorStateTime() << '\n';
		}
		else
		{
			//##DEBUG##
			if((commandCode.GetBit(5) != GetStatusFlagDMA()) && (commandCode.GetBit(5) != GetStatusFlagDMA()))
			{
				std::wcout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
				std::wcout << "VDP commandCode.GetBit(5) != GetStatusFlagDMA()\n";
				std::wcout << "__LINE__:\t" << __LINE__ << "\n";
				std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
				std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
				std::wcout << "busGranted:\t" << busGranted << "\n";
				std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
				std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
				std::wcout << "status:\t" << status.GetData() << "\n";
				std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
				std::wcout << "dmd1:\t" << dmd1 << "\n";
				std::wcout << "dmd0:\t" << dmd1 << "\n";
				std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
				std::wcout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
			}

			//If a DMA transfer is currently in progress and we currently have the bus,
			//advance the processor state until the DMA operation is complete, or we reach
			//the end of the current timeslice.
			if(dmaTransferActive && busGranted)
			{
				//Delay execution of the DMA worker thread until events have been
				//processed for this timeslice. This is required when resuming a paused
				//DMA operation for example, when we need to wait for the ExecuteTimeslice
				//method to be called and to fully process events before we can safely
				//modify the processor state.
				{
					boost::mutex::scoped_lock lock(accessMutex);
					while(!eventsProcessedForTimeslice)
					{
						eventProcessingCompleted.wait(lock);
					}
				}

				//Advance the DMA operation
				if(dmaAdvanceUntilDMAComplete)
				{
					//If we've been requested to advance the current DMA operation until
					//it is complete, ignore the timeslice length and advance until we
					//reach the desired state.

					//##DEBUG##
					//std::wcout << "VDP - DMAWorkerThreadForce: " << lastTimesliceTotalExecuteTime << '\t' << stateLastUpdateMclk << '\t' << stateLastUpdateMclkUnused << "\n";

					UpdateInternalState(currentTimesliceTotalMclkCycles, true, false, false, false, false, true, true);
				}
				else if(GetProcessorStateMclkCurrent() < currentTimesliceTotalMclkCycles)
				{
					//If we currently have the bus, advance the processor state until the DMA
					//operation is complete, or we reach the end of the current timeslice.

					//##DEBUG##
	//				std::wcout << "VDP - DMAWorkerThread: " << lastTimesliceTotalExecuteTime << '\t' << stateLastUpdateMclk << '\t' << stateLastUpdateMclkUnused << "\n";

					UpdateInternalState(currentTimesliceTotalMclkCycles, true, false, false, false, false, true, false);
				}

				//Update the timeslice execution progress for this device
				GetDeviceContext()->SetCurrentTimesliceProgress(lastTimesliceMclkCyclesRemainingTime + GetProcessorStateTime());

				//If the DMA transfer has been completed, negate BR to release the bus.
				if(!dmaTransferActive)
				{
					//##DEBUG##
	//				if(outputTestDebugMessages)
	//				{
	//					std::wcout << "SetLineState - VDP_LINE_BR:\t" << false << '\t' << GetProcessorStateTime() << '\t' << GetProcessorStateMclkCurrent() << '\n';
	//				}

					//Calculate the time at which the line access change will occur, in
					//system time.
					double accessTime = GetProcessorStateTime() - lastTimesliceMclkCyclesRemainingTime;

					//This is a hack to handle VDP port access while a DMA transfer
					//operation is pending, but the bus hasn't yet been granted. This is
					//probably not the correct way the hardware would handle this kind of
					//event. We have this hack in place to work around a limitation in our
					//current M68000 core, which isn't able to grant the bus between two
					//halves of a long-word operation. Until we have a microcode level
					//M68000 core online, we cache these invalid reads, and process them
					//when the DMA operation is complete.
					if(dmaTransferInvalidPortWriteCached)
					{
						//##DEBUG##
						if(outputTestDebugMessages)
						{
							std::wcout << "VDP Performing a cached write!\n";
						}

						dmaTransferInvalidPortWriteCached = false;
						WriteInterface(0, dmaTransferInvalidPortWriteAddressCache, dmaTransferInvalidPortWriteDataCache, GetDeviceContext(), accessTime, 0);
					}

					//Since we've reached the end of this DMA operation, reset the
					//timeslice execution progress to the end of the timeslice.
					GetDeviceContext()->SetCurrentTimesliceProgress(currentTimesliceLength);

					//Note that by negating BR, the M68000 should negate BG in response.
					//This may not occur immediately however. In this case, we will have
					//already cleared the dmaTransferActive flag, so our worker thread
					//will continue to run without advancing the processor state until the
					//bus is released.
					memoryBus->SetLineState(LINE_BR, Data(GetLineWidth(LINE_BR), 0), GetDeviceContext(), GetDeviceContext(), accessTime, ACCESSCONTEXT_BR_RELEASE);

					//##DEBUG##
		//			std::wcout << "DMA Transfer complete\n";
				}
			}

			//If the VDP still has the bus, but we've negated BR or reached the end of the
			//current timeslice, suspend the worker thread until the next timeslice is
			//received, or the BR line state changes.
			if(busGranted)
			{
				//##DEBUG##
	//			std::wcout << L"DMAWorkerThread pausing\t" << GetProcessorStateTime() << '\n';
	//			std::wcout << '\t' << workerThreadActive << '\t' << workerThreadPaused << '\t' << busGranted << '\n';

				//Suspend the DMA worker thread until a new timeslice is received.
				workerThreadPaused = true;
				GetDeviceContext()->SetTransientExecutionActive(false);
				workerThreadIdle.notify_all();
				workerThreadUpdate.wait(lock);
				GetDeviceContext()->SetTransientExecutionActive(busGranted);
				workerThreadPaused = false;

				//##DEBUG##
	//			std::wcout << L"DMAWorkerThread resuming\t" << GetProcessorStateTime() << '\n';
			}
		}
	}

	//##DEBUG##
//	std::wcout << L"DMAWorkerThread terminating\t" << GetProcessorStateTime() << '\n';
//	std::wcout << '\t' << workerThreadActive << '\t' << busGranted << '\n';

	workerThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetCELineID(const wchar_t* lineName, bool inputLine) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"LDS")
	{
		return CELINE_LDS;
	}
	else if(lineNameString == L"UDS")
	{
		return CELINE_UDS;
	}
	else if(lineNameString == L"R/W")
	{
		return CELINE_RW;
	}
	else if(lineNameString == L"AS")
	{
		return CELINE_AS;
	}
	else if(lineNameString == L"RMWCycleInProgress")
	{
		return CELINE_RMWCYCLEINPROGRESS;
	}
	else if(lineNameString == L"RMWCycleFirstOperation")
	{
		return CELINE_RMWCYCLEFIRSTOPERATION;
	}
	else if(lineNameString == L"LWR")
	{
		return CELINE_LWR;
	}
	else if(lineNameString == L"UWR")
	{
		return CELINE_UWR;
	}
	else if(lineNameString == L"CAS0")
	{
		return CELINE_CAS0;
	}
	else if(lineNameString == L"RAS0")
	{
		return CELINE_RAS0;
	}
	else if(lineNameString == L"OE0")
	{
		return CELINE_OE0;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch(lineID)
	{
	case CELINE_LDS:
		ceLineMaskLowerDataStrobeInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_UDS:
		ceLineMaskUpperDataStrobeInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RW:
		ceLineMaskReadHighWriteLowInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_AS:
		ceLineMaskAddressStrobeInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RMWCYCLEINPROGRESS:
		ceLineMaskRMWCycleInProgress = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RMWCYCLEFIRSTOPERATION:
		ceLineMaskRMWCycleFirstOperation = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch(lineID)
	{
	case CELINE_LDS:
		ceLineMaskLowerDataStrobeOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_UDS:
		ceLineMaskUpperDataStrobeOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RW:
		ceLineMaskReadHighWriteLowOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_AS:
		ceLineMaskAddressStrobeOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_LWR:
		ceLineMaskLWR = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_UWR:
		ceLineMaskUWR = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_CAS0:
		ceLineMaskCAS0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RAS0:
		ceLineMaskRAS0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_OE0:
		ceLineMaskOE0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, double accessTime) const
{
	bool vdpIsSource = (caller == this->GetDeviceContext());
	bool currentLowerDataStrobe = (currentCELineState & ceLineMaskLowerDataStrobeInput) != 0;
	bool currentUpperDataStrobe = (currentCELineState & ceLineMaskUpperDataStrobeInput) != 0;
	bool operationIsWrite = (currentCELineState & ceLineMaskReadHighWriteLowInput) == 0;
	bool rmwCycleInProgress = (currentCELineState & ceLineMaskRMWCycleInProgress) != 0;
	bool rmwCycleFirstOperation = (currentCELineState & ceLineMaskRMWCycleFirstOperation) != 0;
	if(vdpIsSource)
	{
		currentLowerDataStrobe = true;
		currentUpperDataStrobe = true;
		operationIsWrite = false;
		rmwCycleInProgress = false;
		rmwCycleFirstOperation = false;
	}
	return BuildCELine(location, vdpIsSource, currentLowerDataStrobe, currentUpperDataStrobe, operationIsWrite, rmwCycleInProgress, rmwCycleFirstOperation);
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller) const
{
	bool vdpIsSource = (caller == this->GetDeviceContext());
	bool currentLowerDataStrobe = (currentCELineState & ceLineMaskLowerDataStrobeInput) != 0;
	bool currentUpperDataStrobe = (currentCELineState & ceLineMaskUpperDataStrobeInput) != 0;
	bool operationIsWrite = (currentCELineState & ceLineMaskReadHighWriteLowInput) == 0;
	bool rmwCycleInProgress = (currentCELineState & ceLineMaskRMWCycleInProgress) != 0;
	bool rmwCycleFirstOperation = (currentCELineState & ceLineMaskRMWCycleFirstOperation) != 0;
	if(vdpIsSource)
	{
		currentLowerDataStrobe = true;
		currentUpperDataStrobe = true;
		operationIsWrite = false;
		rmwCycleInProgress = false;
		rmwCycleFirstOperation = false;
	}
	return BuildCELine(location, vdpIsSource, currentLowerDataStrobe, currentUpperDataStrobe, operationIsWrite, rmwCycleInProgress, rmwCycleFirstOperation);
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::BuildCELine(unsigned int targetAddress, bool vdpIsSource, bool currentLowerDataStrobe, bool currentUpperDataStrobe, bool operationIsWrite, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	//Calculate the state of all the various CE lines
	bool lineLWR = operationIsWrite && currentLowerDataStrobe;
	bool lineUWR = operationIsWrite && currentUpperDataStrobe;
	bool lineCAS0 = !operationIsWrite && ((!vdpIsSource && (targetAddress <= 0xDFFFFF)) || (vdpIsSource && (targetAddress <= 0xFFFFFF)));
	bool lineRAS0 = (!vdpIsSource && (targetAddress >= 0xE00000) && (targetAddress <= 0xFFFFFF)) || (vdpIsSource && (targetAddress >= 0x800000) && (targetAddress <= 0xFFFFFF));
	bool lineOE0 = (!operationIsWrite && !vdpIsSource && (targetAddress >= 0xE00000) && (targetAddress <= 0xFFFFFF)) || (!operationIsWrite && vdpIsSource && (targetAddress <= 0xFFFFFF));

	//If this bus operation is part of a Read-Modify-Write cycle, we need to handle it
	//slightly differently. The VDP asserts its CE lines when the AS line is asserted,
	//indicating that a valid address has been placed on the bus. Based on the state of
	//the address lines and other lines like R/W at that moment, the VDP determines the
	//state of its output CE lines, and asserts them. The VDP does NOT re-evaluate the
	//state of the CE lines over the course of the bus operation however, it simply holds
	//them at the state it determined when AS was first asserted. when AS is negated, the
	//VDP negates its CE lines. When the M68000 is driving the bus and executes a TAS
	//instruction however, this involves an indivisible read-modify-write cycle. A
	//read-modify-write cycle is exactly the same as a read followed by a write, except
	//that AS remains asserted through both the read and write, being asserted at the
	//start of the read, and negated when the write is complete. The effect this has on
	//the VDP is that the CE lines are frozen at the same state they would be for a read
	//throughout both the read and write cycles. In particular, this affects the LWR, UWR,
	//CAS0, and OE0 lines, which use the R/W line state to determine their output. This
	//can result in devices performing a second (ignored) read at the write phase of the
	//TAS instruction, essentially meaning the write phase of the TAS instruction is
	//ignored. This reportedly affects some games on the Sega Mega Drive (Gargoyles and
	//Ex-Mutants), which rely on the write phase of the TAS instruction failing. We
	//emulate this behaviour here, by caching the output CE line state on the read
	//component of a read-modify-write cycle, and retrieving the CE line state on the
	//write component. This is done through the use of two pseudo-CE lines from the
	//M68000, which indicate whether we are performing a read-modify-write cycle, and
	//whether this is the first operation of that cycle, respectively.
	if(rmwCycleInProgress)
	{
		if(rmwCycleFirstOperation)
		{
			lineLWRSavedStateRMW = lineLWR;
			lineUWRSavedStateRMW = lineUWR;
			lineCAS0SavedStateRMW = lineCAS0;
			lineRAS0SavedStateRMW = lineRAS0;
			lineOE0SavedStateRMW = lineOE0;
		}
		else
		{
			lineLWR = lineLWRSavedStateRMW;
			lineUWR = lineUWRSavedStateRMW;
			lineCAS0 = lineCAS0SavedStateRMW;
			lineRAS0 = lineRAS0SavedStateRMW;
			lineOE0 = lineOE0SavedStateRMW;
		}
	}

	//Build the actual CE line state based on the asserted CE lines
	unsigned int ceLineState = 0;
	ceLineState |= lineLWR? ceLineMaskLWR: 0x0;
	ceLineState |= lineUWR? ceLineMaskUWR: 0x0;
	ceLineState |= lineCAS0? ceLineMaskCAS0: 0x0;
	ceLineState |= lineRAS0? ceLineMaskRAS0: 0x0;
	ceLineState |= lineOE0? ceLineMaskOE0: 0x0;
	if(vdpIsSource)
	{
		//If the VDP has the bus, we need to drive the M68K CE lines ourselves.
		ceLineState |= ceLineMaskLowerDataStrobeOutput;
		ceLineState |= ceLineMaskUpperDataStrobeOutput;
		ceLineState |= ceLineMaskReadHighWriteLowOutput;
		ceLineState |= ceLineMaskAddressStrobeOutput;
	}

	//Return the generated CE line state
	return ceLineState;
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
//The real VDP does its own address line decoding, and doesn't have a dedicated CE line.
//The VDP has 23 address line inputs CA0-CA22, and it will respond to a read or write
//when the address lines match the following state: 110**000 ******** 000****. Note that
//this line decoding is based on how the VDP responds in the Mega Drive. It may be that
//some of the external pins allow the base address of the VDP to be adjusted by grounding
//or lifting certain pins, however if any pins perform this function, they are currently
//undocumented. Target register decoding is based on CA3-CA1:
//3210
//000* - Data Port
//001* - Control Port
//01** - HV Counter
//10** - SN76489 PSG
//110* - Unused
//111* - Test Register
//In our implementation, we allow the system to map the VDP to the bus as if it had a
//dedicated CE line, so this core can be made to respond to access on any address. Target
//register decoding assumes the lower 4 bits of the target address correspond with
//CA3-CA0.
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult S315_5313::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	IBusInterface::AccessResult accessResult(true);
	boost::mutex::scoped_lock lock(accessMutex);
	while(!eventsProcessedForTimeslice)
	{
		eventProcessingCompleted.wait(lock);
	}

	//Convert the access time into a cycle count relative to MCLK, rounding up the result
	//to the nearest MCLK cycle.
	unsigned int accessMclkCycle = ConvertAccessTimeToMclkCount(accessTime + lastTimesliceMclkCyclesRemainingTime);
	double accessMclkCycleInAccessTime = ConvertMclkCountToAccessTime(accessMclkCycle);
	if(accessMclkCycleInAccessTime < accessTime)
	{
		accessMclkCycle += 1;
	}

	//Check that the access is targeting the VDP, not the embedded PSG.
	if((location & 0xC) != 0x8)
	{
		//##DEBUG##
		if(outputTestDebugMessages)
		{
			std::wcout << "VDPReadInterface:\t" << std::hex << std::uppercase << location << '\t' << accessMclkCycle << '\t' << GetProcessorStateMclkCurrent() << '\t' << accessTime << '\t' << lastTimesliceMclkCyclesRemainingTime << '\n';
		}

		//##DEBUG##
		if(commandCode.GetBit(5) && !dmd1)
		{
			std::wcout << "######################################################\n";
			std::wcout << "VDP ReadInterface called when a DMA transfer was in progress!\n";
			std::wcout << "location:\t" << location << "\n";
			std::wcout << "accessTime:\t" << accessTime << "\n";
			std::wcout << "accessMclkCycle:\t" << accessMclkCycle << "\n";
			std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
			std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
			std::wcout << "busGranted:\t" << busGranted << "\n";
			std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
			std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
			std::wcout << "status:\t" << status.GetData() << "\n";
			std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
			std::wcout << "dmd1:\t" << dmd1 << "\n";
			std::wcout << "dmd0:\t" << dmd1 << "\n";
			std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
			std::wcout << "######################################################\n";
		}

		//##DEBUG##
		if(busGranted)
		{
			//outputTestDebugMessages = true;
			std::wcout << "######################################################\n";
			std::wcout << "VDP ReadInterface called when busGranted was set!\n";
			std::wcout << "location:\t" << location << "\n";
			std::wcout << "accessTime:\t" << accessTime << "\n";
			std::wcout << "accessMclkCycle:\t" << accessMclkCycle << "\n";
			std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
			std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
			std::wcout << "busGranted:\t" << busGranted << "\n";
			std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
			std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
			std::wcout << "status:\t" << status.GetData() << "\n";
			std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
			std::wcout << "dmd1:\t" << dmd1 << "\n";
			std::wcout << "dmd0:\t" << dmd1 << "\n";
			std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
			std::wcout << "######################################################\n";
		}

		//Trigger a system rollback if the device has been accessed out of order
		if(lastAccessTime > accessTime)
		{
			GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
		}
		lastAccessTime = accessTime;

		//Update the current VDP state
		UpdateInternalState(accessMclkCycle, false, false, false, false, false, false, false);
	}

	//Decode the target address to determine which port was accessed
	switch((location >> 1) & 0x7)
	{
	case 0: //000 - Data Port
		//If the VDP was prepared to latch the next write to the control port as the
		//second half of a command data block, that state is cancelled when the data port
		//is read from or written to, or the control port is read.
		commandWritePending = false;

		//Ensure we have a valid read target loaded
		if(!ValidReadTargetInCommandCode())
		{
			//##TODO## If we reach this point in code, we've attempted to read from an
			//invalid read target. This causes a lockup on the real hardware, as the
			//requested data never becomes available for reading. We should be logging a
			//critical error here.
		}
		else
		{
			//Advance the internal state until we have a data value buffered in the read
			//buffer.
			UpdateInternalState(accessMclkCycle, true, false, false, false, true, false, true);

			//Calculate any delay which occurred in processing this operation, and return
			//the delay time to the caller.
			accessResult.executionTime += ((lastTimesliceMclkCyclesRemainingTime + GetProcessorStateTime()) - accessTime);
			if(accessResult.executionTime < 0.0)
			{
				//##DEBUG##
				std::wcout << "VDP negative executionTime: " << accessResult.executionTime << '\t' << GetProcessorStateTime() << '\t' << lastTimesliceMclkCyclesRemainingTime << '\t' << accessTime << '\n';
			}
		}

		//##DEBUG##
		//##TODO## Keep this check in at runtime, but log a hard error or assertion
		//failure.
		if(!readDataAvailable)
		{
			std::wcout << "VDP Error! Reached M5ReadFIFO when no valid data had been cached!\n";
			std::wcout << "\tCommandCode=0x" << std::hex << std::uppercase << fifoBuffer[fifoNextWriteEntry].codeRegData.GetData() << "\tAddress=0x" << std::hex << std::uppercase << fifoBuffer[fifoNextWriteEntry].addressRegData.GetData() << '\n';
		}

		//Now that we have a data value loaded into the read buffer, return it to the
		//caller.
		data = readBuffer;

		//Now that the cached data has been read from the buffer, the readDataAvailable
		//flag is cleared. This will cause the VDP to read the next word of data from the
		//read target on the next available access slot.
		readDataAvailable = false;
		readDataHalfCached = false;

		break;
	case 1: //001 - Control Port
		//If the VDP was prepared to latch the next write to the control port as the
		//second half of a command data block, that state is cancelled when the data port
		//is read from or written to, or the control port is read.
		commandWritePending = false;

		//Read the current value of the status register. Note that the status register is
		//a 10-bit register, so the call below will only affect the lower 10 bits of the
		//data value.
		data.SetLowerBits(status);

		//##DEBUG##
//		std::wcout << "VDP - ReadSR: 0x" << std::hex << std::uppercase << status.GetData() << '\n';

		//Hardware tests have shown that when reading the status register, only the lower
		//10 lines of the full 16 data lines are touched, corresponding with the 10-bit
		//status register value. The upper 6 lines are left floating. When the M68000
		//reads the status register from the VDP, this has the effect of the upper 6 data
		//lines retaining their previous values from the last bus read operation. We
		//emulate that behaviour here by using data line masking for the result.
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = statusRegisterMask;

		//Since the status register has now been read, clear the sprite overflow and
		//collision flags.
		SetStatusFlagSpriteOverflow(false);
		SetStatusFlagSpriteCollision(false);

		break;
	case 2: //01* - HV Counter
	case 3:
		data = GetHVCounter();
		break;
	case 4: //10* - SN76489 PSG
	case 5:
		if(psg != 0)
		{
			return psg->ReadInterface(0, (location >> 1) & 0x1, data, caller, accessTime, accessContext);
		}
		break;
	case 6: //110 - Unused
		//##TODO## Perform hardware tests to confirm reads from this unused register
		//location don't assert any data lines.
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0;
		break;
	case 7: //111 - Test Register
		//##TODO## Perform hardware tests to determine if reads from the test register
		//return valid data.
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0;
		break;
	}

	//##DEBUG##
//	std::wcout << "VDPReadInterface:\t" << std::hex << std::uppercase << location << '\t' << data.GetData() << '\t' << accessTime << '\n';

	return accessResult;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult S315_5313::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	IBusInterface::AccessResult accessResult(true);
	boost::mutex::scoped_lock lock(accessMutex);
	while(!eventsProcessedForTimeslice)
	{
		eventProcessingCompleted.wait(lock);
	}

	//Convert the access time into a cycle count relative to MCLK, rounding up the result
	//to the nearest MCLK cycle.
	unsigned int accessMclkCycle = ConvertAccessTimeToMclkCount(accessTime + lastTimesliceMclkCyclesRemainingTime);
	double accessMclkCycleInAccessTime = ConvertMclkCountToAccessTime(accessMclkCycle);
	if(accessMclkCycleInAccessTime < accessTime)
	{
		accessMclkCycle += 1;
	}

	//Check that the access is targeting the VDP, not the embedded PSG.
	//##TODO## Find a neater way to implement this
	if((location & 0xC) != 0x8)
	{
		//##DEBUG##
		if(outputTestDebugMessages)
		{
			std::wcout << "VDPWriteInterface: " << std::hex << std::uppercase << location << '\t' << data.GetData() << '\t' << accessMclkCycle << '\t' << GetProcessorStateMclkCurrent() << '\t' << accessTime << '\t' << lastTimesliceMclkCyclesRemainingTime << '\n';
		}

		//This is a hack to handle VDP port access while a DMA transfer operation is
		//pending, but the bus hasn't yet been granted. This is probably not the correct
		//way the hardware would handle this kind of event. We have this hack in place to
		//work around a limitation in our current M68000 core, which isn't able to grant
		//the bus between two halves of a long-word operation. Until we have a microcode
		//level M68000 core online, we cache these invalid reads, and process them when
		//the DMA operation is complete.
		if(commandCode.GetBit(5) && !dmd1 && !busGranted)
		{
			//##DEBUG##
			if(outputTestDebugMessages)
			{
				std::wcout << "VDP WriteInterface called while a bus request was pending! Caching the write.\n";
			}

			if((location & 0xE) == 0)
			{
				dmaTransferInvalidPortWriteCached = true;
				dmaTransferInvalidPortWriteAddressCache = location;
				dmaTransferInvalidPortWriteDataCache = data;
			}
			else if((location & 0xE) == 2)
			{
				dmaTransferInvalidPortWriteCached = true;
				dmaTransferInvalidPortWriteAddressCache = location;
				dmaTransferInvalidPortWriteDataCache = data;
			}
			return accessResult;
		}

		//##DEBUG##
		if((commandCode.GetBit(5) != GetStatusFlagDMA()) && (commandCode.GetBit(5) != GetStatusFlagDMA()))
		{
			std::wcout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
			std::wcout << "VDP commandCode.GetBit(5) != GetStatusFlagDMA()\n";
			std::wcout << "__LINE__:\t" << __LINE__ << "\n";
			std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
			std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
			std::wcout << "busGranted:\t" << busGranted << "\n";
			std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
			std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
			std::wcout << "status:\t" << status.GetData() << "\n";
			std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
			std::wcout << "dmd1:\t" << dmd1 << "\n";
			std::wcout << "dmd0:\t" << dmd1 << "\n";
			std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
			std::wcout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
		}

		//##DEBUG##
		if(commandCode.GetBit(5) && !dmd1)
		{
			std::wcout << "######################################################\n";
			std::wcout << "VDP WriteInterface called when a DMA transfer was in progress!\n";
			std::wcout << "location:\t" << location << "\n";
			std::wcout << "data:\t" << data.GetData() << "\n";
			std::wcout << "accessTime:\t" << accessTime << "\n";
			std::wcout << "accessMclkCycle:\t" << accessMclkCycle << "\n";
			std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
			std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
			std::wcout << "busGranted:\t" << busGranted << "\n";
			std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
			std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
			std::wcout << "status:\t" << status.GetData() << "\n";
			std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
			std::wcout << "dmd1:\t" << dmd1 << "\n";
			std::wcout << "dmd0:\t" << dmd1 << "\n";
			std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
			std::wcout << "######################################################\n";
		}

		//##DEBUG##
		if(busGranted && (caller != GetDeviceContext()))
		{
			//outputTestDebugMessages = true;
			std::wcout << "######################################################\n";
			std::wcout << "VDP WriteInterface called when busGranted was set!\n";
			std::wcout << "location:\t" << location << "\n";
			std::wcout << "data:\t" << data.GetData() << "\n";
			std::wcout << "accessTime:\t" << accessTime << "\n";
			std::wcout << "accessMclkCycle:\t" << accessMclkCycle << "\n";
			std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
			std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
			std::wcout << "busGranted:\t" << busGranted << "\n";
			std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
			std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
			std::wcout << "status:\t" << status.GetData() << "\n";
			std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
			std::wcout << "dmd1:\t" << dmd1 << "\n";
			std::wcout << "dmd0:\t" << dmd1 << "\n";
			std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
			std::wcout << "######################################################\n";
		}

		//Trigger a system rollback if the device has been accessed out of order
		if(lastAccessTime > accessTime)
		{
			GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
		}
		lastAccessTime = accessTime;

		//Update the current VDP state to the target access time
		UpdateInternalState(accessMclkCycle, false, false, false, false, false, false, false);
	}

	//Decode the target address to determine which port was accessed
	switch((location >> 1) & 0x7)
	{
	case 0:{ //000 - Data Port
		//If the VDP was prepared to latch the next write to the control port as the
		//second half of a command data block, that state is cancelled when the data port
		//is read from or written to, or the control port is read.
		commandWritePending = false;

		//If the 4-word FIFO is full at the time a data port write is attempted, we
		//advance the device state forward one slot, until there's one space free in the
		//FIFO, and calculate the time during which the accessing device is stalled.
		UpdateInternalState(accessMclkCycle, true, false, false, true, false, false, true);

		//Calculate any delay which occurred in processing this operation, and return the
		//delay time to the caller.
		accessResult.executionTime += ((lastTimesliceMclkCyclesRemainingTime + GetProcessorStateTime()) - accessTime);
		if(accessResult.executionTime < 0.0)
		{
			//##DEBUG##
			std::wcout << "VDP negative executionTime: " << accessResult.executionTime << '\t' << GetProcessorStateTime() << '\t' << lastTimesliceMclkCyclesRemainingTime << '\t' << accessTime << '\n';
		}

		//Save the code and address register data for this data port write
		if(codeAndAddressRegistersModifiedSinceLastWrite)
		{
			fifoBuffer[fifoNextWriteEntry].codeRegData = commandCode;
			fifoBuffer[fifoNextWriteEntry].addressRegData = commandAddress;
			codeAndAddressRegistersModifiedSinceLastWrite = false;
		}
		else
		{
			unsigned int fifoLastWriteEntry = (fifoNextWriteEntry+(fifoBufferSize-1)) % fifoBufferSize;
			fifoBuffer[fifoNextWriteEntry].codeRegData = fifoBuffer[fifoLastWriteEntry].codeRegData;
			fifoBuffer[fifoNextWriteEntry].addressRegData = fifoBuffer[fifoLastWriteEntry].addressRegData + autoIncrementData;
		}

		//Write the new data value to the FIFO
		//##TODO## Check if writes to invalid targets stay in the FIFO until an access
		//slot opens up for each one, at which point they are discarded, but the access
		//slot is consumed, or if they are discarded straight away and the next pending
		//write processed. Most likely, a write to an invalid target consumes an access
		//slot, but we should test by examining the FIFO full/empty flags to be sure.
		fifoBuffer[fifoNextWriteEntry].dataPortWriteData = data;
		fifoBuffer[fifoNextWriteEntry].dataWriteHalfWritten = false;
		fifoBuffer[fifoNextWriteEntry].pendingDataWrite = true;

		//Advance the FIFO to the next slot
		//##FIX## Our tests show that command port writes can be made while the FIFO is
		//full, and the FIFO still reads as full after the write. How is this possible if
		//the command code and address data is written directly to the next available FIFO
		//entry? We should do a test to make sure this case doesn't corrupt or in any way
		//affect the next write in the FIFO, such as sending it to the new address and
		//write target, rather than the address and write target it was intended for, but
		//if this cannot be made to occur, then it seems clear that the command code and
		//address data must be cached separately from the FIFO, and that this data is only
		//written into the FIFO when an actual data port write is made. If this is the
		//case, it supports the possibility that all read processing is actually separate
		//from the write FIFO too.
		fifoNextWriteEntry = (fifoNextWriteEntry+1) % fifoBufferSize;

		//Update the FIFO full and empty flags in the status register
		bool fifoEmpty = IsWriteFIFOEmpty();
		bool fifoFull = IsWriteFIFOFull();
		SetStatusFlagFIFOEmpty(fifoEmpty);
		SetStatusFlagFIFOFull(fifoFull);

		//##TODO## I believe performing a write to a read target prevents subsequent read
		//operations from succeeding. Do more testing to determine if this is actually due
		//to CD4 being set, or rather, some flag or status indicating that data is
		//available for reading being cleared. By performing tests with the write,
		//previous read cache operation, and subsequent read in different states, we
		//should be able to gather more information. If at any point two successive read
		//operations can be made to work after an invalid port write, CD4 cannot be set by
		//a data port write.
		commandCode.SetBit(4, true);

		//##FIX## This comment is incorrect. We didn't just write over it.
		//Now that we've added a new write to the FIFO, we invalidate any cached read data
		//that may have been buffered, since if it existed, we just wrote over it.
		readDataAvailable = false;
		readDataHalfCached = false;
		break;}
	case 1:{ //001 - Control Port
		//It is almost certain that control port writes are not processed until the FIFO
		//is empty, otherwise the changes which are made to the the command code and
		//address register would interfere with pending writes held in the FIFO. See the
		//thread at http://gendev.spritesmind.net/forum/viewtopic.php?t=790 for
		//discussion. We emulate that here by forcing control port writes to wait until
		//the FIFO is empty.
		//##FIX## Hardware tests have now shown this assumption to be incorrect. Data port
		//writes and control port writes can both be made regardless of the current state
		//of the FIFO, so each entry in the FIFO must somehow cache its write address and
		//write target. Also note that control port writes don't seem to delay the
		//progression of the FIFO, and we have specifically tested a FIFO that had half
		//its writes going to one write target, and half the other writes going to another
		//write target, at different addresses, with the FIFO full and a full control port
		//write while the FIFO was not empty, followed by data port writes in order to set
		//all this up. All data was written correctly and was verified by reading it back.
		//This clearly demonstrates that the write FIFO must latch the current target
		//address and code register data, or at the very least, the write target from the
		//code register data. Writing a read target to the code register has no apparent
		//effect, but performing a data port read does fully flush the FIFO before
		//returning the read data. After a data port read, the FIFO is always empty.
//		UpdateInternalState(accessMclkCycle, true, true, false, false, false, true);

		//Calculate any delay which occurred in processing this operation, and return the
		//delay time to the caller.
		accessResult.executionTime += ((lastTimesliceMclkCyclesRemainingTime + GetProcessorStateTime()) - accessTime);
		if(accessResult.executionTime < 0.0)
		{
			//##DEBUG##
			std::wcout << "VDP negative executionTime: " << accessResult.executionTime << '\t' << GetProcessorStateTime() << '\t' << lastTimesliceMclkCyclesRemainingTime << '\t' << accessTime << '\n';
		}

		//Calculate the number of mclk cycles that this operation was delayed
		unsigned int accessMclkCycleDelay = GetProcessorStateMclkCurrent() - accessMclkCycle;

		//##FIX## Testing has indicated that control port writes are ignored while DMA
		//operations are in progress. Note that this does not simply mean when CD5 is set
		//however, as control port writes can be made while a DMA fill operation is setup,
		//but waiting for a data port write to trigger it, and we know from current
		//testing that the DMA busy flag reads as set after a DMA fill has been setup, but
		//before the fill data is written.
		//##TODO## Confirm that control port writes can be made while a DMA fill operation
		//is waiting for a data port write
		//##TODO## Check if the DMA busy flag remains set if a partial control port write
		//is made while a DMA fill operation is waiting for a data port write, in
		//particular when we write the same data over again for the first command word. If
		//it does not, it would appear that CD5 is cleared when a partial control port
		//write is made.
		if(!commandWritePending)
		{
			ProcessCommandDataWriteFirstHalf(data);

			//Once a partial control port write has been made, hardware tests indicate
			//that CD4 is set. This occurs even if the partial control port write is
			//interpreted as a register write. CD4 is most likely set at this time for
			//performance reasons. When CD4 is set, no read targets are valid. If a valid
			//read target could be setup by a partial control port write, read pre-caching
			//could begin between the two halves of a full control port write, which would
			//be a bad thing if the indicated read target after the first control port
			//write was not actually what the caller wanted to access.
			commandCode.SetBit(4, true);

			//Contrary to other documentation, and the behaviour of other emulators,
			//register writes go directly to the code and address registers first as a
			//normal command word write. This is the cause of the apparent "clearing" of
			//the code register when a register write occurs, as reported by Charles
			//MacDonald. The register write is then processed from the new address
			//register data. Hardware tests have shown the upper bits of the command and
			//address registers remain unaffected.
			if(commandCode.GetBit(1) && !commandCode.GetBit(0))
			{
				//Register Write
				//-----------------------------------------------------------------
				//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
				//|---------------------------------------------------------------|
				//| 1 | 0 | / |    Register No    |             Data              |
				//|---------------------------------------------------------------|
				//|CD1|CD0|A13|A12|A11|A10|A9 |A8 |A7 |A6 |A5 |A4 |A3 |A2 |A1 |A0 |
				//-----------------------------------------------------------------

				//Perform the register write
				unsigned int registerNo = originalCommandAddress.GetDataSegment(8, 5);
				Data registerData(8, originalCommandAddress.GetDataSegment(0, 8));
				if(registerNo < registerCount)
				{
					RegisterSpecialUpdateFunction(accessMclkCycle + accessMclkCycleDelay, accessTime, accessResult.executionTime, caller, accessContext, registerNo, registerData);
					AccessTarget accessTarget;
					accessTarget.AccessTime(accessMclkCycle + accessMclkCycleDelay);
					SetRegisterData(registerNo, accessTarget, registerData);
				}

				//Since the command word has just been latched as a register write, the
				//VDP is no longer expecting a second command word to be written.
				commandWritePending = false;
			}
		}
		else
		{
			ProcessCommandDataWriteSecondHalf(data);

			//Check if we need to obtain the bus due to an external DMA transfer operation
			if(commandCode.GetBit(5) //The command code has a DMA operation flagged
			&& !dmd1)                //DMA is set to perform an external copy operation
			{
				//Note that a DMA operation will be triggered regardless of the state of
				//CD4, as this bit isn't tested when deciding whether to trigger DMA, and
				//write target decoding only looks at the state of CD3-CD0.

				//Note that at this point, the VDP will request the bus. If the bus
				//request is granted, the DMA worker thread will go active to process the
				//transfer. When the transfer is completed, the VDP will release the bus.
				//##TODO## Determine what happens if the bus request is not granted
				//straight away. IE, What happens if another control port write occurs
				//before the bus is granted? Does the VDP process the write, or does it
				//lock up until the bus is granted? We can test this by physically
				//preventing the VDP from asserting the BR line, so that the M68000 never
				//grants the bus, and see how the VDP responds if we continue to access
				//it.
				//##TODO## Our M68000 core currently doesn't support granting the bus
				//between the two halves of a long-word memory write, but we need to
				//support this here, as a DMA transfer could be triggered by the first
				//word of a long-word write to the control port. Find a way to implement
				//support for the behaviour we need.
				boost::mutex::scoped_lock lock(workerThreadMutex);
				memoryBus->SetLineState(LINE_BR, Data(GetLineWidth(LINE_BR), 1), GetDeviceContext(), GetDeviceContext(), accessTime + accessResult.executionTime, ACCESSCONTEXT_BR_ASSERT);

				//Set the initial DMA transfer register settings
				dmaTransferActive = true;
				dmaTransferReadDataCached = false;
				//Note that we technically don't need to set these here, as they are only
				//correctly initialized once the bus request is granted, but we set them
				//here anyway as it can be useful for debugging purposes to know when the
				//DMA operation was triggered relative to when the bus was granted.
				dmaTransferNextReadMclk = accessMclkCycle;
				dmaTransferLastTimesliceUsedReadDelay = 0;

				//##DEBUG##
//				if(outputTestDebugMessages)
//				{
//					std::wcout << "SetLineState - VDP_LINE_BR:\t" << true << '\t' << GetProcessorStateTime() << '\t' << GetProcessorStateMclkCurrent() << '\n';
//				}
			}
		}

		//Hardware tests have shown that the command address is cached internally,
		//separate from the command address register that gets written to on a command
		//port write. When the command address is incremented during read and write
		//operations, it is this internal copy of the command address which gets
		//incremented. Even doing a partial command port write will cause the internal
		//command address register to be fully reloaded from the original command address.
		//Even if the upper 2 bits of the command address have been changed since the
		//original address was setup, they will be reloaded on a partial command port
		//write. We emulate that behaviour here by keeping a separate copy of the command
		//address in its original, unmodified state.
		//##TODO## Recent hardware tests dispute these findings. Do more testing to
		//determine what the correct behaviour is.
		commandAddress = originalCommandAddress;

		//Flag that the command and address registers have been modified since the last
		//write. This is important in order to ensure that subsequent data port writes
		//latch the new data, rather than incrementing the write address from the previous
		//write target.
		codeAndAddressRegistersModifiedSinceLastWrite = true;

		//Now that the command code has changed, any cached read data is invalidated. We
		//clear this flag here now so that new data will be retrieved for the target on
		//the next available access slot, if it is a valid read target.
		readDataAvailable = false;
		readDataHalfCached = false;
		break;}
	case 2: //01* - HV Counter
	case 3:
		break;
	case 4: //10* - SN76489 PSG
	case 5:
		if(psg != 0)
		{
			return psg->WriteInterface(0, (location >> 1) & 0x1, data, caller, accessTime, accessContext);
		}
		break;
	case 6: //110 - Unused
		break;
	case 7: //111 - Test Register
		//##TODO## Implement test register writes
		break;
	}

	return accessResult;
}

//----------------------------------------------------------------------------------------
//Port functions
//----------------------------------------------------------------------------------------
Data S315_5313::GetHVCounter() const
{
	//Since the status register has already been updated when the device was accessed,
	//build the hv counter based on the current processor state.

	//Build the 8-bit external hcounter
	//The internal hcounter is 9-bit, and the external hcounter is 8-bit. The upper 8 bits
	//of the 9-bit internal hcounter are used to build the 8-bit external hcounter, as
	//shown below:
	//Internal counter: 876543210
	//External counter: 87654321
	Data currentHCounter(9);
	currentHCounter = (hvCounterLatched)? hcounterLatchedData: hcounter;
	Data externalHCounter(8);
	externalHCounter = currentHCounter.GetUpperBits(8);

	//Build the 8-bit external vcounter
	//The internal vcounter is 9-bit, and the external vcounter is 8-bit. The way the
	//internal counter is mapped to the external counter depends on the interlace mode.
	//Given that the internal vcounter is 9-bit, the way the bits are mapped from the
	//internal counter to the external counter is given below:
	//Internal counter:           876543210
	//External, no Interlacing:    76543210
	//External, interlace normal:  76543218
	//External, interlace double:  65432107
	Data currentVCounter(9);
	currentVCounter = (hvCounterLatched)? vcounterLatchedData: vcounter;
	Data externalVCounter(8);
	if(interlaceEnabled && interlaceDouble)
	{
		currentVCounter <<= 1;
	}
	externalVCounter = currentVCounter.GetLowerBits(8);
	if(interlaceEnabled)
	{
		externalVCounter.SetBit(0, currentVCounter.GetBit(8));
	}

	//Build and return the combined 16-bit external HV counter
	//-----------------------------------------------------------------
	//| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|-------------------------------|-------------------------------|
	//|   External V counter (8-bit)  |   External H counter (8-bit)  |
	//|---------------------------------------------------------------|
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|               External HV counter (16-bit)                    |
	//-----------------------------------------------------------------
	Data externalHVCounter(16);
	externalHVCounter.SetUpperHalf(externalVCounter);
	externalHVCounter.SetLowerHalf(externalHCounter);

	//##DEBUG##
	//std::wcout << "VDP - ReadHVCounter: 0x" << std::hex << std::uppercase << externalHVCounter.GetData() << '\n';

	return externalHVCounter;
}

//----------------------------------------------------------------------------------------
void S315_5313::ProcessCommandDataWriteFirstHalf(const Data& data)
{
	//Command data first half
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//| Code  |                    Address                            |
	//|-------|-------------------------------------------------------|
	//|CD1|CD0|A13|A12|A11|A10|A9 |A8 |A7 |A6 |A5 |A4 |A3 |A2 |A1 |A0 |
	//-----------------------------------------------------------------
	commandWritePending = true;
	commandCode.SetDataSegment(0, 2, data.GetDataSegment(14, 2));
	originalCommandAddress.SetDataSegment(0, 14, data.GetDataSegment(0, 14));
}

//----------------------------------------------------------------------------------------
void S315_5313::ProcessCommandDataWriteSecondHalf(const Data& data)
{
	//Command data second half
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|                               |     Code      |       |Address|
	//| /   /   /   /   /   /   /   / |---------------| /   / |-------|
	//|                               |CD5|CD4|CD3|CD2|       |A15|A14|
	//-----------------------------------------------------------------

	//Hardware tests have shown that when the DMA enable bit is cleared, the state of CD5
	//is unable to be modified by a control port write. The current state of CD5 is
	//retained, regardless of the new value of CD5 written in the second command port
	//write. This has been verified through hardware testing. We can verify that CD5
	//cannot be changed from clear to set by attempting to setup a valid DMA operation
	//while DMA is disabled, then enabling DMA operations and observing that the DMA
	//operation does not trigger. We can verify that CD5 cannot be changed from set to
	//clear by setting up a DMA fill operation, but not performing the data port write
	//immediately, but instead doing a register write to disable DMA operations, then
	//rewriting the first control word for the DMA fill. We can observe that the DMA busy
	//flag in the status register remains set after disabling DMA operations, and that the
	//DMA fill is still able to be triggered by writing to the data port. Additionally,
	//writing a new second command word where CD5 is clear doesn't stop the DMA fill
	//operation from being triggered when the data port is written to. Also note that this
	//shows that all the DMA enable bit does is prevent CD5 from being modified, it does
	//not actually disable DMA operations or affect the way a currently executing DMA
	//operation advances.
	unsigned int commandCodeBitsToSet = (dmaEnabled)? 4: 3;

	commandWritePending = false;
	commandCode.SetDataSegment(2, commandCodeBitsToSet, data.GetDataSegment(4, commandCodeBitsToSet));
	originalCommandAddress.SetDataSegment(14, 2, data.GetDataSegment(0, 2));

	//Keep the state of CD5 and the DMA flag in the status register in sync. On the real
	//hardware, the DMA flag in the status register is the literal realtime state of the
	//CD5 bit in the command code register. Note that this means that the DMA bit in the
	//status register is set when a control port write has been made to setup a DMA fill
	//operation, but the data port write has not yet been made to trigger the fill. This
	//is the way the real VDP behaves, and this has been confirmed though hardware tests.
	//Although the status register returns the realtime state of CD5 for the DMA busy
	//flag, we keep the two separated in our core for maximum efficiency on status
	//register reads, and keep them manually in sync whenever the CD5 bit is modified.
	SetStatusFlagDMA(commandCode.GetBit(5));
}

//----------------------------------------------------------------------------------------
void S315_5313::RegisterSpecialUpdateFunction(unsigned int mclkCycle, double accessTime, double accessDelay, IDeviceContext* caller, unsigned int accessContext, unsigned int registerNo, const Data& data)
{
	switch(registerNo)
	{
	case 0:
		hvCounterLatchEnabled = data.GetBit(1);
		if(hintEnabled != data.GetBit(4))
		{
			hintEnabled = data.GetBit(4);
			if(hintEnabled)
			{
				if(hintPending)
				{
					//Trigger HInt for M68000
					memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), hintIPLLineState), GetDeviceContext(), caller, accessTime + accessDelay, accessContext);
					hintPending = false;
				}
				else
				{
					//If horizontal interrupts have changed from disabled to enabled, and
					//there wasn't already a horizontal interrupt pending, trigger a
					//rollback, otherwise there's a chance a horizontal interrupt might
					//not be flagged at the correct time, since the GetNextEvent function
					//might not have generated a timing point for the hint counter
					//expiring.
					GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
				}
			}
		}
		break;
	case 1:
		screenModeV30Cached = data.GetBit(3);
		dmaEnabled = data.GetBit(4);
		displayEnabledCached = data.GetBit(6);

		if(vintEnabled != data.GetBit(5))
		{
			vintEnabled = data.GetBit(5);
			if(vintEnabled && vintPending)
			{
				//Trigger VInt for M68000
				memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), vintIPLLineState), GetDeviceContext(), caller, accessTime + accessDelay, accessContext);
				vintPending = false;
			}
			//Note that we never need to generate a rollback if the VINT pending state is
			//changed. We always generate a timing point for the VINT event, since the Z80
			//INT line always generates a VINT, even when the VINT enable bit isn't set.
		}
		break;
	case 5:
		//Handle changes to the sprite attribute (mapping) table base address register
		//affecting which data is written to the sprite cache.
		//##TODO## Perform hardware tests to confirm how we should be handling the AT9 bit
		//from the attribute table address. Apparently, this bit "should be 0 in H40 cell
		//mode", according to the official documentation, but there's a lot of room for
		//interpretation as to what should happen with that. If AT9 is set in H40 mode,
		//is the sprite cache centered around the address as if AT9 is set, or if it is
		//cleared? Is the sprite data from VRAM read as if AT9 is set, or cleared? Can a
		//register change to the sprite attribute table address cause pending writes in
		//the FIFO to miss being written to the sprite cache, or does the sprite cache
		//grab writes from the FIFO before they are written to VRAM? Does a H32/H40 screen
		//mode change have the same effect as changing the sprite table base address
		//register? If the screen mode is H32, are writes past the end of the maximum
		//sprite table size in H32 mode (0x200 bytes), but below the maximum table size in
		//H40 mode (0x280) stored in the cache, or are they ignored? If they're ignored,
		//does that mean that writing a H40 sprite table in H32 mode, then enabling H40
		//mode results in the sprite cache retaining whatever data it previously held in
		//the upper region of the cache? Does switching from H40 mode to H32 mode, then
		//back to H40 mode, result in the upper bytes of the sprite cache being cleared,
		//or do they retain their previous values? Hardware tests are definitely required
		//here.
		spriteAttributeTableBaseAddressDecoded = (data.GetDataSegment(0, 7) << 9);
		break;
	case 10:
		if(hintCounterReloadValue != data.GetData())
		{
			//##DEBUG##
			//std::wcout << "VDP - HCounter data changed: " << hintCounterReloadValue << '\t' << data.GetData() << '\n';

			hintCounterReloadValue = data.GetData();
			//If the HINT counter data has been modified while hint is disabled, trigger a
			//rollback.
			if(!hintEnabled)
			{
				GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
			}
		}
		break;
	case 11:
		verticalScrollModeCached = data.GetBit(2);
		if(exintEnabled != data.GetBit(3))
		{
			exintEnabled = data.GetBit(3);
			if(exintEnabled && exintPending)
			{
				//Trigger EXInt for M68000
				memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), exintIPLLineState), GetDeviceContext(), caller, accessTime + accessDelay, accessContext);
				exintPending = false;
			}
			else
			{
				//##TODO## Figure out what we really need to do here. Since external
				//interrupts aren't implemented yet, it's a bit hard to be sure.
//				//If external interrupts have changed from disabled to enabled, trigger a rollback.
//				GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
			}
		}
		break;
	case 12:
		if(screenModeRS0Cached != data.GetBit(7))
		{
			screenModeRS0Cached = data.GetBit(7);
			//If the RS0 bit has changed, we need to trigger a rollback, since any change
			//to this register will almost certainly affect when the next VDP timing point
			//event occurs.
			GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(screenModeRS1Cached != data.GetBit(0))
		{
			screenModeRS1Cached = data.GetBit(0);
			//If the RS1 bit has changed, we need to trigger a rollback, since any change
			//to this register will almost certainly affect when the next VDP timing point
			//event occurs.
			GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
		}
		interlaceDoubleCached = data.GetBit(2);
		interlaceEnabledCached = data.GetBit(1);
		break;
	case 15:
		autoIncrementData = data.GetData();
		break;
	case 19:
		dmaLengthCounter = (dmaLengthCounter & 0xFF00) | data.GetData();
		break;
	case 20:
		dmaLengthCounter = (dmaLengthCounter & 0x00FF) | (data.GetData() << 8);
		break;
	case 21:
		dmaSourceAddressByte1 = data.GetData();
		break;
	case 22:
		dmaSourceAddressByte2 = data.GetData();
		break;
	case 23:
		dmaSourceAddressByte3 = data.GetDataSegment(0, 7);
		dmd0 = data.GetBit(6);
		dmd1 = data.GetBit(7);
		break;
	}
}

//----------------------------------------------------------------------------------------
//HV counter internal/linear conversion
//----------------------------------------------------------------------------------------
unsigned int S315_5313::HCounterValueFromVDPInternalToLinear(const HScanSettings& hscanSettings, unsigned int hcounterCurrent)
{
	return (hcounterCurrent >= hscanSettings.hcounterBlankingInitialValue)? hscanSettings.hcounterActiveScanMaxValue + ((hcounterCurrent - hscanSettings.hcounterBlankingInitialValue) + 1): hcounterCurrent;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::VCounterValueFromVDPInternalToLinear(const VScanSettings& vscanSettings, unsigned int vcounterCurrent, bool oddFlagSet)
{
	unsigned int blankingInitialValue = oddFlagSet? vscanSettings.vcounterBlankingInitialValueOddFlag: vscanSettings.vcounterBlankingInitialValue;
	return (vcounterCurrent >= blankingInitialValue)? vscanSettings.vcounterActiveScanMaxValue + ((vcounterCurrent - blankingInitialValue) + 1): vcounterCurrent;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::HCounterValueFromLinearToVDPInternal(const HScanSettings& hscanSettings, unsigned int hcounterCurrent)
{
	return (hcounterCurrent > hscanSettings.hcounterActiveScanMaxValue)? ((hcounterCurrent - hscanSettings.hcounterActiveScanMaxValue) - 1) + hscanSettings.hcounterBlankingInitialValue: hcounterCurrent;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::VCounterValueFromLinearToVDPInternal(const VScanSettings& vscanSettings, unsigned int vcounterCurrent, bool oddFlagSet)
{
	unsigned int blankingInitialValue = oddFlagSet? vscanSettings.vcounterBlankingInitialValueOddFlag: vscanSettings.vcounterBlankingInitialValue;
	return (vcounterCurrent > vscanSettings.vcounterActiveScanMaxValue)? ((vcounterCurrent - vscanSettings.vcounterActiveScanMaxValue) - 1) + blankingInitialValue: vcounterCurrent;
}

//----------------------------------------------------------------------------------------
//Video scan settings functions
//----------------------------------------------------------------------------------------
const S315_5313::HScanSettings& S315_5313::GetHScanSettings(bool screenModeRS0Active, bool screenModeRS1Active)
{
	//Analog screen sections in relation to HCounter (H32 mode):
	//-----------------------------------------------------------------
	//| Screen section | HCounter  |Pixel| Pixel |Serial|Serial |MCLK |
	//| (PAL/NTSC H32) |  value    |clock| clock |clock |clock  |ticks|
	//|                |           |ticks|divider|ticks |divider|     |
	//|----------------|-----------|-----|-------|------|-------|-----|
	//|Left border     |0x00B-0x017|  13 |SCLK/2 |   26 |MCLK/5 | 130 |
	//|----------------|-----------|-----|-------|------|-------|-----|
	//|Active display  |0x018-0x117| 256 |SCLK/2 |  512 |MCLK/5 |2560 |
	//|----------------|-----------|-----|-------|------|-------|-----|
	//|Right border    |0x118-0x125|  14 |SCLK/2 |   28 |MCLK/5 | 140 |
	//|----------------|-----------|-----|-------|------|-------|-----|
	//|Back porch      |0x126-0x127|   9 |SCLK/2 |   18 |MCLK/5 |  90 |
	//|(Right Blanking)|0x1D2-0x1D8|     |       |      |       |     |
	//|----------------|-----------|-----|-------|------|-------|-----|
	//|Horizontal sync |0x1D9-0x1F2|  26 |SCLK/2 |   52 |MCLK/5 | 260 |
	//|----------------|-----------|-----|-------|------|-------|-----|
	//|Front porch     |0x1F3-0x00A|  24 |SCLK/2 |   48 |MCLK/5 | 240 |
	//|(Left Blanking) |           |     |       |      |       |     |
	//|----------------|-----------|-----|-------|------|-------|-----|
	//|TOTALS          |           | 342 |       |  684 |       |3420 |
	//-----------------------------------------------------------------

	//Analog screen sections in relation to HCounter (H40 mode):
	//--------------------------------------------------------------------
	//| Screen section |   HCounter    |Pixel| Pixel |EDCLK| EDCLK |MCLK |
	//| (PAL/NTSC H40) |    value      |clock| clock |ticks|divider|ticks|
	//|                |               |ticks|divider|     |       |     |
	//|----------------|---------------|-----|-------|-----|-------|-----|
	//|Left border     |0x00D-0x019    |  13 |EDCLK/2|  26 |MCLK/4 | 104 |
	//|----------------|---------------|-----|-------|-----|-------|-----|
	//|Active display  |0x01A-0x159    | 320 |EDCLK/2| 640 |MCLK/4 |2560 |
	//|----------------|---------------|-----|-------|-----|-------|-----|
	//|Right border    |0x15A-0x167    |  14 |EDCLK/2|  28 |MCLK/4 | 112 |
	//|----------------|---------------|-----|-------|-----|-------|-----|
	//|Back porch      |0x168-0x16C    |   9 |EDCLK/2|  18 |MCLK/4 |  72 |
	//|(Right Blanking)|0x1C9-0x1CC    |     |       |     |       |     |
	//|----------------|---------------|-----|-------|-----|-------|-----|
	//|Horizontal sync |0x1CD.0-0x1D4.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
	//|                |0x1D4.5-0x1D5.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
	//|                |0x1D5.5-0x1DC.0| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
	//|                |0x1DD.0        |   1 |EDCLK/2|   2 |MCLK/4 |   8 |
	//|                |0x1DE.0-0x1E5.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
	//|                |0x1E5.5-0x1E6.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
	//|                |0x1E6.5-0x1EC.0| 6.5 |EDCLK/2|  13 |MCLK/5 |  65 |
	//|                |===============|=====|=======|=====|=======|=====|
	//|        Subtotal|0x1CD-0x1EC    | (32)|       | (64)|       |(314)|
	//|----------------|---------------|-----|-------|-----|-------|-----|
	//|Front porch     |0x1ED          |   1 |EDCLK/2|   2 |MCLK/5 |  10 |
	//|(Left Blanking) |0x1EE-0x00C    |  31 |EDCLK/2|  62 |MCLK/4 | 248 |
	//|                |===============|=====|=======|=====|=======|=====|
	//|        Subtotal|0x1ED-0x00C    | (32)|       | (64)|       |(258)|
	//|----------------|---------------|-----|-------|-----|-------|-----|
	//|TOTALS          |               | 420 |       | 840 |       |3420 |
	//--------------------------------------------------------------------

	//Digital render events in relation to HCounter:
	//----------------------------------------------------
	//|        Video |PAL/NTSC         |PAL/NTSC         |
	//|         Mode |H32     (RSx=00) |H40     (RSx=11) |
	//|              |V28/V30 (M2=*)   |V28/V30 (M2=*)   |
	//| Event        |Int any (LSMx=**)|Int any (LSMx=**)|
	//|--------------------------------------------------|
	//|HCounter      |[1]0x000-0x127   |[1]0x000-0x16C   |
	//|progression   |[2]0x1D2-0x1FF   |[2]0x1C9-0x1FF   |
	//|9-bit internal|                 |                 |
	//|--------------------------------------------------|
	//|VCounter      |HCounter changes |HCounter changes | //##TODO## Confirm these values
	//|increment     |from 0x84 to 0x85|from 0xA4 to 0xA5|
	//|              |in [1].          |in [1].          |
	//|--------------------------------------------------|
	//|HBlank set    |HCounter changes |HCounter changes |
	//|              |from 0x125 to    |from 0x165 to    |
	//|              |0x126 in [1].    |0x166 in [1].    |
	//|--------------------------------------------------|
	//|HBlank cleared|HCounter changes |HCounter changes |
	//|              |from 0x009 to    |from 0x00A to    |
	//|              |0x00A in [1].    |0x00B in [1].    |
	//|--------------------------------------------------|
	//|F flag set    |HCounter changes |HCounter changes | //##TODO## Confirm these values
	//|              |from 0x00 to 0x01|from 0x00 to 0x01|
	//|              |at start of [1]  |at start of [1]  |
	//|--------------------------------------------------|
	//|ODD flag      |HCounter changes |HCounter changes | //##TODO## Confirm these values
	//|toggled       |from 0x00 to 0x01|from 0x00 to 0x01|
	//|              |at start of [1]  |at start of [1]  |
	//|--------------------------------------------------|
	//|HINT flagged  |HCounter changes |HCounter changes | //##TODO## Confirm these values
	//|via IPL lines |from 0x0EE to    |from 0x12E to    |
	//|              |0x0EF in [1].    |0x12F in [1].    |
	//|--------------------------------------------------|
	//|VINT flagged  |                 |                 | //##TODO## Measure these values
	//|via IPL lines |                 |                 |
	//|              |                 |                 |
	//|--------------------------------------------------|
	//|HSYNC asserted|HCounter changes |HCounter changes |
	//|              |from 0x1D8 to    |from 0x1CC to    |
	//|              |0x1D9 in [2].    |0x1CD in [2].    |
	//|--------------------------------------------------|
	//|HSYNC negated |HCounter changes |HCounter changes |
	//|              |from 0x1F2 to    |from 0x1EC to    |
	//|              |0x1F3 in [2].    |0x1ED in [2].    |
	//----------------------------------------------------
	//##TODO## Hcounter values shown in the table above as 8-bit values have only been
	//tested in hardware to 8 bits of accuracy. We've taken a guess as to the 9-bit
	//internal values to populate the settings structures below. Perform analysis on the
	//real hardware to determine the true 9-bit values.
	//##TODO## The values for HINT flagged are based on an educated guess. Confirm them
	//through hardware tests.
	static const HScanSettings h32ScanSettings(0x127, 0x1D2, 0x10A, 0x126, 0x00A, 0x001, 0x002, 0x1FF, 0x0EF, 0x000, 0x1D9, 0x1F3, 0x018, 0x117, 256, 0x00B, 0x017, 13, 0x118, 0x125, 14);
	static const HScanSettings h40ScanSettings(0x16C, 0x1C9, 0x14A, 0x166, 0x00B, 0x001, 0x002, 0x1FF, 0x12F, 0x000, 0x1CD, 0x1ED, 0x01A, 0x159, 320, 0x00D, 0x019, 13, 0x15A, 0x167, 14);
	return (screenModeRS1Active)? h40ScanSettings: h32ScanSettings;
}

//----------------------------------------------------------------------------------------
const S315_5313::VScanSettings& S315_5313::GetVScanSettings(bool screenModeV30Active, bool palModeActive, bool interlaceActive)
{
	//Analog screen sections in relation to VCounter:
	//-------------------------------------------------------------------------------------------
	//|           Video |NTSC             |NTSC             |PAL              |PAL              |
	//|            Mode |H32/H40(RSx00/11)|H32/H40(RSx00/11)|H32/H40(RSx00/11)|H32/H40(RSx00/11)|
	//|                 |V28     (M2=0)   |V30     (M2=1)   |V28     (M2=0)   |V30     (M2=1)   |
	//|                 |Int none(LSMx=*0)|Int none(LSMx=*0)|Int none(LSMx=*0)|Int none(LSMx=*0)|
	//|                 |------------------------------------------------------------------------
	//|                 | VCounter  |Line | VCounter  |Line | VCounter  |Line | VCounter  |Line |
	//| Screen section  |  value    |count|  value    |count|  value    |count|  value    |count|
	//|-----------------|-----------|-----|-----------|-----|-----------|-----|-----------|-----|
	//|Active display   |0x000-0x0DF| 224 |0x000-0x1FF| 240*|0x000-0x0DF| 224 |0x000-0x0EF| 240 |
	//|-----------------|-----------|-----|-----------|-----|-----------|-----|-----------|-----|
	//|Bottom border    |0x0E0-0x0E7|   8 |           |   0 |0x0E0-0x0FF|  32 |0x0F0-0x107|  24 |
	//|-----------------|-----------|-----|-----------|-----|-----------|-----|-----------|-----|
	//|Bottom blanking  |0x0E8-0x0EA|   3 |           |   0 |0x100-0x102|   3 |0x108-0x10A|   3 |
	//|-----------------|-----------|-----|-----------|-----|-----------|-----|-----------|-----|
	//|Vertical sync    |0x1E5-0x1E7|   3 |           |   0 |0x1CA-0x1CC|   3 |0x1D2-0x1D4|   3 |
	//|-----------------|-----------|-----|-----------|-----|-----------|-----|-----------|-----|
	//|Top blanking     |0x1E8-0x1F4|  13 |           |   0 |0x1CD-0x1D9|  13 |0x1D5-0x1E1|  13 |
	//|-----------------|-----------|-----|-----------|-----|-----------|-----|-----------|-----|
	//|Top border       |0x1F5-0x1FF|  11 |           |   0 |0x1DA-0x1FF|  38 |0x1E2-0x1FF|  30 |
	//|-----------------|-----------|-----|-----------|-----|-----------|-----|-----------|-----|
	//|TOTALS           |           | 262 |           | 240*|           | 313 |           | 313 |
	//-------------------------------------------------------------------------------------------
	//*When V30 mode and NTSC mode are both active, no border, blanking, or retrace
	//occurs. A 30-row display is setup and rendered, however, immediately following the
	//end of the 30th row, the 1st row starts again. In addition, the VCounter is never
	//reset, which usually happens at the beginning of vertical blanking. Instead, the
	//VCounter continuously counts from 0x000-0x1FF, then wraps around back to 0x000 and
	//begins again. Since there are only 240 lines output as part of the display, this
	//means the actual line being rendered is desynchronized from the VCounter. Digital
	//events such as vblank flags being set/cleared, VInt being triggered, the odd flag
	//being toggled, and so forth, still occur at the correct VCounter positions they
	//would occur in (IE, the same as PAL mode V30), however, since the VCounter has 512
	//lines per cycle, this means VInt is triggered at a slower rate than normal.
	//##TODO## Confirm on the hardware that the rendering row is desynchronized from the
	//VCounter. This would seem unlikely, since a separate render line counter would have
	//to be maintained apart from VCounter for this to occur.

	//Digital render events in relation to VCounter under NTSC mode:
	//#ODD - Runs only when the ODD flag is set
	//----------------------------------------------------------------------------------------
	//|        Video |NTSC             |NTSC             |NTSC             |NTSC             |
	//|         Mode |H32/H40(RSx00/11)|H32/H40(RSx00/11)|H32/H40(RSx00/11)|H32/H40(RSx00/11)|
	//|              |V28     (M2=0)   |V28     (M2=0)   |V30     (M2=1)   |V30     (M2=1)   |
	//| Event        |Int none(LSMx=*0)|Int both(LSMx=*1)|Int none(LSMx=*0)|Int both(LSMx=*1)|
	//|--------------------------------------------------------------------------------------|
	//|VCounter      |[1]0x000-0x0EA   |[1]0x000-0x0EA   |[1]0x000-0x1FF   |[1]0x000-0x1FF   |
	//|progression   |[2]0x1E5-0x1FF   |[2]0x1E4(#ODD)   |                 |                 |
	//|9-bit internal|                 |[3]0x1E5-0x1FF   |                 |                 |
	//|--------------------------------------------------------------------------------------|
	//|VBlank set    |Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x0DF to    |     <Same>      |from 0x0EF to    |     <Same>      |
	//|              |0x0E0 in [1].    |                 |0x0F0 in [1].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|VBlank cleared|Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x1FE to    |     <Same>      |from 0x1FE to    |     <Same>      |
	//|              |0x1FF in [2].    |                 |0x1FF in [1].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|F flag set    |while VCounter is|                 |while VCounter is|                 |
	//|              |set to 0x0E0 in  |     <Same>      |set to 0x0F0 in  |     <Same>      |
	//|              |[1].             |                 |[1].             |                 |
	//|--------------------------------------------------------------------------------------|
	//|VSYNC asserted|Vcounter changes |                 |      Never      |                 |
	//|              |from 0x0E7 to    |     <Same>      |                 |     <Same>      |
	//|              |0x0E8 in [1].    |                 |                 |                 |
	//|--------------------------------------------------------------------------------------|
	//|VSYNC cleared |Vcounter changes |                 |      Never      |                 |
	//|              |from 0x1F4 to    |     <Same>      |                 |     <Same>      |
	//|              |0x1F5 in [2].    |                 |                 |                 |
	//|--------------------------------------------------------------------------------------|
	//|ODD flag      |while VCounter is|                 |while VCounter is|                 |
	//|toggled       |set to 0x0E0 in  |     <Same>      |set to 0x0F0 in  |     <Same>      |
	//|              |[1].             |                 |[1].             |                 |
	//----------------------------------------------------------------------------------------

	//Digital render events in relation to VCounter under PAL mode:
	//#ODD - Runs only when the ODD flag is set
	//----------------------------------------------------------------------------------------
	//|        Video |PAL              |PAL              |PAL              |PAL              |
	//|         Mode |H32/H40(RSx00/11)|H32/H40(RSx00/11)|H32/H40(RSx00/11)|H32/H40(RSx00/11)|
	//|              |V28     (M2=0)   |V28     (M2=0)   |V30     (M2=1)   |V30     (M2=1)   |
	//| Event        |Int none(LSMx=*0)|Int both(LSMx=*1)|Int none(LSMx=*0)|Int both(LSMx=*1)|
	//|--------------------------------------------------------------------------------------|
	//|VCounter      |[1]0x000-0x102   |[1]0x000-0x101   |[1]0x000-0x10A   |[1]0x000-0x109   |
	//|progression   |[2]0x1CA-0x1FF   |[2]0x1C9(#ODD)   |[2]0x1D2-0x1FF   |[2]0x1D1(#ODD)   |
	//|9-bit internal|                 |[3]0x1CA-0x1FF   |                 |[3]0x1D2-0x1FF   |
	//|--------------------------------------------------------------------------------------|
	//|VBlank set    |Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x0DF to    |     <Same>      |from 0x0EF to    |     <Same>      |
	//|              |0x0E0 in [1].    |                 |0x0F0 in [1].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|VBlank cleared|Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x1FE to    |     <Same>      |from 0x1FE to    |     <Same>      |
	//|              |0x1FF in [2].    |                 |0x1FF in [2].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|F flag set    |while VCounter is|                 |while VCounter is|                 |
	//|              |set to 0x0E0 in  |     <Same>      |set to 0x0F0 in  |     <Same>      |
	//|              |[1].             |                 |[1].             |                 |
	//|--------------------------------------------------------------------------------------|
	//|VSYNC asserted|Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x0FF to    |     <Same>      |from 0x107 to    |     <Same>      |
	//|              |0x100 in [1].    |                 |0x108 in [1].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|VSYNC cleared |Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x1D9 to    |     <Same>      |from 0x1E1 to    |     <Same>      |
	//|              |0x1DA in [2].    |                 |0x1E2 in [2].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|ODD flag      |while VCounter is|                 |while VCounter is|                 |
	//|toggled       |set to 0x0E0 in  |     <Same>      |set to 0x0F0 in  |     <Same>      |
	//|              |[1].             |                 |[1].             |                 |
	//----------------------------------------------------------------------------------------

	//##TODO## Evaluate the way we're using the first parameter vcounterActiveScanMaxValue
	//in code. Note that we subtract 1 from this value for the interlace values in PAL
	//mode, but we use the same values for both interlace and non-interlace modes in NTSC.
	//Confirm if this is correct.
	static const VScanSettings v28PalNoIntScanSettings( 0x102, 0x1CA, 0x1C9, 0x0E0, 0x1FF, 0x1FF, 0x100, 0x1DA, 313, 0x000, 0x0DF, 224, 0x1DA, 0x1FF, 38, 0x0E0, 0x0FF, 32);
	static const VScanSettings v28PalIntEnScanSettings( 0x101, 0x1CA, 0x1C9, 0x0E0, 0x1FF, 0x1FF, 0x100, 0x1DA, 313, 0x000, 0x0DF, 224, 0x1DA, 0x1FF, 38, 0x0E0, 0x0FF, 32);
	static const VScanSettings v30PalNoIntScanSettings( 0x10A, 0x1D2, 0x1D1, 0x0F0, 0x1FF, 0x1FF, 0x108, 0x1E2, 313, 0x000, 0x0EF, 240, 0x1E2, 0x1FF, 30, 0x0F0, 0x107, 24);
	static const VScanSettings v30PalIntEnScanSettings( 0x109, 0x1D2, 0x1D1, 0x0F0, 0x1FF, 0x1FF, 0x108, 0x1E2, 313, 0x000, 0x0EF, 240, 0x1E2, 0x1FF, 30, 0x0F0, 0x107, 24);
	static const VScanSettings v28NtscNoIntScanSettings(0x0EA, 0x1E5, 0x1E4, 0x0E0, 0x1FF, 0x1FF, 0x0E8, 0x1F5, 262, 0x000, 0x0DF, 224, 0x1F5, 0x1FF, 11, 0x0E0, 0x0E7,  8);
	static const VScanSettings v28NtscIntEnScanSettings(0x0EA, 0x1E5, 0x1E4, 0x0E0, 0x1FF, 0x1FF, 0x0E8, 0x1F5, 262, 0x000, 0x0DF, 224, 0x1F5, 0x1FF, 11, 0x0E0, 0x0E7,  8);
	static const VScanSettings v30NtscNoIntScanSettings(0x1FF, 0x200, 0x200, 0x0F0, 0x1FF, 0x1FF, 0x1FF, 0x1FF, 262, 0x000, 0x0EF, 240, 0x200, 0x200,  0, 0x200, 0x200,  0);
	static const VScanSettings v30NtscIntEnScanSettings(0x1FF, 0x200, 0x200, 0x0F0, 0x1FF, 0x1FF, 0x1FF, 0x1FF, 262, 0x000, 0x0EF, 240, 0x200, 0x200,  0, 0x200, 0x200,  0);

	//Select the scan settings which correspond with the current screen mode
	if(palModeActive)
	{
		if(screenModeV30Active)
		{
			return (interlaceActive)? v30PalIntEnScanSettings: v30PalNoIntScanSettings;
		}
		else
		{
			return (interlaceActive)? v28PalIntEnScanSettings: v28PalNoIntScanSettings;
		}
	}
	else
	{
		if(screenModeV30Active)
		{
			return (interlaceActive)? v30NtscIntEnScanSettings: v30NtscNoIntScanSettings;
		}
		else
		{
			return (interlaceActive)? v28NtscIntEnScanSettings: v28NtscNoIntScanSettings;
		}
	}
}

//----------------------------------------------------------------------------------------
//HV counter comparison functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetPixelClockStepsBetweenHVCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterTarget, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterTarget)
{
	unsigned int totalPixelClockSteps = 0;

	//Check if the hcounter is going to pass the vcounter increment point when advancing
	//it from its current position to the target position
	if(((hcounterCurrent < hcounterTarget) && (hcounterCurrent < hscanSettings.vcounterIncrementPoint) && (hcounterTarget >= hscanSettings.vcounterIncrementPoint))
	|| ((hcounterCurrent > hcounterTarget) && ((hcounterCurrent < hscanSettings.vcounterIncrementPoint) || (hcounterTarget >= hscanSettings.vcounterIncrementPoint))))
	{
		//If the hcounter advancement is going to increment the vcounter, advance the
		//hcounter up to the vcounter increment point, and increment the vcounter.
		totalPixelClockSteps += GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, hscanSettings.vcounterIncrementPoint);
		hcounterCurrent = hscanSettings.vcounterIncrementPoint;
		vcounterCurrent = AddStepsToVCounter(hscanSettings, hcounterCurrent, vscanSettings, interlaceIsEnabled, oddFlagSet, vcounterCurrent, 1);
	}

	//Calculate the total number of steps required to advance the vcounter to its final
	//position
	if(vcounterCurrent != vcounterTarget)
	{
		//##FIX## This function first advances the hcounter to the vcounter increment point,
		//then leaves it there and advances the vcounter.
		totalPixelClockSteps += GetPixelClockStepsBetweenVCounterValues(hscanSettings, hcounterCurrent, vscanSettings, interlaceIsEnabled, oddFlagSet, vcounterCurrent, vcounterTarget);
		hcounterCurrent = hscanSettings.vcounterIncrementPoint;
	}

	//Calculate the total number of steps required to advance the hcounter to its final
	//position
	totalPixelClockSteps += GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, hcounterTarget);

	return totalPixelClockSteps;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetPixelClockStepsBetweenHCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterTarget)
{
	//Convert the internal hcounter values into linear values
	hcounterCurrent = HCounterValueFromVDPInternalToLinear(hscanSettings, hcounterCurrent);
	hcounterTarget = HCounterValueFromVDPInternalToLinear(hscanSettings, hcounterTarget);

	//Calculate the number of pixel clock steps required to advance the current hcounter
	//to the target value.
	return ((hcounterTarget + hscanSettings.hcounterStepsPerIteration) - hcounterCurrent) % hscanSettings.hcounterStepsPerIteration;
}

//----------------------------------------------------------------------------------------
//##TODO## What happens if the target vcounter is the extra line in an odd interlace frame?
unsigned int S315_5313::GetPixelClockStepsBetweenVCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterTarget)
{
	unsigned int totalPixelClockSteps = 0;
	if(vcounterCurrent != vcounterTarget)
	{
		bool hcounterAdvancedToIncrementPoint = false;
		if(hcounterCurrent != hscanSettings.vcounterIncrementPoint)
		{
			//Add the number of pixel clock steps required to advance the hcounter to the
			//point where the vcounter is incremented.
			totalPixelClockSteps += GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, hscanSettings.vcounterIncrementPoint);
			hcounterAdvancedToIncrementPoint = true;
		}

		//Convert the internal vcounter values into linear values
		vcounterCurrent = VCounterValueFromVDPInternalToLinear(vscanSettings, vcounterCurrent, oddFlagSet);
		vcounterTarget = VCounterValueFromVDPInternalToLinear(vscanSettings, vcounterTarget, oddFlagSet);

		//If the current horizontal and vertical counters haven't yet passed the point
		//where the odd flag is toggled, and the target vcounter value is passed that
		//point, update the value of the odd flag.
		if(((vcounterCurrent < vscanSettings.vblankSetPoint) || ((vcounterCurrent == vscanSettings.vblankSetPoint) && (hcounterCurrent < hscanSettings.oddFlagTogglePoint))) && (vcounterTarget > vscanSettings.vblankSetPoint))
		{
			oddFlagSet = interlaceIsEnabled && !oddFlagSet;
		}

		//Calculate the number of vcounter increment steps required to reach the target
		//vcounter value.
		unsigned int vcounterStepsPerIteration = oddFlagSet? vscanSettings.vcounterStepsPerIterationOddFlag: vscanSettings.vcounterStepsPerIteration;
		unsigned int totalVCounterStepsBetweenValues = ((vcounterTarget + vcounterStepsPerIteration) - vcounterCurrent) % vcounterStepsPerIteration;

		if(hcounterAdvancedToIncrementPoint)
		{
			//Subtract 1 from the total number of vcounter increment steps, since we know we
			//will already have advanced one vcounter step by advancing the hcounter to the
			//vcounter increment point, which we did above. Note that it is safe to subtract
			//here because we know the current and target vcounter values are different, since
			//we've already filtered for that above.
			totalVCounterStepsBetweenValues -= 1;
		}

		//Add the number of pixel clock steps required to advance the required number of
		//vcounter steps.
		totalPixelClockSteps += totalVCounterStepsBetweenValues * hscanSettings.hcounterStepsPerIteration;
	}
	return totalPixelClockSteps;
}

//----------------------------------------------------------------------------------------
//HV counter advancement functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::AddStepsToHCounter(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterStepsToAdd)
{
	//Convert the internal hcounter value into a linear value
	hcounterCurrent = HCounterValueFromVDPInternalToLinear(hscanSettings, hcounterCurrent);

	//Calculate the initial value for the target hcounter value
	unsigned int hcounterTarget = hcounterCurrent + hcounterStepsToAdd;

	//Wrap the hcounter value back around to the start if we've passed the total number of
	//steps per iteration.
	hcounterTarget %= hscanSettings.hcounterStepsPerIteration;

	//Convert the linear hcounter value back into an internal value
	hcounterTarget = HCounterValueFromLinearToVDPInternal(hscanSettings, hcounterTarget);

	//Return the incremented hcounter
	return hcounterTarget;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::AddStepsToVCounter(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterStepsToAdd)
{
	//Convert the internal vcounter value into a linear value
	vcounterCurrent = VCounterValueFromVDPInternalToLinear(vscanSettings, vcounterCurrent, oddFlagSet);

	//Calculate the initial value for the target vcounter value
	unsigned int vcounterTarget = vcounterCurrent + vcounterStepsToAdd;

	//If the current horizontal and vertical counters haven't yet passed the point
	//where the odd flag is toggled, and the target vcounter value is passed that
	//point, update the value of the odd flag.
	if(((vcounterCurrent < vscanSettings.vblankSetPoint) || ((vcounterCurrent == vscanSettings.vblankSetPoint) && (hcounterCurrent < hscanSettings.oddFlagTogglePoint))) && (vcounterTarget > vscanSettings.vblankSetPoint))
	{
		oddFlagSet = interlaceIsEnabled && !oddFlagSet;
	}

	//Wrap the vcounter value back around to the start if we've passed the total number of
	//steps per iteration.
	unsigned int vcounterStepsPerIteration = oddFlagSet? vscanSettings.vcounterStepsPerIterationOddFlag: vscanSettings.vcounterStepsPerIteration;
	vcounterTarget %= vcounterStepsPerIteration;

	//Convert the linear vcounter value back into an internal value
	vcounterTarget = VCounterValueFromLinearToVDPInternal(vscanSettings, vcounterTarget, oddFlagSet);

	//Return the incremented vcounter
	return vcounterTarget;
}

//----------------------------------------------------------------------------------------
void S315_5313::AdvanceHVCounters(const HScanSettings& hscanSettings, unsigned int& hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool& oddFlagSet, unsigned int& vcounterCurrent, unsigned int pixelClockSteps)
{
	//Convert the internal hcounter and vcounter values into linear values
	hcounterCurrent = HCounterValueFromVDPInternalToLinear(hscanSettings, hcounterCurrent);
	vcounterCurrent = VCounterValueFromVDPInternalToLinear(vscanSettings, vcounterCurrent, oddFlagSet);

	//Calculate the number of times the vcounter needs to be incremented in order to reach
	//the odd flag toggle point.
	unsigned int vcounterStepsPerIteration = oddFlagSet? vscanSettings.vcounterStepsPerIterationOddFlag: vscanSettings.vcounterStepsPerIteration;
	unsigned int vcounterIncrementStepsUntilOddFlagToggle;
	if((vcounterCurrent < vscanSettings.vblankSetPoint) || ((vcounterCurrent == vscanSettings.vblankSetPoint) && (hcounterCurrent < hscanSettings.oddFlagTogglePoint)))
	{
		vcounterIncrementStepsUntilOddFlagToggle = vscanSettings.vblankSetPoint - vcounterCurrent;
	}
	else
	{
		vcounterIncrementStepsUntilOddFlagToggle = (vcounterStepsPerIteration - vcounterCurrent) + vscanSettings.vblankSetPoint;
	}

	//Calculate the number of times the hcounter needs to be incremented in order to reach
	//the odd flag toggle point.
	unsigned int hcounterIncrementStepsUntilOddFlagToggle;
	if(hcounterCurrent < hscanSettings.oddFlagTogglePoint)
	{
		hcounterIncrementStepsUntilOddFlagToggle = hscanSettings.oddFlagTogglePoint - hcounterCurrent;
	}
	else
	{
		hcounterIncrementStepsUntilOddFlagToggle = (hscanSettings.hcounterStepsPerIteration - hcounterCurrent) + hscanSettings.oddFlagTogglePoint;
	}

	//Calculate the number of pixel clock steps until the odd flag needs to be toggled
	unsigned int pixelClockStepsUntilOddFlagToggle = (vcounterIncrementStepsUntilOddFlagToggle * hscanSettings.hcounterStepsPerIteration) + hcounterIncrementStepsUntilOddFlagToggle;

	//While there's enough cycles remaining to reach the odd flag toggle point, advance to
	//that point, and update the odd flag.
	while(pixelClockStepsUntilOddFlagToggle <= pixelClockSteps)
	{
		//Advance the hcounter and vcounter to the odd flag toggle point
		hcounterCurrent = (hcounterCurrent + hcounterIncrementStepsUntilOddFlagToggle) % hscanSettings.hcounterStepsPerIteration;
		vcounterCurrent = (vcounterCurrent + vcounterIncrementStepsUntilOddFlagToggle) % vcounterStepsPerIteration;

		//Update the odd flag, now that we've reached the toggle point.
		oddFlagSet = interlaceIsEnabled & !oddFlagSet;

		//Update the remaining pixel clock steps to advance
		pixelClockSteps -= pixelClockStepsUntilOddFlagToggle;

		//Recalculate the number of vcounter steps per iteration, to take into account the
		//updated odd flag setting.
		vcounterStepsPerIteration = oddFlagSet? vscanSettings.vcounterStepsPerIterationOddFlag: vscanSettings.vcounterStepsPerIteration;

		//Calculate the time to the next odd flag toggle point
		pixelClockStepsUntilOddFlagToggle = vcounterStepsPerIteration * hscanSettings.hcounterStepsPerIteration;
	}

	//Calculate the number of times the hcounter needs to be incremented in order to reach
	//the vcounter increment point.
	unsigned int hcounterIncrementStepsUntilVCounterIncrement;
	if(hcounterCurrent < hscanSettings.vcounterIncrementPoint)
	{
		hcounterIncrementStepsUntilVCounterIncrement = hscanSettings.vcounterIncrementPoint - hcounterCurrent;
	}
	else
	{
		hcounterIncrementStepsUntilVCounterIncrement = (hscanSettings.hcounterStepsPerIteration - hcounterCurrent) + hscanSettings.vcounterIncrementPoint;
	}

	//Calculate the number of times the hcounter and vcounter each need to be incremented
	//to reach their final positions.
	unsigned int hcounterIncrementSteps = pixelClockSteps;
	unsigned int vcounterIncrementSteps = 0;
	if(hcounterIncrementStepsUntilVCounterIncrement <= pixelClockSteps)
	{
		vcounterIncrementSteps = ((pixelClockSteps - hcounterIncrementStepsUntilVCounterIncrement) + hscanSettings.hcounterStepsPerIteration) / hscanSettings.hcounterStepsPerIteration;
	}

	//Advance hcounter and vcounter to their final positions
	unsigned int hcounterFinal = (hcounterCurrent + hcounterIncrementSteps) % hscanSettings.hcounterStepsPerIteration;
	unsigned int vcounterFinal = (vcounterCurrent + vcounterIncrementSteps) % vcounterStepsPerIteration;

	//Convert the final hcounter and vcounter values from linear values back to internal
	//values, and return them to the caller.
	hcounterCurrent = HCounterValueFromLinearToVDPInternal(hscanSettings, hcounterFinal);
	vcounterCurrent = VCounterValueFromLinearToVDPInternal(vscanSettings, vcounterFinal, oddFlagSet);
}

//----------------------------------------------------------------------------------------
//Pixel clock functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetUpdatedVSRAMReadCacheIndex(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int vsramReadCacheIndexCurrent, unsigned int hcounterInitial, unsigned int vcounterInitial, unsigned int hcounterFinal, unsigned int vcounterFinal, bool screenModeRS0Current, bool screenModeRS1Current, unsigned int vscrollMode)
{
	//If we're advancing through an active scan region, update the VSRAM read cache index
	//if it passes a point where a new value is latched.
	unsigned int vsramReadCacheIndexNew = vsramReadCacheIndexCurrent;

	//Calculate the HV counter position of the last VSRAM read to be performed by the
	//render process prior to the current HV counter position, and the index into VSRAM of
	//that read operation. Note that hardware tests have shown that the disable display
	//bit in register 1, as well as bit 0 in register 0, do not prevent the VDP from
	//reading from VSRAM during active scan. The VSRAM reads occur regardless of the state
	//of these register settings.
	//##TODO## Perform hardware tests to determine whether VSRAM data is read on the first
	//active line of the display at 0x1FF, where the sprite data is read for the upcoming
	//line, but no actual pattern data is output. We can perform this test by reading the
	//HV counter data along with reads from VSRAM, and checking where the VSRAM cache data
	//first starts being modified.
	//##TODO## Perform hardware tests using a logic analyzer to absolutely confirm the
	//timing of VSRAM reads.
	unsigned int vsramLastReadHCounter;
	unsigned int vsramLastReadVCounter;
	unsigned int vsramLastReadIndex;
	if(!screenModeRS1Current)
	{
		//VSRAM read operations (H32):
		//----------------------------------
		//|Read|Layer|Column|Index|HCounter|
		//|--------------------------------|
		//|  1 |  A  |   1  | 0x00|  0x003 |
		//|  2 |  B  |   1  | 0x02|  0x00B |
		//|  3 |  A  |   2  | 0x04|  0x013 |
		//|  4 |  B  |   2  | 0x06|  0x01B |
		//|  5 |  A  |   3  | 0x08|  0x023 |
		//|  6 |  B  |   3  | 0x0A|  0x02B |
		//|  7 |  A  |   4  | 0x0C|  0x033 |
		//|  8 |  B  |   4  | 0x0E|  0x03B |
		//|  9 |  A  |   5  | 0x10|  0x043 |
		//| 10 |  B  |   5  | 0x12|  0x04B |
		//| 11 |  A  |   6  | 0x14|  0x053 |
		//| 12 |  B  |   6  | 0x16|  0x05B |
		//| 13 |  A  |   7  | 0x18|  0x063 |
		//| 14 |  B  |   7  | 0x1A|  0x06B |
		//| 15 |  A  |   8  | 0x1C|  0x073 |
		//| 16 |  B  |   8  | 0x1E|  0x07B |
		//| 17 |  A  |   9  | 0x20|  0x083 |
		//| 18 |  B  |   9  | 0x22|  0x08B |
		//| 19 |  A  |  10  | 0x24|  0x093 |
		//| 20 |  B  |  10  | 0x26|  0x09B |
		//| 21 |  A  |  11  | 0x28|  0x0A3 |
		//| 22 |  B  |  11  | 0x2A|  0x0AB |
		//| 23 |  A  |  12  | 0x2C|  0x0B3 |
		//| 24 |  B  |  12  | 0x2E|  0x0BB |
		//| 25 |  A  |  13  | 0x30|  0x0C3 |
		//| 26 |  B  |  13  | 0x32|  0x0CB |
		//| 27 |  A  |  14  | 0x34|  0x0D3 |
		//| 28 |  B  |  14  | 0x36|  0x0DB |
		//| 29 |  A  |  15  | 0x38|  0x0E3 |
		//| 30 |  B  |  15  | 0x3A|  0x0EB |
		//| 31 |  A  |  16  | 0x3C|  0x0F3 |
		//| 32 |  B  |  16  | 0x3E|  0x0FB |
		//----------------------------------
		vsramLastReadVCounter = vcounterFinal;
		const unsigned int vsramFirstReadHCounterOnLine = 0x003;
		if((vcounterFinal > vscanSettings.activeDisplayVCounterLastValue) //VCounter is past the end of the active scan region
		|| (vcounterFinal < vscanSettings.activeDisplayVCounterFirstValue) //VCounter is before the start of the active scan region
		|| ((vcounterFinal == vscanSettings.activeDisplayVCounterFirstValue) && ((hcounterFinal < vsramFirstReadHCounterOnLine) || (hcounterFinal >= hscanSettings.vcounterIncrementPoint)))) //VCounter is on the same line as the start of the active scan region, but hasn't reached the first VSRAM read position yet
		{
			vsramLastReadVCounter = vscanSettings.activeDisplayVCounterLastValue;
			vsramLastReadHCounter = 0x0FB;
			vsramLastReadIndex = 0x3E;
		}
		else if(hcounterFinal < 0x003) {vsramLastReadHCounter = 0x0FB; vsramLastReadIndex = 0x3E; vsramLastReadVCounter = vcounterFinal-1;}
		else if(hcounterFinal < 0x00B) {vsramLastReadHCounter = 0x003; vsramLastReadIndex = 0x00;}
		else if(hcounterFinal < 0x013) {vsramLastReadHCounter = 0x00B; vsramLastReadIndex = 0x02;}
		else if(hcounterFinal < 0x01B) {vsramLastReadHCounter = 0x013; vsramLastReadIndex = 0x04;}
		else if(hcounterFinal < 0x023) {vsramLastReadHCounter = 0x01B; vsramLastReadIndex = 0x06;}
		else if(hcounterFinal < 0x02B) {vsramLastReadHCounter = 0x023; vsramLastReadIndex = 0x08;}
		else if(hcounterFinal < 0x033) {vsramLastReadHCounter = 0x02B; vsramLastReadIndex = 0x0A;}
		else if(hcounterFinal < 0x03B) {vsramLastReadHCounter = 0x033; vsramLastReadIndex = 0x0C;}
		else if(hcounterFinal < 0x043) {vsramLastReadHCounter = 0x03B; vsramLastReadIndex = 0x0E;}
		else if(hcounterFinal < 0x04B) {vsramLastReadHCounter = 0x043; vsramLastReadIndex = 0x10;}
		else if(hcounterFinal < 0x053) {vsramLastReadHCounter = 0x04B; vsramLastReadIndex = 0x12;}
		else if(hcounterFinal < 0x05B) {vsramLastReadHCounter = 0x053; vsramLastReadIndex = 0x14;}
		else if(hcounterFinal < 0x063) {vsramLastReadHCounter = 0x05B; vsramLastReadIndex = 0x16;}
		else if(hcounterFinal < 0x06B) {vsramLastReadHCounter = 0x063; vsramLastReadIndex = 0x18;}
		else if(hcounterFinal < 0x073) {vsramLastReadHCounter = 0x06B; vsramLastReadIndex = 0x1A;}
		else if(hcounterFinal < 0x07B) {vsramLastReadHCounter = 0x073; vsramLastReadIndex = 0x1C;}
		else if(hcounterFinal < 0x083) {vsramLastReadHCounter = 0x07B; vsramLastReadIndex = 0x1E;}
		else if(hcounterFinal < 0x08B) {vsramLastReadHCounter = 0x083; vsramLastReadIndex = 0x20;}
		else if(hcounterFinal < 0x093) {vsramLastReadHCounter = 0x08B; vsramLastReadIndex = 0x22;}
		else if(hcounterFinal < 0x09B) {vsramLastReadHCounter = 0x093; vsramLastReadIndex = 0x24;}
		else if(hcounterFinal < 0x0A3) {vsramLastReadHCounter = 0x09B; vsramLastReadIndex = 0x26;}
		else if(hcounterFinal < 0x0AB) {vsramLastReadHCounter = 0x0A3; vsramLastReadIndex = 0x28;}
		else if(hcounterFinal < 0x0B3) {vsramLastReadHCounter = 0x0AB; vsramLastReadIndex = 0x2A;}
		else if(hcounterFinal < 0x0BB) {vsramLastReadHCounter = 0x0B3; vsramLastReadIndex = 0x2C;}
		else if(hcounterFinal < 0x0C3) {vsramLastReadHCounter = 0x0BB; vsramLastReadIndex = 0x2E;}
		else if(hcounterFinal < 0x0CB) {vsramLastReadHCounter = 0x0C3; vsramLastReadIndex = 0x30;}
		else if(hcounterFinal < 0x0D3) {vsramLastReadHCounter = 0x0CB; vsramLastReadIndex = 0x32;}
		else if(hcounterFinal < 0x0DB) {vsramLastReadHCounter = 0x0D3; vsramLastReadIndex = 0x34;}
		else if(hcounterFinal < 0x0E3) {vsramLastReadHCounter = 0x0DB; vsramLastReadIndex = 0x36;}
		else if(hcounterFinal < 0x0EB) {vsramLastReadHCounter = 0x0E3; vsramLastReadIndex = 0x38;}
		else if(hcounterFinal < 0x0F3) {vsramLastReadHCounter = 0x0EB; vsramLastReadIndex = 0x3A;}
		else if(hcounterFinal < 0x0FB) {vsramLastReadHCounter = 0x0F3; vsramLastReadIndex = 0x3C;}
		else
		{
			vsramLastReadHCounter = 0x0FB;
			vsramLastReadIndex = 0x3E;
			//If the hcounter has passed the vcounter increment point, the real vcounter
			//value to use is one less than the current value.
			if(hcounterFinal >= hscanSettings.vcounterIncrementPoint)
			{
				vsramLastReadVCounter = vcounterFinal-1;
			}
		}
	}
	else
	{
		//VSRAM read operations (H40):
		//----------------------------------
		//|Read|Layer|Column|Index|HCounter|
		//|--------------------------------|
		//|  1 |  A  |   1  | 0x00|  0x000 |
		//|  2 |  B  |   1  | 0x02|  0x008 |
		//|  3 |  A  |   2  | 0x04|  0x010 |
		//|  4 |  B  |   2  | 0x06|  0x018 |
		//|  5 |  A  |   3  | 0x08|  0x020 |
		//|  6 |  B  |   3  | 0x0A|  0x028 |
		//|  7 |  A  |   4  | 0x0C|  0x030 |
		//|  8 |  B  |   4  | 0x0E|  0x038 |
		//|  9 |  A  |   5  | 0x10|  0x040 |
		//| 10 |  B  |   5  | 0x12|  0x048 |
		//| 11 |  A  |   6  | 0x14|  0x050 |
		//| 12 |  B  |   6  | 0x16|  0x058 |
		//| 13 |  A  |   7  | 0x18|  0x060 |
		//| 14 |  B  |   7  | 0x1A|  0x068 |
		//| 15 |  A  |   8  | 0x1C|  0x070 |
		//| 16 |  B  |   8  | 0x1E|  0x078 |
		//| 17 |  A  |   9  | 0x20|  0x080 |
		//| 18 |  B  |   9  | 0x22|  0x088 |
		//| 19 |  A  |  10  | 0x24|  0x090 |
		//| 20 |  B  |  10  | 0x26|  0x098 |
		//| 21 |  A  |  11  | 0x28|  0x0A0 |
		//| 22 |  B  |  11  | 0x2A|  0x0A8 |
		//| 23 |  A  |  12  | 0x2C|  0x0B0 |
		//| 24 |  B  |  12  | 0x2E|  0x0B8 |
		//| 25 |  A  |  13  | 0x30|  0x0C0 |
		//| 26 |  B  |  13  | 0x32|  0x0C8 |
		//| 27 |  A  |  14  | 0x34|  0x0D0 |
		//| 28 |  B  |  14  | 0x36|  0x0D8 |
		//| 29 |  A  |  15  | 0x38|  0x0E0 |
		//| 30 |  B  |  15  | 0x3A|  0x0E8 |
		//| 31 |  A  |  16  | 0x3C|  0x0F0 |
		//| 32 |  B  |  16  | 0x3E|  0x0F8 |
		//| 33 |  A  |  17  | 0x40|  0x100 |
		//| 34 |  B  |  17  | 0x42|  0x108 |
		//| 35 |  A  |  18  | 0x44|  0x110 |
		//| 36 |  B  |  18  | 0x46|  0x118 |
		//| 37 |  A  |  19  | 0x48|  0x120 |
		//| 38 |  B  |  19  | 0x4A|  0x128 |
		//| 39 |  A  |  20  | 0x4C|  0x130 |
		//| 40 |  B  |  20  | 0x4E|  0x138 |
		//----------------------------------
		vsramLastReadVCounter = vcounterFinal;
		if((vcounterFinal > vscanSettings.activeDisplayVCounterLastValue) //VCounter is past the end of the active scan region
		|| (vcounterFinal < vscanSettings.activeDisplayVCounterFirstValue) //VCounter is before the start of the active scan region
		|| ((vcounterFinal == vscanSettings.activeDisplayVCounterFirstValue) && (hcounterFinal >= hscanSettings.vcounterIncrementPoint))) //VCounter is on the same line as the start of the active scan region, but hasn't reached the first VSRAM read position yet
		{
			vsramLastReadVCounter = vscanSettings.activeDisplayVCounterLastValue;
			vsramLastReadHCounter = 0x138;
			vsramLastReadIndex = 0x4E;
		}
		else if(hcounterFinal < 0x008) {vsramLastReadHCounter = 0x000; vsramLastReadIndex = 0x00;}
		else if(hcounterFinal < 0x010) {vsramLastReadHCounter = 0x008; vsramLastReadIndex = 0x02;}
		else if(hcounterFinal < 0x018) {vsramLastReadHCounter = 0x010; vsramLastReadIndex = 0x04;}
		else if(hcounterFinal < 0x020) {vsramLastReadHCounter = 0x018; vsramLastReadIndex = 0x06;}
		else if(hcounterFinal < 0x028) {vsramLastReadHCounter = 0x020; vsramLastReadIndex = 0x08;}
		else if(hcounterFinal < 0x030) {vsramLastReadHCounter = 0x028; vsramLastReadIndex = 0x0A;}
		else if(hcounterFinal < 0x038) {vsramLastReadHCounter = 0x030; vsramLastReadIndex = 0x0C;}
		else if(hcounterFinal < 0x040) {vsramLastReadHCounter = 0x038; vsramLastReadIndex = 0x0E;}
		else if(hcounterFinal < 0x048) {vsramLastReadHCounter = 0x040; vsramLastReadIndex = 0x10;}
		else if(hcounterFinal < 0x050) {vsramLastReadHCounter = 0x048; vsramLastReadIndex = 0x12;}
		else if(hcounterFinal < 0x058) {vsramLastReadHCounter = 0x050; vsramLastReadIndex = 0x14;}
		else if(hcounterFinal < 0x060) {vsramLastReadHCounter = 0x058; vsramLastReadIndex = 0x16;}
		else if(hcounterFinal < 0x068) {vsramLastReadHCounter = 0x060; vsramLastReadIndex = 0x18;}
		else if(hcounterFinal < 0x070) {vsramLastReadHCounter = 0x068; vsramLastReadIndex = 0x1A;}
		else if(hcounterFinal < 0x078) {vsramLastReadHCounter = 0x070; vsramLastReadIndex = 0x1C;}
		else if(hcounterFinal < 0x080) {vsramLastReadHCounter = 0x078; vsramLastReadIndex = 0x1E;}
		else if(hcounterFinal < 0x088) {vsramLastReadHCounter = 0x080; vsramLastReadIndex = 0x20;}
		else if(hcounterFinal < 0x090) {vsramLastReadHCounter = 0x088; vsramLastReadIndex = 0x22;}
		else if(hcounterFinal < 0x098) {vsramLastReadHCounter = 0x090; vsramLastReadIndex = 0x24;}
		else if(hcounterFinal < 0x0A0) {vsramLastReadHCounter = 0x098; vsramLastReadIndex = 0x26;}
		else if(hcounterFinal < 0x0A8) {vsramLastReadHCounter = 0x0A0; vsramLastReadIndex = 0x28;}
		else if(hcounterFinal < 0x0B0) {vsramLastReadHCounter = 0x0A8; vsramLastReadIndex = 0x2A;}
		else if(hcounterFinal < 0x0B8) {vsramLastReadHCounter = 0x0B0; vsramLastReadIndex = 0x2C;}
		else if(hcounterFinal < 0x0C0) {vsramLastReadHCounter = 0x0B8; vsramLastReadIndex = 0x2E;}
		else if(hcounterFinal < 0x0C8) {vsramLastReadHCounter = 0x0C0; vsramLastReadIndex = 0x30;}
		else if(hcounterFinal < 0x0D0) {vsramLastReadHCounter = 0x0C8; vsramLastReadIndex = 0x32;}
		else if(hcounterFinal < 0x0D8) {vsramLastReadHCounter = 0x0D0; vsramLastReadIndex = 0x34;}
		else if(hcounterFinal < 0x0E0) {vsramLastReadHCounter = 0x0D8; vsramLastReadIndex = 0x36;}
		else if(hcounterFinal < 0x0E8) {vsramLastReadHCounter = 0x0E0; vsramLastReadIndex = 0x38;}
		else if(hcounterFinal < 0x0F0) {vsramLastReadHCounter = 0x0E8; vsramLastReadIndex = 0x3A;}
		else if(hcounterFinal < 0x0F8) {vsramLastReadHCounter = 0x0F0; vsramLastReadIndex = 0x3C;}
		else if(hcounterFinal < 0x100) {vsramLastReadHCounter = 0x0F8; vsramLastReadIndex = 0x3E;}
		else if(hcounterFinal < 0x108) {vsramLastReadHCounter = 0x100; vsramLastReadIndex = 0x40;}
		else if(hcounterFinal < 0x110) {vsramLastReadHCounter = 0x108; vsramLastReadIndex = 0x42;}
		else if(hcounterFinal < 0x118) {vsramLastReadHCounter = 0x110; vsramLastReadIndex = 0x44;}
		else if(hcounterFinal < 0x120) {vsramLastReadHCounter = 0x118; vsramLastReadIndex = 0x46;}
		else if(hcounterFinal < 0x128) {vsramLastReadHCounter = 0x120; vsramLastReadIndex = 0x48;}
		else if(hcounterFinal < 0x130) {vsramLastReadHCounter = 0x128; vsramLastReadIndex = 0x4A;}
		else if(hcounterFinal < 0x138) {vsramLastReadHCounter = 0x130; vsramLastReadIndex = 0x4C;}
		else
		{
			vsramLastReadHCounter = 0x138;
			vsramLastReadIndex = 0x4E;
			//If the hcounter has passed the vcounter increment point, the real vcounter
			//value to use is one less than the current value.
			if(hcounterFinal >= hscanSettings.vcounterIncrementPoint)
			{
				vsramLastReadVCounter = vcounterFinal-1;
			}
		}
	}

	//Take into account the current vscroll mode, and adjust the last read index
	//accordingly. If the vscroll mode is set to overall scrolling rather than column
	//based scrolling, all columns use the scroll value for the first column. We mask the
	//read index to apply that effect here.
	if(!vscrollMode)
	{
		vsramLastReadIndex &= 0x03;
	}

	//If we advanced past the last VSRAM read slot in the last advance operation, latch
	//the last VSRAM read cache index as the new VSRAM read cache index.
	if(EventOccursWithinCounterRange(hscanSettings, hcounterInitial, vcounterInitial, hcounterFinal, vcounterFinal, vsramLastReadHCounter, vsramLastReadVCounter))
	{
		vsramReadCacheIndexNew = vsramLastReadIndex;
	}

	return vsramReadCacheIndexNew;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetPixelClockTicksUntilNextAccessSlot(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int hcounterCurrent, bool screenModeRS0Current, bool screenModeRS1Current, bool displayEnabled, unsigned int vcounterCurrent)
{
	//Check if the current line, and the line following, are within the active display
	//region. Active display lines only allow external VRAM access at specific access
	//slots, while non-active display lines allow free access to VRAM, except where memory
	//refresh cycles occur.
	//##TODO## We need to check all settings which blank or disable the display here, as
	//they affect access slot timing.
	//##TODO## Confirm that the active display region is defined by vblank, which is
	//actually first asserted at vcounter=0x1FF, meaning there is one additional line at
	//the start of the display. This extra line is probably where sprite mapping and
	//pattern data is latched for the first line of the display. Test this on hardware.
	//##NOTE## The Sega logo for "Mickey Mania" clearly shows us that sprite data is
	//definitely not latched on the last line of the display and then used for the first,
	//since the sprite data is modified during vblank, and the modified data is used for
	//the first row of sprite data on the display.
	//##FIX## Define the active display region here to include the extra line at 0x1FF.
	bool currentLineActiveDisplayRegion = (vcounterCurrent >= vscanSettings.activeDisplayVCounterFirstValue) && (vcounterCurrent <= vscanSettings.activeDisplayVCounterLastValue);
	bool nextLineActiveDisplayRegion = (vcounterCurrent+1 >= vscanSettings.activeDisplayVCounterFirstValue) && (vcounterCurrent+1 <= vscanSettings.activeDisplayVCounterLastValue);

	//Determine when the next access slot will occur, based on the hcounter position and
	//the current screen mode settings. Note that we can safely assume the horizontal
	//screen mode doesn't change when making these calculations.
	//##TODO## Consider using table lookups here to eliminate branching.
	unsigned int nextAccessSlotHCounter = 0;
	if(!currentLineActiveDisplayRegion || !displayEnabled)
	{
		if(!screenModeRS1Current)
		{
			//Refresh cycle slots (H32):
			//---------------
			//|Slot|HCounter|
			//|-------------|
			//|  1 |  0x035 |
			//|  2 |  0x075 |
			//|  3 |  0x0B5 |
			//|  4 |  0x0F5 |
			//---------------
			if     (hcounterCurrent == 0x034) {nextAccessSlotHCounter = 0x036;}
			else if(hcounterCurrent == 0x074) {nextAccessSlotHCounter = 0x076;}
			else if(hcounterCurrent == 0x0B4) {nextAccessSlotHCounter = 0x0B6;}
			else if(hcounterCurrent == 0x0F4) {nextAccessSlotHCounter = 0x0F6;}
			else if(nextLineActiveDisplayRegion && ((hcounterCurrent + 1) == hscanSettings.vcounterIncrementPoint))
			{
				//If we're at the end of a non-display line, and the vcounter is about to
				//be incremented on the next pixel clock cycle and bring us into an active
				//display line, set the next access slot based on the first allowed access
				//slot after vcounter increment in an active display line for this screen
				//mode.
				nextAccessSlotHCounter = 0x121;
			}
			else
			{
				//If none of the above cases apply, our next access slot is on the next
				//hcounter value.
				nextAccessSlotHCounter = AddStepsToHCounter(hscanSettings, hcounterCurrent, 1);
			}
		}
		else
		{
			//Refresh cycle slots (H40):
			//---------------
			//|Slot|HCounter|
			//|-------------|
			//|  1 |  0x032 |
			//|  2 |  0x072 |
			//|  3 |  0x0B2 |
			//|  4 |  0x0F2 |
			//|  5 |  0x132 |
			//---------------
			if     (hcounterCurrent == 0x031) {nextAccessSlotHCounter = 0x033;}
			else if(hcounterCurrent == 0x071) {nextAccessSlotHCounter = 0x073;}
			else if(hcounterCurrent == 0x0B1) {nextAccessSlotHCounter = 0x0B3;}
			else if(hcounterCurrent == 0x0F1) {nextAccessSlotHCounter = 0x0F3;}
			else if(hcounterCurrent == 0x131) {nextAccessSlotHCounter = 0x133;}
			else if(nextLineActiveDisplayRegion && ((hcounterCurrent + 1) == hscanSettings.vcounterIncrementPoint))
			{
				//If we're at the end of a non-display line, and the vcounter is about to
				//be incremented on the next pixel clock cycle and bring us into an active
				//display line, set the next access slot based on the first allowed access
				//slot after vcounter increment in an active display line for this screen
				//mode.
				nextAccessSlotHCounter = 0x1CE;
			}
			else
			{
				//If none of the above cases apply, our next access slot is on the next
				//hcounter value.
				nextAccessSlotHCounter = AddStepsToHCounter(hscanSettings, hcounterCurrent, 1);
			}
		}
	}
	else
	{
		if(!screenModeRS1Current)
		{
			//##TODO## Confirm the point at which vcounter is incremented
			//External access slots (H32):
			//---------------
			//|Slot|HCounter|
			//|-------------|
			//|  1 |  0x005 |
			//|  2 |  0x015 |
			//|  3 |  0x025 |
			//|  4 |  0x045 |
			//|  5 |  0x055 |
			//|  6 |  0x065 |
			//|  7 |  0x085 |
			//|  8 |  0x095 |
			//|  9 |  0x0A5 |
			//| 10 |  0x0C5 |
			//| 11 |  0x0D5 |
			//| 12 |  0x0E5 |
			//| 13 |  0x103 |
			//| 14 |  0x105 | //Vcounter increments at either 0x10A or 0x10B
			//| 15 |  0x121 | //Next line
			//| 16 |  0x1E7 | //Next line
			//---------------
			if     (hcounterCurrent < 0x005) {nextAccessSlotHCounter = 0x005;}
			else if(hcounterCurrent < 0x015) {nextAccessSlotHCounter = 0x015;}
			else if(hcounterCurrent < 0x025) {nextAccessSlotHCounter = 0x025;}
			else if(hcounterCurrent < 0x045) {nextAccessSlotHCounter = 0x045;}
			else if(hcounterCurrent < 0x055) {nextAccessSlotHCounter = 0x055;}
			else if(hcounterCurrent < 0x065) {nextAccessSlotHCounter = 0x065;}
			else if(hcounterCurrent < 0x085) {nextAccessSlotHCounter = 0x085;}
			else if(hcounterCurrent < 0x095) {nextAccessSlotHCounter = 0x095;}
			else if(hcounterCurrent < 0x0A5) {nextAccessSlotHCounter = 0x0A5;}
			else if(hcounterCurrent < 0x0C5) {nextAccessSlotHCounter = 0x0C5;}
			else if(hcounterCurrent < 0x0D5) {nextAccessSlotHCounter = 0x0D5;}
			else if(hcounterCurrent < 0x0E5) {nextAccessSlotHCounter = 0x0E5;}
			else if(hcounterCurrent < 0x103) {nextAccessSlotHCounter = 0x103;}
			else if(hcounterCurrent < 0x105) {nextAccessSlotHCounter = 0x105;}
			else if(!nextLineActiveDisplayRegion && (hcounterCurrent < hscanSettings.vcounterIncrementPoint))
			{
				//If we're going to pass from an active line to a non-active line on the
				//next vcounter increment, and there are no remaining access slots on the
				//current line before the vcounter increment, we set the next access slot
				//to occur on the vcounter increment point.
				nextAccessSlotHCounter = hscanSettings.vcounterIncrementPoint;
			}
			else if(hcounterCurrent < 0x121) {nextAccessSlotHCounter = 0x121;}
			else if(hcounterCurrent < 0x1E7) {nextAccessSlotHCounter = 0x1E7;}
			else                             {nextAccessSlotHCounter = 0x005;}
		}
		else
		{
			//##TODO## Confirm the point at which vcounter is incremented
			//External access slots (H40):
			//---------------
			//|Slot|HCounter|
			//|-------------|
			//|  1 |  0x002 |
			//|  2 |  0x012 |
			//|  3 |  0x022 |
			//|  4 |  0x042 |
			//|  5 |  0x052 |
			//|  6 |  0x062 |
			//|  7 |  0x082 |
			//|  8 |  0x092 |
			//|  9 |  0x0A2 |
			//| 10 |  0x0C2 |
			//| 11 |  0x0D2 |
			//| 12 |  0x0E2 |
			//| 13 |  0x102 |
			//| 14 |  0x112 |
			//| 15 |  0x122 |
			//| 16 |  0x140 |
			//| 17 |  0x142 | //Vcounter increments at either 0x14A or 0x14B
			//| 18 |  0x1CE | //Next line
			//---------------
			if     (hcounterCurrent < 0x002) {nextAccessSlotHCounter = 0x002;}
			else if(hcounterCurrent < 0x012) {nextAccessSlotHCounter = 0x012;}
			else if(hcounterCurrent < 0x022) {nextAccessSlotHCounter = 0x022;}
			else if(hcounterCurrent < 0x042) {nextAccessSlotHCounter = 0x042;}
			else if(hcounterCurrent < 0x052) {nextAccessSlotHCounter = 0x052;}
			else if(hcounterCurrent < 0x062) {nextAccessSlotHCounter = 0x062;}
			else if(hcounterCurrent < 0x082) {nextAccessSlotHCounter = 0x082;}
			else if(hcounterCurrent < 0x092) {nextAccessSlotHCounter = 0x092;}
			else if(hcounterCurrent < 0x0A2) {nextAccessSlotHCounter = 0x0A2;}
			else if(hcounterCurrent < 0x0C2) {nextAccessSlotHCounter = 0x0C2;}
			else if(hcounterCurrent < 0x0D2) {nextAccessSlotHCounter = 0x0D2;}
			else if(hcounterCurrent < 0x0E2) {nextAccessSlotHCounter = 0x0E2;}
			else if(hcounterCurrent < 0x102) {nextAccessSlotHCounter = 0x102;}
			else if(hcounterCurrent < 0x112) {nextAccessSlotHCounter = 0x112;}
			else if(hcounterCurrent < 0x122) {nextAccessSlotHCounter = 0x122;}
			else if(hcounterCurrent < 0x140) {nextAccessSlotHCounter = 0x140;}
			else if(hcounterCurrent < 0x142) {nextAccessSlotHCounter = 0x142;}
			else if(!nextLineActiveDisplayRegion && (hcounterCurrent < hscanSettings.vcounterIncrementPoint))
			{
				//If we're going to pass from an active line to a non-active line on the
				//next vcounter increment, and there are no remaining access slots on the
				//current line before the vcounter increment, we set the next access slot
				//to occur on the vcounter increment point.
				nextAccessSlotHCounter = hscanSettings.vcounterIncrementPoint;
			}
			else if(hcounterCurrent < 0x1CE) {nextAccessSlotHCounter = 0x1CE;}
			else                             {nextAccessSlotHCounter = 0x002;}
		}
	}

	//Calculate the number of pixel clock steps which are required to reach the next
	//target hcounter value
	unsigned int pixelClockTicksToAccessSlot = GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, nextAccessSlotHCounter);

	//Return the number of pixel clock steps until the next available access slot
	return pixelClockTicksToAccessSlot;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetPixelClockTicksForMclkTicks(const HScanSettings& hscanSettings, unsigned int mclkTicks, unsigned int hcounterCurrent, bool screenModeRS0Active, bool screenModeRS1Active, unsigned int& mclkTicksUnused)
{
	//Calculate the total number of pixel clock ticks which will execute after the given
	//number of mclk ticks, based on the current screen mode.
	const unsigned int pixelClockDivider = 2;
	unsigned int pixelClockTicks;
	if(!screenModeRS0Active)
	{
		//If the RS0 bit is clear, the serial clock is generated internally, using a
		//divider from the MCLK input. If the RS1 bit is clear, selecting a H32 screen
		//mode, the serial clock is calculated as MCLK/5. If the RS1 bit is set, selecting
		//a H40 screen mode, the serial clock is calculated as MCLK/4.
		unsigned int serialClockDivider;
		if(!screenModeRS1Active)
		{
			//In H32 mode, if the RS0 bit is not set, the serial clock is calculated as
			//MCLK/5. The relation of hcounter to the various clock signals, and the
			//alignment of hcounter with the analog video output, is shown below. This
			//information has been determined through extensive and precise hardware
			//tests.
			//-----------------------------------------------------------------
			//| Screen section | HCounter  |Pixel| Pixel |Serial|Serial |MCLK |
			//| (PAL/NTSC H32) |  value    |clock| clock |clock |clock  |ticks|
			//|                |           |ticks|divider|ticks |divider|     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Left border     |0x00B-0x017|  13 |SCLK/2 |   26 |MCLK/5 | 130 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Active display  |0x018-0x117| 256 |SCLK/2 |  512 |MCLK/5 |2560 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Right border    |0x118-0x125|  14 |SCLK/2 |   28 |MCLK/5 | 140 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Back porch      |0x126-0x127|   9 |SCLK/2 |   18 |MCLK/5 |  90 |
			//|(Right Blanking)|0x1D2-0x1D8|     |       |      |       |     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Horizontal sync |0x1D9-0x1F2|  26 |SCLK/2 |   52 |MCLK/5 | 260 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Front porch     |0x1F3-0x00A|  24 |SCLK/2 |   48 |MCLK/5 | 240 |
			//|(Left Blanking) |           |     |       |      |       |     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|TOTALS          |           | 342 |       |  684 |       |3420 |
			//-----------------------------------------------------------------
			serialClockDivider = 5;
		}
		else
		{
			//In H40 mode, if the RS0 bit is not set, the serial clock is calculated as
			//MCLK/4. The relation of hcounter to the various clock signals, and the
			//alignment of hcounter with the analog video output, is shown below. This
			//information has been determined through extensive and precise hardware
			//tests. Note that on the Sega Mega Drive, this video mode does not produce a
			//valid video signal for PAL or NTSC screens. The horizontal sync period is
			//too short, and the line is too fast overall. The external EDCLK input needs
			//to be enabled by setting the RS0 bit, which supplies a modified pixel clock
			//input that slows down the horizontal sync period, to keep the number of MCLK
			//ticks required per line to 3420.
			//-----------------------------------------------------------------
			//| Screen section | HCounter  |Pixel| Pixel |Serial|Serial |MCLK |
			//| (PAL/NTSC H40) |  value    |clock| clock |clock |clock  |ticks|
			//|                |           |ticks|divider|ticks |divider|     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Left border     |0x00D-0x019|  13 |EDCLK/2|   26 |MCLK/4 | 104 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Active display  |0x01A-0x159| 320 |EDCLK/2|  640 |MCLK/4 |2560 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Right border    |0x15A-0x167|  14 |EDCLK/2|   28 |MCLK/4 | 112 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Back porch      |0x168-0x16C|   9 |EDCLK/2|   18 |MCLK/4 |  72 |
			//|(Right Blanking)|0x1C9-0x1CC|     |       |      |       |     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Horizontal sync |0x1CD-0x1EC|  32 |EDCLK/2|   64 |MCLK/4 | 256 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Front porch     |0x1ED-0x00C|  32 |EDCLK/2|   64 |MCLK/4 | 256 |
			//|(Left Blanking) |           |     |       |      |       |     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|TOTALS          |           | 420 |       |  840 |       |3360 |
			//-----------------------------------------------------------------
			serialClockDivider = 4;
		}
		pixelClockTicks = mclkTicks / (serialClockDivider * pixelClockDivider);
		mclkTicksUnused = mclkTicks % (serialClockDivider * pixelClockDivider);
	}
	else
	{
		//If the RS0 bit is set, the serial clock is provided through the external EDCLK
		//input. We emulate the external EDCLK input present on the Mega Drive system here
		//as if it was an internally generated clock signal. In reality, the generation of
		//this clock signal is external to the VDP itself, and this should eventually be
		//separated from the VDP core itself, so that a different EDCLK input signal can
		//be supplied if another system has different video sync requirements.
		if(!screenModeRS1Active)
		{
			//The external EDCLK input is not designed to be used with H32 mode. Selecting
			//the EDCLK input without enabling H40 mode produces a video signal with a far
			//too quick horizontal sync rate to be compatible with PAL or NTSC video
			//signals. This video mode could be displayed however, with a monitor that is
			//capable of syncing at the correct rate to display the image.
			//--------------------------------------------------------------------
			//| Screen section |   HCounter    |Pixel| Pixel |EDCLK| EDCLK |MCLK |
			//| (PAL/NTSC H32) |    value      |clock| clock |ticks|divider|ticks|
			//| (EDCLK Input)  |               |ticks|divider|     |       |     |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Left border     |0x00B-0x017    |  13 |EDCLK/2|  26 |MCLK/4 | 104 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Active display  |0x018-0x117    | 256 |EDCLK/2| 512 |MCLK/4 |2048 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Right border    |0x118-0x125    |  14 |EDCLK/2|  28 |MCLK/4 | 112 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Back porch      |0x126-0x127    |   9 |EDCLK/2|  18 |MCLK/4 |  72 |
			//|(Right Blanking)|0x1D2-0x1D8    |     |       |     |       |     |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Horizontal sync |0x1D9.0-0x1E0.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1E0.5-0x1E1.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1E1.5-0x1E9.0| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1E9.0        |   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1EA.0-0x1F1.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1F1.5-0x1F2.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1F2.5-0x1F3.0| 0.5 |EDCLK/2|   1 |MCLK/5 |   4 |
			//|                |===============|=====|=======|=====|=======|=====|
			//|        Subtotal|0x1D9-0x1F2    | (26)|       | (52)|       |(253)|
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Front porch     |0x1F3-0x1FA    |   7 |EDCLK/2|  14 |MCLK/5 |  70 |
			//|(Left Blanking) |0x1FA-0x00A    |  17 |EDCLK/2|  34 |MCLK/4 | 136 |
			//|                |===============|=====|=======|=====|=======|=====|
			//|        Subtotal|0x1ED-0x00C    | (24)|       | (48)|       |(206)|
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|TOTALS          |               | 342 |       | 684 |       |2795 |
			//--------------------------------------------------------------------

			//Calculate the number of complete hcounter lines which were advanced over the
			//time period, and the number of pixel clock ticks used to advance that number
			//of lines.
			const unsigned int mclkTicksPerLine = 2795;
			unsigned int completeLinesAdvanced = mclkTicks / mclkTicksPerLine;
			unsigned int mclkTicksUsed = (completeLinesAdvanced * mclkTicksPerLine);
			unsigned int mclkTicksRemaining = mclkTicks - mclkTicksUsed;
			pixelClockTicks = completeLinesAdvanced * hscanSettings.hcounterStepsPerIteration;

			//Using the above information, the table below shows the number of MCLK cycles
			//that hcounter values around the affected blanking area remain set for.
			//--------------------------
			//|Hcounter|Hcounter| MCLK |
			//| value  | linear |cycles|
			//|internal| value  |      |
			//|--------|--------|------|
			//|...     |...     |8     |
			//|0x1D7   |0x12D   |8     |
			//|0x1D8   |0x12E   |8     |
			//|0x1D9   |0x12F   |10    |
			//|0x1DA   |0x130   |10    |
			//|0x1DB   |0x131   |10    |
			//|0x1DC   |0x132   |10    |
			//|0x1DD   |0x133   |10    |
			//|0x1DE   |0x134   |10    |
			//|0x1DF   |0x135   |10    |
			//|0x1E0   |0x136   |9     |
			//|0x1E1   |0x137   |9     |
			//|0x1E2   |0x138   |10    |
			//|0x1E3   |0x139   |10    |
			//|0x1E4   |0x13A   |10    |
			//|0x1E5   |0x13B   |10    |
			//|0x1E6   |0x13C   |10    |
			//|0x1E7   |0x13D   |10    |
			//|0x1E8   |0x13E   |10    |
			//|0x1E9   |0x13F   |8     |
			//|0x1EA   |0x140   |10    |
			//|0x1EB   |0x141   |10    |
			//|0x1EC   |0x142   |10    |
			//|0x1ED   |0x143   |10    |
			//|0x1EE   |0x144   |10    |
			//|0x1EF   |0x145   |10    |
			//|0x1F0   |0x146   |10    |
			//|0x1F1   |0x147   |9     |
			//|0x1F2   |0x148   |9     |
			//|0x1F3   |0x149   |10    |
			//|0x1F4   |0x14A   |10    |
			//|0x1F5   |0x14B   |10    |
			//|0x1F6   |0x14C   |10    |
			//|0x1F7   |0x14D   |10    |
			//|0x1F8   |0x14E   |10    |
			//|0x1F9   |0x14F   |10    |
			//|0x1FA   |0x150   |8     |
			//|0x1FB   |0x151   |8     |
			//|...     |...     |8     |
			//--------------------------
			//We use this information below to accurately calculate the number of mclk
			//cycles used to advance the hcounter.
			while(mclkTicksRemaining > 0)
			{
				//Determine the current clock rate, and the number of steps we can advance
				//the pixel clock until the clock rate changes.
				unsigned int nextHCounterTargetStep;
				unsigned int currentMclksPerStep;
				if(hcounterCurrent < 0x1D9)
				{
					nextHCounterTargetStep = 0x1D9;
					currentMclksPerStep = 8;
				}
				else if(hcounterCurrent < 0x1E0)
				{
					nextHCounterTargetStep = 0x1E0;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1E2)
				{
					nextHCounterTargetStep = 0x1E2;
					currentMclksPerStep = 9;
				}
				else if(hcounterCurrent < 0x1E9)
				{
					nextHCounterTargetStep = 0x1E9;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1EA)
				{
					nextHCounterTargetStep = 0x1EA;
					currentMclksPerStep = 8;
				}
				else if(hcounterCurrent < 0x1F1)
				{
					nextHCounterTargetStep = 0x1F1;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1F3)
				{
					nextHCounterTargetStep = 0x1F3;
					currentMclksPerStep = 9;
				}
				else if(hcounterCurrent < 0x1FA)
				{
					nextHCounterTargetStep = 0x1FA;
					currentMclksPerStep = 10;
				}
				else
				{
					nextHCounterTargetStep = 0x1D9;
					currentMclksPerStep = 8;
				}

				//Calculate the number of pixel clock steps, and mclk ticks, which are
				//required to reach the next target hcounter value.
				unsigned int hcounterStepsToTarget = GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, nextHCounterTargetStep);
				unsigned int mclkTicksToTarget = hcounterStepsToTarget * currentMclksPerStep;

				//Calculate the number of mclk ticks we can actually advance in the time
				//we have.
				unsigned int mclkTicksToAdvance = (mclkTicksRemaining < mclkTicksToTarget)? mclkTicksRemaining: mclkTicksToTarget;

				//Calculate the number of pixel clock ticks we can advance
				unsigned int pixelClockTicksToAdvance = (mclkTicksToAdvance / currentMclksPerStep);
				pixelClockTicks += pixelClockTicksToAdvance;

				//Calculate the number of useful mclk cycles in the step we're advancing.
				//If some mclk ticks are left over, we still allow them to be deducted
				//from mclkTicksRemaining in order to indicate we have consumed all
				//available mclk ticks. The correct number of mclk ticks which were
				//actually used to advance the pixel clock is reflected in mclkTicksUsed.
				mclkTicksUsed += pixelClockTicksToAdvance * currentMclksPerStep;

				//Update hcounter to the position we've just advanced to
				hcounterCurrent = nextHCounterTargetStep;

				//Update the remaining count of mclk cycles
				mclkTicksRemaining -= mclkTicksToAdvance;
			}

			//Calculate the amount of time remaining
			mclkTicksUnused = mclkTicks - mclkTicksUsed;
		}
		else
		{
			//In H40 mode on the Mega Drive, the EDCLK input needs to be used in order to
			//produce a valid video signal. EDCLK is a complex signal, it is not a regular
			//clock pulse. When the external HSYNC line has not been asserted, EDCLK is
			//simply MCLK/4. As soon as HSYNC is asserted however, EDCLK starts outputting
			//an alternating pattern, which lasts for 66 EDCLK ticks after HSYNC has been
			//asserted. In this pattern, EDCLK switches to MCLK/5 for 15 ticks, then
			//switches back to, MCLK/4 for 2 ticks, then back to MCLK/5 for 15 ticks, and
			//so on. This continues until 4 complete periods of MCLK/5 being output for 15
			//ticks have been completed, with 2 blocks of MCLK/4 separating each one. The
			//relation of hcounter to the various clock signals, and the alignment of
			//hcounter with the analog video output, is shown below. This information has
			//been determined through extensive and precise hardware tests.
			//##NOTE## Our own testing as well as tests done by Jorge Nuno confirm that in
			//H40 mode, the left border starts being displayed 64SC cycles after the end
			//of HSYNC. We know from VRAM snooping that HSYNC ends when the HCounter
			//advances to 0x1ED in H40 mode, and there are 2 SC cycles to every pixel
			//clock cycle, which is what advances the HCounter and outputs a pixel to the
			//screen, so when the left border is about to start being drawn to the screen,
			//our internal 9-bit HCounter has a value of 0x00D, so at 0x00E, one border
			//pixel will have been drawn.
			//--------------------------------------------------------------------
			//| Screen section |   HCounter    |Pixel| Pixel |EDCLK| EDCLK |MCLK |
			//| (PAL/NTSC H40) |    value      |clock| clock |ticks|divider|ticks|
			//|                |               |ticks|divider|     |       |     |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Left border     |0x00D-0x019    |  13 |EDCLK/2|  26 |MCLK/4 | 104 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Active display  |0x01A-0x159    | 320 |EDCLK/2| 640 |MCLK/4 |2560 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Right border    |0x15A-0x167    |  14 |EDCLK/2|  28 |MCLK/4 | 112 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Back porch      |0x168-0x16C    |   9 |EDCLK/2|  18 |MCLK/4 |  72 |
			//|(Right Blanking)|0x1C9-0x1CC    |     |       |     |       |     |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Horizontal sync |0x1CD.0-0x1D4.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1D4.5-0x1D5.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1D5.5-0x1DC.0| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1DD.0        |   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1DE.0-0x1E5.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1E5.5-0x1E6.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1E6.5-0x1EC.0| 6.5 |EDCLK/2|  13 |MCLK/5 |  65 |
			//|                |===============|=====|=======|=====|=======|=====|
			//|        Subtotal|0x1CD-0x1EC    | (32)|       | (64)|       |(314)|
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Front porch     |0x1ED          |   1 |EDCLK/2|   2 |MCLK/5 |  10 |
			//|(Left Blanking) |0x1EE-0x00C    |  31 |EDCLK/2|  62 |MCLK/4 | 248 |
			//|                |===============|=====|=======|=====|=======|=====|
			//|        Subtotal|0x1ED-0x00C    | (32)|       | (64)|       |(258)|
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|TOTALS          |               | 420 |       | 840 |       |3420 |
			//--------------------------------------------------------------------

			//Calculate the number of complete hcounter lines which were advanced over the
			//time period, and the number of pixel clock ticks used to advance that number
			//of lines.
			const unsigned int mclkTicksPerLine = 3420;
			unsigned int completeLinesAdvanced = mclkTicks / mclkTicksPerLine;
			unsigned int mclkTicksUsed = (completeLinesAdvanced * mclkTicksPerLine);
			unsigned int mclkTicksRemaining = mclkTicks - mclkTicksUsed;
			pixelClockTicks = completeLinesAdvanced * hscanSettings.hcounterStepsPerIteration;

			//Using the above information, the table below shows the number of MCLK cycles
			//that hcounter values around the affected blanking area remain set for.
			//--------------------------
			//|Hcounter|Hcounter| MCLK |
			//| value  | linear |cycles|
			//|internal| value  |      |
			//|--------|--------|------|
			//|...     |...     |8     |
			//|0x1CB   |0x16F   |8     |
			//|0x1CC   |0x170   |8     |
			//|0x1CD   |0x171   |10    |
			//|0x1CE   |0x172   |10    |
			//|0x1CF   |0x173   |10    |
			//|0x1D0   |0x174   |10    |
			//|0x1D1   |0x175   |10    |
			//|0x1D2   |0x176   |10    |
			//|0x1D3   |0x177   |10    |
			//|0x1D4   |0x178   |9     |
			//|0x1D5   |0x179   |9     |
			//|0x1D6   |0x17A   |10    |
			//|0x1D7   |0x17B   |10    |
			//|0x1D8   |0x17C   |10    |
			//|0x1D9   |0x17D   |10    |
			//|0x1DA   |0x17E   |10    |
			//|0x1DB   |0x17F   |10    |
			//|0x1DC   |0x180   |10    |
			//|0x1DD   |0x181   |8     |
			//|0x1DE   |0x182   |10    |
			//|0x1DF   |0x183   |10    |
			//|0x1E0   |0x184   |10    |
			//|0x1E1   |0x185   |10    |
			//|0x1E2   |0x186   |10    |
			//|0x1E3   |0x187   |10    |
			//|0x1E4   |0x188   |10    |
			//|0x1E5   |0x189   |9     |
			//|0x1E6   |0x18A   |9     |
			//|0x1E7   |0x18B   |10    |
			//|0x1E8   |0x18C   |10    |
			//|0x1E9   |0x18D   |10    |
			//|0x1EA   |0x18E   |10    |
			//|0x1EB   |0x18F   |10    |
			//|0x1EC   |0x190   |10    |
			//|0x1ED   |0x191   |10    |
			//|0x1EE   |0x192   |8     |
			//|0x1EF   |0x193   |8     |
			//|...     |...     |8     |
			//--------------------------
			//We use this information below to accurately calculate the number of mclk
			//cycles used to advance the hcounter.
			while(mclkTicksRemaining > 0)
			{
				//Determine the current clock rate, and the number of steps we can advance
				//the pixel clock until the clock rate changes.
				unsigned int nextHCounterTargetStep;
				unsigned int currentMclksPerStep;
				if(hcounterCurrent < 0x1CD)
				{
					nextHCounterTargetStep = 0x1CD;
					currentMclksPerStep = 8;
				}
				else if(hcounterCurrent < 0x1D4)
				{
					nextHCounterTargetStep = 0x1D4;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1D6)
				{
					nextHCounterTargetStep = 0x1D6;
					currentMclksPerStep = 9;
				}
				else if(hcounterCurrent < 0x1DD)
				{
					nextHCounterTargetStep = 0x1DD;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1DE)
				{
					nextHCounterTargetStep = 0x1DE;
					currentMclksPerStep = 8;
				}
				else if(hcounterCurrent < 0x1E5)
				{
					nextHCounterTargetStep = 0x1E5;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1E7)
				{
					nextHCounterTargetStep = 0x1E7;
					currentMclksPerStep = 9;
				}
				else if(hcounterCurrent < 0x1EE)
				{
					nextHCounterTargetStep = 0x1EE;
					currentMclksPerStep = 10;
				}
				else
				{
					nextHCounterTargetStep = 0x1CD;
					currentMclksPerStep = 8;
				}

				//Calculate the number of pixel clock steps, and mclk ticks, which are
				//required to reach the next target hcounter value.
				unsigned int hcounterStepsToTarget = GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, nextHCounterTargetStep);
				unsigned int mclkTicksToTarget = hcounterStepsToTarget * currentMclksPerStep;

				//Calculate the number of mclk ticks we can actually advance in the time
				//we have.
				unsigned int mclkTicksToAdvance = (mclkTicksRemaining < mclkTicksToTarget)? mclkTicksRemaining: mclkTicksToTarget;

				//Calculate the number of pixel clock ticks we can advance
				unsigned int pixelClockTicksToAdvance = (mclkTicksToAdvance / currentMclksPerStep);
				pixelClockTicks += pixelClockTicksToAdvance;

				//Calculate the number of useful mclk cycles in the step we're advancing.
				//If some mclk ticks are left over, we still allow them to be deducted
				//from mclkTicksRemaining in order to indicate we have consumed all
				//available mclk ticks. The correct number of mclk ticks which were
				//actually used to advance the pixel clock is reflected in mclkTicksUsed.
				mclkTicksUsed += pixelClockTicksToAdvance * currentMclksPerStep;

				//Update hcounter to the position we've just advanced to
				hcounterCurrent = nextHCounterTargetStep;

				//Update the remaining count of mclk cycles
				mclkTicksRemaining -= mclkTicksToAdvance;
			}

			//Calculate the amount of time remaining
			mclkTicksUnused = mclkTicks - mclkTicksUsed;
		}
	}

	return pixelClockTicks;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetMclkTicksForPixelClockTicks(const HScanSettings& hscanSettings, unsigned int pixelClockTicks, unsigned int hcounterCurrent, bool screenModeRS0Active, bool screenModeRS1Active)
{
	//Calculate the total number of mclk ticks which will be consumed in order to advance
	//the given number of pixel clock ticks, based on the given position in the current
	//scanline, and the current screen mode settings.
	const unsigned int pixelClockDivider = 2;
	unsigned int mclkTicks;
	if(!screenModeRS0Active)
	{
		//If the RS0 bit is clear, the serial clock is generated internally, using a
		//divider from the MCLK input. If the RS1 bit is clear, selecting a H32 screen
		//mode, the serial clock is calculated as MCLK/5. If the RS1 bit is set, selecting
		//a H40 screen mode, the serial clock is calculated as MCLK/4.
		unsigned int serialClockDivider;
		if(!screenModeRS1Active)
		{
			//In H32 mode, if the RS0 bit is not set, the serial clock is calculated as
			//MCLK/5. The relation of hcounter to the various clock signals, and the
			//alignment of hcounter with the analog video output, is shown below. This
			//information has been determined through extensive and precise hardware
			//tests.
			//-----------------------------------------------------------------
			//| Screen section | HCounter  |Pixel| Pixel |Serial|Serial |MCLK |
			//| (PAL/NTSC H32) |  value    |clock| clock |clock |clock  |ticks|
			//|                |           |ticks|divider|ticks |divider|     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Left border     |0x00B-0x017|  13 |SCLK/2 |   26 |MCLK/5 | 130 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Active display  |0x018-0x117| 256 |SCLK/2 |  512 |MCLK/5 |2560 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Right border    |0x118-0x125|  14 |SCLK/2 |   28 |MCLK/5 | 140 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Back porch      |0x126-0x127|   9 |SCLK/2 |   18 |MCLK/5 |  90 |
			//|(Right Blanking)|0x1D2-0x1D8|     |       |      |       |     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Horizontal sync |0x1D9-0x1F2|  26 |SCLK/2 |   52 |MCLK/5 | 260 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Front porch     |0x1F3-0x00A|  24 |SCLK/2 |   48 |MCLK/5 | 240 |
			//|(Left Blanking) |           |     |       |      |       |     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|TOTALS          |           | 342 |       |  684 |       |3420 |
			//-----------------------------------------------------------------
			serialClockDivider = 5;
		}
		else
		{
			//In H40 mode, if the RS0 bit is not set, the serial clock is calculated as
			//MCLK/4. The relation of hcounter to the various clock signals, and the
			//alignment of hcounter with the analog video output, is shown below. This
			//information has been determined through extensive and precise hardware
			//tests. Note that on the Sega Mega Drive, this video mode does not produce a
			//valid video signal for PAL or NTSC screens. The horizontal sync period is
			//too short, and the line is too fast overall. The external EDCLK input needs
			//to be enabled by setting the RS0 bit, which supplies a modified pixel clock
			//input that slows down the horizontal sync period, to keep the number of MCLK
			//ticks required per line to 3420.
			//-----------------------------------------------------------------
			//| Screen section | HCounter  |Pixel| Pixel |Serial|Serial |MCLK |
			//| (PAL/NTSC H40) |  value    |clock| clock |clock |clock  |ticks|
			//|                |           |ticks|divider|ticks |divider|     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Left border     |0x00D-0x019|  13 |EDCLK/2|   26 |MCLK/4 | 104 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Active display  |0x01A-0x159| 320 |EDCLK/2|  640 |MCLK/4 |2560 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Right border    |0x15A-0x167|  14 |EDCLK/2|   28 |MCLK/4 | 112 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Back porch      |0x168-0x16C|   9 |EDCLK/2|   18 |MCLK/4 |  72 |
			//|(Right Blanking)|0x1C9-0x1CC|     |       |      |       |     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Horizontal sync |0x1CD-0x1EC|  32 |EDCLK/2|   64 |MCLK/4 | 256 |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|Front porch     |0x1ED-0x00C|  32 |EDCLK/2|   64 |MCLK/4 | 256 |
			//|(Left Blanking) |           |     |       |      |       |     |
			//|----------------|-----------|-----|-------|------|-------|-----|
			//|TOTALS          |           | 420 |       |  840 |       |3360 |
			//-----------------------------------------------------------------
			serialClockDivider = 4;
		}
		mclkTicks = pixelClockTicks * (serialClockDivider * pixelClockDivider);
	}
	else
	{
		//If the RS0 bit is set, the serial clock is provided through the external EDCLK
		//input. We emulate the external EDCLK input present on the Mega Drive system here
		//as if it was an internally generated clock signal. In reality, the generation of
		//this clock signal is external to the VDP itself, and this should eventually be
		//separated from the VDP core itself, so that a different EDCLK input signal can
		//be supplied if another system has different video sync requirements.
		if(!screenModeRS1Active)
		{
			//The external EDCLK input is not designed to be used with H32 mode. Selecting
			//the EDCLK input without enabling H40 mode produces a video signal with a far
			//too quick horizontal sync rate to be compatible with PAL or NTSC video
			//signals. This video mode could be displayed however, with a monitor that is
			//capable of syncing at the correct rate to display the image.
			//--------------------------------------------------------------------
			//| Screen section |   HCounter    |Pixel| Pixel |EDCLK| EDCLK |MCLK |
			//| (PAL/NTSC H32) |    value      |clock| clock |ticks|divider|ticks|
			//| (EDCLK Input)  |               |ticks|divider|     |       |     |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Left border     |0x00B-0x017    |  13 |EDCLK/2|  26 |MCLK/4 | 104 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Active display  |0x018-0x117    | 256 |EDCLK/2| 512 |MCLK/4 |2048 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Right border    |0x118-0x125    |  14 |EDCLK/2|  28 |MCLK/4 | 112 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Back porch      |0x126-0x127    |   9 |EDCLK/2|  18 |MCLK/4 |  72 |
			//|(Right Blanking)|0x1D2-0x1D8    |     |       |     |       |     |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Horizontal sync |0x1D9.0-0x1E0.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1E0.5-0x1E1.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1E1.5-0x1E9.0| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1E9.0        |   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1EA.0-0x1F1.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1F1.5-0x1F2.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1F2.5-0x1F3.0| 0.5 |EDCLK/2|   1 |MCLK/5 |   4 |
			//|                |===============|=====|=======|=====|=======|=====|
			//|        Subtotal|0x1D9-0x1F2    | (26)|       | (52)|       |(253)|
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Front porch     |0x1F3-0x1FA    |   7 |EDCLK/2|  14 |MCLK/5 |  70 |
			//|(Left Blanking) |0x1FA-0x00A    |  17 |EDCLK/2|  34 |MCLK/4 | 136 |
			//|                |===============|=====|=======|=====|=======|=====|
			//|        Subtotal|0x1ED-0x00C    | (24)|       | (48)|       |(206)|
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|TOTALS          |               | 342 |       | 684 |       |2795 |
			//--------------------------------------------------------------------

			//Calculate the number of complete hcounter lines which were advanced over the
			//time period, and the number of mclk ticks used to advance that number of
			//lines.
			const unsigned int mclkTicksPerLine = 2795;
			unsigned int completeLinesAdvanced = pixelClockTicks / hscanSettings.hcounterStepsPerIteration;
			unsigned int pixelClockTicksUsed = (completeLinesAdvanced * hscanSettings.hcounterStepsPerIteration);
			unsigned int pixelClockTicksRemaining = pixelClockTicks - pixelClockTicksUsed;
			mclkTicks = completeLinesAdvanced * mclkTicksPerLine;

			//Using the above information, the table below shows the number of MCLK cycles
			//that hcounter values around the affected blanking area remain set for.
			//--------------------------
			//|Hcounter|Hcounter| MCLK |
			//| value  | linear |cycles|
			//|internal| value  |      |
			//|--------|--------|------|
			//|...     |...     |8     |
			//|0x1D7   |0x12D   |8     |
			//|0x1D8   |0x12E   |8     |
			//|0x1D9   |0x12F   |10    |
			//|0x1DA   |0x130   |10    |
			//|0x1DB   |0x131   |10    |
			//|0x1DC   |0x132   |10    |
			//|0x1DD   |0x133   |10    |
			//|0x1DE   |0x134   |10    |
			//|0x1DF   |0x135   |10    |
			//|0x1E0   |0x136   |9     |
			//|0x1E1   |0x137   |9     |
			//|0x1E2   |0x138   |10    |
			//|0x1E3   |0x139   |10    |
			//|0x1E4   |0x13A   |10    |
			//|0x1E5   |0x13B   |10    |
			//|0x1E6   |0x13C   |10    |
			//|0x1E7   |0x13D   |10    |
			//|0x1E8   |0x13E   |10    |
			//|0x1E9   |0x13F   |8     |
			//|0x1EA   |0x140   |10    |
			//|0x1EB   |0x141   |10    |
			//|0x1EC   |0x142   |10    |
			//|0x1ED   |0x143   |10    |
			//|0x1EE   |0x144   |10    |
			//|0x1EF   |0x145   |10    |
			//|0x1F0   |0x146   |10    |
			//|0x1F1   |0x147   |9     |
			//|0x1F2   |0x148   |9     |
			//|0x1F3   |0x149   |10    |
			//|0x1F4   |0x14A   |10    |
			//|0x1F5   |0x14B   |10    |
			//|0x1F6   |0x14C   |10    |
			//|0x1F7   |0x14D   |10    |
			//|0x1F8   |0x14E   |10    |
			//|0x1F9   |0x14F   |10    |
			//|0x1FA   |0x150   |8     |
			//|0x1FB   |0x151   |8     |
			//|...     |...     |8     |
			//--------------------------
			//We use this information below to accurately calculate the number of mclk
			//cycles used to advance the hcounter.
			while(pixelClockTicksRemaining > 0)
			{
				//Determine the current clock rate, and the number of steps we can advance
				//the pixel clock until the clock rate changes.
				unsigned int nextHCounterTargetStep;
				unsigned int currentMclksPerStep;
				if(hcounterCurrent < 0x1D9)
				{
					nextHCounterTargetStep = 0x1D9;
					currentMclksPerStep = 8;
				}
				else if(hcounterCurrent < 0x1E0)
				{
					nextHCounterTargetStep = 0x1E0;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1E2)
				{
					nextHCounterTargetStep = 0x1E2;
					currentMclksPerStep = 9;
				}
				else if(hcounterCurrent < 0x1E9)
				{
					nextHCounterTargetStep = 0x1E9;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1EA)
				{
					nextHCounterTargetStep = 0x1EA;
					currentMclksPerStep = 8;
				}
				else if(hcounterCurrent < 0x1F1)
				{
					nextHCounterTargetStep = 0x1F1;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1F3)
				{
					nextHCounterTargetStep = 0x1F3;
					currentMclksPerStep = 9;
				}
				else if(hcounterCurrent < 0x1FA)
				{
					nextHCounterTargetStep = 0x1FA;
					currentMclksPerStep = 10;
				}
				else
				{
					nextHCounterTargetStep = 0x1D9;
					currentMclksPerStep = 8;
				}

				//Calculate the number of pixel clock steps which are required to reach
				//the next target hcounter value.
				unsigned int hcounterStepsToTarget = GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, nextHCounterTargetStep);

				//Calculate the number of pixel clock ticks we can advance
				unsigned int pixelClockTicksToAdvance = (hcounterStepsToTarget <= pixelClockTicksRemaining)? hcounterStepsToTarget: pixelClockTicksRemaining;

				//Increment the number of mclk ticks advanced
				mclkTicks += pixelClockTicksToAdvance * currentMclksPerStep;

				//Update hcounter to the position we've just advanced to. This result is
				//only valid if we didn't run out of pixel clock ticks before the target
				//was reached. If we didn't manage to reach the target, hcounterCurrent is
				//no longer required, so it doesn't matter that this gets set incorrectly.
				hcounterCurrent = nextHCounterTargetStep;

				//Update the remaining pixel clock ticks
				pixelClockTicksRemaining -= pixelClockTicksToAdvance;
			}
		}
		else
		{
			//In H40 mode on the Mega Drive, the EDCLK input needs to be used in order to
			//produce a valid video signal. EDCLK is a complex signal, it is not a regular
			//clock pulse. When the external HSYNC line has not been asserted, EDCLK is
			//simply MCLK/4. As soon as HSYNC is asserted however, EDCLK starts outputting
			//an alternating pattern, which lasts for 66 EDCLK ticks after HSYNC has been
			//asserted. In this pattern, EDCLK switches to MCLK/5 for 15 ticks, then
			//switches back to, MCLK/4 for 2 ticks, then back to MCLK/5 for 15 ticks, and
			//so on. This continues until 4 complete periods of MCLK/5 being output for 15
			//ticks have been completed, with 2 blocks of MCLK/4 separating each one. The
			//relation of hcounter to the various clock signals, and the alignment of
			//hcounter with the analog video output, is shown below. This information has
			//been determined through extensive and precise hardware tests.
			//##NOTE## Our own testing as well as tests done by Jorge Nuno confirm that in
			//H40 mode, the left border starts being displayed 64SC cycles after the end
			//of HSYNC. We know from VRAM snooping that HSYNC ends when the HCounter
			//advances to 0x1ED in H40 mode, and there are 2 SC cycles to every pixel
			//clock cycle, which is what advances the HCounter and outputs a pixel to the
			//screen, so when the left border is about to start being drawn to the screen,
			//our internal 9-bit HCounter has a value of 0x00D, so at 0x00E, one border
			//pixel will have been drawn.
			//--------------------------------------------------------------------
			//| Screen section |   HCounter    |Pixel| Pixel |EDCLK| EDCLK |MCLK |
			//| (PAL/NTSC H40) |    value      |clock| clock |ticks|divider|ticks|
			//|                |               |ticks|divider|     |       |     |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Left border     |0x00D-0x019    |  13 |EDCLK/2|  26 |MCLK/4 | 104 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Active display  |0x01A-0x159    | 320 |EDCLK/2| 640 |MCLK/4 |2560 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Right border    |0x15A-0x167    |  14 |EDCLK/2|  28 |MCLK/4 | 112 |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Back porch      |0x168-0x16C    |   9 |EDCLK/2|  18 |MCLK/4 |  72 |
			//|(Right Blanking)|0x1C9-0x1CC    |     |       |     |       |     |
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Horizontal sync |0x1CD.0-0x1D4.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1D4.5-0x1D5.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1D5.5-0x1DC.0| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1DD.0        |   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1DE.0-0x1E5.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
			//|                |0x1E5.5-0x1E6.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
			//|                |0x1E6.5-0x1EC.0| 6.5 |EDCLK/2|  13 |MCLK/5 |  65 |
			//|                |===============|=====|=======|=====|=======|=====|
			//|        Subtotal|0x1CD-0x1EC    | (32)|       | (64)|       |(314)|
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|Front porch     |0x1ED          |   1 |EDCLK/2|   2 |MCLK/5 |  10 |
			//|(Left Blanking) |0x1EE-0x00C    |  31 |EDCLK/2|  62 |MCLK/4 | 248 |
			//|                |===============|=====|=======|=====|=======|=====|
			//|        Subtotal|0x1ED-0x00C    | (32)|       | (64)|       |(258)|
			//|----------------|---------------|-----|-------|-----|-------|-----|
			//|TOTALS          |               | 420 |       | 840 |       |3420 |
			//--------------------------------------------------------------------

			//Calculate the number of complete hcounter lines which were advanced over the
			//time period, and the number of mclk ticks used to advance that number of
			//lines.
			const unsigned int mclkTicksPerLine = 3420;
			unsigned int completeLinesAdvanced = pixelClockTicks / hscanSettings.hcounterStepsPerIteration;
			unsigned int pixelClockTicksUsed = (completeLinesAdvanced * hscanSettings.hcounterStepsPerIteration);
			unsigned int pixelClockTicksRemaining = pixelClockTicks - pixelClockTicksUsed;
			mclkTicks = completeLinesAdvanced * mclkTicksPerLine;

			//Using the above information, the table below shows the number of MCLK cycles
			//that hcounter values around the affected blanking area remain set for.
			//--------------------------
			//|Hcounter|Hcounter| MCLK |
			//| value  | linear |cycles|
			//|internal| value  |      |
			//|--------|--------|------|
			//|...     |...     |8     |
			//|0x1CB   |0x16F   |8     |
			//|0x1CC   |0x170   |8     |
			//|0x1CD   |0x171   |10    |
			//|0x1CE   |0x172   |10    |
			//|0x1CF   |0x173   |10    |
			//|0x1D0   |0x174   |10    |
			//|0x1D1   |0x175   |10    |
			//|0x1D2   |0x176   |10    |
			//|0x1D3   |0x177   |10    |
			//|0x1D4   |0x178   |9     |
			//|0x1D5   |0x179   |9     |
			//|0x1D6   |0x17A   |10    |
			//|0x1D7   |0x17B   |10    |
			//|0x1D8   |0x17C   |10    |
			//|0x1D9   |0x17D   |10    |
			//|0x1DA   |0x17E   |10    |
			//|0x1DB   |0x17F   |10    |
			//|0x1DC   |0x180   |10    |
			//|0x1DD   |0x181   |8     |
			//|0x1DE   |0x182   |10    |
			//|0x1DF   |0x183   |10    |
			//|0x1E0   |0x184   |10    |
			//|0x1E1   |0x185   |10    |
			//|0x1E2   |0x186   |10    |
			//|0x1E3   |0x187   |10    |
			//|0x1E4   |0x188   |10    |
			//|0x1E5   |0x189   |9     |
			//|0x1E6   |0x18A   |9     |
			//|0x1E7   |0x18B   |10    |
			//|0x1E8   |0x18C   |10    |
			//|0x1E9   |0x18D   |10    |
			//|0x1EA   |0x18E   |10    |
			//|0x1EB   |0x18F   |10    |
			//|0x1EC   |0x190   |10    |
			//|0x1ED   |0x191   |10    |
			//|0x1EE   |0x192   |8     |
			//|0x1EF   |0x193   |8     |
			//|...     |...     |8     |
			//--------------------------
			//We use this information below to accurately calculate the number of mclk
			//cycles used to advance the hcounter.
			while(pixelClockTicksRemaining > 0)
			{
				//Determine the current clock rate, and the number of steps we can advance
				//the pixel clock until the clock rate changes.
				unsigned int nextHCounterTargetStep;
				unsigned int currentMclksPerStep;
				if(hcounterCurrent < 0x1CD)
				{
					nextHCounterTargetStep = 0x1CD;
					currentMclksPerStep = 8;
				}
				else if(hcounterCurrent < 0x1D4)
				{
					nextHCounterTargetStep = 0x1D4;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1D6)
				{
					nextHCounterTargetStep = 0x1D6;
					currentMclksPerStep = 9;
				}
				else if(hcounterCurrent < 0x1DD)
				{
					nextHCounterTargetStep = 0x1DD;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1DE)
				{
					nextHCounterTargetStep = 0x1DE;
					currentMclksPerStep = 8;
				}
				else if(hcounterCurrent < 0x1E5)
				{
					nextHCounterTargetStep = 0x1E5;
					currentMclksPerStep = 10;
				}
				else if(hcounterCurrent < 0x1E7)
				{
					nextHCounterTargetStep = 0x1E7;
					currentMclksPerStep = 9;
				}
				else if(hcounterCurrent < 0x1EE)
				{
					nextHCounterTargetStep = 0x1EE;
					currentMclksPerStep = 10;
				}
				else
				{
					nextHCounterTargetStep = 0x1CD;
					currentMclksPerStep = 8;
				}

				//Calculate the number of pixel clock steps which are required to reach
				//the next target hcounter value.
				unsigned int hcounterStepsToTarget = GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, nextHCounterTargetStep);

				//Calculate the number of pixel clock ticks we can advance
				unsigned int pixelClockTicksToAdvance = (hcounterStepsToTarget <= pixelClockTicksRemaining)? hcounterStepsToTarget: pixelClockTicksRemaining;

				//Increment the number of mclk ticks advanced
				mclkTicks += pixelClockTicksToAdvance * currentMclksPerStep;

				//Update hcounter to the position we've just advanced to. This result is
				//only valid if we didn't run out of pixel clock ticks before the target
				//was reached. If we didn't manage to reach the target, hcounterCurrent is
				//no longer required, so it doesn't matter that this gets set incorrectly.
				hcounterCurrent = nextHCounterTargetStep;

				//Update the remaining pixel clock ticks
				pixelClockTicksRemaining -= pixelClockTicksToAdvance;
			}
		}
	}
	return mclkTicks;
}

//----------------------------------------------------------------------------------------
//Access time functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::ConvertAccessTimeToMclkCount(double accessTime) const
{
	//##DEBUG##
	if(accessTime < 0)
	{
		std::wcout << "######################################################\n";
		std::wcout << "VDP ConvertAccessTimeToMclkCount accessTime < 0!\n";
		std::wcout << "accessTime:\t" << accessTime << "\n";
		std::wcout << "lastTimesliceMclkCyclesRemainingTime:\t" << lastTimesliceMclkCyclesRemainingTime << "\n";
		std::wcout << "currentTimesliceLength:\t" << currentTimesliceLength << "\n";
		std::wcout << "GetProcessorStateTime:\t" << GetProcessorStateTime() << "\n";
		std::wcout << "######################################################\n";
	}

	//Calculate the current mclk cycle counter
	double roundOffAdjustment = 0.1 * (1000000000.0 / clockMclkCurrent);
	unsigned int mclkCyclesTarget = (unsigned int)((accessTime + roundOffAdjustment) * (clockMclkCurrent / 1000000000.0));
	return mclkCyclesTarget;
}

//----------------------------------------------------------------------------------------
double S315_5313::ConvertMclkCountToAccessTime(unsigned int mclkCount) const
{
	//Calculate the access time
	double accessTime = (double)mclkCount * (1000000000.0 / clockMclkCurrent);
	return accessTime;
}

//----------------------------------------------------------------------------------------
//Processor state advancement functions
//----------------------------------------------------------------------------------------
//The purpose of this function is to advance the internal state of the device up to the
//indicated time. This includes updates to the status register, DMA operations, the FIFO
//buffer, and any other internal changes that occur within the VDP over a given period of
//time. This update function is always called before each register change, so it can
//assume that no register changes have occurred since the last update step.
//----------------------------------------------------------------------------------------
//We need to keep a copy of all state which can affect the number of available slots,
//and refresh them as we advance through the execution. In order to support status
//register updates, we need to fully perform sprite calculations. In order to
//calculate the render slots, we need to track the raster position, and take into
//account screen mode settings. Basically, we need to duplicate some of the render
//logic.

//Settings which affect slots:
//-R1B6(DISP): Disables the display. We know there are no access limitations when the
//display is disabled. We also know that the display can be disabled between lines,
//then enabled again before the next line starts, and the next line will still be
//drawn, so it takes effect immediately. We also know as a side-effect of disabling
//the display during hblank that the maximum number of sprites for the next line is
//reduced, which affects mickey mania. Lets start with this taking effect immediately,
//although the effect it has on the sprite rendering will need to be researched in
//order to emulate mickey mania properly, and to get the status register correct in
//this case too.
//-R1B3(M2): Switches between V28 and V30 modes. No idea when it takes effect.
//-R12B0(RS1): Switches between H32/H40 mode. Not sure if this can be done mid-line
//(notes from charles macdonald suggest this may be possible), but we know it can be
//done mid-frame.

//Settings which MAY affect slots, but need to be tested:
//-R0B0(U2): Disables video output. It's possible that access may be allowed freely like
//when the display is disabled.
//-R1B7(U1): Affects the display mode.
//-R1B0(U2): Affects the display mode.
//-R1B2(M5): Switches between M4 and M5. We know this can happen mid-frame, but we
//don't know if it can happen mid-line.

//Settings which affect status register:
//None known

//Settings which MAY affect status register, but need to be tested:
//-R5(SAT): Defines the location of the sprite attribute table. Obviously it affects
//sprites, but we don't know for sure if it can be updated during a frame or a line.
//Same goes for sprite data in VRAM. Some changes are possible, but they're limited I
//believe. Charles has some notes on this IIRC.

//So there really isn't that much we need to cache at all. We just need to keep track
//of the current render position, and use these few state parameters to advance from
//that point to the next time interval. We don't actually need to evaluate any layers
//at all, and for sprites, we only care about the size and position of each sprite, to
//support dot overflow and collision flags.

//Actually, how does the collision flag work again? Doesn't it get set if
//non-transparent pixels overlap in sprites? That would require us to touch the VRAM
//and evaluate the colour value for each pixel in each sprite. If this is the case, we
//now have to worry about VRAM updates affecting the calculation too. We also still
//don't know exactly when the sprites are rendered, but notes from Charles suggest
//that they're done on the line before the line that they're displayed on. Behaviour
//from Mickey Mania also suggests the sprite table is built during hblank. It's likely
//what really happens is that the sprite table is populated in the hblank before the
//line those sprites are used on begins, and that the actual pixel colours for each
//sprite are only evaluated during rendering, in-sync with the raster position. It
//would then be mid-frame that collision and overflow flags are set, at the exact
//pixel location when they are evalulated (although it might actually be a bit before,
//with results being buffered and then output later).

//For now, let's assume that the collision flag is set if two pixels, non-transparent
//or not, overlap during active scan. This is almost certainly wrong, but let's just
//get our new update function working first, and fix up the sprite details later. It's
//not going to radically modify the implementation anyway, and we need to do testing
//to be sure of the correct behaviour.

//bool AdvanceProcessorState(double advanceTime, bool stopAtNextAccessSlot)
//Advances the processor state, assuming no state changes occur between the current
//processor update time and the advanceTime parameter. Returns false if the
//stotAtNextAccessSlot parameter was set and the target access time wasn't reached,
//otherwise returns true. This function should be used by our UpdateInternalState()
//function to do the actual advance. Our UpdateInternalState() function will manage
//advancing the FIFO and DMA. If the FIFO is empty (or becomes empty during the update
//process), the UpdateInternalState() function can just call AdvanceProcessorState()
//with stopAtNextAccessSlot set to false to finish the update step. Since we need to
//know what the current internal state time is in order to process DMA operations, a
//GetProcessorStateTime() function will be provided to give access to the current
//advancement time, in the case that a request for an access slot means the end of the
//timeblock isn't reached.
//----------------------------------------------------------------------------------------
void S315_5313::UpdateInternalState(unsigned int mclkCyclesTarget, bool checkFifoStateBeforeUpdate, bool stopWhenFifoEmpty, bool stopWhenFifoFull, bool stopWhenFifoNotFull, bool stopWhenReadDataAvailable, bool stopWhenNoDMAOperationInProgress, bool allowAdvancePastCycleTarget)
{
	//Gather some info about the current state
	bool dmaOperationWillRun = commandCode.GetBit(5) && (!dmd1 || dmd0 || dmaFillOperationRunning);
	bool readOperationWillRun = ValidReadTargetInCommandCode() && !readDataAvailable;
	bool writeOperationWillRun = !IsWriteFIFOEmpty();

	//##DEBUG##
	if(stopWhenNoDMAOperationInProgress && !dmaOperationWillRun)
	{
		std::wcout << "######################################################\n";
		std::wcout << "VDP UpdateInternalState called with stopWhenNoDMAOperationInProgress when no DMA operation was running!\n";
		std::wcout << "mclkCyclesTarget:\t" << mclkCyclesTarget << "\n";
		std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
		std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
		std::wcout << "busGranted:\t" << busGranted << "\n";
		std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
		std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
		std::wcout << "status:\t" << status.GetData() << "\n";
		std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
		std::wcout << "dmd1:\t" << dmd1 << "\n";
		std::wcout << "dmd0:\t" << dmd1 << "\n";
		std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
		std::wcout << "######################################################\n";
	}

	//Check if we're already sitting on one of the target states
	bool targetFifoStateReached = false;
	if(checkFifoStateBeforeUpdate)
	{
		//If we're already at the target state, we exit the function immediately, since
		//there's no more work to do.
		if(TargetProcessorStateReached(stopWhenFifoEmpty, stopWhenFifoFull, stopWhenFifoNotFull, stopWhenReadDataAvailable, stopWhenNoDMAOperationInProgress))
		{
			return;
		}
	}

	//Check if we need to stop at an access slot on the next step
	bool stopAtAccessSlot = writeOperationWillRun || readOperationWillRun || dmaOperationWillRun;

	//Advance the VDP until we reach the target state
	while(!targetFifoStateReached && (!AdvanceProcessorStateNew(mclkCyclesTarget, stopAtAccessSlot, allowAdvancePastCycleTarget) || allowAdvancePastCycleTarget))
	{
		//Advance a DMA transfer operation while the write FIFO or read cache is not full,
		//and there has been enough time since the read cache became empty to fully read
		//another value from external memory.
		//##TODO## We assume here that a read operation is performed immediately, whether
		//there is room in the FIFO or not currently to save it, and that the data then
		//gets held as pending until a FIFO slot opens up, at which time, the pending data
		//write then gets moved into the FIFO, and a new read operation begins
		//immediately. Do some hardware tests to confirm this is the way the real VDP
		//behaves, and confirm the timing of everything.
		while(commandCode.GetBit(5) && !dmd1 && busGranted && (!dmaTransferReadDataCached || !IsWriteFIFOFull())
		  && ((dmaTransferNextReadMclk + (dmaTransferReadTimeInMclkCycles - dmaTransferLastTimesliceUsedReadDelay)) <= GetProcessorStateMclkCurrent()))
		{
			//If there is space in the DMA transfer read cache, read a new data value into
			//the read cache.
			if(!dmaTransferReadDataCached)
			{
				CacheDMATransferReadData(dmaTransferNextReadMclk);
			}

			//Advance the dmaTransferLastReadMclk counter, and clear the count of used
			//read delay cycles from the last timeslice, which have just been consumed.
			dmaTransferNextReadMclk += (dmaTransferReadTimeInMclkCycles - dmaTransferLastTimesliceUsedReadDelay);
			dmaTransferLastTimesliceUsedReadDelay = 0;

			//If there is space in the write FIFO to store another write value, empty the
			//DMA transfer read cache data into the FIFO.
			if(!IsWriteFIFOFull())
			{
				PerformDMATransferOperation();
				AdvanceDMAState();
			}
		}

		//Advance a DMA fill operation if fill data has been latched to trigger the fill,
		//and the write FIFO is empty. If a data port write has been made during an active
		//DMA fill operation, that data port write is performed first, and we carry on the
		//fill once the FIFO returns to an empty state.
		if(commandCode.GetBit(5) && dmd1 && !dmd0 && dmaFillOperationRunning && IsWriteFIFOEmpty())
		{
			PerformDMAFillOperation();
			AdvanceDMAState();
		}

		//Advance a DMA copy operation
		if(commandCode.GetBit(5) && dmd1 && dmd0)
		{
			PerformDMACopyOperation();
			AdvanceDMAState();
		}

		//Perform a VRAM read cache operation
		bool readOperationPerformed = false;
		if(IsWriteFIFOEmpty() && !readDataAvailable && ValidReadTargetInCommandCode())
		//##TODO## Evaluate this old code, and figure out what to do with it.
		//if(!readDataAvailable && ValidReadTargetInCommandCode())
		{
			PerformReadCacheOperation();
			readOperationPerformed = true;
		}

		//Perform a VRAM write operation
		if(!IsWriteFIFOEmpty() && !readOperationPerformed)
		{
			PerformFIFOWriteOperation();
		}

		//If a DMA transfer operation is in progress, and there's a read value held in the
		//DMA transfer read cache as pending, and the write FIFO now has a slot available,
		//load the cached DMA transfer read data into the write FIFO. This restores the
		//FIFO state back to full at the end of this update step if a DMA transfer has
		//more data pending, which is essential in order to ensure that the FIFO full flag
		//appears as full immediately after a DMA transfer operation has reached the end
		//of the source data.
		if(commandCode.GetBit(5) && !dmd1 && dmaTransferReadDataCached && !IsWriteFIFOFull())
		{
			//If there is space in the write FIFO to store another write value, empty the
			//DMA transfer read cache data into the FIFO.
			PerformDMATransferOperation();
			AdvanceDMAState();
		}

		//Update the FIFO full and empty flags in the status register
		SetStatusFlagFIFOEmpty(IsWriteFIFOEmpty());
		SetStatusFlagFIFOFull(IsWriteFIFOFull());

		//Gather some info about the current state
		dmaOperationWillRun = commandCode.GetBit(5) && (!dmd1 || dmd0 || dmaFillOperationRunning);
		readOperationWillRun = ValidReadTargetInCommandCode() && !readDataAvailable;
		writeOperationWillRun = !IsWriteFIFOEmpty();

		//Check if we need to stop at an access slot on the next step
		stopAtAccessSlot = writeOperationWillRun || readOperationWillRun || dmaOperationWillRun;

		//Stop the update process if one of the target states has been reached
		targetFifoStateReached = TargetProcessorStateReached(stopWhenFifoEmpty, stopWhenFifoFull, stopWhenFifoNotFull, stopWhenReadDataAvailable, stopWhenNoDMAOperationInProgress);
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformReadCacheOperation()
{
	//Hardware tests have shown that when doing a read from VSRAM or CRAM, not all the
	//bits in the 16-bit result are set as a result of the read. The bits which are not
	//set as a result of the read from VSRAM or CRAM, are set based on the next available
	//entry in the FIFO buffer. There is one interesting and quite special case to note
	//however. If a write is made to the data port, immediately followed by a read, and
	//the write itself has not yet been processed by the time the read is attempted, due
	//to no access slot being available, the read is processed BEFORE the write. The
	//written data overwrites the contents of the next available entry in the FIFO buffer,
	//and uninitialized bits in the read data now get set based on the corresponding bits
	//in the written data. This can be observed by attempting to write to a read target,
	//immediately followed by a read. We emulate this behaviour here by loading the first
	//pending data port write, if one exists, into the FIFO buffer at the next available
	//entry, then using the next available entry in the FIFO as the initial value for the
	//read operation. The data port write will then be processed later, and perform the
	//same write into the FIFO buffer, resulting in no change, and it will then be
	//processed as normal.
	//##TODO## I now suspect the above theory is incorrect. We have found from additional
	//hardware testing that, generally speaking, after writing to a read target,
	//attempting a read locks up the hardware. I suspect the exception to this rule, which
	//we have observed, happens when a read pre-cache is in progress or complete at the
	//time the write occurs. In this case, the next read works, with the returned value
	//possibly being combined with the invalid write data rather than the previous FIFO
	//contents. After this read however, the following read will cause a lock-up. The
	//exact reason for this lockup is unknown, but for our purposes, we will assume CD4 is
	//set after a data port write occurs.
	//if(!IsWriteFIFOEmpty())
	//{
	//	fifoBuffer[fifoNextReadEntry] = *pendingDataPortWrites.begin();
	//}

	//Note that we have confirmed that a VRAM read operation doesn't use the actual write
	//FIFO to store the read value. This has been tested on the hardware by alternating
	//between VRAM, CRAM, and VSRAM read operations, and observing the resulting values of
	//the uninitialized bits from CRAM and VSRAM reads. The results of these tests show
	//that the contents of the FIFO is not modified as a result of a read operation, it is
	//merely the uninitialized bits in the read data which obtain their value from the
	//next entry in the FIFO. The fact that reads are processed before writes, as detailed
	//above, also confirms that the data from the FIFO and the read data is combined at
	//the time the read occurs, IE, it is not the live state of the FIFO that the read
	//value is combined with at the time the data is being output over the data port, as
	//when a write has been combined with a read as described above, the result is the
	//same, no matter how long after the write has occurred the user actually then reads
	//the data port to obtain the latched read value.
	//##TODO## Confirm the above assertion about the live state of the FIFO not being used
	//at the time the read value is output over the data port. Re-running our original
	//test with the NOP operations inserted should give us the answer.
	if(!readDataHalfCached)
	{
		readBuffer = fifoBuffer[fifoNextReadEntry].dataPortWriteData;
	}

	//All possible combinations of the code flags and data port reads have been tested on
	//the hardware. Reads are decoded based on the lower 5 bits of the code data.
	RAMAccessTarget ramAccessTarget;
	ramAccessTarget.AccessTime(GetProcessorStateMclkCurrent());
	switch(commandCode.GetDataSegment(0, 5))
	{
	case 0x00:{ //?00000 VRAM Read
		//Note that hardware tests have shown that the LSB of the address is ignored for
		//16-bit reads from VRAM.
		Data tempAddress(commandAddress);
		tempAddress.SetBit(0, false);

		//##NOTE## Hardware tests have shown that the upper byte of the 16-bit value is
		//read first. This has been observed by examining the results when performing
		//reads from odd VRAM addresses. It appears that when data is read from an odd
		//address, the flag is set indicating that read data is available, even though the
		//data has actually only been half cached. If a data port read is then made before
		//the other byte is cached, the read will return a data value where only the upper
		//byte of the result comes from the target address, and the lower byte of the
		//result retains whatever the previous value was from the last read value to
		//successfully cache a lower byte.
		//##TODO## Implement these results into the way we perform reads.
		//##TODO## Comment what's going on here with the read operations. The
		//M5ReadVRAM8Bit() function inverts the LSB of the address, so this is a bit
		//confusing.
		if(!readDataHalfCached)
		{
			Data tempDataBuffer(8);
			M5ReadVRAM8Bit(tempAddress+1, tempDataBuffer, ramAccessTarget);
			readBuffer.SetByte(1, tempDataBuffer.GetData());
			readDataHalfCached = true;
		}
		else
		{
			M5ReadVRAM8Bit(tempAddress, readBuffer, ramAccessTarget);
			readDataHalfCached = false;
			readDataAvailable = true;
		}
		break;}
	case 0x04: //?00100 VSRAM Read
		M5ReadVSRAM(commandAddress, readBuffer, ramAccessTarget);
		readDataAvailable = true;
		break;
	case 0x08: //?01000 CRAM Read
		M5ReadCRAM(commandAddress, readBuffer, ramAccessTarget);
		readDataAvailable = true;
		break;
	case 0x0C: //?01100 8-bit VRAM Read (undocumented)
		//This undocumented read mode performs a true 8-bit VRAM read. The lower 8 bits
		//return an 8-bit value read from VRAM, while the upper 8 bits are unaffected.
		M5ReadVRAM8Bit(commandAddress, readBuffer, ramAccessTarget);
		readDataAvailable = true;
		break;
	default: //Invalid
		//##TODO## Update these comments, and the way we handle invalid read attempts.
		//Any attempts to read from the data port when the lower five bits don't match one
		//of the above patterns causes the VDP to lock up. A reset is unable to restore
		//normal operation. Only power cycling the device can bring the VDP back from this
		//state.
		//Update cached data for a read operation. Note that on the real VDP, attempting
		//to read from an invalid target causes the system to lock up when reading from
		//the data port. The reason this occurs is that the VDP never successfully fetches
		//a data word for the read request, so the data port read is waiting for the
		//readDataAvailable flag to be set, which never actually occurs in this case. We
		//catch cases where this would occur in our emulator at the time the data port is
		//read, so we can report the error to the user, and avoid the infinite loop that
		//would otherwise occur.
		//##TODO## Raise some kind of hard error when this occurs.
		break;
	}

	//##FIX## Is this correct? We need to sort out how we track incremented address
	//register data for operations such as reads and DMA fill/transfer operations.
	//Increment the target address
	if(!readDataHalfCached)
	{
		commandAddress += autoIncrementData;
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformFIFOWriteOperation()
{
	//##TODO## Update all the comments here
	FIFOBufferEntry& fifoBufferEntry = fifoBuffer[fifoNextReadEntry];

	//Move the pending write through the physical 4-word FIFO. The physical FIFO is only
	//maintained to correctly support old data being combined and blended with data being
	//read from CRAM or VSRAM in the uninitialized bits.
	//fifoBuffer[fifoNextReadEntry] = data;
	//fifoNextReadEntry = (fifoNextReadEntry+1) % fifoBufferSize;

	//Process the write
	//##FIX## We know from VRAM snooping as well as the official documentation, that
	//access to VRAM is byte-wide, while access to CRAM and VSRAM is word-wide. What this
	//means is it actually takes two access clock cycles to read a word from VRAM, and it
	//takes two access clock cycles to write a word to VRAM. We know from hardware tests
	//that only a single FIFO entry is used to store a full word-wide write to VRAM, so
	//there must be some kind of internal flag which records whether only one byte has
	//been written to VRAM from a pending word-wide write. Note that most likely, the
	//lower byte is written first, owing to the byteswapped layout in VRAM, but this
	//should be tested in hardware by snooping on the VRAM bus during the write operation.

	//All possible combinations of the code flags and data port writes have been tested
	//on the hardware. Writes are decoded based on the lower 4 bits of the code data.
	RAMAccessTarget ramAccessTarget;
	ramAccessTarget.AccessTime(GetProcessorStateMclkCurrent());
	switch(fifoBufferEntry.codeRegData.GetDataSegment(0, 4))
	{
	case 0x01:{ //??0001 VRAM Write
		//-Hardware tests have verified that if the LSB of the address is set, it is
		//ignored when determining the target VRAM address, but it acts as a special flag
		//causing the data to be byteswapped for writes to VRAM. This is true for any
		//write to VRAM, including writes performed as part of a DMA transfer. The LSB of
		//the address is ignored for reads from VRAM, IE, no byteswapping is ever
		//performed on reads.
		//-It should be noted that the real VDP actually stores all VRAM data byteswapped
		//in the physical memory. This has been confirmed by using a logic analyzer to
		//snoop on the VRAM bus during operation. This means that in reality, byteswapping
		//on VRAM writes really occurs when the LSB is unset, while the data is
		//byteswapped if the LSB is set, and all reads are byteswapped. We don't byteswap
		//the actual VRAM data in our emulator, as not only is this byteswapping virtually
		//transparent to the caller except in the case of DMA fills and copies (refer to
		//the implementation for further info), it would be slower for us to byteswap
		//everything on every read and write to VRAM. Since it's faster for us, and more
		//convenient and logical for the user therefore for the data to be stored
		//sequentially, we don't store data as byteswapped in VRAM.
		//-Note that the real VDP also stores the VRAM data in a non-linear fashion, with
		//data within each 0x400 byte block stored in an interleaved format. The
		//byteswapped data is striped in 4-byte groups within each 0x400 byte block, with
		//all the first bytes of each 4-byte set at 0x000-0x100, then the second bytes at
		//0x100-0x200, and so on within each 0x400 byte block. This is necessary in order
		//to support the serial access mode used to read data from VRAM. We also don't
		//implement this interleaved VRAM in our emulator, as it is an implementation
		//detail that has no effect on the logical operation of the VDP.

		//Calculate the VRAM address to read from. Note that the LSB of the address data
		//is discarded, and instead, it is set based on whether we're writing the first or
		//the second byte in a word-wide write operation. This is as per the confirmed
		//behaviour of the real hardware. 16-bit writes to VRAM are always aligned to a
		//16-bit boundary.
		Data tempAddress(fifoBufferEntry.addressRegData);
		tempAddress.SetBit(0, fifoBufferEntry.dataWriteHalfWritten);

		//Calculate the data to be written to VRAM. Note that the LSB of the original
		//VRAM write address specified by the caller is used here to select which byte of
		//the 16-bit data to write to VRAM first. If the bit is clear, the lower byte of
		//the 16-bit value is written first, otherwise, the upper byte of the 16-bit value
		//is written. Note that this is as per the observed behaviour of the real
		//hardware. The apparent lower byte of each 16-bit value is written before the
		//upper byte, IE, if a 16-bit write was occurring at VRAM address 0x20, the byte
		//at address 0x21 would first be written, followed by address 0x20. This is how it
		//appears to the user, however, since the real VRAM data is byteswapped, it's
		//actually writing the data sequentially. Note that we pass a sequential address
		//to our 8-bit VRAM write function below, however, this function inverts the LSB
		//of the target address internally, so the target address will be byteswapped
		//before the write occurs.
		unsigned int dataByteToRead = (fifoBufferEntry.addressRegData.GetBit(0) ^ fifoBufferEntry.dataWriteHalfWritten)? 1: 0;
		Data writeData(8, fifoBufferEntry.dataPortWriteData.GetByte(dataByteToRead));

		//Perform the VRAM write
		M5WriteVRAM8Bit(tempAddress, writeData, ramAccessTarget);

		//If we've just written the first half of the word-wide write operation, flag that
		//the first half has been completed, and that the second half of the write is
		//still pending, otherwise flag that the write has been completed.
		fifoBufferEntry.dataWriteHalfWritten = !fifoBufferEntry.dataWriteHalfWritten;
		fifoBufferEntry.pendingDataWrite = fifoBufferEntry.dataWriteHalfWritten;
		break;}
	case 0x03: //??0011 CRAM Write
		M5WriteCRAM(fifoBufferEntry.addressRegData, fifoBufferEntry.dataPortWriteData, ramAccessTarget);
		fifoBufferEntry.pendingDataWrite = false;
		break;
	case 0x05: //??0101 VSRAM Write
		M5WriteVSRAM(fifoBufferEntry.addressRegData, fifoBufferEntry.dataPortWriteData, ramAccessTarget);
		fifoBufferEntry.pendingDataWrite = false;
		break;
	default: //Invalid
		//Any attempts to write to the data port when the lower four bits don't match one
		//of the above patterns has no effect. The write is discarded, and the VDP is
		//unaffected.
		fifoBufferEntry.pendingDataWrite = false;
		break;
	}

	//If this pending write has been fully processed, advance to the next entry in the
	//FIFO buffer.
	if(!fifoBufferEntry.pendingDataWrite)
	{
		fifoNextReadEntry = (fifoNextReadEntry+1) % fifoBufferSize;

		//##TODO## CD4 is most likely not set by a DMA fill operation. We can test this
		//though. Try doing a DMA fill to a read target, with a non-empty FIFO at the time
		//the control port data is written. This should trigger an immediate DMA fill
		//operation to a read target, which should do nothing at all. After this, we can
		//attempt a read. Actually, this will probably lock up. Bad things happen when you
		//mix read and write operations. Still, if it locks up, that's enough evidence to
		//indicate that CD4 is not set as a result of a DMA fill operation.
		if(commandCode.GetBit(5) && dmd1 && !dmd0)
		{
			dmaFillOperationRunning = true;
		}

		//Check if a DMA transfer operation is in progress, and the transfer is stalled
		//waiting for a slot to open up in the write FIFO. This will be the case if there
		//is currently a value held in the DMA transfer read cache. In this case, we could
		//not have performed any more external memory reads before this point. Since we
		//just made a write slot available in the FIFO, we set the dmaTransferNextReadMclk
		//variable to the current processor state time, so that the next external memory
		//read for the DMA transfer operation will start no sooner than this time.
		if(commandCode.GetBit(5) && !dmd1 && dmaTransferReadDataCached)
		{
			unsigned int processorStateMclkCurrent = GetProcessorStateMclkCurrent();
			if(dmaTransferLastTimesliceUsedReadDelay == 0)
			{
				dmaTransferNextReadMclk = processorStateMclkCurrent;
			}
			else if(dmaTransferLastTimesliceUsedReadDelay >= processorStateMclkCurrent)
			{
				dmaTransferNextReadMclk = 0;
				dmaTransferLastTimesliceUsedReadDelay -= processorStateMclkCurrent;
			}
			else
			{
				dmaTransferNextReadMclk = (processorStateMclkCurrent - dmaTransferLastTimesliceUsedReadDelay);
				dmaTransferLastTimesliceUsedReadDelay = 0;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformDMAFillOperation()
{
	//##FIX## We need to determine how the VDP knows a write has been made to the data
	//port. VSRAM and CRAM fill targets grab the next available entry in the FIFO, after
	//the write has been made and the FIFO has been advanced, so they perform the fill
	//operation with the wrong data. A VRAM fill target uses the correct data, but it
	//appears the write is still processed first, possibly even both halves of the write.
	//Further testing is required.
	//##TODO## Hardware tests have shown that if writes are pending in the FIFO at the
	//time a control port write sets up a DMA fill operation, the DMA fill operation will
	//trigger without waiting for a data port write, and will use an entry from the
	//current FIFO buffer instead. Exactly which entry is selected, and whether the DMA
	//fill triggers on the next write or the last write, still needs to be confirmed
	//however.
	//Only advance a DMA fill operation if the FIFO is not empty
	//##FIX## The write needs to be processed first
	//##FIX## Hardware tests have shown that if the data port is written to while a DMA
	//fill operation is already in progress, the data port write is processed immediately
	//at the next access slot, before the fill operation is advanced. The data port write
	//occurs at the current incremented address that the DMA fill operation was up to, and
	//the DMA fill operation continues from the incremented address after the write has
	//been processed. We need to emulate this behaviour. It is also clear from this that
	//pending write entries in the FIFO need to be processed before DMA fill update steps.

	//Manipulate the source target address for the DMA fill operation.
	//-Note that on the real hardware, the VDP stores its data in VRAM with each 16-bit
	//word byteswapped, so a value of 0x1234 would be stored in the physical memory as
	//0x3412. This has been confirmed through the use of a logic analyzer snooping on the
	//VRAM bus during operation. For most operations, this byteswapping of VRAM memory is
	//transparent to the user, since the VDP automatically byteswaps all word-wide reads
	//and writes to and from VRAM, so the data is read and written as if it was not
	//byteswapped at all. DMA fill and copy operations are the only time where the
	//byteswapping behaviour of the real hardware is visible to the user, as the byte-wide
	//VRAM access that occurs as part of these operations allow reads and writes to and
	//from odd addresses in VRAM. In the real hardware, these addresses are used directly,
	//without modification, to read and write the byteswapped data, meaning that reads and
	//writes from odd addresses actually access the upper byte of a word, and reads and
	//writes to even addresses actually access to the lower byte of the word. For our
	//emulator, we store data in VRAM without byteswapping, to simplify the implementation
	//and present the data to the user in the form they would expect when using the
	//debugger. In order to correctly implement the behaviour of a DMA fill or copy
	//however, we therefore have to swap odd and even addresses when performing byte-wide
	//access, so that we get the correct result.
	//-Note that the official documentation from Sega listing the order the data is
	//written to VRAM during a DMA fill operation is technically accurate, but is very
	//misleading, since the order and pattern of the writes that they list shows the
	//actual write pattern to the byteswapped memory, with the order of all bytes being
	//reversed to what the reader may expect. At no point in the official documentation is
	//it mentioned that the data in VRAM is byteswapped.
	//Data targetAddressByteswapped(16, commandAddress.GetData());
	//targetAddressByteswapped.SetBit(0, !targetAddressByteswapped.GetBit(0));

	//Check if this DMA fill operation is targeting VRAM. Right now, we have special case
	//handling for VRAM write targets as opposed to CRAM or VSRAM write targets, as there
	//are some implementation quirks in the VDP that result in different behaviour for
	//CRAM or VSRAM targets.
	//##TODO## Find a way to unify this code, and end up with a clean implementation.
	bool fillTargetsVRAM = (commandCode.GetDataSegment(0, 4) == 0x01);

	//Increment the target address for the last entry
	unsigned int fifoLastReadEntry = (fifoNextReadEntry+(fifoBufferSize-1)) % fifoBufferSize;
	fifoBuffer[fifoLastReadEntry].addressRegData += autoIncrementData;

	//##TODO## We need to determine exactly how the VDP latches the DMA fill data. It
	//seems to me, the most likely explanation is that a DMA fill is triggered after the
	//first half of a data port write is written, and then kicks in and repeats the second
	//half of that write repeatedly. Since a write to CRAM or VSRAM is 16-bit, and
	//completes in one step, this results in the FIFO being advanced, and the entire write
	//being repeated.
	Data fillData(16);
	if(fillTargetsVRAM)
	{
		//Extract the upper byte of the written data. This single byte of data is used to
		//perform the fill. Hardware tests have shown no other data gets written to the
		//FIFO using a DMA fill operation other than the normal write that triggers the
		//fill, and the FIFO is not advanced at all during the fill. Also note that the
		//byteswapping behaviour of VRAM writes has no impact on which data is used for
		//the fill operation. The upper byte of the data written to the data port is
		//always used for the fill, regardless of whether the write is performed to an
		//even or odd VRAM address.
		fillData = fifoBuffer[fifoLastReadEntry].dataPortWriteData.GetUpperBits(8);
	}
	else
	{
		//##FIX## Hardware tests have shown that when a DMA fill is being performed to
		//CRAM or VSRAM, the write to the data port isn't used as the fill data for the
		//operation. What happens instead is that the data write is performed as normal,
		//and the FIFO is advanced, then when the DMA fill operation triggers, it is the
		//data in the next available FIFO slot that is used for the fill data. This only
		//occurs for DMA fills to CRAM or VSRAM, and is no doubt due to the fact that VRAM
		//requires two byte-wide writes to commit a single word-wide write to the VRAM,
		//while CRAM and VSRAM perform a single word-wide operation.
		fillData = fifoBuffer[fifoNextReadEntry].dataPortWriteData;
	}

	//##NOTE## Hardware tests have shown that during a DMA fill operation, the FIFO flags
	//in the status register remain with the empty flag set, and the full flag cleared,
	//throughout the fill operation, with of course the exception of when the data port
	//write occurs to trigger the fill, if that write is waiting in the FIFO at the time
	//of the control port read.
	//Note that the DMA busy flag remains set as soon as the control port is written, even
	//if the data port write to trigger the fill hasn't yet been made. The DMA busy flag
	//remains set until a data port write has been made and the DMA fill is complete.
	//##FIX## Hardware tests have shown that DMA fill operations to CRAM and VSRAM are
	//possible, and work correctly, with the exception that the wrong data is used for the
	//fill, as described above. We need to emulate this by actually using the specified
	//write target when performing the write below. Also note that a single DMA fill write
	//is a 16-bit operation to VSRAM and CRAM, with the full 16-bit fill value being used
	//at each target address, and the LSB of the address being masked.
	//Transfer the next byte of the fill operation
	//##TODO## Test on hardware to determine what happens when the data port is written to
	//while a DMA fill operation is in progress.
	RAMAccessTarget ramAccessTarget;
	ramAccessTarget.AccessTime(GetProcessorStateMclkCurrent());
	switch(commandCode.GetDataSegment(0, 4))
	{
	case 0x01: //??0001 VRAM Write
		M5WriteVRAM8Bit(fifoBuffer[fifoLastReadEntry].addressRegData, fillData, ramAccessTarget);
		break;
	case 0x03: //??0011 CRAM Write
		M5WriteCRAM(fifoBuffer[fifoLastReadEntry].addressRegData, fillData, ramAccessTarget);
		break;
	case 0x05: //??0101 VSRAM Write
		M5WriteVSRAM(fifoBuffer[fifoLastReadEntry].addressRegData, fillData, ramAccessTarget);
		break;
	}

	//##FIX## This is incorrect. We know from testing that if a data port write occurs
	//while a DMA fill operation is in progress, that write occurs at the currently
	//incremented address that the next fill write would go to, and the fill operation
	//proceeds at the incremented address after the write.
	//##NOTE## Fixing this could be as simple as setting the
	//codeAndAddressRegistersModifiedSinceLastWrite register to true here, which should
	//trigger the next data port write to obtain its code and address register data
	//directly from commandAddress and commandCode, rather than using the previous FIFO
	//buffer entry contents. This won't work fully actually, since the DMA fill operation
	//then needs to continue at the incremented address after the write. It does therefore
	//seem as if the actual address data that is used and modified actively by the DMA
	//fill update step is the address data stored in the FIFO entry which triggered the
	//fill. I just noticed above that this is actually what we are using. Why are we
	//incrementing commandAddress here then? We don't actually seem to be using it
	//anywhere. We should update our code, and our comments.
	//Increment the target address
	commandAddress += autoIncrementData;
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformDMACopyOperation()
{
	//Get the current source address
	unsigned int sourceAddress = (dmaSourceAddressByte1) | (dmaSourceAddressByte2 << 8);

	//Manipulate the source and target addresses for the DMA copy operation.
	//-Note that on the real hardware, the VDP stores its data in VRAM with each 16-bit
	//word byteswapped, so a value of 0x1234 would be stored in the physical memory as
	//0x3412. This has been confirmed through the use of a logic analyzer snooping on the
	//VRAM bus during operation. For most operations, this byteswapping of VRAM memory is
	//transparent to the user, since the VDP automatically byteswaps all word-wide reads
	//and writes to and from VRAM, so the data is read and written as if it was not
	//byteswapped at all. DMA fill and copy operations are the only time where the
	//byteswapping behaviour of the real hardware is visible to the user, as the byte-wide
	//VRAM access that occurs as part of these operations allow reads and writes to and
	//from odd addresses in VRAM. In the real hardware, these addresses are used directly,
	//without modification, to read and write the byteswapped data, meaning that reads and
	//writes from odd addresses actually access the upper byte of a word, and reads and
	//writes to even addresses actually access to the lower byte of the word. For our
	//emulator, we store data in VRAM without byteswapping, to simplify the implementation
	//and present the data to the user in the form they would expect when using the
	//debugger. In order to correctly implement the behaviour of a DMA fill or copy
	//however, we therefore have to swap odd and even addresses when performing byte-wide
	//access, so that we get the correct result.
	Data sourceAddressByteswapped(16, sourceAddress);
	sourceAddressByteswapped.SetBit(0, !sourceAddressByteswapped.GetBit(0));
	Data targetAddressByteswapped(16, commandAddress.GetData());
	targetAddressByteswapped.SetBit(0, !targetAddressByteswapped.GetBit(0));

	//Ensure that CD4 is set when performing a DMA copy. Hardware tests have shown that
	//while the state of CD0-CD3 is completely ignored for DMA copy operations, CD4 must
	//be set, otherwise the VDP locks up hard when the DMA operation is triggered.
	if(!commandCode.GetBit(4))
	{
		//##TODO## Log an error or trigger an assert here
	}

	//Perform the copy. Note that hardware tests have shown that DMA copy operations
	//always target VRAM, regardless of the state of CD0-CD3.
	RAMAccessTarget ramAccessTarget;
	ramAccessTarget.AccessTime(GetProcessorStateMclkCurrent());
	unsigned char data;
	data = vram->Read(sourceAddressByteswapped.GetData(), ramAccessTarget);
	vram->Write(targetAddressByteswapped.GetData(), data, ramAccessTarget);

	//Increment the target address
	commandAddress += autoIncrementData;
}

//----------------------------------------------------------------------------------------
void S315_5313::CacheDMATransferReadData(unsigned int mclkTime)
{
	//Get the current source address. Note that for a DMA transfer operation, the source
	//address is stored without the LSB, so we need to shift the complete address data up
	//by one.
	unsigned int sourceAddress = (dmaSourceAddressByte1 << 1) | (dmaSourceAddressByte2 << 9) | (dmaSourceAddressByte3 << 17);

	//Read the next data word to transfer from the source
	memoryBus->ReadMemory(sourceAddress, dmaTransferReadCache, GetDeviceContext(), dmaTransferNextReadMclk, ACCESSCONTEXT_DMAREAD);

	//Flag that data has been cached for the DMA transfer operation
	dmaTransferReadDataCached = true;
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformDMATransferOperation()
{
	//Add the data write to the FIFO
	//-Note that we can grab the current working value for the command code and address
	//register data, since we know we obtained exclusive bus access when the DMA transfer
	//command word was written.
	//-Note that a DMA transfer will be triggered regardless of the state of CD4, as this
	//bit isn't tested when deciding whether to trigger an external DMA transfer, and
	//write target decoding only looks at the state of CD3-CD0.
	//-Note that hardware testing has confirmed that DMA transfers move the data through
	//the FIFO in the same manner as normal VRAM write operations, with the same rules for
	//memory wrapping and byte swapping.
	//-Note that genvdp.txt by Charles MacDonald reports that DMA transfers to CRAM are
	//aborted when the target address passes 0x7F. This is incorrect. The palette
	//corruption reported in "Batman and Robin" on the Mega Drive is due to the fact that
	//DMA operations actively update the DMA source count registers as the operation is
	//being performed, meaning that no matter what transfer count was used, the DMA length
	//registers should both be 0 after the DMA operation is completed. Batman and Robin
	//only writes to the lower transfer count byte when performing some transfers to CRAM,
	//meaning that unless the upper byte is already 0, too much data will be transferred
	//to CRAM, corrupting the palette. If the DMA source and DMA length registers are
	//correctly updated by DMA operations, as hardware tests have proven does occur, this
	//bug will not occur.
	if(codeAndAddressRegistersModifiedSinceLastWrite)
	{
		fifoBuffer[fifoNextWriteEntry].codeRegData = commandCode;
		fifoBuffer[fifoNextWriteEntry].addressRegData = commandAddress;
		codeAndAddressRegistersModifiedSinceLastWrite = false;
	}
	else
	{
		unsigned int fifoLastWriteEntry = (fifoNextWriteEntry+(fifoBufferSize-1)) % fifoBufferSize;
		fifoBuffer[fifoNextWriteEntry].codeRegData = fifoBuffer[fifoLastWriteEntry].codeRegData;
		fifoBuffer[fifoNextWriteEntry].addressRegData = fifoBuffer[fifoLastWriteEntry].addressRegData + autoIncrementData;
	}
	fifoBuffer[fifoNextWriteEntry].dataPortWriteData = dmaTransferReadCache;
	fifoBuffer[fifoNextWriteEntry].dataWriteHalfWritten = false;
	fifoBuffer[fifoNextWriteEntry].pendingDataWrite = true;

	//Advance the FIFO to the next slot
	fifoNextWriteEntry = (fifoNextWriteEntry+1) % fifoBufferSize;

	//Now that the cached DMA transfer data has been written to the FIFO, clear the flag
	//indicating that DMA transfer data is currently cached.
	dmaTransferReadDataCached = false;
}

//----------------------------------------------------------------------------------------
void S315_5313::AdvanceDMAState()
{
	//Decrement the DMA transfer count registers. Note that the transfer count is
	//decremented before it is tested against 0, so a transfer count of 0 is equivalent to
	//a transfer count of 0x10000.
	dmaLengthCounter = (dmaLengthCounter - 1) & 0xFFFF;

	//Increment the DMA source address registers. Note that all DMA operations cause the
	//DMA source address registers to be advanced, including a DMA fill operation, even
	//though a DMA fill doesn't actually use the source address. This has been confirmed
	//through hardware tests. Also note that only the lower two DMA source address
	//registers are modified. Register 0x17, which contains the upper 7 bits, of the
	//source address for a DMA transfer operation, is not updated, which prevents a DMA
	//transfer operation from crossing a 0x20000 byte boundary. This behaviour is
	//undocumented but well known, and has been verified through hardware tests.
	unsigned int incrementedDMASourceAddress = dmaSourceAddressByte1 | (dmaSourceAddressByte2 << 8);
	++incrementedDMASourceAddress;
	dmaSourceAddressByte1 = incrementedDMASourceAddress & 0xFF;
	dmaSourceAddressByte2 = (incrementedDMASourceAddress >> 8) & 0xFF;

	//If the DMA length counter is 0 after a DMA operation has been advanced, we've
	//reached the end of the DMA operation. In this case, we clear CD5. This flags the DMA
	//operation as completed. If we're in a DMA transfer operation, this will also cause
	//the bus to be released by the DMA worker thread.
	if(dmaLengthCounter == 0)
	{
		commandCode.SetBit(5, false);
		SetStatusFlagDMA(false);

		//If we were running a DMA fill or DMA transfer operation, flag that it is now
		//completed.
		dmaFillOperationRunning = false;
		dmaTransferActive = false;
	}

	//Flag that the cached DMA settings have been modified
	cachedDMASettingsChanged = true;
}

//----------------------------------------------------------------------------------------
bool S315_5313::TargetProcessorStateReached(bool stopWhenFifoEmpty, bool stopWhenFifoFull, bool stopWhenFifoNotFull, bool stopWhenReadDataAvailable, bool stopWhenNoDMAOperationInProgress)
{
	//Check if a DMA operation is currently running
	bool dmaOperationRunning = commandCode.GetBit(5) && (!dmd1 || dmd0 || dmaFillOperationRunning);

	//Check if we've reached one of the target processor states
	bool targetProcessorStateReached = false;
	if(stopWhenFifoEmpty && GetStatusFlagFIFOEmpty())
	{
		targetProcessorStateReached = true;
	}
	else if(stopWhenFifoFull && GetStatusFlagFIFOFull())
	{
		targetProcessorStateReached = true;
	}
	else if(stopWhenFifoNotFull && !GetStatusFlagFIFOFull())
	{
		targetProcessorStateReached = true;
	}
	else if(stopWhenReadDataAvailable && readDataAvailable)
	{
		targetProcessorStateReached = true;
	}
	else if(stopWhenNoDMAOperationInProgress && !dmaOperationRunning)
	{
		targetProcessorStateReached = true;
	}

	//Return the result of the processor state check
	return targetProcessorStateReached;
}

//----------------------------------------------------------------------------------------
//##TODO## Remove this old function
//bool S315_5313::AdvanceProcessorState(unsigned int mclkCyclesTarget, bool stopAtNextAccessSlot, bool allowAdvancePastTargetForAccessSlot)
//{
//	//Ensure that we aren't trying to trigger an update out of order
//	if((!stopAtNextAccessSlot || !allowAdvancePastTargetForAccessSlot) && (mclkCyclesTarget < stateLastUpdateMclk))
//	{
//		//##TODO## Raise an assert if this occurs
//		//##DEBUG##
//		std::wcout << "######################################################\n";
//		std::wcout << "VDP AdvanceProcessorState called out of order!\n";
//		std::wcout << "mclkCyclesTarget:\t" << mclkCyclesTarget << "\n";
//		std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
//		std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
//		std::wcout << "######################################################\n";
//		return true;
//	}
//
//	unsigned int hcounterBeforeUpdate = hcounter.GetData();
//
//	//Calculate the total number of mclk cycles which need to be advanced
//	unsigned int mclkCyclesToExecute = (mclkCyclesTarget - stateLastUpdateMclk) + stateLastUpdateMclkUnused;
//
//	//Calculate the total number of pixel clock cycles which will occur if the VDP is
//	//advanced the given number of mclk cycles.
//	//##FIX## This won't work, since the video mode may need to change within the update
//	//step, invalidating our result. Currently, this function may tell us that we need to
//	//execute 20 pixel clock steps, while in reality we only need to execute 16, or vice
//	//versa.
//	//##TODO## In order to solve our screen mode settings change issue, we need to fold
//	//the current logic contained within the AdvanceStatusRegisterAndHVCounter() function
//	//into this function, so that we can perform our calculations within the "safe" blocks
//	//between screen mode changes, and simply combine the totals across all our blocks to
//	//work out overall targets.
//	unsigned int mclkRemainingCycles;
//	unsigned int totalPixelClockCyclesToExecute;
//	totalPixelClockCyclesToExecute = GetPixelClockTicksForMclkTicks(mclkCyclesToExecute, hcounterBeforeUpdate, screenModeH40, mclkRemainingCycles);
//
//	//Locate and stop the update at the next available access slot, if requested.
//	bool result = true;
//	if(stopAtNextAccessSlot)
//	{
//		//Calculate the number of pixel clock cycles until the next available access slot
//		unsigned int totalPixelClockCyclesUntilAccessSlot;
//		totalPixelClockCyclesUntilAccessSlot = GetPixelClockTicksUntilNextAccessSlot(hcounterBeforeUpdate, screenModeH40);
//
//		//If an access slot occurs within the update region, stop the update at the access
//		//slot.
//		//##FIX## Evaluate the way we use this function, and make sure that if an access
//		//slot lies exactly on the target MCLK cycle count, that an access slot is
//		//actually allowed at that location. We return false here in this case, so it's
//		//possible the calling code doesn't actually allow access when false is returned.
//		if((totalPixelClockCyclesUntilAccessSlot < totalPixelClockCyclesToExecute) || allowAdvancePastTargetForAccessSlot)
//		{
//			//Since we are stopping at an access slot, set the number of pixel clock
//			//cycles to execute as the number of cycles until the access slot occurs, and
//			//return false to indicate to the caller that an access slot stopped the
//			//update before the target was reached.
//			totalPixelClockCyclesToExecute = totalPixelClockCyclesUntilAccessSlot;
//			result = false;
//		}
//	}
//
//	//Update the status register and HV counter
//	AdvanceStatusRegisterAndHVCounter(totalPixelClockCyclesToExecute);
//
//	//Update the current state mclk cycle counter and remaining mclk cycles
//	if(!result)
//	{
//		//##FIX## This can't work, because screenModeH40 is a member of the class, which
//		//gets changed during the AdvanceStatusRegisterAndHVCounter() function call above.
//		//Potentially, in a single step, more than one video mode is active. We need the
//		//actual AdvanceStatusRegisterAndHVCounter() function to return the number of mclk
//		//cycles which were advanced.
//		stateLastUpdateMclk = (stateLastUpdateMclk + GetMclkTicksForPixelClockTicks(totalPixelClockCyclesToExecute, hcounterBeforeUpdate, screenModeH40)) - stateLastUpdateMclkUnused;
//		stateLastUpdateMclkUnused = 0;
//	}
//	else
//	{
//		stateLastUpdateMclk = mclkCyclesTarget;
//		stateLastUpdateMclkUnused = mclkRemainingCycles;
//	}
//
//	return result;
//}

//----------------------------------------------------------------------------------------
bool S315_5313::AdvanceProcessorStateNew(unsigned int mclkCyclesTarget, bool stopAtNextAccessSlot, bool allowAdvancePastTargetForAccessSlot)
{
	//Ensure that we aren't trying to trigger an update out of order
//	if((!stopAtNextAccessSlot || !allowAdvancePastTargetForAccessSlot) && (mclkCyclesTarget < (stateLastUpdateMclk + stateLastUpdateMclkUnused)))
	if((mclkCyclesTarget < (stateLastUpdateMclk + stateLastUpdateMclkUnused)) && (!stopAtNextAccessSlot || !allowAdvancePastTargetForAccessSlot))
	{
		//##TODO## Raise an assert if this occurs
		//##DEBUG##
		//outputTestDebugMessages = true;
		std::wcout << "######################################################\n";
		std::wcout << "VDP AdvanceProcessorStateNew called out of order!\n";
		std::wcout << "mclkCyclesTarget:\t" << mclkCyclesTarget << "\n";
		std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
		std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
		std::wcout << "######################################################\n";
		return true;
	}

	//##DEBUG##
	//std::wcout << "-VDP AdvanceProcessorStateNew called: " << currentTimesliceTotalMclkCycles << "\t" << mclkCyclesTarget << "\t" << stateLastUpdateMclk << "\t" << stateLastUpdateMclkUnused << "\t" << stateLastUpdateMclkUnusedFromLastTimeslice << "\n";

	//Grab the latest settings for any registers which affect the video mode. Note that we
	//only advance the VDP state between register writes, so we know the currently latched
	//video mode settings are valid, and that any video mode register changes which have
	//been made need to be applied the next time those settings are latched.
	bool interlaceEnabledNew = interlaceEnabledCached;
	bool interlaceDoubleNew = interlaceDoubleCached;
	bool screenModeRS0New = screenModeRS0Cached;
	bool screenModeRS1New = screenModeRS1Cached;
	bool screenModeV30New = screenModeV30Cached;
	bool palModeNew = palModeLineState;

	//Check whether any of the relevant video mode settings have changed since they were
	//latched.
	//##NOTE## We used to latch changes to the H40 screen mode at hblank. While we know
	//this was incorrect, we haven't totally mapped out the behaviour of the VDP when this
	//register setting is toggled mid-line. I believe changes to the H40 screen mode
	//setting do take effect immediately, however, we're keeping the old code intact below
	//to hold off the change until hblank, in case it comes in handy for further testing
	//or development.
	//##TODO## Determine exactly how the VDP reacts to the H40 screen mode state being
	//changed at all the various points during a line.
	//##FIX## We've re-enabled the old behaviour. A number of changes need to occur to
	//ensure that these settings can safely be changed mid-line, and since we still don't
	//fully know the correct behaviour of the hardware, we're going to keep these settings
	//being latched at hblank for the time being.
	bool hscanSettingsChanged = (screenModeRS0 != screenModeRS0New) || (screenModeRS1 != screenModeRS1New);
//	bool hscanSettingsChanged = false;
//	screenModeRS0 = screenModeRS0New;
//	screenModeRS1 = screenModeRS1New;
	//##TODO## Currently, changes to the palMode flag are applied at vblank. Test how the
	//real hardware deals with changes to this line by toggling the line state at runtime.
	bool vscanSettingsChanged = (screenModeV30 != screenModeV30New) || (palMode != palModeNew) || (interlaceEnabled != interlaceEnabledNew);

	//Calculate the total number of mclk cycles which need to be advanced
	unsigned int mclkCyclesToExecute = (mclkCyclesTarget - (stateLastUpdateMclk + stateLastUpdateMclkUnused));

	//Advance the device until we reach the target position
	bool stoppedAtAccessSlot = false;
	unsigned int mclkCyclesAdvanced = 0;
	while(((mclkCyclesAdvanced < mclkCyclesToExecute) && (!stopAtNextAccessSlot || !stoppedAtAccessSlot)) || (allowAdvancePastTargetForAccessSlot && stopAtNextAccessSlot && !stoppedAtAccessSlot))
	{
		//Obtain the current hscan and vscan settings
		const HScanSettings& hscanSettings = GetHScanSettings(screenModeRS0, screenModeRS1);
		const VScanSettings& vscanSettings = GetVScanSettings(screenModeV30, palMode, interlaceEnabled);

		//Calculate the number of mclk cycles to advance in this next step in order to
		//reach the target mclk cycle count.
		unsigned int mclkCyclesToAdvanceThisStep = mclkCyclesToExecute - mclkCyclesAdvanced;
		
		//If any screen mode changes are pending, check if we're going to encounter any
		//update points in this step.
		bool screenModeSettingsChangeRequired = false;
		bool updateStepIsHBlank = false;
		bool updateStepIsVBlank = false;
		if(hscanSettingsChanged || vscanSettingsChanged)
		{
			//Calculate the number of steps before the hblank and vblank events occur, and
			//new settings are latched.
			unsigned int pixelClockStepsBeforeHBlank = 0;
			if(hscanSettingsChanged)
			{
				pixelClockStepsBeforeHBlank = GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounter.GetData(), hscanSettings.hblankSetPoint);
			}
			unsigned int pixelClockStepsBeforeVBlank = 0;
			if(vscanSettingsChanged)
			{
				pixelClockStepsBeforeVBlank = GetPixelClockStepsBetweenHVCounterValues(hscanSettings, hcounter.GetData(), hscanSettings.vcounterIncrementPoint, vscanSettings, interlaceEnabled, GetStatusFlagOddInterlaceFrame(), vcounter.GetData(), vscanSettings.vblankSetPoint);
			}

			//Check whether the next update step occurs at hblank or vblank. If both hscan
			//and vscan settings have changed, select the one which occurs first.
			updateStepIsVBlank = (vscanSettingsChanged && !hscanSettingsChanged) || (vscanSettingsChanged && hscanSettingsChanged && (pixelClockStepsBeforeVBlank < pixelClockStepsBeforeHBlank));
			updateStepIsHBlank = !updateStepIsVBlank;

			//Determine how many pixel clock ticks are going to occur before the next
			//screen mode settings change.
			unsigned int pixelClockTicksBeforeScreenModeChange = updateStepIsVBlank? pixelClockStepsBeforeVBlank: pixelClockStepsBeforeHBlank;

			//If a screen mode change is going to occur before the target mclk cycle
			//count, only advance up to the screen mode change in this step.
			unsigned int mclkTicksBeforeScreenModeChange = GetMclkTicksForPixelClockTicks(hscanSettings, pixelClockTicksBeforeScreenModeChange, hcounter.GetData(), screenModeRS0, screenModeRS1);
			if((mclkTicksBeforeScreenModeChange < mclkCyclesToAdvanceThisStep) || allowAdvancePastTargetForAccessSlot)
			{
				mclkCyclesToAdvanceThisStep = mclkTicksBeforeScreenModeChange;
				screenModeSettingsChangeRequired = true;
			}
		}

		//If the caller has requested the update to stop at the next access slot, check if
		//we're going to encounter one in this step.
		if(stopAtNextAccessSlot)
		{
			//Calculate the number of pixel clock cycles until the next available access
			//slot
			unsigned int totalPixelClockCyclesUntilAccessSlot;
			totalPixelClockCyclesUntilAccessSlot = GetPixelClockTicksUntilNextAccessSlot(hscanSettings, vscanSettings, hcounter.GetData(), screenModeRS0, screenModeRS1, displayEnabledCached, vcounter.GetData());

			//If an access slot occurs within the update region, stop the update at the
			//access slot.
			//##TODO## Update the comments below
			//##FIX## Evaluate the way we use this function, and make sure that if an access
			//slot lies exactly on the target MCLK cycle count, that an access slot is
			//actually allowed at that location. We return false here in this case, so it's
			//possible the calling code doesn't actually allow access when false is returned.
			unsigned int mclkTicksBeforeAccessSlot = GetMclkTicksForPixelClockTicks(hscanSettings, totalPixelClockCyclesUntilAccessSlot, hcounter.GetData(), screenModeRS0, screenModeRS1);
			if((mclkTicksBeforeAccessSlot <= mclkCyclesToAdvanceThisStep) //The access slot occurs on or before the end of the update time
			|| allowAdvancePastTargetForAccessSlot) //The access slot is allowed to pass the end of the update time, and no screen mode settings changes are required, or they occur on or after the slot.
			{
				//If a screen mode settings change needs to be applied and the access slot
				//occurs before the point where the settings change is applied, clear the
				//flag indicating we need to apply a settings change in this step. If the
				//access slot occurs at the same time as the settings change however,
				//apply the settings change in this step, and return the access slot.
				if(screenModeSettingsChangeRequired && (mclkTicksBeforeAccessSlot < mclkCyclesToAdvanceThisStep))
				{
					screenModeSettingsChangeRequired = false;
				}

				//Since we are stopping at an access slot, set the number of mclk cycles
				//to execute as the number of cycles until the access slot occurs, and
				//flag that the update cycle was stopped at an access slot before the
				//target was reached.
				mclkCyclesToAdvanceThisStep = mclkTicksBeforeAccessSlot;
				stoppedAtAccessSlot = true;
			}
		}

		//Check if we're going to pass the point at which the next event occurs with this
		//next update step. We need this check here, to handle the case where a calling
		//device causes the state of the VDP to be advanced past the end of the current
		//timeslice, but we have a timing point to run at the end of this timeslice to
		//execute an event. We latch the screen mode settings at the time of the event in
		//this case, so we can correctly handle the event later during the timing point.
		if((nextEventToExecute.mclkCycleCounter >= GetProcessorStateMclkCurrent()) //The event occurs at or after the current time
		&& (nextEventToExecute.mclkCycleCounter < (GetProcessorStateMclkCurrent() + mclkCyclesToAdvanceThisStep))) //The event occurs during the next update step we're about to make
		{
			eventSnapshotScreenModeRS0 = screenModeRS0;
			eventSnapshotScreenModeRS1 = screenModeRS1;
			eventSnapshotScreenModeV30 = screenModeV30;
			eventSnapshotPalMode = palMode;
			eventSnapshotInterlaceEnabled = interlaceEnabled;
			eventSnapshotSaved = true;
		}

		//Calculate the total number of pixel clock cycles which will occur if the VDP is
		//advanced the target number of mclk cycles.
		unsigned int mclkRemainingCycles;
		unsigned int totalPixelClockCyclesToExecute;
		totalPixelClockCyclesToExecute = GetPixelClockTicksForMclkTicks(hscanSettings, stateLastUpdateMclkUnusedFromLastTimeslice + stateLastUpdateMclkUnused + mclkCyclesToAdvanceThisStep, hcounter.GetData(), screenModeRS0, screenModeRS1, mclkRemainingCycles);

		//##DEBUG##
		//##FIX## This check is wrong.
		//if(mclkCyclesToAdvanceThisStep <= 0)
		//{
		//	std::wcout << "######################################################\n";
		//	std::wcout << "VDP AdvanceProcessorStateNew stalled!\n";
		//	std::wcout << "mclkCyclesTarget:\t" << mclkCyclesTarget << "\n";
		//	std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
		//	std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
		//	std::wcout << "totalPixelClockCyclesToExecute:\t" << totalPixelClockCyclesToExecute << "\n";
		//	std::wcout << "mclkRemainingCycles:\t" << mclkRemainingCycles << "\n";
		//	std::wcout << "stateLastUpdateMclkUnusedFromLastTimeslice:\t" << stateLastUpdateMclkUnusedFromLastTimeslice << "\n";
		//	std::wcout << "mclkCyclesToAdvanceThisStep:\t" << mclkCyclesToAdvanceThisStep << "\n";
		//	std::wcout << "mclkCyclesAdvanced:\t" << mclkCyclesAdvanced << "\n";
		//	std::wcout << "hcounter:\t" << hcounter.GetData() << "\n";
		//	std::wcout << "vcounter:\t" << vcounter.GetData() << "\n";
		//	std::wcout << "######################################################\n";
		//}

		//Advance the required number of pixel clock steps to reach the target
		unsigned int hcounterBeforeAdvance = hcounter.GetData();
		unsigned int vcounterBeforeAdvance = vcounter.GetData();
		AdvanceStatusRegisterAndHVCounterWithCurrentSettings(totalPixelClockCyclesToExecute);

		//Update the number of remaining mclk cycles from the previous timeslice
		if((stateLastUpdateMclkUnused + mclkCyclesToAdvanceThisStep) > stateLastUpdateMclkUnusedFromLastTimeslice)
		{
			stateLastUpdateMclkUnusedFromLastTimeslice = 0;
		}
		else
		{
			stateLastUpdateMclkUnusedFromLastTimeslice -= (stateLastUpdateMclkUnused + mclkCyclesToAdvanceThisStep);
		}

		//Update the current state mclk cycle counter and remaining mclk cycles
		stateLastUpdateMclk += ((stateLastUpdateMclkUnused + mclkCyclesToAdvanceThisStep) - mclkRemainingCycles);
		stateLastUpdateMclkUnused = mclkRemainingCycles;

		//Update the index of the last value to be read from VSRAM by the render process,
		//based on the HV counter position before and after this advance step, and the
		//current vertical scroll mode. Note that if we weren't in the active scan region
		//as defined by the vertical counter, the current vsram read cache index will
		//remain unchanged.
		vsramReadCacheIndex = GetUpdatedVSRAMReadCacheIndex(hscanSettings, vscanSettings, vsramReadCacheIndex, hcounterBeforeAdvance, vcounterBeforeAdvance, hcounter.GetData(), vcounter.GetData(), screenModeRS0, screenModeRS1, verticalScrollModeCached);

		//If vertical scan information has changed, and we've just advanced to vblank,
		//latch the new screen mode settings.
		if(screenModeSettingsChangeRequired && updateStepIsVBlank)
		{
			//If the interlace mode has changed, the new setting is latched when the
			//vblank set event occurs. This has been verified in all video modes through
			//hardware tests.
			interlaceEnabled = interlaceEnabledNew;
			interlaceDouble = interlaceDoubleNew;
			//##TODO## Verify that changes to the PAL line state are latched at vblank
			palMode = palModeLineState;
			//##TODO## Verify that the V28/V30 mode change is latched at vblank
			screenModeV30 = screenModeV30New;

			//Now that we've processed this screen mode settings change, flag that no
			//settings change is required.
			vscanSettingsChanged = false;
		}

		//If horizontal scan information has changed, and we've just advanced to hblank,
		//latch the new screen mode settings.
		if(screenModeSettingsChangeRequired && updateStepIsHBlank)
		{
			//##FIX## These settings changes are supposed to take effect immediately
			screenModeRS0 = screenModeRS0New;
			screenModeRS1 = screenModeRS1New;

			//Now that we've processed this screen mode settings change, flag that no
			//settings change is required.
			hscanSettingsChanged = false;
		}

		//Update the total number of mclk cycles advanced so far
		mclkCyclesAdvanced += mclkCyclesToAdvanceThisStep;
	}

	//Calculate the new processor state time. Note that we don't have to do this within
	//the loop above, as internally, this core always tracks the current time in mclk
	//cycles. We only need to calculate the current processor time as a timeslice progress
	//value for external interaction.
	stateLastUpdateTime = ConvertMclkCountToAccessTime(GetProcessorStateMclkCurrent());

	//If we stopped at an access slot rather than running until the specified time was
	//reached, return false.
	return !stoppedAtAccessSlot;
}

//----------------------------------------------------------------------------------------
//##TODO## Remove this old function
//void S315_5313::AdvanceStatusRegisterAndHVCounter(unsigned int pixelClockSteps)
//{
//	//Grab the latest settings for any registers which affect the video mode. Note that
//	//we only advance the VDP state between register writes, so we know the currently
//	//latched video mode settings are valid, and that any video mode register changes
//	//which have been made need to be applied the next time those settings are latched.
//	bool interlaceEnabledNew = interlaceEnabledCached;
//	bool interlaceDoubleNew = interlaceDoubleCached;
//	bool screenModeH40New = screenModeH40Cached;
//	bool screenModeV30New = screenModeV30Cached;
//
//	//Check whether any of the relevant video mode settings have changed since they were
//	//latched.
//	bool hscanSettingsChanged = (screenModeH40 != screenModeH40New);
//	//##TODO## Currently, changes to the palMode flag are applied at vblank. Test how
//	//the real hardware deals with changes to this line by toggling the line state at
//	//runtime.
//	bool vscanSettingsChanged = (screenModeV30 != screenModeV30New) || (palMode != palModeLineState) || (interlaceEnabled != interlaceEnabledNew);
//
//	//Obtain the current hscan and vscan settings
//	const HScanSettings* hscanSettings;
//	const VScanSettings* vscanSettings;
//	hscanSettings = &GetHScanSettings(screenModeH40);
//	vscanSettings = &GetVScanSettings(screenModeV30, palMode, interlaceEnabled);
//
//	//Calculate the number of steps before the hblank and vblank events occur, and new
//	//settings are latched.
//	unsigned int pixelClockStepsBeforeHBlank;
//	unsigned int pixelClockStepsBeforeVBlank;
//	pixelClockStepsBeforeHBlank = GetPixelClockStepsBetweenHCounterValues(*hscanSettings, hcounter.GetData(), hscanSettings->hblankSetPoint);
//	pixelClockStepsBeforeVBlank = GetPixelClockStepsBetweenVCounterValues(*hscanSettings, hcounter.GetData(), *vscanSettings, interlaceEnabled, GetStatusFlagOddInterlaceFrame(), vcounter.GetData(), vscanSettings->vblankSetPoint);
//
//	//Check if the vblank event is going to occur before the hblank event. We need to know
//	//which one comes first so we know which one to advance to first, if required.
//	bool vblankBeforeHBlank = pixelClockStepsBeforeVBlank < pixelClockStepsBeforeHBlank;
//
//	//Set the current number of pixel clock steps we still need to advance
//	unsigned int pixelClockStepsRemaining = pixelClockSteps;
//
//	//If vertical scan information has changed, and vblank is going to occur before
//	//hblank, we need to advance to the latch point at the start of vblank.
//	if(vscanSettingsChanged && vblankBeforeHBlank && (pixelClockStepsRemaining <= pixelClockStepsBeforeVBlank))
//	{
//		//Advance the required number of pixel clock steps to reach the vblank event
//		AdvanceStatusRegisterAndHVCounterWithCurrentSettings(pixelClockStepsBeforeVBlank);
//		pixelClockStepsRemaining -= pixelClockStepsBeforeVBlank;
//		//If the interlace mode has changed, the new setting is latched when the vblank set
//		//event occurs. This has been verified in all video modes through hardware tests.
//		interlaceEnabled = interlaceEnabledNew;
//		interlaceDouble = interlaceDoubleNew;
//		//##TODO## Verify that changes to the PAL line state are latched at vblank
//		palMode = palModeLineState;
//		//##TODO## Verify that the V28/V30 mode change is latched at vblank
//		screenModeV30 = screenModeV30New;
//	}
//
//	//If the horizontal cell width has changed, we need to advance to the latch point at
//	//the start of hblank.
//	if(hscanSettingsChanged && (pixelClockStepsRemaining <= pixelClockStepsBeforeHBlank))
//	{
//		//Advance the required number of pixel clock steps to reach the hblank event
//		AdvanceStatusRegisterAndHVCounterWithCurrentSettings(pixelClockStepsBeforeHBlank);
//		pixelClockStepsRemaining -= pixelClockStepsBeforeHBlank;
//		//##TODO## Verify that H32/H40 screen mode changes are processed at hblank
//		screenModeH40 = screenModeH40New;
//
//		//Update the latched hscan settings
//		hscanSettings = &GetHScanSettings(screenModeH40);
//
//		//Recalculate pixelClockStepsBeforeVBlank
//		pixelClockStepsBeforeVBlank = GetPixelClockStepsBetweenVCounterValues(*hscanSettings, hscanSettings->hblankSetPoint, *vscanSettings, interlaceEnabled, GetStatusFlagOddInterlaceFrame(), vcounter.GetData(), vscanSettings->vblankSetPoint);
//	}
//
//	//If vertical scan information has changed, and vblank is going to occur after hblank,
//	//we need to advance to the latch point at the start of vblank.
//	if(vscanSettingsChanged && !vblankBeforeHBlank && (pixelClockStepsRemaining <= pixelClockStepsBeforeVBlank))
//	{
//		//Advance the required number of pixel clock steps to reach the vblank event
//		AdvanceStatusRegisterAndHVCounterWithCurrentSettings(pixelClockStepsBeforeVBlank);
//		pixelClockStepsRemaining -= pixelClockStepsBeforeVBlank;
//		//If the interlace mode has changed, the new setting is latched when the vblank set
//		//event occurs. This has been verified in all video modes through hardware tests.
//		interlaceEnabled = interlaceEnabledNew;
//		interlaceDouble = interlaceDoubleNew;
//		//##TODO## Verify that changes to the PAL line state are latched at vblank
//		palMode = palModeLineState;
//		//##TODO## Verify that the V28/V30 mode change is latched at vblank
//		screenModeV30 = screenModeV30New;
//	}
//
//	//Since all external video mode settings changes which can make the progression of the
//	//HV counter change over time have already been processed above, we can now simply
//	//jump the HV counter to its final position using the current screen mode settings,
//	//and the remaining number of pixel clock update steps.
//	AdvanceStatusRegisterAndHVCounterWithCurrentSettings(pixelClockStepsRemaining);
//}

//----------------------------------------------------------------------------------------
void S315_5313::AdvanceStatusRegisterAndHVCounterWithCurrentSettings(unsigned int pixelClockSteps)
{
	//Obtain the current hscan and vscan settings
	const HScanSettings* hscanSettings = &GetHScanSettings(screenModeRS0, screenModeRS1);
	const VScanSettings* vscanSettings = &GetVScanSettings(screenModeV30, palMode, interlaceEnabled);

	//Advance the HV counter
	unsigned int hcounterCurrent = hcounter.GetData();
	unsigned int vcounterCurrent = vcounter.GetData();
	bool oddFlagSet = GetStatusFlagOddInterlaceFrame();
	AdvanceHVCounters(*hscanSettings, hcounterCurrent, *vscanSettings, interlaceEnabled, oddFlagSet, vcounterCurrent, pixelClockSteps);

	//Save the new values for the HV counter
	hcounter = hcounterCurrent;
	vcounter = vcounterCurrent;

	//##TODO## Update the cached read data for the VSRAM buffer

	//Update the status register

	//Note that the FIFO flags are set by the routine which manages the FIFO.

	//We set the F flag when a vint is generated, and clear it when the interrupt is
	//acknowledged.

	//Note that the sprite overflow and collision flags are cleared when the status
	//register is read.
	//##NOTE## As soon as we come across a case where the sprite overflow or collision
	//flags are set, we can stop searching for more. We know the flag will remain set for
	//the rest of the update cycle.
	//##TODO## Update the sprite overflow flag
	//##TODO## Confirm whether the sprite overflow flag is set just for a sprite overflow,
	//or for a dot overflow as well.
	if(!GetStatusFlagSpriteOverflow())
	{
		//##STUB##
		SetStatusFlagSpriteOverflow(false);
	}

	//##TODO## Update the sprite collision flag
	//##TODO## Verify sprite collision behaviour on the hardware. Test opaque pixels
	//overlapping. Test sprites overlapping off the screen, both partially visible and
	//completely hidden. Determine whether this flag is set during active scan at the
	//pixel location where the overlap occurs, or whether it is set during blanking when
	//the sprites are parsed. Check how sprite overflows and sprite masking interacts with
	//the sprite collision flag.
	//##TODO## A comment by Eke in http://gendev.spritesmind.net/forum/viewtopic.php?t=778
	//says that the sprite collision flag is also cleared during vblank, not just when the
	//status register is read. Most likely, this applies to the sprite overflow flag as
	//well. Perform some hardware tests to confirm the exact time at which these flags are
	//set and cleared.
	//##TODO## Some testing on VRAM access timing shows that the sprite collision flag
	//seems to be actively set during the sprite pattern reads in hblank, so it seems that
	//this flag is set as each sprite pattern block is read. Most likely, the sprite
	//overflow flag is set while performing sprite mapping reads, if there are still
	//blocks remaining when the last sprite mapping read slot is used on a scanline.
	if(!GetStatusFlagSpriteCollision())
	{
		//##STUB##
		SetStatusFlagSpriteCollision(false);
	}

	//Save the new value for the ODD flag
	SetStatusFlagOddInterlaceFrame(oddFlagSet);

	//Update the vblank and hblank flags
	bool vblankFlag = (vcounter >= vscanSettings->vblankSetPoint) && (vcounter < vscanSettings->vblankClearedPoint);
	bool hblankFlag = (hcounter >= hscanSettings->hblankSetPoint) || (hcounter < hscanSettings->hblankClearedPoint);
	SetStatusFlagVBlank(vblankFlag);
	SetStatusFlagHBlank(hblankFlag);

	//##DEBUG##
//	std::wcout << "Advance:\t" << pixelClockSteps << '\n';
//	std::wcout << "Hcounter:\t" << hcounterCurrent << '\t' << "Vcounter:\t" << vcounterCurrent << '\n';
//	std::wcout << "Hblank:\t" << hblankFlag << '\t' << "Vblank:\t" << vblankFlag << '\n';

	//Note that the DMA flag is set and cleared by the routine which manages DMA
	//operations.

	//Update the PAL flag
	SetStatusFlagPAL(palMode);
}

//----------------------------------------------------------------------------------------
double S315_5313::GetProcessorStateTime() const
{
	return stateLastUpdateTime;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetProcessorStateMclkCurrent() const
{
	return stateLastUpdateMclk + stateLastUpdateMclkUnused;
}

//----------------------------------------------------------------------------------------
//FIFO functions
//----------------------------------------------------------------------------------------
bool S315_5313::IsWriteFIFOEmpty() const
{
	//The FIFO is empty if the next FIFO entry for read doesn't have a write pending
	return !fifoBuffer[fifoNextReadEntry].pendingDataWrite;
}

//----------------------------------------------------------------------------------------
bool S315_5313::IsWriteFIFOFull() const
{
	//The FIFO is full if the first and last FIFO entries for reading both have a write
	//pending
	unsigned int fifoLastReadEntry = (fifoNextReadEntry+(fifoBufferSize-1)) % fifoBufferSize;
	return fifoBuffer[fifoNextReadEntry].pendingDataWrite && fifoBuffer[fifoLastReadEntry].pendingDataWrite;
}

//----------------------------------------------------------------------------------------
//Mode 5 control functions
//----------------------------------------------------------------------------------------
bool S315_5313::ValidReadTargetInCommandCode() const
{
	//Return true if bits 0, 1, and 4 of the commandCode register are not set. See the
	//mapping of commandCode data to valid read targets in the ReadVideoMemory function to
	//see why this is a valid test.
	return (commandCode & 0x13) == 0;
}

////----------------------------------------------------------------------------------------
//bool S315_5313::ReadVideoMemory(unsigned int mclkCycle, Data& data)
//{
//	RAMAccessTarget ramAccessTarget;
//	ramAccessTarget.AccessTime(mclkCycle);
//
//	//All possible combinations of the code flags and data port reads have been tested on
//	//the hardware. Reads are decoded based on the lower 5 bits of the code data.
//	switch(commandCode.GetDataSegment(0, 5))
//	{
//	case 0x00:	//?00000 VRAM Read
//		M5ReadVRAM(commandAddress, data, ramAccessTarget);
//		break;
//	case 0x04:	//?00100 VSRAM Read
//		M5ReadVSRAM(commandAddress, data, ramAccessTarget);
//		break;
//	case 0x08:	//?01000 CRAM Read
//		M5ReadCRAM(commandAddress, data, ramAccessTarget);
//		break;
//	case 0x0C:	//?01100 8-bit VRAM Read (undocumented)
//		//This undocumented read mode performs a true 8-bit VRAM read. The lower 8 bits
//		//return an 8-bit value read from VRAM, while the upper 8 bits are unaffected.
//		M5ReadVRAM8Bit(commandAddress, data, ramAccessTarget);
//		break;
//	default:	//Invalid
//		//Any attempts to read from the data port when the lower five bits don't match one
//		//of the above patterns causes the VDP to lock up. A reset is unable to restore
//		//normal operation. Only power cycling the device can bring the VDP back from this
//		//state.
//		//##TODO## Raise some kind of hard error when this occurs.
//		return false;
//	}
//
//	//We always increment the command address, regardless of whether the target was valid
//	//or not. This is probably the way the real device would work, but since incremented
//	//addresses are temporary, and are reloaded from the original address register
//	//whenever a command word is written, there's no way to know if this behaviour is
//	//correct. For the same reason, it also doesn't matter, since there's no possible
//	//impact from either incrementing or not incrementing on invalid targets, but this way
//	//is simpler and produces less code.
////	commandAddress += autoIncrementData;
//
//	return true;
//}

////----------------------------------------------------------------------------------------
//void S315_5313::WriteVideoMemory(unsigned int mclkCycle, const Data& data)
//{
//	RAMAccessTarget ramAccessTarget;
//	ramAccessTarget.AccessTime(mclkCycle);
//
//	//All possible combinations of the code flags and data port writes have been tested
//	//on the hardware. Writes are decoded based on the lower 4 bits of the code data.
//	switch(commandCode.GetDataSegment(0, 4))
//	{
//	case 0x01:	//??0001 VRAM Write
//		M5WriteVRAM(commandAddress, data, ramAccessTarget);
//		break;
//	case 0x03:	//??0011 CRAM Write
//		M5WriteCRAM(commandAddress, data, ramAccessTarget);
//		break;
//	case 0x05:	//??0101 VSRAM Write
//		M5WriteVSRAM(commandAddress, data, ramAccessTarget);
//		break;
//	default:	//Invalid
//		//Any attempts to write to the data port when the lower four bits don't match
//		//one of the above patterns has no effect. The write is discarded, and the
//		//VDP is unaffected.
//		break;
//	}
//
//	//We always increment the command address, regardless of whether the target was valid
//	//or not. This is probably the way the real device would work, but since incremented
//	//addresses are temporary, and are reloaded from the original address register
//	//whenever a command word is written, there's no way to know if this behaviour is
//	//correct. For the same reason, it also doesn't matter, since there's no possible
//	//impact from either incrementing or not incrementing on invalid targets, but this way
//	//is simpler and produces less code.
//	commandAddress += autoIncrementData;
//}

//----------------------------------------------------------------------------------------
//void S315_5313::M5ReadVRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget)
//{
//	//Note that hardware tests have shown that the LSB of the address is ignored for
//	//16-bit reads from VRAM.
//	Data tempAddress(address);
//	tempAddress.SetBit(0, false);
//
//	//Read the data. The full 16-bit data value is retrieved from VRAM.
//	data.SetByte(1, vram->BufferRead(tempAddress.GetData(), accessTarget));
//	data.SetByte(0, vram->BufferRead(tempAddress.GetData()+1, accessTarget));
//}

//----------------------------------------------------------------------------------------
void S315_5313::M5ReadVRAM8Bit(const Data& address, Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that in the case of 8-bit VRAM reads, the data is directly read from VRAM at
	//the literal address referenced by the address register. Note however that since the
	//actual VDP stores data in VRAM as byteswapped 16-bit values, this means that reads
	//from odd addresses return the even byte of a 16-bit value, and reads from an even
	//address return the odd byte. Since we don't byteswap the VRAM data in our emulation
	//core, we need to invert the LSB of the address here to get the same result as the
	//real hardware.
	Data tempAddress(address);
	tempAddress.SetBit(0, !tempAddress.GetBit(0));

	//Read the data. Only a single 8-bit read is performed from VRAM in this case. The
	//upper 8 bits retain their previous value.
	//##TODO## Snoop on the VRAM bus to confirm only a single byte is read for this target
	data.SetByte(0, vram->Read(tempAddress.GetData(), accessTarget));
}

//----------------------------------------------------------------------------------------
void S315_5313::M5ReadCRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that hardware tests have shown that not only is the LSB of the address ignored
	//for both reads and writes to CRAM and VSRAM, but the upper 9 bits of the address is
	//also masked. The address value wraps to be within the range 0x00-0x7E, where the LSB
	//of the address is always clear.
	unsigned int tempAddress = address.GetData() & 0x7E;

	//Read the data. Note that not all bits in the 16-bit result are affected by the read
	//from CRAM. Hardware tests have shown that CRAM reads return a 9-bit value, with the
	//mask 0x0EEE. The remaining bits retain their previous value, corresponding with the
	//existing value in the FIFO buffer that the read data is being saved into.
	unsigned int dataMask = 0x0EEE;
	Data tempData(16);
	tempData.SetByte(1, cram->Read(tempAddress, accessTarget));
	tempData.SetByte(0, cram->Read(tempAddress+1, accessTarget));
	data = (data & ~dataMask) | (tempData & dataMask);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5ReadVSRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that hardware tests have shown that not only is the LSB of the address ignored
	//for both reads and writes to CRAM and VSRAM, but the upper 9 bits of the address is
	//also masked. The address value wraps to be within the range 0x00-0x7E, where the LSB
	//of the address is always clear.
	unsigned int tempAddress = address.GetData() & 0x7E;

	//##TODO## Hardware testing has shown that the VSRAM buffer is 0x50 bytes in size,
	//while the target address into the VSRAM buffer wraps at 0x80 bytes. This leaves 0x30
	//bytes where reads and writes can be attempted, which are beyond the end of the VSRAM
	//buffer. Hardware testing has shown that that the last read value from VSRAM is
	//cached in an internal register, and attempts to read from VSRAM at an invalid
	//address simply return the cached value which remains in the VSRAM read buffer. Note
	//that the actual VDP rendering process itself reads values from VSRAM, and these
	//reads update the VSRAM read buffer.
	//##TODO## Perform hardware tests to confirm whether manual reads from VSRAM update
	//the VSRAM read buffer, or whether it is only populated by the VDP itself during
	//rendering. Also confirm whether writes to VSRAM update the VSRAM read buffer, IE,
	//whether there is a single buffer which is used for both reads and writes. Note that
	//we should be able to determine this by disabling VDP video output, possibly through
	//the normal display enable/disable bit in register 1, or if not, perhaps through the
	//undocumented bit in reg 0 which disables all video output. This should get the VDP
	//out of the way, so we can read VSRAM without the rendering process affecting the
	//VSRAM read buffer.
	unsigned int dataMask = 0x07FF;
	if(tempAddress >= 0x50)
	{
		tempAddress = vsramReadCacheIndex % 0x50;
	}

	//Read the data. Note that not all bits in the 16-bit result are affected by the
	//read from VSRAM. Hardware tests have shown that VSRAM reads return an 11-bit
	//value, with the mask 0x07FF. The remaining bits retain their previous value,
	//corresponding with the existing value in the FIFO buffer that the read data is
	//being saved into.
	Data tempData(16);
	tempData.SetByte(1, vsram->Read(tempAddress, accessTarget));
	tempData.SetByte(0, vsram->Read(tempAddress+1, accessTarget));
	data = (data & ~dataMask) | (tempData & dataMask);

	//##TODO## Determine whether this is correct
	vsramReadCacheIndex = tempAddress;
}

//----------------------------------------------------------------------------------------
//This target isn't directly accessible as a write target for the VDP, but we use this
//function to implement the two 8-bit halves of a 16-bit VRAM write performed for the
//16-bit VRAM write target, as well as implement DMA fill and copy functionality.
//----------------------------------------------------------------------------------------
void S315_5313::M5WriteVRAM8Bit(const Data& address, const Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that in the case of 8-bit VRAM writes, the data is directly written to VRAM at
	//the literal address referenced by the address register. Note however that since the
	//actual VDP stores data in VRAM as byteswapped 16-bit values, this means that writes
	//to odd addresses modify the even byte of a 16-bit value, and writes to an even
	//address modify the odd byte. Since we don't byteswap the VRAM data in our emulation
	//core, we need to invert the LSB of the address here to get the same result as the
	//real hardware.
	Data tempAddress(address);
	tempAddress.SetBit(0, !tempAddress.GetBit(0));
	unsigned int tempAddressData = tempAddress.GetData();

	//Update the sprite cache if required. The sprite cache is an internal memory buffer
	//which is designed to maintain a mirror of a portion of the sprite attribute table.
	//The first 4 bytes of each 8-byte table entry are stored in the cache. Since the
	//sprite cache is not reloaded when the sprite attribute table address is changed,
	//correct emulation of the cache is required in order to correctly emulate VDP sprite
	//support. Level 6-3 of "Castlevania Bloodlines" on the Mega Drive is known to rely on
	//the sprite cache not being invalidated by a table address change, in order to
	//implement an "upside down" effect.
	if((tempAddressData >= spriteAttributeTableBaseAddressDecoded) //Target address is at or above the start of the sprite table
	&& (tempAddressData < (spriteAttributeTableBaseAddressDecoded + (spriteCacheSize * 2))) //Target address is before the end of the sprite table
	&& ((tempAddressData & 0x4) == 0)) //Target address is within the first 4 bytes of a sprite table entry
	{
		//Calculate the address of this write in the sprite cache. We first convert the
		//target address into a relative byte index into the sprite attribute table, then
		//we strip out bit 2 from the address, to discard addresses in the upper 4 bytes
		//of each table entry, which we filtered out above.
		unsigned int spriteCacheAddress = (tempAddressData - spriteAttributeTableBaseAddressDecoded);
		spriteCacheAddress = ((spriteCacheAddress >> 1) & ~0x3) | (spriteCacheAddress & 0x3);

		//Perform the write to the sprite cache
		spriteCache->Write(spriteCacheAddress, data.GetByte(0), accessTarget);
	}

	//Write the data
	vram->Write(tempAddressData, data.GetByte(0), accessTarget);
}

////----------------------------------------------------------------------------------------
//void S315_5313::M5WriteVRAM(const Data& address, const Data& data, const RAMAccessTarget& accessTarget)
//{
//	//-Hardware tests have verified that if the LSB of the address is set, it is ignored
//	//when determining the target VRAM address, but it acts as a special flag causing the
//	//data to be byteswapped for writes to VRAM. This is true for any write to VRAM,
//	//including writes performed as part of a DMA transfer. The LSB of the address is
//	//ignored for reads from VRAM, IE, no byteswapping is ever performed on reads.
//	//-It should be noted that the real VDP actually stores all VRAM data byteswapped in
//	//the physical memory. This has been confirmed by using a logic analyzer to snoop on
//	//the VRAM bus during operation. This means that in reality, byteswapping on VRAM
//	//writes really occurs when the LSB is unset, while the data is byteswapped if the LSB
//	//is set, and all reads are byteswapped. We don't byteswap the actual VRAM data in our
//	//emulator, as not only is this byteswapping virtually transparent to the caller
//	//except in the case of DMA fills and copies (refer to the implementation for further
//	//info), it would be slower for us to byteswap everything on every read and write to
//	//VRAM. Since it's faster for us, and more convenient and logical for the user
//	//therefore for the data to be stored sequentially, we don't store data as byteswapped
//	//in VRAM.
//	//-Note that the real VDP also stores the VRAM data in a non-linear fashion, with data
//	//within each 0x400 byte block stored in an interleaved format. The byteswapped data
//	//is striped in 4-byte groups within each 0x400 byte block, with all the first bytes
//	//of each 4-byte set at 0x000-0x100, then the second bytes at 0x100-0x200, and so on
//	//within each 0x400 byte block. This is necessary in order to support the serial
//	//access mode used to read data from VRAM. We also don't implement this interleaved
//	//VRAM in our emulator, as it is an implementation detail that has no effect on the
//	//logical operation of the VDP.
//	Data tempAddress(address);
//	if(tempAddress.GetBit(0))
//	{
//		tempAddress.SetBit(0, false);
//		vram->BufferWrite(tempAddress.GetData(), data.GetByte(0), accessTarget);
//		vram->BufferWrite(tempAddress.GetData()+1, data.GetByte(1), accessTarget);
//	}
//	else
//	{
//		vram->BufferWrite(tempAddress.GetData(), data.GetByte(1), accessTarget);
//		vram->BufferWrite(tempAddress.GetData()+1, data.GetByte(0), accessTarget);
//	}
//}

//----------------------------------------------------------------------------------------
void S315_5313::M5WriteCRAM(const Data& address, const Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that hardware tests have shown that not only is the LSB of the address ignored
	//for both reads and writes to CRAM and VSRAM, but the upper 9 bits of the address is
	//also masked. The address value wraps to be within the range 0x00-0x7E, where the LSB
	//of the address is always clear.
	unsigned int tempAddress = address.GetData() & 0x7E;

	//Build a masked data value to write to CRAM. Hardware tests have shown that reads
	//back from CRAM only return data from bits corresponding with the mask 0x0EEE. We
	//mask the data when saving to CRAM here, to make things more sensible when using the
	//debugger. Since the masked bits are almost certainly discarded when saving to CRAM
	//on the real system, we discard them here on save as well, even though it's not
	//technically necessary, since we never attempt to use the invalid bits.
	unsigned int dataMask = 0x0EEE;
	Data tempData(16);
	tempData = data & dataMask;

	//Write the masked data to CRAM
	cram->Write(tempAddress, tempData.GetByte(1), accessTarget);
	cram->Write(tempAddress+1, tempData.GetByte(0), accessTarget);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5WriteVSRAM(const Data& address, const Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that hardware tests have shown that not only is the LSB of the address ignored
	//for both reads and writes to CRAM and VSRAM, but the upper 9 bits of the address is
	//also masked. The address value wraps to be within the range 0x00-0x7E, where the LSB
	//of the address is always clear.
	unsigned int tempAddress = address.GetData() & 0x7E;

	//Build a masked data value to write to VSRAM. Hardware tests have shown that reads
	//back from VSRAM only return data from bits corresponding with the mask 0x07FF. We
	//mask the data when saving to VSRAM here, to make things more sensible when using the
	//debugger. Since the masked bits are almost certainly discarded when saving to VSRAM
	//on the real system, we discard them here on save as well, even though it's not
	//technically necessary, since we never attempt to use the invalid bits.
	unsigned int dataMask = 0x07FF;
	Data tempData(16);
	tempData = data & dataMask;

	//##TODO## Comment this, as per the M5ReadVSRAM function, and perform hardware tests
	//to confirm whether VSRAM writes should update the VSRAM read buffer.
	if(tempAddress < 0x50)
	{
		//Write the masked data to VSRAM
		vsram->Write(tempAddress, tempData.GetByte(1), accessTarget);
		vsram->Write(tempAddress+1, tempData.GetByte(0), accessTarget);

		//##TODO## Determine whether this is correct
		vsramReadCacheIndex = tempAddress;
	}
}
