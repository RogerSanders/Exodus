#ifndef __VERSIONNO_H__
#define __VERSIONNO_H__
#include <string>

//##TODO## Implement this for use on our plugin interface
class VersionNo
{
public:
	// Constructors
	inline VersionNo(unsigned int major, unsigned int minor = 0);
	inline VersionNo(unsigned int major, unsigned int minor, unsigned int patch);
	inline VersionNo(unsigned int major, unsigned int minor, unsigned int patch, unsigned int build);

	// Build version functions
	unsigned int GetMajorVersionNo() const;
	unsigned int GetMinorVersionNo() const;
	unsigned int GetPatchVersionNo() const;
	unsigned int GetBuildVersionNo() const;
	std::wstring GetVersionString() const;

private:
	unsigned int _major;
	unsigned int _minor;
	unsigned int _patch;
	unsigned int _build;
	bool _patchDefined;
	bool _buildDefined;
};

#endif
