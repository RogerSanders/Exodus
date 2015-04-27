//----------------------------------------------------------------------------------------
//Parent functions
//----------------------------------------------------------------------------------------
void IGenericAccessGroup::SetParentForTargetEntry(IGenericAccessGroupEntry* entry, IGenericAccessGroup* aparent) const
{
	entry->SetParent(aparent);
}
