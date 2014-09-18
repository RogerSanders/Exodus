#ifndef __VERSIONNO_H__
#define __VERSIONNO_H__

//##TODO## Implement this for use on our plugin interface
class VersionNo
{
public:
	//Constructors
	inline VersionNo(unsigned int amajor, unsigned int aminor = 0);
	inline VersionNo(unsigned int amajor, unsigned int aminor, unsigned int apatch);
	inline VersionNo(unsigned int amajor, unsigned int aminor, unsigned int apatch, unsigned int abuild);

	//Build version functions
	unsigned int GetMajorVersionNo() const;
	unsigned int GetMinorVersionNo() const;
	unsigned int GetPatchVersionNo() const;
	unsigned int GetBuildVersionNo() const;
	std::wstring GetVersionString() const;

private:
	unsigned int major;
	unsigned int minor;
	unsigned int patch;
	unsigned int build;
	bool patchDefined;
	bool buildDefined;
};

#endif
