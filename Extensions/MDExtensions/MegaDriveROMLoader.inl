//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct MegaDriveROMLoader::MegaDriveROMHeader
{
	std::string segaString;        //0x100-0x110
	std::string copyrightString;   //0x110-0x120
	std::string gameTitleJapan;    //0x120-0x150
	std::string gameTitleOverseas; //0x150-0x180
	std::string versionString;     //0x180-0x18E
	unsigned short checksum;       //0x18E-0x190
	std::string controllerString;  //0x190-0x1A0
	unsigned int romLocationStart; //0x1A0-0x1A4
	unsigned int romLocationEnd;   //0x1A4-0x1A8
	unsigned int ramLocationStart; //0x1A8-0x1AC
	unsigned int ramLocationEnd;   //0x1AC-0x1B0
	unsigned char bramSetting[4];  //0x1B0-0x1B4
	unsigned int bramLocationStart;//0x1B4-0x1B8
	unsigned int bramLocationEnd;  //0x1B8-0x1BC
	std::string bramUnusedData;    //0x1BC-0x1C0
	std::string unknownString;     //0x1C0-0x1F0
	std::string regionString;      //0x1F0-0x200
	unsigned int fileSize;
};
