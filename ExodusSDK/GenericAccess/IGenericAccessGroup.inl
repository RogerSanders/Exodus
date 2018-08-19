//----------------------------------------------------------------------------------------------------------------------
// Parent functions
//----------------------------------------------------------------------------------------------------------------------
void IGenericAccessGroup::SetParentForTargetEntry(IGenericAccessGroupEntry* entry, IGenericAccessGroup* parent) const
{
	entry->SetParent(parent);
}
