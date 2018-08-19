#ifndef __FILESTREAMREFERENCE_H__
#define __FILESTREAMREFERENCE_H__
#include "StreamInterface/StreamInterface.pkg"
#include "IGUIExtensionInterface.h"

class FileStreamReference
{
public:
	//Constructors
	inline FileStreamReference(const IGUIExtensionInterface& guiExtensionInterface);
	inline ~FileStreamReference();

	//File handling functions
	inline bool OpenExistingFileForRead(const std::wstring& path);
	inline void CloseFileStream();

	//Dereference operators
	inline const Stream::IStream& operator*() const;
	inline Stream::IStream& operator*();
	inline const Stream::IStream& operator->() const;
	inline Stream::IStream& operator->();

private:
	const IGUIExtensionInterface& _guiExtensionInterface;
	Stream::IStream* _stream;
};

#include "FileStreamReference.inl"
#endif
