#include "XMLEntity.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
void XMLEntity::Initialize()
{
	children.clear();
	attributes.clear();
	binaryDataName.clear();
	binaryDataPresent = false;
}

//----------------------------------------------------------------------------------------
//Parent and child functions
//----------------------------------------------------------------------------------------
void XMLEntity::AddBinaryDataEntitiesToList(std::list<XMLEntity*>& binaryEntityList)
{
	if(binaryDataPresent && !inlineBinaryData)
	{
		binaryEntityList.push_back(this);
	}
	for(EntityList::iterator i = children.begin(); i != children.end(); ++i)
	{
		i->AddBinaryDataEntitiesToList(binaryEntityList);
	}
}

//----------------------------------------------------------------------------------------
//Attribute functions
//----------------------------------------------------------------------------------------
XMLAttribute* XMLEntity::GetAttribute(const std::wstring& name)
{
	for(AttributeList::iterator i = attributes.begin(); i != attributes.end(); ++i)
	{
		if(i->first == name)
		{
			return &i->second;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void XMLEntity::AddAttribute(const XMLAttribute& attribute)
{
	attributes.push_back(AttributeListEntry(attribute.GetName(), attribute));
}
