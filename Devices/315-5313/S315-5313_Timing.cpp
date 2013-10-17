#include "S315_5313.h"

//----------------------------------------------------------------------------------------
//Horizontal scan timing settings
//----------------------------------------------------------------------------------------
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
//|Front porch     |0x126-0x127|   9 |SCLK/2 |   18 |MCLK/5 |  90 |
//|(Right Blanking)|0x1D2-0x1D8|     |       |      |       |     |
//|----------------|-----------|-----|-------|------|-------|-----|
//|Horizontal sync |0x1D9-0x1F2|  26 |SCLK/2 |   52 |MCLK/5 | 260 |
//|----------------|-----------|-----|-------|------|-------|-----|
//|Back porch      |0x1F3-0x00A|  24 |SCLK/2 |   48 |MCLK/5 | 240 |
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
//|Front porch     |0x168-0x16C    |   9 |EDCLK/2|  18 |MCLK/4 |  72 |
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
//|Back porch      |0x1ED          |   1 |EDCLK/2|   2 |MCLK/5 |  10 |
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
//|VCounter      |HCounter changes |HCounter changes |
//|increment     |from 0x109 to    |from 0x149 to    |
//|              |0x10A in [1].    |0x14A in [1].    |
//|--------------------------------------------------| //Logic analyzer tests conducted on 2012-11-03 confirm 18 SC
//|HBlank set    |HCounter changes |HCounter changes | //cycles between HBlank set in status register and HSYNC
//|              |from 0x125 to    |from 0x165 to    | //asserted in H32 mode, and 21 SC cycles in H40 mode.
//|              |0x126 in [1].    |0x166 in [1].    | //Note this actually means in H40 mode, HBlank is set at 0x166.5.
//|--------------------------------------------------| //Logic analyzer tests conducted on 2012-11-03 confirm 46 SC
//|HBlank cleared|HCounter changes |HCounter changes | //cycles between HSYNC cleared and HBlank cleared in status
//|              |from 0x009 to    |from 0x00A to    | //register in H32 mode, and 61 SC cycles in H40 mode.
//|              |0x00A in [1].    |0x00B in [1].    | //Note this actually means in H40 mode, HBlank is cleared at 0x00B.5.
//|--------------------------------------------------|
//|F flag set    |HCounter changes |HCounter changes | //Logic analyzer tests conducted on 2012-11-03 confirm 28 SC
//|              |from 0x000 to    |from 0x000 to    | //cycles between HSYNC cleared and f flag set in status
//|              |0x001 in [1]     |0x001 in [1]     | //register in H32 mode, and 40 SC cycles in H40 mode.
//|--------------------------------------------------|
//|ODD flag      |HCounter changes |HCounter changes | //Logic analyzer tests conducted on 2012-11-03 confirm 30 SC
//|toggled       |from 0x001 to    |from 0x001 to    | //cycles between HSYNC cleared and odd flag toggled in status
//|              |0x002 in [1]     |0x002 in [1]     | //register in H32 mode, and 42 SC cycles in H40 mode.
//|--------------------------------------------------|
//|HINT flagged  |HCounter changes |HCounter changes | //Logic analyzer tests conducted on 2012-11-02 confirm 74 SC
//|via IPL lines |from 0x109 to    |from 0x149 to    | //cycles between HINT flagged in IPL lines and HSYNC
//|              |0x10A in [1].    |0x14A in [1].    | //asserted in H32 mode, and 78 SC cycles in H40 mode.
//|--------------------------------------------------|
//|VINT flagged  |HCounter changes |HCounter changes | //Logic analyzer tests conducted on 2012-11-02 confirm 28 SC
//|via IPL lines |from 0x000 to    |from 0x000 to    | //cycles between HSYNC cleared and VINT flagged in IPL lines
//|              |0x001 in [1].    |0x001 in [1].    | //in H32 mode, and 40 SC cycles in H40 mode.
//|--------------------------------------------------|
//|HSYNC asserted|HCounter changes |HCounter changes |
//|              |from 0x1D8 to    |from 0x1CC to    |
//|              |0x1D9 in [2].    |0x1CD in [2].    |
//|--------------------------------------------------|
//|HSYNC negated |HCounter changes |HCounter changes |
//|              |from 0x1F2 to    |from 0x1EC to    |
//|              |0x1F3 in [2].    |0x1ED in [2].    |
//----------------------------------------------------
//##TODO##
//-There are 40 SC cycles from HSYNC negated to INT asserted in H40 mode
//-There are 28 SC cycles from HSYNC negated to INT asserted in H32 mode
//-There are 91 SC cycles from INT negated to HSYNC asserted in H40 mode
//-There are 87 SC cycles from INT negated to HSYNC asserted in H32 mode
//##TODO## Hardware tests confirm that the INT line remains asserted, even after the Z80
//runs an interrupt acknowledge cycle.
const S315_5313::HScanSettings S315_5313::h32ScanSettingsStatic(0x127, 0x1D2, 0x10A, 0x126, 0x00A, 0x001, 0x002, 0x1FF, 0x001, 0x1D9, 0x1F3, 0x018, 0x117, 256, 0x00B, 0x017, 13, 0x118, 0x125, 14, 0x1F3, 0x00A, 24, 0x126, 0x1D8, 9);
const S315_5313::HScanSettings S315_5313::h40ScanSettingsStatic(0x16C, 0x1C9, 0x14A, 0x166, 0x00B, 0x001, 0x002, 0x1FF, 0x001, 0x1CD, 0x1ED, 0x01A, 0x159, 320, 0x00D, 0x019, 13, 0x15A, 0x167, 14, 0x1ED, 0x00C, 32, 0x168, 0x1CC, 9);

//----------------------------------------------------------------------------------------
//Vertical scan timing settings
//----------------------------------------------------------------------------------------
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
//*When V30 mode and NTSC mode are both active, no border, blanking, or retrace occurs. A
//30-row display is setup and rendered, however, immediately following the end of the 30th
//row, the 1st row starts again. In addition, the VCounter is never reset, which usually
//happens at the beginning of vertical blanking. Instead, the VCounter continuously counts
//from 0x000-0x1FF, then wraps around back to 0x000 and begins again. Since there are only
//240 lines output as part of the display, this means the actual line being rendered is
//desynchronized from the VCounter. Digital events such as vblank flags being set/cleared,
//VInt being triggered, the odd flag being toggled, and so forth, still occur at the
//correct VCounter positions they would occur in (IE, the same as PAL mode V30), however,
//since the VCounter has 512 lines per cycle, this means VInt is triggered at a slower
//rate than normal.
//##TODO## Confirm on the hardware that the rendering row is desynchronized from the
//VCounter. This would seem unlikely, since a separate render line counter would have to
//be maintained apart from VCounter for this to occur.

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
//|VBlank set    |VCounter changes |                 |VCounter changes |                 |
//|              |from 0x0DF to    |     <Same>      |from 0x0EF to    |     <Same>      |
//|              |0x0E0 in [1].    |                 |0x0F0 in [1].    |                 |
//|--------------------------------------------------------------------------------------|
//|VBlank cleared|VCounter changes |                 |VCounter changes |                 |
//|              |from 0x1FE to    |     <Same>      |from 0x1FE to    |     <Same>      |
//|              |0x1FF in [2].    |                 |0x1FF in [1].    |                 |
//|--------------------------------------------------------------------------------------|
//|F flag set    |At indicated     |                 |At indicated     |                 |
//|              |HCounter position|                 |HCounter position|                 |
//|              |while VCounter is|     <Same>      |while VCounter is|     <Same>      |
//|              |set to 0x0E0 in  |                 |set to 0x0F0 in  |                 |
//|              |[1].             |                 |[1].             |                 |
//|--------------------------------------------------------------------------------------|
//|VSYNC asserted|VCounter changes |                 |      Never      |                 |
//|              |from 0x0E7 to    |     <Same>      |                 |     <Same>      |
//|              |0x0E8 in [1].    |                 |                 |                 |
//|--------------------------------------------------------------------------------------|
//|VSYNC cleared |VCounter changes |                 |      Never      |                 |
//|              |from 0x1F4 to    |     <Same>      |                 |     <Same>      |
//|              |0x1F5 in [2].    |                 |                 |                 |
//|--------------------------------------------------------------------------------------|
//|ODD flag      |At indicated     |                 |At indicated     |                 |
//|toggled       |HCounter position|                 |HCounter position|                 |
//|              |while VCounter is|     <Same>      |while VCounter is|     <Same>      |
//|              |set to 0x0E0 in  |                 |set to 0x0F0 in  |                 |
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
//|VBlank set    |VCounter changes |                 |VCounter changes |                 |
//|              |from 0x0DF to    |     <Same>      |from 0x0EF to    |     <Same>      |
//|              |0x0E0 in [1].    |                 |0x0F0 in [1].    |                 |
//|--------------------------------------------------------------------------------------|
//|VBlank cleared|VCounter changes |                 |VCounter changes |                 |
//|              |from 0x1FE to    |     <Same>      |from 0x1FE to    |     <Same>      |
//|              |0x1FF in [2].    |                 |0x1FF in [2].    |                 |
//|--------------------------------------------------------------------------------------|
//|F flag set    |At indicated     |                 |At indicated     |                 |
//|              |HCounter position|                 |HCounter position|                 |
//|              |while VCounter is|     <Same>      |while VCounter is|     <Same>      |
//|              |set to 0x0E0 in  |                 |set to 0x0F0 in  |                 |
//|              |[1].             |                 |[1].             |                 |
//|--------------------------------------------------------------------------------------|
//|VSYNC asserted|VCounter changes |                 |VCounter changes |                 |
//|              |from 0x0FF to    |     <Same>      |from 0x107 to    |     <Same>      |
//|              |0x100 in [1].    |                 |0x108 in [1].    |                 |
//|--------------------------------------------------------------------------------------|
//|VSYNC cleared |VCounter changes |                 |VCounter changes |                 |
//|              |from 0x1D9 to    |     <Same>      |from 0x1E1 to    |     <Same>      |
//|              |0x1DA in [2].    |                 |0x1E2 in [2].    |                 |
//|--------------------------------------------------------------------------------------|
//|ODD flag      |At indicated     |                 |At indicated     |                 |
//|toggled       |HCounter position|                 |HCounter position|                 |
//|              |while VCounter is|     <Same>      |while VCounter is|     <Same>      |
//|              |set to 0x0E0 in  |                 |set to 0x0F0 in  |                 |
//|              |[1].             |                 |[1].             |                 |
//----------------------------------------------------------------------------------------
//##TODO## Evaluate the way we're using the first parameter vcounterActiveScanMaxValue in
//code. Note that we subtract 1 from this value for the interlace values in PAL mode, but
//we use the same values for both interlace and non-interlace modes in NTSC. Confirm if
//this is correct.
const S315_5313::VScanSettings S315_5313::v28PalNoIntScanSettingsStatic (0x102, 0x1CA, 0x1C9, 0x0E0, 0x1FF, 0x1FF, 0x100, 0x1DA, 313, 0x000, 0x0DF, 224, 0x1DA, 0x1FF, 38, 0x0E0, 0x0FF, 32, 0x1CD, 0x1D9, 13, 0x100, 0x102, 3);
const S315_5313::VScanSettings S315_5313::v28PalIntEnScanSettingsStatic (0x101, 0x1CA, 0x1C9, 0x0E0, 0x1FF, 0x1FF, 0x100, 0x1DA, 313, 0x000, 0x0DF, 224, 0x1DA, 0x1FF, 38, 0x0E0, 0x0FF, 32, 0x1CD, 0x1D9, 13, 0x100, 0x102, 3);
const S315_5313::VScanSettings S315_5313::v30PalNoIntScanSettingsStatic (0x10A, 0x1D2, 0x1D1, 0x0F0, 0x1FF, 0x1FF, 0x108, 0x1E2, 313, 0x000, 0x0EF, 240, 0x1E2, 0x1FF, 30, 0x0F0, 0x107, 24, 0x1D5, 0x1E1, 13, 0x108, 0x10A, 3);
const S315_5313::VScanSettings S315_5313::v30PalIntEnScanSettingsStatic (0x109, 0x1D2, 0x1D1, 0x0F0, 0x1FF, 0x1FF, 0x108, 0x1E2, 313, 0x000, 0x0EF, 240, 0x1E2, 0x1FF, 30, 0x0F0, 0x107, 24, 0x1D5, 0x1E1, 13, 0x108, 0x10A, 3);
const S315_5313::VScanSettings S315_5313::v28NtscNoIntScanSettingsStatic(0x0EA, 0x1E5, 0x1E4, 0x0E0, 0x1FF, 0x1FF, 0x0E8, 0x1F5, 262, 0x000, 0x0DF, 224, 0x1F5, 0x1FF, 11, 0x0E0, 0x0E7,  8, 0x1E8, 0x1F4, 13, 0x0E8, 0x0EA, 3);
const S315_5313::VScanSettings S315_5313::v28NtscIntEnScanSettingsStatic(0x0EA, 0x1E5, 0x1E4, 0x0E0, 0x1FF, 0x1FF, 0x0E8, 0x1F5, 262, 0x000, 0x0DF, 224, 0x1F5, 0x1FF, 11, 0x0E0, 0x0E7,  8, 0x1E8, 0x1F4, 13, 0x0E8, 0x0EA, 3);
const S315_5313::VScanSettings S315_5313::v30NtscNoIntScanSettingsStatic(0x1FF, 0x200, 0x200, 0x0F0, 0x1FF, 0x1FF, 0x1FF, 0x1FF, 262, 0x000, 0x0EF, 240, 0x200, 0x200,  0, 0x200, 0x200,  0, 0x000, 0x000,  0, 0x000, 0x000, 0);
const S315_5313::VScanSettings S315_5313::v30NtscIntEnScanSettingsStatic(0x1FF, 0x200, 0x200, 0x0F0, 0x1FF, 0x1FF, 0x1FF, 0x1FF, 262, 0x000, 0x0EF, 240, 0x200, 0x200,  0, 0x200, 0x200,  0, 0x000, 0x000,  0, 0x000, 0x000, 0);

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const S315_5313::VRAMRenderOp S315_5313::vramOperationsH32ActiveLine[171] = {
	//End of left scrolled 2 cells
	{S315_5313::VRAMRenderOp::PATTERN_B, 1},

	//Cell block 1
	{S315_5313::VRAMRenderOp::MAPPING_A, 2}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A, 2}, {S315_5313::VRAMRenderOp::PATTERN_A, 3}, {S315_5313::VRAMRenderOp::MAPPING_B, 2}, {S315_5313::VRAMRenderOp::MAPPING_S, 0}, {S315_5313::VRAMRenderOp::PATTERN_B, 2}, {S315_5313::VRAMRenderOp::PATTERN_B, 3},
	{S315_5313::VRAMRenderOp::MAPPING_A, 4}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A, 4}, {S315_5313::VRAMRenderOp::PATTERN_A, 5}, {S315_5313::VRAMRenderOp::MAPPING_B, 4}, {S315_5313::VRAMRenderOp::MAPPING_S, 1}, {S315_5313::VRAMRenderOp::PATTERN_B, 4}, {S315_5313::VRAMRenderOp::PATTERN_B, 5},
	{S315_5313::VRAMRenderOp::MAPPING_A, 6}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A, 6}, {S315_5313::VRAMRenderOp::PATTERN_A, 7}, {S315_5313::VRAMRenderOp::MAPPING_B, 6}, {S315_5313::VRAMRenderOp::MAPPING_S, 2}, {S315_5313::VRAMRenderOp::PATTERN_B, 6}, {S315_5313::VRAMRenderOp::PATTERN_B, 7},
	{S315_5313::VRAMRenderOp::MAPPING_A, 8}, {S315_5313::VRAMRenderOp::REFRESH,   0}, {S315_5313::VRAMRenderOp::PATTERN_A, 8}, {S315_5313::VRAMRenderOp::PATTERN_A, 9}, {S315_5313::VRAMRenderOp::MAPPING_B, 8}, {S315_5313::VRAMRenderOp::MAPPING_S, 3}, {S315_5313::VRAMRenderOp::PATTERN_B, 8}, {S315_5313::VRAMRenderOp::PATTERN_B, 9},

	//Cell block 2
	{S315_5313::VRAMRenderOp::MAPPING_A,10}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,10}, {S315_5313::VRAMRenderOp::PATTERN_A,11}, {S315_5313::VRAMRenderOp::MAPPING_B,10}, {S315_5313::VRAMRenderOp::MAPPING_S, 4}, {S315_5313::VRAMRenderOp::PATTERN_B,10}, {S315_5313::VRAMRenderOp::PATTERN_B,11},
	{S315_5313::VRAMRenderOp::MAPPING_A,12}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,12}, {S315_5313::VRAMRenderOp::PATTERN_A,13}, {S315_5313::VRAMRenderOp::MAPPING_B,12}, {S315_5313::VRAMRenderOp::MAPPING_S, 5}, {S315_5313::VRAMRenderOp::PATTERN_B,12}, {S315_5313::VRAMRenderOp::PATTERN_B,13},
	{S315_5313::VRAMRenderOp::MAPPING_A,14}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,14}, {S315_5313::VRAMRenderOp::PATTERN_A,15}, {S315_5313::VRAMRenderOp::MAPPING_B,14}, {S315_5313::VRAMRenderOp::MAPPING_S, 6}, {S315_5313::VRAMRenderOp::PATTERN_B,14}, {S315_5313::VRAMRenderOp::PATTERN_B,15},
	{S315_5313::VRAMRenderOp::MAPPING_A,16}, {S315_5313::VRAMRenderOp::REFRESH,   0}, {S315_5313::VRAMRenderOp::PATTERN_A,16}, {S315_5313::VRAMRenderOp::PATTERN_A,17}, {S315_5313::VRAMRenderOp::MAPPING_B,16}, {S315_5313::VRAMRenderOp::MAPPING_S, 7}, {S315_5313::VRAMRenderOp::PATTERN_B,16}, {S315_5313::VRAMRenderOp::PATTERN_B,17},

	//Cell block 3
	{S315_5313::VRAMRenderOp::MAPPING_A,18}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,18}, {S315_5313::VRAMRenderOp::PATTERN_A,19}, {S315_5313::VRAMRenderOp::MAPPING_B,18}, {S315_5313::VRAMRenderOp::MAPPING_S, 8}, {S315_5313::VRAMRenderOp::PATTERN_B,18}, {S315_5313::VRAMRenderOp::PATTERN_B,19},
	{S315_5313::VRAMRenderOp::MAPPING_A,20}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,20}, {S315_5313::VRAMRenderOp::PATTERN_A,21}, {S315_5313::VRAMRenderOp::MAPPING_B,20}, {S315_5313::VRAMRenderOp::MAPPING_S, 9}, {S315_5313::VRAMRenderOp::PATTERN_B,20}, {S315_5313::VRAMRenderOp::PATTERN_B,21},
	{S315_5313::VRAMRenderOp::MAPPING_A,22}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,22}, {S315_5313::VRAMRenderOp::PATTERN_A,23}, {S315_5313::VRAMRenderOp::MAPPING_B,22}, {S315_5313::VRAMRenderOp::MAPPING_S,10}, {S315_5313::VRAMRenderOp::PATTERN_B,22}, {S315_5313::VRAMRenderOp::PATTERN_B,23},
	{S315_5313::VRAMRenderOp::MAPPING_A,24}, {S315_5313::VRAMRenderOp::REFRESH,   0}, {S315_5313::VRAMRenderOp::PATTERN_A,24}, {S315_5313::VRAMRenderOp::PATTERN_A,25}, {S315_5313::VRAMRenderOp::MAPPING_B,24}, {S315_5313::VRAMRenderOp::MAPPING_S,11}, {S315_5313::VRAMRenderOp::PATTERN_B,24}, {S315_5313::VRAMRenderOp::PATTERN_B,25},

	//Cell block 4
	{S315_5313::VRAMRenderOp::MAPPING_A,26}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,26}, {S315_5313::VRAMRenderOp::PATTERN_A,27}, {S315_5313::VRAMRenderOp::MAPPING_B,26}, {S315_5313::VRAMRenderOp::MAPPING_S,12}, {S315_5313::VRAMRenderOp::PATTERN_B,26}, {S315_5313::VRAMRenderOp::PATTERN_B,27},
	{S315_5313::VRAMRenderOp::MAPPING_A,28}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,28}, {S315_5313::VRAMRenderOp::PATTERN_A,29}, {S315_5313::VRAMRenderOp::MAPPING_B,28}, {S315_5313::VRAMRenderOp::MAPPING_S,13}, {S315_5313::VRAMRenderOp::PATTERN_B,28}, {S315_5313::VRAMRenderOp::PATTERN_B,29},
	{S315_5313::VRAMRenderOp::MAPPING_A,30}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,30}, {S315_5313::VRAMRenderOp::PATTERN_A,31}, {S315_5313::VRAMRenderOp::MAPPING_B,30}, {S315_5313::VRAMRenderOp::MAPPING_S,14}, {S315_5313::VRAMRenderOp::PATTERN_B,30}, {S315_5313::VRAMRenderOp::PATTERN_B,31},
	{S315_5313::VRAMRenderOp::MAPPING_A,32}, {S315_5313::VRAMRenderOp::REFRESH,   0}, {S315_5313::VRAMRenderOp::PATTERN_A,32}, {S315_5313::VRAMRenderOp::PATTERN_A,33}, {S315_5313::VRAMRenderOp::MAPPING_B,32}, {S315_5313::VRAMRenderOp::MAPPING_S,15}, {S315_5313::VRAMRenderOp::PATTERN_B,32}, {S315_5313::VRAMRenderOp::PATTERN_B,33},

	//End of line
	{S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0},
	{S315_5313::VRAMRenderOp::PATTERN_S, 0}, {S315_5313::VRAMRenderOp::PATTERN_S, 1}, {S315_5313::VRAMRenderOp::PATTERN_S, 2}, {S315_5313::VRAMRenderOp::PATTERN_S, 3}, {S315_5313::VRAMRenderOp::PATTERN_S, 4},
	{S315_5313::VRAMRenderOp::PATTERN_S, 5}, {S315_5313::VRAMRenderOp::PATTERN_S, 6}, {S315_5313::VRAMRenderOp::PATTERN_S, 7}, {S315_5313::VRAMRenderOp::PATTERN_S, 8}, {S315_5313::VRAMRenderOp::PATTERN_S, 9},
	{S315_5313::VRAMRenderOp::PATTERN_S,10}, {S315_5313::VRAMRenderOp::PATTERN_S,11}, {S315_5313::VRAMRenderOp::PATTERN_S,12},
	{S315_5313::VRAMRenderOp::ACC_SLOT,  0},
	{S315_5313::VRAMRenderOp::PATTERN_S,13}, {S315_5313::VRAMRenderOp::PATTERN_S,14},

	//HBlank start
	{S315_5313::VRAMRenderOp::PATTERN_S,15}, {S315_5313::VRAMRenderOp::PATTERN_S,16}, {S315_5313::VRAMRenderOp::PATTERN_S,17}, {S315_5313::VRAMRenderOp::PATTERN_S,18}, {S315_5313::VRAMRenderOp::PATTERN_S,19},
	{S315_5313::VRAMRenderOp::PATTERN_S,20}, {S315_5313::VRAMRenderOp::PATTERN_S,21}, {S315_5313::VRAMRenderOp::PATTERN_S,22}, {S315_5313::VRAMRenderOp::PATTERN_S,23}, {S315_5313::VRAMRenderOp::PATTERN_S,24},
	{S315_5313::VRAMRenderOp::PATTERN_S,25},
	{S315_5313::VRAMRenderOp::ACC_SLOT,  0},

	//New line
	{S315_5313::VRAMRenderOp::HSCROLL,   0}, {S315_5313::VRAMRenderOp::PATTERN_S,26}, {S315_5313::VRAMRenderOp::PATTERN_S,27}, {S315_5313::VRAMRenderOp::PATTERN_S,28}, {S315_5313::VRAMRenderOp::PATTERN_S,29},

	//Start of left scrolled 2 cells
	{S315_5313::VRAMRenderOp::MAPPING_A, 0}, {S315_5313::VRAMRenderOp::PATTERN_S,30}, {S315_5313::VRAMRenderOp::PATTERN_A, 0}, {S315_5313::VRAMRenderOp::PATTERN_A, 1}, {S315_5313::VRAMRenderOp::MAPPING_B, 0}, {S315_5313::VRAMRenderOp::PATTERN_S,31}, {S315_5313::VRAMRenderOp::PATTERN_B, 0}
};

//----------------------------------------------------------------------------------------
const S315_5313::VRAMRenderOp S315_5313::vramOperationsH32InactiveLine[171] = {
	//End of left scrolled 2 cells
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//Cell block 1
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::REFRESH,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//Cell block 2
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::REFRESH,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//Cell block 3
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::REFRESH,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//Cell block 4
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::REFRESH,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//End of line
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//HBlank start
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//New line
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//Start of left scrolled 2 cells
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::REFRESH,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}
};

//----------------------------------------------------------------------------------------
const S315_5313::VRAMRenderOp S315_5313::vramOperationsH40ActiveLine[210] = {
	//Cell block 1
	{S315_5313::VRAMRenderOp::MAPPING_A, 2}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A, 2}, {S315_5313::VRAMRenderOp::PATTERN_A, 3}, {S315_5313::VRAMRenderOp::MAPPING_B, 2}, {S315_5313::VRAMRenderOp::MAPPING_S, 0}, {S315_5313::VRAMRenderOp::PATTERN_B, 2}, {S315_5313::VRAMRenderOp::PATTERN_B, 3},
	{S315_5313::VRAMRenderOp::MAPPING_A, 4}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A, 4}, {S315_5313::VRAMRenderOp::PATTERN_A, 5}, {S315_5313::VRAMRenderOp::MAPPING_B, 4}, {S315_5313::VRAMRenderOp::MAPPING_S, 1}, {S315_5313::VRAMRenderOp::PATTERN_B, 4}, {S315_5313::VRAMRenderOp::PATTERN_B, 5},
	{S315_5313::VRAMRenderOp::MAPPING_A, 6}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A, 6}, {S315_5313::VRAMRenderOp::PATTERN_A, 7}, {S315_5313::VRAMRenderOp::MAPPING_B, 6}, {S315_5313::VRAMRenderOp::MAPPING_S, 2}, {S315_5313::VRAMRenderOp::PATTERN_B, 6}, {S315_5313::VRAMRenderOp::PATTERN_B, 7},
	{S315_5313::VRAMRenderOp::MAPPING_A, 8}, {S315_5313::VRAMRenderOp::REFRESH,   0}, {S315_5313::VRAMRenderOp::PATTERN_A, 8}, {S315_5313::VRAMRenderOp::PATTERN_A, 9}, {S315_5313::VRAMRenderOp::MAPPING_B, 8}, {S315_5313::VRAMRenderOp::MAPPING_S, 3}, {S315_5313::VRAMRenderOp::PATTERN_B, 8}, {S315_5313::VRAMRenderOp::PATTERN_B, 9},

	//Cell block 2
	{S315_5313::VRAMRenderOp::MAPPING_A,10}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,10}, {S315_5313::VRAMRenderOp::PATTERN_A,11}, {S315_5313::VRAMRenderOp::MAPPING_B,10}, {S315_5313::VRAMRenderOp::MAPPING_S, 4}, {S315_5313::VRAMRenderOp::PATTERN_B,10}, {S315_5313::VRAMRenderOp::PATTERN_B,11},
	{S315_5313::VRAMRenderOp::MAPPING_A,12}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,12}, {S315_5313::VRAMRenderOp::PATTERN_A,13}, {S315_5313::VRAMRenderOp::MAPPING_B,12}, {S315_5313::VRAMRenderOp::MAPPING_S, 5}, {S315_5313::VRAMRenderOp::PATTERN_B,12}, {S315_5313::VRAMRenderOp::PATTERN_B,13},
	{S315_5313::VRAMRenderOp::MAPPING_A,14}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,14}, {S315_5313::VRAMRenderOp::PATTERN_A,15}, {S315_5313::VRAMRenderOp::MAPPING_B,14}, {S315_5313::VRAMRenderOp::MAPPING_S, 6}, {S315_5313::VRAMRenderOp::PATTERN_B,14}, {S315_5313::VRAMRenderOp::PATTERN_B,15},
	{S315_5313::VRAMRenderOp::MAPPING_A,16}, {S315_5313::VRAMRenderOp::REFRESH,   0}, {S315_5313::VRAMRenderOp::PATTERN_A,16}, {S315_5313::VRAMRenderOp::PATTERN_A,17}, {S315_5313::VRAMRenderOp::MAPPING_B,16}, {S315_5313::VRAMRenderOp::MAPPING_S, 7}, {S315_5313::VRAMRenderOp::PATTERN_B,16}, {S315_5313::VRAMRenderOp::PATTERN_B,17},

	//Cell block 3
	{S315_5313::VRAMRenderOp::MAPPING_A,18}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,18}, {S315_5313::VRAMRenderOp::PATTERN_A,19}, {S315_5313::VRAMRenderOp::MAPPING_B,18}, {S315_5313::VRAMRenderOp::MAPPING_S, 8}, {S315_5313::VRAMRenderOp::PATTERN_B,18}, {S315_5313::VRAMRenderOp::PATTERN_B,19},
	{S315_5313::VRAMRenderOp::MAPPING_A,20}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,20}, {S315_5313::VRAMRenderOp::PATTERN_A,21}, {S315_5313::VRAMRenderOp::MAPPING_B,20}, {S315_5313::VRAMRenderOp::MAPPING_S, 9}, {S315_5313::VRAMRenderOp::PATTERN_B,20}, {S315_5313::VRAMRenderOp::PATTERN_B,21},
	{S315_5313::VRAMRenderOp::MAPPING_A,22}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,22}, {S315_5313::VRAMRenderOp::PATTERN_A,23}, {S315_5313::VRAMRenderOp::MAPPING_B,22}, {S315_5313::VRAMRenderOp::MAPPING_S,10}, {S315_5313::VRAMRenderOp::PATTERN_B,22}, {S315_5313::VRAMRenderOp::PATTERN_B,23},
	{S315_5313::VRAMRenderOp::MAPPING_A,24}, {S315_5313::VRAMRenderOp::REFRESH,   0}, {S315_5313::VRAMRenderOp::PATTERN_A,24}, {S315_5313::VRAMRenderOp::PATTERN_A,25}, {S315_5313::VRAMRenderOp::MAPPING_B,24}, {S315_5313::VRAMRenderOp::MAPPING_S,11}, {S315_5313::VRAMRenderOp::PATTERN_B,24}, {S315_5313::VRAMRenderOp::PATTERN_B,25},

	//Cell block 4
	{S315_5313::VRAMRenderOp::MAPPING_A,26}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,26}, {S315_5313::VRAMRenderOp::PATTERN_A,27}, {S315_5313::VRAMRenderOp::MAPPING_B,26}, {S315_5313::VRAMRenderOp::MAPPING_S,12}, {S315_5313::VRAMRenderOp::PATTERN_B,26}, {S315_5313::VRAMRenderOp::PATTERN_B,27},
	{S315_5313::VRAMRenderOp::MAPPING_A,28}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,28}, {S315_5313::VRAMRenderOp::PATTERN_A,29}, {S315_5313::VRAMRenderOp::MAPPING_B,28}, {S315_5313::VRAMRenderOp::MAPPING_S,13}, {S315_5313::VRAMRenderOp::PATTERN_B,28}, {S315_5313::VRAMRenderOp::PATTERN_B,29},
	{S315_5313::VRAMRenderOp::MAPPING_A,30}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,30}, {S315_5313::VRAMRenderOp::PATTERN_A,31}, {S315_5313::VRAMRenderOp::MAPPING_B,30}, {S315_5313::VRAMRenderOp::MAPPING_S,14}, {S315_5313::VRAMRenderOp::PATTERN_B,30}, {S315_5313::VRAMRenderOp::PATTERN_B,31},
	{S315_5313::VRAMRenderOp::MAPPING_A,32}, {S315_5313::VRAMRenderOp::REFRESH,   0}, {S315_5313::VRAMRenderOp::PATTERN_A,32}, {S315_5313::VRAMRenderOp::PATTERN_A,33}, {S315_5313::VRAMRenderOp::MAPPING_B,32}, {S315_5313::VRAMRenderOp::MAPPING_S,15}, {S315_5313::VRAMRenderOp::PATTERN_B,32}, {S315_5313::VRAMRenderOp::PATTERN_B,33},

	//Cell block 5
	{S315_5313::VRAMRenderOp::MAPPING_A,34}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,34}, {S315_5313::VRAMRenderOp::PATTERN_A,35}, {S315_5313::VRAMRenderOp::MAPPING_B,34}, {S315_5313::VRAMRenderOp::MAPPING_S,16}, {S315_5313::VRAMRenderOp::PATTERN_B,34}, {S315_5313::VRAMRenderOp::PATTERN_B,35},
	{S315_5313::VRAMRenderOp::MAPPING_A,36}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,36}, {S315_5313::VRAMRenderOp::PATTERN_A,37}, {S315_5313::VRAMRenderOp::MAPPING_B,36}, {S315_5313::VRAMRenderOp::MAPPING_S,17}, {S315_5313::VRAMRenderOp::PATTERN_B,36}, {S315_5313::VRAMRenderOp::PATTERN_B,37},
	{S315_5313::VRAMRenderOp::MAPPING_A,38}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::PATTERN_A,38}, {S315_5313::VRAMRenderOp::PATTERN_A,39}, {S315_5313::VRAMRenderOp::MAPPING_B,38}, {S315_5313::VRAMRenderOp::MAPPING_S,18}, {S315_5313::VRAMRenderOp::PATTERN_B,38}, {S315_5313::VRAMRenderOp::PATTERN_B,39},
	{S315_5313::VRAMRenderOp::MAPPING_A,40}, {S315_5313::VRAMRenderOp::REFRESH,   0}, {S315_5313::VRAMRenderOp::PATTERN_A,40}, {S315_5313::VRAMRenderOp::PATTERN_A,41}, {S315_5313::VRAMRenderOp::MAPPING_B,40}, {S315_5313::VRAMRenderOp::MAPPING_S,19}, {S315_5313::VRAMRenderOp::PATTERN_B,40}, {S315_5313::VRAMRenderOp::PATTERN_B,41},

	//End of line
	{S315_5313::VRAMRenderOp::ACC_SLOT,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT,  0},
	{S315_5313::VRAMRenderOp::PATTERN_S, 0}, {S315_5313::VRAMRenderOp::PATTERN_S, 1}, {S315_5313::VRAMRenderOp::PATTERN_S, 2}, {S315_5313::VRAMRenderOp::PATTERN_S, 3}, {S315_5313::VRAMRenderOp::PATTERN_S, 4},
	{S315_5313::VRAMRenderOp::PATTERN_S, 5}, {S315_5313::VRAMRenderOp::PATTERN_S, 6}, {S315_5313::VRAMRenderOp::PATTERN_S, 7}, {S315_5313::VRAMRenderOp::PATTERN_S, 8}, {S315_5313::VRAMRenderOp::PATTERN_S, 9},
	{S315_5313::VRAMRenderOp::PATTERN_S,10}, {S315_5313::VRAMRenderOp::PATTERN_S,11}, {S315_5313::VRAMRenderOp::PATTERN_S,12}, {S315_5313::VRAMRenderOp::PATTERN_S,13}, {S315_5313::VRAMRenderOp::PATTERN_S,14},
	{S315_5313::VRAMRenderOp::PATTERN_S,15}, {S315_5313::VRAMRenderOp::PATTERN_S,16},

	//HBlank start
	{S315_5313::VRAMRenderOp::PATTERN_S,17}, {S315_5313::VRAMRenderOp::PATTERN_S,18}, {S315_5313::VRAMRenderOp::PATTERN_S,19}, {S315_5313::VRAMRenderOp::PATTERN_S,20}, {S315_5313::VRAMRenderOp::PATTERN_S,21},
	{S315_5313::VRAMRenderOp::PATTERN_S,22},
	{S315_5313::VRAMRenderOp::ACC_SLOT,  0},
	{S315_5313::VRAMRenderOp::PATTERN_S,23}, {S315_5313::VRAMRenderOp::PATTERN_S,24}, {S315_5313::VRAMRenderOp::PATTERN_S,25}, {S315_5313::VRAMRenderOp::PATTERN_S,26}, {S315_5313::VRAMRenderOp::PATTERN_S,27},
	{S315_5313::VRAMRenderOp::PATTERN_S,28}, {S315_5313::VRAMRenderOp::PATTERN_S,29}, {S315_5313::VRAMRenderOp::PATTERN_S,30}, {S315_5313::VRAMRenderOp::PATTERN_S,31}, {S315_5313::VRAMRenderOp::PATTERN_S,32},
	{S315_5313::VRAMRenderOp::PATTERN_S,33},

	//New line
	{S315_5313::VRAMRenderOp::HSCROLL,   0}, {S315_5313::VRAMRenderOp::PATTERN_S,34}, {S315_5313::VRAMRenderOp::PATTERN_S,35}, {S315_5313::VRAMRenderOp::PATTERN_S,36}, {S315_5313::VRAMRenderOp::PATTERN_S,37},

	//Left scrolled 2 cells
	{S315_5313::VRAMRenderOp::MAPPING_A, 0}, {S315_5313::VRAMRenderOp::PATTERN_S,38}, {S315_5313::VRAMRenderOp::PATTERN_A, 0}, {S315_5313::VRAMRenderOp::PATTERN_A, 1}, {S315_5313::VRAMRenderOp::MAPPING_B, 0}, {S315_5313::VRAMRenderOp::PATTERN_S,39}, {S315_5313::VRAMRenderOp::PATTERN_B, 0}, {S315_5313::VRAMRenderOp::PATTERN_B, 1}
};

//----------------------------------------------------------------------------------------
const S315_5313::VRAMRenderOp S315_5313::vramOperationsH40InactiveLine[210] = {
	//Cell block 1
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::REFRESH,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//Cell block 2
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::REFRESH,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//Cell block 3
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::REFRESH,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//Cell block 4
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::REFRESH,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//Cell block 5
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::REFRESH,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//End of line
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//HBlank start
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//New line
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0},

	//Left scrolled 2 cells
	{S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::REFRESH,  0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}, {S315_5313::VRAMRenderOp::ACC_SLOT, 0}
};

//----------------------------------------------------------------------------------------
const S315_5313::InternalRenderOp S315_5313::internalOperationsH32[342] = {
	{S315_5313::InternalRenderOp::SPRITEMAPPINGCLEAR, 0}, {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x000-0x003
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x004-0x007
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x008-0x00B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x00C-0x00F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x010-0x013
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x014-0x017
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x018-0x01B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x01C-0x01F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x020-0x023
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x024-0x027
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x028-0x02B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x02C-0x02F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x030-0x033
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x034-0x037
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x038-0x03B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x03C-0x03F

	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x040-0x043
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x044-0x047
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x048-0x04B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x04C-0x04F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x050-0x053
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x054-0x057
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x058-0x05B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x05C-0x05F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x060-0x063
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x064-0x067
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x068-0x06B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x06C-0x06F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x070-0x073
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x074-0x077
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x078-0x07B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x07C-0x07F

	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x080-0x083
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x084-0x087
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x088-0x08B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x08C-0x08F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x090-0x093
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x094-0x097
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x098-0x09B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x09C-0x09F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0A0-0x0A3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0A4-0x0A7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0A8-0x0AB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0AC-0x0AF
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0B0-0x0B3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0B4-0x0B7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0B8-0x0BB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0BC-0x0BF

	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0C0-0x0C3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0C4-0x0C7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0C8-0x0CB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0CC-0x0CF
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0D0-0x0D3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0D4-0x0D7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0D8-0x0DB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0DC-0x0DF
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0E0-0x0E3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0E4-0x0E7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0E8-0x0EB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0EC-0x0EF
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0F0-0x0F3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0F4-0x0F7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0F8-0x0FB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0FC-0x0FF

	{S315_5313::InternalRenderOp::SPRITEPATTERNCLEAR, 0}, {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x100-0x103
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x104-0x107
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::SPRITECACHE,  0}, {S315_5313::InternalRenderOp::SPRITECACHE,  1}, //0x108-0x10B
	{S315_5313::InternalRenderOp::SPRITECACHE,  2},       {S315_5313::InternalRenderOp::SPRITECACHE,  3}, {S315_5313::InternalRenderOp::SPRITECACHE,  4}, {S315_5313::InternalRenderOp::SPRITECACHE,  5}, //0x10C-0x10F
	{S315_5313::InternalRenderOp::SPRITECACHE,  6},       {S315_5313::InternalRenderOp::SPRITECACHE,  7}, {S315_5313::InternalRenderOp::SPRITECACHE,  8}, {S315_5313::InternalRenderOp::SPRITECACHE,  9}, //0x110-0x113
	{S315_5313::InternalRenderOp::SPRITECACHE, 10},       {S315_5313::InternalRenderOp::SPRITECACHE, 11}, {S315_5313::InternalRenderOp::SPRITECACHE, 12}, {S315_5313::InternalRenderOp::SPRITECACHE, 13}, //0x114-0x117
	{S315_5313::InternalRenderOp::SPRITECACHE, 14},       {S315_5313::InternalRenderOp::SPRITECACHE, 15}, {S315_5313::InternalRenderOp::SPRITECACHE, 16}, {S315_5313::InternalRenderOp::SPRITECACHE, 17}, //0x118-0x11B
	{S315_5313::InternalRenderOp::SPRITECACHE, 18},       {S315_5313::InternalRenderOp::SPRITECACHE, 19}, {S315_5313::InternalRenderOp::SPRITECACHE, 20}, {S315_5313::InternalRenderOp::SPRITECACHE, 21}, //0x11C-0x11F
	{S315_5313::InternalRenderOp::SPRITECACHE, 22},       {S315_5313::InternalRenderOp::SPRITECACHE, 23}, {S315_5313::InternalRenderOp::SPRITECACHE, 24}, {S315_5313::InternalRenderOp::SPRITECACHE, 25}, //0x120-0x123
	{S315_5313::InternalRenderOp::SPRITECACHE, 26},       {S315_5313::InternalRenderOp::SPRITECACHE, 27}, {S315_5313::InternalRenderOp::SPRITECACHE, 28}, {S315_5313::InternalRenderOp::SPRITECACHE, 29}, //0x124-0x127

	{S315_5313::InternalRenderOp::SPRITECACHE, 30},       {S315_5313::InternalRenderOp::SPRITECACHE, 31},                                                                                                 //0x1D2-0x1D3
	{S315_5313::InternalRenderOp::SPRITECACHE, 32},       {S315_5313::InternalRenderOp::SPRITECACHE, 33}, {S315_5313::InternalRenderOp::SPRITECACHE, 34}, {S315_5313::InternalRenderOp::SPRITECACHE, 35}, //0x1D4-0x1D7
	{S315_5313::InternalRenderOp::SPRITECACHE, 36},       {S315_5313::InternalRenderOp::SPRITECACHE, 37}, {S315_5313::InternalRenderOp::SPRITECACHE, 38}, {S315_5313::InternalRenderOp::SPRITECACHE, 39}, //0x1D8-0x1DB
	{S315_5313::InternalRenderOp::SPRITECACHE, 40},       {S315_5313::InternalRenderOp::SPRITECACHE, 41}, {S315_5313::InternalRenderOp::SPRITECACHE, 42}, {S315_5313::InternalRenderOp::SPRITECACHE, 43}, //0x1DC-0x1DF
	{S315_5313::InternalRenderOp::SPRITECACHE, 44},       {S315_5313::InternalRenderOp::SPRITECACHE, 45}, {S315_5313::InternalRenderOp::SPRITECACHE, 46}, {S315_5313::InternalRenderOp::SPRITECACHE, 47}, //0x1E0-0x1E3
	{S315_5313::InternalRenderOp::SPRITECACHE, 48},       {S315_5313::InternalRenderOp::SPRITECACHE, 49}, {S315_5313::InternalRenderOp::SPRITECACHE, 50}, {S315_5313::InternalRenderOp::SPRITECACHE, 51}, //0x1E4-0x1E7
	{S315_5313::InternalRenderOp::SPRITECACHE, 52},       {S315_5313::InternalRenderOp::SPRITECACHE, 53}, {S315_5313::InternalRenderOp::SPRITECACHE, 54}, {S315_5313::InternalRenderOp::SPRITECACHE, 55}, //0x1E8-0x1EB
	{S315_5313::InternalRenderOp::SPRITECACHE, 56},       {S315_5313::InternalRenderOp::SPRITECACHE, 57}, {S315_5313::InternalRenderOp::SPRITECACHE, 58}, {S315_5313::InternalRenderOp::SPRITECACHE, 59}, //0x1EC-0x1EF
	{S315_5313::InternalRenderOp::SPRITECACHE, 60},       {S315_5313::InternalRenderOp::SPRITECACHE, 61}, {S315_5313::InternalRenderOp::SPRITECACHE, 62}, {S315_5313::InternalRenderOp::SPRITECACHE, 63}, //0x1F0-0x1F3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x1F4-0x1F7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x1F8-0x1FB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x1FC-0x1FF
};

//----------------------------------------------------------------------------------------
const S315_5313::InternalRenderOp S315_5313::internalOperationsH40[420] = {
	{S315_5313::InternalRenderOp::SPRITEMAPPINGCLEAR, 0}, {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x000-0x003
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x004-0x007
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x008-0x00B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x00C-0x00F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x010-0x013
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x014-0x017
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x018-0x01B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x01C-0x01F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x020-0x023
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x024-0x027
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x028-0x02B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x02C-0x02F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x030-0x033
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x034-0x037
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x038-0x03B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x03C-0x03F

	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x040-0x043
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x044-0x047
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x048-0x04B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x04C-0x04F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x050-0x053
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x054-0x057
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x058-0x05B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x05C-0x05F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x060-0x063
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x064-0x067
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x068-0x06B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x06C-0x06F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x070-0x073
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x074-0x077
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x078-0x07B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x07C-0x07F

	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x080-0x083
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x084-0x087
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x088-0x08B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x08C-0x08F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x090-0x093
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x094-0x097
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x098-0x09B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x09C-0x09F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0A0-0x0A3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0A4-0x0A7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0A8-0x0AB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0AC-0x0AF
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0B0-0x0B3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0B4-0x0B7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0B8-0x0BB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0BC-0x0BF

	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0C0-0x0C3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0C4-0x0C7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0C8-0x0CB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0CC-0x0CF
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0D0-0x0D3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0D4-0x0D7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0D8-0x0DB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0DC-0x0DF
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0E0-0x0E3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0E4-0x0E7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0E8-0x0EB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0EC-0x0EF
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0F0-0x0F3
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0F4-0x0F7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0F8-0x0FB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x0FC-0x0FF

	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x100-0x103
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x104-0x107
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x108-0x10B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x10C-0x10F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x110-0x113
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x114-0x117
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x118-0x11B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x11C-0x11F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x120-0x123
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x124-0x127
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x128-0x12B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x12C-0x12F
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x130-0x133
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x134-0x137
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x138-0x13B
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x13C-0x13F

	{S315_5313::InternalRenderOp::SPRITEPATTERNCLEAR, 0}, {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x140-0x143
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x144-0x147
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::SPRITECACHE,  0}, {S315_5313::InternalRenderOp::SPRITECACHE,  1}, //0x148-0x14B
	{S315_5313::InternalRenderOp::SPRITECACHE,  2},       {S315_5313::InternalRenderOp::SPRITECACHE,  3}, {S315_5313::InternalRenderOp::SPRITECACHE,  4}, {S315_5313::InternalRenderOp::SPRITECACHE,  5}, //0x14C-0x14F
	{S315_5313::InternalRenderOp::SPRITECACHE,  6},       {S315_5313::InternalRenderOp::SPRITECACHE,  7}, {S315_5313::InternalRenderOp::SPRITECACHE,  8}, {S315_5313::InternalRenderOp::SPRITECACHE,  9}, //0x150-0x153
	{S315_5313::InternalRenderOp::SPRITECACHE, 10},       {S315_5313::InternalRenderOp::SPRITECACHE, 11}, {S315_5313::InternalRenderOp::SPRITECACHE, 12}, {S315_5313::InternalRenderOp::SPRITECACHE, 13}, //0x154-0x157
	{S315_5313::InternalRenderOp::SPRITECACHE, 14},       {S315_5313::InternalRenderOp::SPRITECACHE, 15}, {S315_5313::InternalRenderOp::SPRITECACHE, 16}, {S315_5313::InternalRenderOp::SPRITECACHE, 17}, //0x158-0x15B
	{S315_5313::InternalRenderOp::SPRITECACHE, 18},       {S315_5313::InternalRenderOp::SPRITECACHE, 19}, {S315_5313::InternalRenderOp::SPRITECACHE, 20}, {S315_5313::InternalRenderOp::SPRITECACHE, 21}, //0x15C-0x15F
	{S315_5313::InternalRenderOp::SPRITECACHE, 22},       {S315_5313::InternalRenderOp::SPRITECACHE, 23}, {S315_5313::InternalRenderOp::SPRITECACHE, 24}, {S315_5313::InternalRenderOp::SPRITECACHE, 25}, //0x160-0x163
	{S315_5313::InternalRenderOp::SPRITECACHE, 26},       {S315_5313::InternalRenderOp::SPRITECACHE, 27}, {S315_5313::InternalRenderOp::SPRITECACHE, 28}, {S315_5313::InternalRenderOp::SPRITECACHE, 29}, //0x164-0x167
	{S315_5313::InternalRenderOp::SPRITECACHE, 30},       {S315_5313::InternalRenderOp::SPRITECACHE, 31}, {S315_5313::InternalRenderOp::SPRITECACHE, 32}, {S315_5313::InternalRenderOp::SPRITECACHE, 33}, //0x168-0x16B
	{S315_5313::InternalRenderOp::SPRITECACHE, 34},                                                                                                                                                       //0x16C

	{S315_5313::InternalRenderOp::SPRITECACHE, 35},       {S315_5313::InternalRenderOp::SPRITECACHE, 36}, {S315_5313::InternalRenderOp::SPRITECACHE, 37},                                                 //0x1C9-0x1CB
	{S315_5313::InternalRenderOp::SPRITECACHE, 38},       {S315_5313::InternalRenderOp::SPRITECACHE, 39}, {S315_5313::InternalRenderOp::SPRITECACHE, 40}, {S315_5313::InternalRenderOp::SPRITECACHE, 41}, //0x1CC-0x1CF
	{S315_5313::InternalRenderOp::SPRITECACHE, 42},       {S315_5313::InternalRenderOp::SPRITECACHE, 43}, {S315_5313::InternalRenderOp::SPRITECACHE, 44}, {S315_5313::InternalRenderOp::SPRITECACHE, 45}, //0x1D0-0x1D3
	{S315_5313::InternalRenderOp::SPRITECACHE, 46},       {S315_5313::InternalRenderOp::SPRITECACHE, 47}, {S315_5313::InternalRenderOp::SPRITECACHE, 48}, {S315_5313::InternalRenderOp::SPRITECACHE, 49}, //0x1D4-0x1D7
	{S315_5313::InternalRenderOp::SPRITECACHE, 50},       {S315_5313::InternalRenderOp::SPRITECACHE, 51}, {S315_5313::InternalRenderOp::SPRITECACHE, 52}, {S315_5313::InternalRenderOp::SPRITECACHE, 53}, //0x1D8-0x1DB
	{S315_5313::InternalRenderOp::SPRITECACHE, 54},       {S315_5313::InternalRenderOp::SPRITECACHE, 55}, {S315_5313::InternalRenderOp::SPRITECACHE, 56}, {S315_5313::InternalRenderOp::SPRITECACHE, 57}, //0x1DC-0x1DF
	{S315_5313::InternalRenderOp::SPRITECACHE, 58},       {S315_5313::InternalRenderOp::SPRITECACHE, 59}, {S315_5313::InternalRenderOp::SPRITECACHE, 60}, {S315_5313::InternalRenderOp::SPRITECACHE, 61}, //0x1E0-0x1E3
	{S315_5313::InternalRenderOp::SPRITECACHE, 62},       {S315_5313::InternalRenderOp::SPRITECACHE, 63}, {S315_5313::InternalRenderOp::SPRITECACHE, 64}, {S315_5313::InternalRenderOp::SPRITECACHE, 65}, //0x1E4-0x1E7
	{S315_5313::InternalRenderOp::SPRITECACHE, 66},       {S315_5313::InternalRenderOp::SPRITECACHE, 67}, {S315_5313::InternalRenderOp::SPRITECACHE, 68}, {S315_5313::InternalRenderOp::SPRITECACHE, 69}, //0x1E8-0x1EB
	{S315_5313::InternalRenderOp::SPRITECACHE, 70},       {S315_5313::InternalRenderOp::SPRITECACHE, 71}, {S315_5313::InternalRenderOp::SPRITECACHE, 72}, {S315_5313::InternalRenderOp::SPRITECACHE, 73}, //0x1EC-0x1EF
	{S315_5313::InternalRenderOp::SPRITECACHE, 74},       {S315_5313::InternalRenderOp::SPRITECACHE, 75}, {S315_5313::InternalRenderOp::SPRITECACHE, 76}, {S315_5313::InternalRenderOp::SPRITECACHE, 77}, //0x1F0-0x1F3
	{S315_5313::InternalRenderOp::SPRITECACHE, 78},       {S315_5313::InternalRenderOp::SPRITECACHE, 79}, {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x1F4-0x1F7
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x1F8-0x1FB
	{S315_5313::InternalRenderOp::NONE, 0},               {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         {S315_5313::InternalRenderOp::NONE, 0},         //0x1FC-0x1FF
};

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
	//Select the scan settings which correspond with the current screen mode
	return (screenModeRS1Active)? h40ScanSettingsStatic: h32ScanSettingsStatic;
}

//----------------------------------------------------------------------------------------
const S315_5313::VScanSettings& S315_5313::GetVScanSettings(bool screenModeV30Active, bool palModeActive, bool interlaceActive)
{
	//Select the scan settings which correspond with the current screen mode
	if(palModeActive)
	{
		if(screenModeV30Active)
		{
			return (interlaceActive)? v30PalIntEnScanSettingsStatic: v30PalNoIntScanSettingsStatic;
		}
		else
		{
			return (interlaceActive)? v28PalIntEnScanSettingsStatic: v28PalNoIntScanSettingsStatic;
		}
	}
	else
	{
		if(screenModeV30Active)
		{
			return (interlaceActive)? v30NtscIntEnScanSettingsStatic: v30NtscNoIntScanSettingsStatic;
		}
		else
		{
			return (interlaceActive)? v28NtscIntEnScanSettingsStatic: v28NtscNoIntScanSettingsStatic;
		}
	}
}

//----------------------------------------------------------------------------------------
//HV counter comparison functions
//----------------------------------------------------------------------------------------
//##TODO## Refactor this to make it more readable
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
unsigned int S315_5313::GetPixelClockStepsBetweenHVCounterValues(bool advanceIfValuesMatch, const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterTarget, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterTarget)
{
	unsigned int totalPixelClockSteps = 0;

	//If we've been requested to still advance the counters if they match, and they are
	//the same value right now, shortcut the rest of the process, and calculate the number
	//of pixel clock steps required to return to the current horizontal and vertical
	//counter values.
	if(advanceIfValuesMatch && (hcounterCurrent == hcounterTarget) && (vcounterCurrent == vcounterTarget))
	{
		AdvanceHVCounters(hscanSettings, hcounterCurrent, vscanSettings, interlaceIsEnabled, oddFlagSet, vcounterCurrent, 1);
		totalPixelClockSteps = GetPixelClockStepsBetweenHVCounterValues(false, hscanSettings, hcounterCurrent, hcounterTarget, vscanSettings, interlaceIsEnabled, oddFlagSet, vcounterCurrent, vcounterTarget);
		++totalPixelClockSteps;
		return totalPixelClockSteps;
	}

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
void S315_5313::BeginHVCounterAdvanceSessionFromCurrentState(HVCounterAdvanceSession& advanceSession)
{
	//Initialize the MCLK and pixel clock counters for this session
	advanceSession.pixelClockTicksAdvanced = 0;
	advanceSession.mclkTicksAdvanced = 0;

	//Cache the current HV counter and video mode state to use in the advance process
	advanceSession.hcounterCurrent = hcounter.GetData();
	advanceSession.vcounterCurrent = vcounter.GetData();
	advanceSession.interlaceEnabledCurrent = interlaceEnabled;
	advanceSession.interlaceDoubleCurrent = interlaceDouble;
	advanceSession.screenModeRS0Current = screenModeRS0;
	advanceSession.screenModeRS1Current = screenModeRS1;
	advanceSession.screenModeV30Current = screenModeV30;
	advanceSession.palModeCurrent = palMode;
	advanceSession.oddFlagCurrent = GetStatusFlagOddInterlaceFrame();

	//Grab the latest settings for any registers which affect the video mode. This logic
	//is based on the update steps performed in the AdvanceProcessorState method, and must
	//predict changes in the same way. See the implementation of that method for further
	//comments and details.
	advanceSession.interlaceEnabledNew = interlaceEnabledCached;
	advanceSession.interlaceDoubleNew = interlaceDoubleCached;
	advanceSession.screenModeRS0New = screenModeRS0Cached;
	advanceSession.screenModeRS1New = screenModeRS1Cached;
	advanceSession.screenModeV30New = screenModeV30Cached;
	advanceSession.palModeNew = palModeLineState;

	//Obtain the current hscan and vscan settings
	advanceSession.hscanSettings = &GetHScanSettings(advanceSession.screenModeRS0Current, advanceSession.screenModeRS1Current);
	advanceSession.vscanSettings = &GetVScanSettings(advanceSession.screenModeV30Current, advanceSession.palModeCurrent, advanceSession.interlaceEnabledCurrent);
}

//----------------------------------------------------------------------------------------
bool S315_5313::AdvanceHVCounterSession(HVCounterAdvanceSession& advanceSession, unsigned int hcounterTarget, unsigned int vcounterTarget, bool advanceIfValuesMatch)
{
	//Check whether any of the relevant video mode settings have changed since they were
	//latched.
	bool hscanSettingsChanged = (advanceSession.screenModeRS0Current != advanceSession.screenModeRS0New) || (advanceSession.screenModeRS1Current != advanceSession.screenModeRS1New);
	bool vscanSettingsChanged = (advanceSession.screenModeV30Current != advanceSession.screenModeV30New) || (advanceSession.palModeCurrent != advanceSession.palModeNew) || (advanceSession.interlaceEnabledCurrent != advanceSession.interlaceEnabledNew);

	//Gather information on the next hblank update point
	unsigned int pixelClockTicksBeforeUpdatePointHBlank = 0;
	if(hscanSettingsChanged)
	{
		//Calculate the number of pixel clock ticks until the hblank event occurs, and new
		//screen mode settings are latched.
		pixelClockTicksBeforeUpdatePointHBlank = GetPixelClockStepsBetweenHCounterValues(*advanceSession.hscanSettings, advanceSession.hcounterCurrent, advanceSession.hscanSettings->hblankSetPoint);
	}

	//Gather information on the next vblank update point
	unsigned int pixelClockTicksBeforeUpdatePointVBlank = 0;
	if(vscanSettingsChanged)
	{
		//Calculate the number of pixel clock ticks until the vblank event occurs, and new
		//screen mode settings are latched.
		pixelClockTicksBeforeUpdatePointVBlank = GetPixelClockStepsBetweenHVCounterValues(true, *advanceSession.hscanSettings, advanceSession.hcounterCurrent, advanceSession.hscanSettings->vcounterIncrementPoint, *advanceSession.vscanSettings, advanceSession.interlaceEnabledCurrent, advanceSession.oddFlagCurrent, advanceSession.vcounterCurrent, advanceSession.vscanSettings->vblankSetPoint);
	}

	//Calculate the number of pixel clock steps before we reach the target HV counter
	//value, assuming the current screen mode settings stay constant.
	unsigned int pixelClockTicksBeforeTarget = GetPixelClockStepsBetweenHVCounterValues(advanceIfValuesMatch, *advanceSession.hscanSettings, advanceSession.hcounterCurrent, hcounterTarget, *advanceSession.vscanSettings, advanceSession.interlaceEnabledCurrent, advanceSession.oddFlagCurrent, advanceSession.vcounterCurrent, vcounterTarget);

	//Set this advance operation to stop at the next update point if required
	unsigned int pixelClockCyclesToAdvanceThisStep = pixelClockTicksBeforeTarget;
	if(hscanSettingsChanged && (pixelClockTicksBeforeUpdatePointHBlank < pixelClockTicksBeforeTarget))
	{
		pixelClockCyclesToAdvanceThisStep = pixelClockTicksBeforeUpdatePointHBlank;
	}
	if(vscanSettingsChanged && (pixelClockTicksBeforeUpdatePointVBlank < pixelClockTicksBeforeTarget))
	{
		pixelClockCyclesToAdvanceThisStep = pixelClockTicksBeforeUpdatePointVBlank;
	}

	//Calculate the number of MCLK cycles to advance this step
	unsigned int mclkCyclesToAdvanceThisStep = GetMclkTicksForPixelClockTicks(*advanceSession.hscanSettings, pixelClockCyclesToAdvanceThisStep, advanceSession.hcounterCurrent, advanceSession.screenModeRS0Current, advanceSession.screenModeRS1Current);

	//Add the number of MCLK cycles and pixel clock cycles we're advancing in this step to
	//the total number of cycles required to advance to the target
	advanceSession.mclkTicksAdvanced += mclkCyclesToAdvanceThisStep;
	advanceSession.pixelClockTicksAdvanced += pixelClockCyclesToAdvanceThisStep;

	//Advance the HV counters to the target position, taking into account changes to the
	//odd flag if interlacing is active.
	AdvanceHVCounters(*advanceSession.hscanSettings, advanceSession.hcounterCurrent, *advanceSession.vscanSettings, advanceSession.interlaceEnabledCurrent, advanceSession.oddFlagCurrent, advanceSession.vcounterCurrent, pixelClockCyclesToAdvanceThisStep);

	//If horizontal scan information has changed, and we've just advanced to hblank, latch
	//the new screen mode settings.
	if(hscanSettingsChanged && (pixelClockTicksBeforeUpdatePointHBlank == pixelClockCyclesToAdvanceThisStep))
	{
		advanceSession.screenModeRS0Current = advanceSession.screenModeRS0New;
		advanceSession.screenModeRS1Current = advanceSession.screenModeRS1New;
		advanceSession.hscanSettings = &GetHScanSettings(advanceSession.screenModeRS0Current, advanceSession.screenModeRS1Current);
	}

	//If vertical scan information has changed, and we've just advanced to vblank, latch
	//the new screen mode settings.
	if(vscanSettingsChanged && (pixelClockTicksBeforeUpdatePointVBlank == pixelClockCyclesToAdvanceThisStep))
	{
		advanceSession.interlaceEnabledCurrent = advanceSession.interlaceEnabledNew;
		advanceSession.interlaceDoubleCurrent = advanceSession.interlaceDoubleNew;
		advanceSession.palModeCurrent = advanceSession.palModeNew;
		advanceSession.screenModeV30Current = advanceSession.screenModeV30New;
		advanceSession.vscanSettings = &GetVScanSettings(advanceSession.screenModeV30Current, advanceSession.palModeCurrent, advanceSession.interlaceEnabledCurrent);
	}

	//If we just advanced the number of pixel clock steps required in order to reach the
	//target HV counter value, flag that the advance operation is complete.
	bool reachedTarget = (pixelClockCyclesToAdvanceThisStep == pixelClockTicksBeforeTarget);
	return reachedTarget;
}

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

	//Calculate the actual number of manual vcounter increment steps that need to be made
	//in order to advance to the odd flag toggle point. If we're going to pass the
	//vcounter increment point when advancing the hcounter to the odd flag toggle point,
	//the manual vcounter increment step count is one less than the actual number of
	//vcounter increment steps that need to occur. We only need to use this value when
	//calculating the number of pixel clock steps required to reach the target counter
	//value.
	unsigned int vcounterManualIncrementStepsUntilOddFlagToggle = vcounterIncrementStepsUntilOddFlagToggle;
	if((hcounterCurrent >= hscanSettings.oddFlagTogglePoint) && (hcounterCurrent < hscanSettings.vcounterIncrementPoint))
	{
		vcounterManualIncrementStepsUntilOddFlagToggle = ((vcounterIncrementStepsUntilOddFlagToggle + vcounterStepsPerIteration) - 1) % vcounterStepsPerIteration;
	}

	//Calculate the number of pixel clock steps until the odd flag needs to be toggled
	unsigned int pixelClockStepsUntilOddFlagToggle = (vcounterManualIncrementStepsUntilOddFlagToggle * hscanSettings.hcounterStepsPerIteration) + hcounterIncrementStepsUntilOddFlagToggle;

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
////----------------------------------------------------------------------------------------
//unsigned int S315_5313::GetUpdatedVSRAMReadCacheIndex(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int vsramReadCacheIndexCurrent, unsigned int hcounterInitial, unsigned int vcounterInitial, unsigned int hcounterFinal, unsigned int vcounterFinal, bool screenModeRS0Current, bool screenModeRS1Current, unsigned int vscrollMode)
//{
//	//If we're advancing through an active scan region, update the VSRAM read cache index
//	//if it passes a point where a new value is latched.
//	unsigned int vsramReadCacheIndexNew = vsramReadCacheIndexCurrent;
//
//	//Calculate the HV counter position of the last VSRAM read to be performed by the
//	//render process prior to the current HV counter position, and the index into VSRAM of
//	//that read operation. Note that hardware tests have shown that the disable display
//	//bit in register 1, as well as bit 0 in register 0, do not prevent the VDP from
//	//reading from VSRAM during active scan. The VSRAM reads occur regardless of the state
//	//of these register settings.
//	//##TODO## Perform hardware tests to determine whether VSRAM data is read on the first
//	//active line of the display at 0x1FF, where the sprite data is read for the upcoming
//	//line, but no actual pattern data is output. We can perform this test by reading the
//	//HV counter data along with reads from VSRAM, and checking where the VSRAM cache data
//	//first starts being modified.
//	//##TODO## Perform hardware tests using a logic analyzer to absolutely confirm the
//	//timing of VSRAM reads.
//	unsigned int vsramLastReadHCounter;
//	unsigned int vsramLastReadVCounter;
//	unsigned int vsramLastReadIndex;
//	if(!screenModeRS1Current)
//	{
//		//VSRAM read operations (H32):
//		//----------------------------------
//		//|Read|Layer|Column|Index|HCounter|
//		//|--------------------------------|
//		//|  1 |  A  |   1  | 0x00|  0x003 |
//		//|  2 |  B  |   1  | 0x02|  0x00B |
//		//|  3 |  A  |   2  | 0x04|  0x013 |
//		//|  4 |  B  |   2  | 0x06|  0x01B |
//		//|  5 |  A  |   3  | 0x08|  0x023 |
//		//|  6 |  B  |   3  | 0x0A|  0x02B |
//		//|  7 |  A  |   4  | 0x0C|  0x033 |
//		//|  8 |  B  |   4  | 0x0E|  0x03B |
//		//|  9 |  A  |   5  | 0x10|  0x043 |
//		//| 10 |  B  |   5  | 0x12|  0x04B |
//		//| 11 |  A  |   6  | 0x14|  0x053 |
//		//| 12 |  B  |   6  | 0x16|  0x05B |
//		//| 13 |  A  |   7  | 0x18|  0x063 |
//		//| 14 |  B  |   7  | 0x1A|  0x06B |
//		//| 15 |  A  |   8  | 0x1C|  0x073 |
//		//| 16 |  B  |   8  | 0x1E|  0x07B |
//		//| 17 |  A  |   9  | 0x20|  0x083 |
//		//| 18 |  B  |   9  | 0x22|  0x08B |
//		//| 19 |  A  |  10  | 0x24|  0x093 |
//		//| 20 |  B  |  10  | 0x26|  0x09B |
//		//| 21 |  A  |  11  | 0x28|  0x0A3 |
//		//| 22 |  B  |  11  | 0x2A|  0x0AB |
//		//| 23 |  A  |  12  | 0x2C|  0x0B3 |
//		//| 24 |  B  |  12  | 0x2E|  0x0BB |
//		//| 25 |  A  |  13  | 0x30|  0x0C3 |
//		//| 26 |  B  |  13  | 0x32|  0x0CB |
//		//| 27 |  A  |  14  | 0x34|  0x0D3 |
//		//| 28 |  B  |  14  | 0x36|  0x0DB |
//		//| 29 |  A  |  15  | 0x38|  0x0E3 |
//		//| 30 |  B  |  15  | 0x3A|  0x0EB |
//		//| 31 |  A  |  16  | 0x3C|  0x0F3 |
//		//| 32 |  B  |  16  | 0x3E|  0x0FB |
//		//----------------------------------
//		vsramLastReadVCounter = vcounterFinal;
//		const unsigned int vsramFirstReadHCounterOnLine = 0x003;
//		if((vcounterFinal > vscanSettings.activeDisplayVCounterLastValue) //VCounter is past the end of the active scan region
//		|| (vcounterFinal < vscanSettings.activeDisplayVCounterFirstValue) //VCounter is before the start of the active scan region
//		|| ((vcounterFinal == vscanSettings.activeDisplayVCounterFirstValue) && ((hcounterFinal < vsramFirstReadHCounterOnLine) || (hcounterFinal >= hscanSettings.vcounterIncrementPoint)))) //VCounter is on the same line as the start of the active scan region, but hasn't reached the first VSRAM read position yet
//		{
//			vsramLastReadVCounter = vscanSettings.activeDisplayVCounterLastValue;
//			vsramLastReadHCounter = 0x0FB;
//			vsramLastReadIndex = 0x3E;
//		}
//		else if(hcounterFinal < 0x003) {vsramLastReadHCounter = 0x0FB; vsramLastReadIndex = 0x3E; vsramLastReadVCounter = vcounterFinal-1;}
//		else if(hcounterFinal < 0x00B) {vsramLastReadHCounter = 0x003; vsramLastReadIndex = 0x00;}
//		else if(hcounterFinal < 0x013) {vsramLastReadHCounter = 0x00B; vsramLastReadIndex = 0x02;}
//		else if(hcounterFinal < 0x01B) {vsramLastReadHCounter = 0x013; vsramLastReadIndex = 0x04;}
//		else if(hcounterFinal < 0x023) {vsramLastReadHCounter = 0x01B; vsramLastReadIndex = 0x06;}
//		else if(hcounterFinal < 0x02B) {vsramLastReadHCounter = 0x023; vsramLastReadIndex = 0x08;}
//		else if(hcounterFinal < 0x033) {vsramLastReadHCounter = 0x02B; vsramLastReadIndex = 0x0A;}
//		else if(hcounterFinal < 0x03B) {vsramLastReadHCounter = 0x033; vsramLastReadIndex = 0x0C;}
//		else if(hcounterFinal < 0x043) {vsramLastReadHCounter = 0x03B; vsramLastReadIndex = 0x0E;}
//		else if(hcounterFinal < 0x04B) {vsramLastReadHCounter = 0x043; vsramLastReadIndex = 0x10;}
//		else if(hcounterFinal < 0x053) {vsramLastReadHCounter = 0x04B; vsramLastReadIndex = 0x12;}
//		else if(hcounterFinal < 0x05B) {vsramLastReadHCounter = 0x053; vsramLastReadIndex = 0x14;}
//		else if(hcounterFinal < 0x063) {vsramLastReadHCounter = 0x05B; vsramLastReadIndex = 0x16;}
//		else if(hcounterFinal < 0x06B) {vsramLastReadHCounter = 0x063; vsramLastReadIndex = 0x18;}
//		else if(hcounterFinal < 0x073) {vsramLastReadHCounter = 0x06B; vsramLastReadIndex = 0x1A;}
//		else if(hcounterFinal < 0x07B) {vsramLastReadHCounter = 0x073; vsramLastReadIndex = 0x1C;}
//		else if(hcounterFinal < 0x083) {vsramLastReadHCounter = 0x07B; vsramLastReadIndex = 0x1E;}
//		else if(hcounterFinal < 0x08B) {vsramLastReadHCounter = 0x083; vsramLastReadIndex = 0x20;}
//		else if(hcounterFinal < 0x093) {vsramLastReadHCounter = 0x08B; vsramLastReadIndex = 0x22;}
//		else if(hcounterFinal < 0x09B) {vsramLastReadHCounter = 0x093; vsramLastReadIndex = 0x24;}
//		else if(hcounterFinal < 0x0A3) {vsramLastReadHCounter = 0x09B; vsramLastReadIndex = 0x26;}
//		else if(hcounterFinal < 0x0AB) {vsramLastReadHCounter = 0x0A3; vsramLastReadIndex = 0x28;}
//		else if(hcounterFinal < 0x0B3) {vsramLastReadHCounter = 0x0AB; vsramLastReadIndex = 0x2A;}
//		else if(hcounterFinal < 0x0BB) {vsramLastReadHCounter = 0x0B3; vsramLastReadIndex = 0x2C;}
//		else if(hcounterFinal < 0x0C3) {vsramLastReadHCounter = 0x0BB; vsramLastReadIndex = 0x2E;}
//		else if(hcounterFinal < 0x0CB) {vsramLastReadHCounter = 0x0C3; vsramLastReadIndex = 0x30;}
//		else if(hcounterFinal < 0x0D3) {vsramLastReadHCounter = 0x0CB; vsramLastReadIndex = 0x32;}
//		else if(hcounterFinal < 0x0DB) {vsramLastReadHCounter = 0x0D3; vsramLastReadIndex = 0x34;}
//		else if(hcounterFinal < 0x0E3) {vsramLastReadHCounter = 0x0DB; vsramLastReadIndex = 0x36;}
//		else if(hcounterFinal < 0x0EB) {vsramLastReadHCounter = 0x0E3; vsramLastReadIndex = 0x38;}
//		else if(hcounterFinal < 0x0F3) {vsramLastReadHCounter = 0x0EB; vsramLastReadIndex = 0x3A;}
//		else if(hcounterFinal < 0x0FB) {vsramLastReadHCounter = 0x0F3; vsramLastReadIndex = 0x3C;}
//		else
//		{
//			vsramLastReadHCounter = 0x0FB;
//			vsramLastReadIndex = 0x3E;
//			//If the hcounter has passed the vcounter increment point, the real vcounter
//			//value to use is one less than the current value.
//			if(hcounterFinal >= hscanSettings.vcounterIncrementPoint)
//			{
//				vsramLastReadVCounter = vcounterFinal-1;
//			}
//		}
//	}
//	else
//	{
//		//VSRAM read operations (H40):
//		//----------------------------------
//		//|Read|Layer|Column|Index|HCounter|
//		//|--------------------------------|
//		//|  1 |  A  |   1  | 0x00|  0x000 |
//		//|  2 |  B  |   1  | 0x02|  0x008 |
//		//|  3 |  A  |   2  | 0x04|  0x010 |
//		//|  4 |  B  |   2  | 0x06|  0x018 |
//		//|  5 |  A  |   3  | 0x08|  0x020 |
//		//|  6 |  B  |   3  | 0x0A|  0x028 |
//		//|  7 |  A  |   4  | 0x0C|  0x030 |
//		//|  8 |  B  |   4  | 0x0E|  0x038 |
//		//|  9 |  A  |   5  | 0x10|  0x040 |
//		//| 10 |  B  |   5  | 0x12|  0x048 |
//		//| 11 |  A  |   6  | 0x14|  0x050 |
//		//| 12 |  B  |   6  | 0x16|  0x058 |
//		//| 13 |  A  |   7  | 0x18|  0x060 |
//		//| 14 |  B  |   7  | 0x1A|  0x068 |
//		//| 15 |  A  |   8  | 0x1C|  0x070 |
//		//| 16 |  B  |   8  | 0x1E|  0x078 |
//		//| 17 |  A  |   9  | 0x20|  0x080 |
//		//| 18 |  B  |   9  | 0x22|  0x088 |
//		//| 19 |  A  |  10  | 0x24|  0x090 |
//		//| 20 |  B  |  10  | 0x26|  0x098 |
//		//| 21 |  A  |  11  | 0x28|  0x0A0 |
//		//| 22 |  B  |  11  | 0x2A|  0x0A8 |
//		//| 23 |  A  |  12  | 0x2C|  0x0B0 |
//		//| 24 |  B  |  12  | 0x2E|  0x0B8 |
//		//| 25 |  A  |  13  | 0x30|  0x0C0 |
//		//| 26 |  B  |  13  | 0x32|  0x0C8 |
//		//| 27 |  A  |  14  | 0x34|  0x0D0 |
//		//| 28 |  B  |  14  | 0x36|  0x0D8 |
//		//| 29 |  A  |  15  | 0x38|  0x0E0 |
//		//| 30 |  B  |  15  | 0x3A|  0x0E8 |
//		//| 31 |  A  |  16  | 0x3C|  0x0F0 |
//		//| 32 |  B  |  16  | 0x3E|  0x0F8 |
//		//| 33 |  A  |  17  | 0x40|  0x100 |
//		//| 34 |  B  |  17  | 0x42|  0x108 |
//		//| 35 |  A  |  18  | 0x44|  0x110 |
//		//| 36 |  B  |  18  | 0x46|  0x118 |
//		//| 37 |  A  |  19  | 0x48|  0x120 |
//		//| 38 |  B  |  19  | 0x4A|  0x128 |
//		//| 39 |  A  |  20  | 0x4C|  0x130 |
//		//| 40 |  B  |  20  | 0x4E|  0x138 |
//		//----------------------------------
//		vsramLastReadVCounter = vcounterFinal;
//		if((vcounterFinal > vscanSettings.activeDisplayVCounterLastValue) //VCounter is past the end of the active scan region
//		|| (vcounterFinal < vscanSettings.activeDisplayVCounterFirstValue) //VCounter is before the start of the active scan region
//		|| ((vcounterFinal == vscanSettings.activeDisplayVCounterFirstValue) && (hcounterFinal >= hscanSettings.vcounterIncrementPoint))) //VCounter is on the same line as the start of the active scan region, but hasn't reached the first VSRAM read position yet
//		{
//			vsramLastReadVCounter = vscanSettings.activeDisplayVCounterLastValue;
//			vsramLastReadHCounter = 0x138;
//			vsramLastReadIndex = 0x4E;
//		}
//		else if(hcounterFinal < 0x008) {vsramLastReadHCounter = 0x000; vsramLastReadIndex = 0x00;}
//		else if(hcounterFinal < 0x010) {vsramLastReadHCounter = 0x008; vsramLastReadIndex = 0x02;}
//		else if(hcounterFinal < 0x018) {vsramLastReadHCounter = 0x010; vsramLastReadIndex = 0x04;}
//		else if(hcounterFinal < 0x020) {vsramLastReadHCounter = 0x018; vsramLastReadIndex = 0x06;}
//		else if(hcounterFinal < 0x028) {vsramLastReadHCounter = 0x020; vsramLastReadIndex = 0x08;}
//		else if(hcounterFinal < 0x030) {vsramLastReadHCounter = 0x028; vsramLastReadIndex = 0x0A;}
//		else if(hcounterFinal < 0x038) {vsramLastReadHCounter = 0x030; vsramLastReadIndex = 0x0C;}
//		else if(hcounterFinal < 0x040) {vsramLastReadHCounter = 0x038; vsramLastReadIndex = 0x0E;}
//		else if(hcounterFinal < 0x048) {vsramLastReadHCounter = 0x040; vsramLastReadIndex = 0x10;}
//		else if(hcounterFinal < 0x050) {vsramLastReadHCounter = 0x048; vsramLastReadIndex = 0x12;}
//		else if(hcounterFinal < 0x058) {vsramLastReadHCounter = 0x050; vsramLastReadIndex = 0x14;}
//		else if(hcounterFinal < 0x060) {vsramLastReadHCounter = 0x058; vsramLastReadIndex = 0x16;}
//		else if(hcounterFinal < 0x068) {vsramLastReadHCounter = 0x060; vsramLastReadIndex = 0x18;}
//		else if(hcounterFinal < 0x070) {vsramLastReadHCounter = 0x068; vsramLastReadIndex = 0x1A;}
//		else if(hcounterFinal < 0x078) {vsramLastReadHCounter = 0x070; vsramLastReadIndex = 0x1C;}
//		else if(hcounterFinal < 0x080) {vsramLastReadHCounter = 0x078; vsramLastReadIndex = 0x1E;}
//		else if(hcounterFinal < 0x088) {vsramLastReadHCounter = 0x080; vsramLastReadIndex = 0x20;}
//		else if(hcounterFinal < 0x090) {vsramLastReadHCounter = 0x088; vsramLastReadIndex = 0x22;}
//		else if(hcounterFinal < 0x098) {vsramLastReadHCounter = 0x090; vsramLastReadIndex = 0x24;}
//		else if(hcounterFinal < 0x0A0) {vsramLastReadHCounter = 0x098; vsramLastReadIndex = 0x26;}
//		else if(hcounterFinal < 0x0A8) {vsramLastReadHCounter = 0x0A0; vsramLastReadIndex = 0x28;}
//		else if(hcounterFinal < 0x0B0) {vsramLastReadHCounter = 0x0A8; vsramLastReadIndex = 0x2A;}
//		else if(hcounterFinal < 0x0B8) {vsramLastReadHCounter = 0x0B0; vsramLastReadIndex = 0x2C;}
//		else if(hcounterFinal < 0x0C0) {vsramLastReadHCounter = 0x0B8; vsramLastReadIndex = 0x2E;}
//		else if(hcounterFinal < 0x0C8) {vsramLastReadHCounter = 0x0C0; vsramLastReadIndex = 0x30;}
//		else if(hcounterFinal < 0x0D0) {vsramLastReadHCounter = 0x0C8; vsramLastReadIndex = 0x32;}
//		else if(hcounterFinal < 0x0D8) {vsramLastReadHCounter = 0x0D0; vsramLastReadIndex = 0x34;}
//		else if(hcounterFinal < 0x0E0) {vsramLastReadHCounter = 0x0D8; vsramLastReadIndex = 0x36;}
//		else if(hcounterFinal < 0x0E8) {vsramLastReadHCounter = 0x0E0; vsramLastReadIndex = 0x38;}
//		else if(hcounterFinal < 0x0F0) {vsramLastReadHCounter = 0x0E8; vsramLastReadIndex = 0x3A;}
//		else if(hcounterFinal < 0x0F8) {vsramLastReadHCounter = 0x0F0; vsramLastReadIndex = 0x3C;}
//		else if(hcounterFinal < 0x100) {vsramLastReadHCounter = 0x0F8; vsramLastReadIndex = 0x3E;}
//		else if(hcounterFinal < 0x108) {vsramLastReadHCounter = 0x100; vsramLastReadIndex = 0x40;}
//		else if(hcounterFinal < 0x110) {vsramLastReadHCounter = 0x108; vsramLastReadIndex = 0x42;}
//		else if(hcounterFinal < 0x118) {vsramLastReadHCounter = 0x110; vsramLastReadIndex = 0x44;}
//		else if(hcounterFinal < 0x120) {vsramLastReadHCounter = 0x118; vsramLastReadIndex = 0x46;}
//		else if(hcounterFinal < 0x128) {vsramLastReadHCounter = 0x120; vsramLastReadIndex = 0x48;}
//		else if(hcounterFinal < 0x130) {vsramLastReadHCounter = 0x128; vsramLastReadIndex = 0x4A;}
//		else if(hcounterFinal < 0x138) {vsramLastReadHCounter = 0x130; vsramLastReadIndex = 0x4C;}
//		else
//		{
//			vsramLastReadHCounter = 0x138;
//			vsramLastReadIndex = 0x4E;
//			//If the hcounter has passed the vcounter increment point, the real vcounter
//			//value to use is one less than the current value.
//			if(hcounterFinal >= hscanSettings.vcounterIncrementPoint)
//			{
//				vsramLastReadVCounter = vcounterFinal-1;
//			}
//		}
//	}
//
//	//Take into account the current vscroll mode, and adjust the last read index
//	//accordingly. If the vscroll mode is set to overall scrolling rather than column
//	//based scrolling, all columns use the scroll value for the first column. We mask the
//	//read index to apply that effect here.
//	if(!vscrollMode)
//	{
//		vsramLastReadIndex &= 0x03;
//	}
//
//	//If we advanced past the last VSRAM read slot in the last advance operation, latch
//	//the last VSRAM read cache index as the new VSRAM read cache index.
//	if(EventOccursWithinCounterRange(hscanSettings, hcounterInitial, vcounterInitial, hcounterFinal, vcounterFinal, vsramLastReadHCounter, vsramLastReadVCounter))
//	{
//		vsramReadCacheIndexNew = vsramLastReadIndex;
//	}
//
//	return vsramReadCacheIndexNew;
//}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetPixelClockTicksUntilNextAccessSlot(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int hcounterCurrent, bool screenModeRS0Current, bool screenModeRS1Current, bool displayEnabled, unsigned int vcounterCurrent)
{
	//Check if the current line, and the line following, are within the active display
	//region. Active display lines only allow external VRAM access at specific access
	//slots, while non-active display lines allow free access to VRAM, except where memory
	//refresh cycles occur. Note that the active display region is defined by vblank,
	//which is actually first asserted at vcounter=0x1FF, meaning there is one additional
	//line at the start of the display. This extra line is where sprite mapping and
	//pattern data is latched for the first line of the display.
	//##TODO## We need to check all settings which blank or disable the display here, as
	//they affect access slot timing.
	bool currentLineActiveDisplayRegion = ((vcounterCurrent >= vscanSettings.activeDisplayVCounterFirstValue) && (vcounterCurrent <= vscanSettings.activeDisplayVCounterLastValue)) || (vcounterCurrent == vscanSettings.vcounterMaxValue);
	bool nextLineActiveDisplayRegion = ((vcounterCurrent+1 >= vscanSettings.activeDisplayVCounterFirstValue) && (vcounterCurrent+1 <= vscanSettings.activeDisplayVCounterLastValue)) || (vcounterCurrent+1 == vscanSettings.vcounterMaxValue);

	//Determine when the next access slot will occur, based on the hcounter position and
	//the current screen mode settings. Note that we can safely assume the horizontal
	//screen mode doesn't change when making these calculations. Also note that hardware
	//tests have shown that there are in fact 5 refresh slots in H32 mode in non-active
	//lines, and 6 refresh slots in H40 mode in non-active lines. This is one more than
	//there is during an active line.
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
			//|  5 |  0x1F5 |
			//---------------
			if     ((hcounterCurrent >= 0x035) && (hcounterCurrent <= 0x03A)) {nextAccessSlotHCounter = 0x03B;}
			else if((hcounterCurrent >= 0x075) && (hcounterCurrent <= 0x07A)) {nextAccessSlotHCounter = 0x07B;}
			else if((hcounterCurrent >= 0x0B5) && (hcounterCurrent <= 0x0BA)) {nextAccessSlotHCounter = 0x0BB;}
			else if((hcounterCurrent >= 0x0F5) && (hcounterCurrent <= 0x0FA)) {nextAccessSlotHCounter = 0x0FB;}
			else if((hcounterCurrent >= 0x1F5) && (hcounterCurrent <= 0x1FA)) {nextAccessSlotHCounter = 0x1FB;}
			else
			{
				//If none of the above cases apply, our next access slot is on the next
				//odd hcounter value.
				unsigned int hcounterStepsToAdd = ((hcounterCurrent % 2) == 0)? 1: 2;
				nextAccessSlotHCounter = AddStepsToHCounter(hscanSettings, hcounterCurrent, hcounterStepsToAdd);

				//If we're at the end of a non-display line, and the vcounter is going to
				//be incremented before the next access slot and bring us into an active
				//display line, set the next access slot based on the first allowed access
				//slot after vcounter increment in an active display line for this screen
				//mode.
				if(displayEnabled && nextLineActiveDisplayRegion && ((hcounterCurrent < hscanSettings.vcounterIncrementPoint) && (nextAccessSlotHCounter >= hscanSettings.vcounterIncrementPoint)))
				{
					nextAccessSlotHCounter = 0x121;
				}
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
			//|  6 |  0x1F2 |
			//---------------
			if     ((hcounterCurrent >= 0x030) && (hcounterCurrent <= 0x035)) {nextAccessSlotHCounter = 0x036;}
			else if((hcounterCurrent >= 0x070) && (hcounterCurrent <= 0x075)) {nextAccessSlotHCounter = 0x076;}
			else if((hcounterCurrent >= 0x0B0) && (hcounterCurrent <= 0x0B5)) {nextAccessSlotHCounter = 0x0B6;}
			else if((hcounterCurrent >= 0x0F0) && (hcounterCurrent <= 0x0F5)) {nextAccessSlotHCounter = 0x0F6;}
			else if((hcounterCurrent >= 0x130) && (hcounterCurrent <= 0x135)) {nextAccessSlotHCounter = 0x136;}
			else if((hcounterCurrent >= 0x1F0) && (hcounterCurrent <= 0x1F5)) {nextAccessSlotHCounter = 0x1F6;}
			else
			{
				//If none of the above cases apply, our next access slot is on the next
				//even hcounter value.
				unsigned int hcounterStepsToAdd = ((hcounterCurrent % 2) != 0)? 1: 2;
				nextAccessSlotHCounter = AddStepsToHCounter(hscanSettings, hcounterCurrent, hcounterStepsToAdd);

				//If we're at the end of a non-display line, and the vcounter is going to
				//be incremented before the next access slot and bring us into an active
				//display line, set the next access slot based on the first allowed access
				//slot after vcounter increment in an active display line for this screen
				//mode.
				if(displayEnabled && nextLineActiveDisplayRegion && ((hcounterCurrent < hscanSettings.vcounterIncrementPoint) && (nextAccessSlotHCounter >= hscanSettings.vcounterIncrementPoint)))
				{
					nextAccessSlotHCounter = 0x1CE;
				}
			}
		}
	}
	else
	{
		if(!screenModeRS1Current)
		{
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
			//| 14 |  0x105 | //Vcounter increments at 0x10A
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
				//to occur on the vcounter increment point, aligned to the nearest
				//following odd hcounter value.
				nextAccessSlotHCounter = hscanSettings.vcounterIncrementPoint;
				nextAccessSlotHCounter += ((nextAccessSlotHCounter % 2) != 0)? 0: 1;
			}
			else if(hcounterCurrent < 0x121) {nextAccessSlotHCounter = 0x121;}
			else if(hcounterCurrent < 0x1E7) {nextAccessSlotHCounter = 0x1E7;}
			else                             {nextAccessSlotHCounter = 0x005;}
		}
		else
		{
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
			//| 17 |  0x142 | //Vcounter increments at 0x14A
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
				//to occur on the vcounter increment point, aligned to the nearest
				//following even hcounter value.
				nextAccessSlotHCounter = hscanSettings.vcounterIncrementPoint;
				nextAccessSlotHCounter += ((nextAccessSlotHCounter % 2) != 0)? 1: 0;
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
