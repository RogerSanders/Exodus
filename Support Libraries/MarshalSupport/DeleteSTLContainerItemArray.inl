namespace MarshalSupport {
namespace Internal {

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const ElementType* elementPointer)
{
	//Note that we allocated this memory buffer as a raw byte array, so we free it in the
	//same form.
	unsigned char* itemArrayAsByteArray = (unsigned char*)itemArray;
	delete[] itemArrayAsByteArray;
}

} //Close namespace Internal
} //Close namespace MarshalSupport
