//----------------------------------------------------------------------------------------
//Access functions
//----------------------------------------------------------------------------------------
void ITimedBufferInt::GetLatestBufferCopy(std::vector<DataType>& buffer) const
{
	unsigned int bufferSize = Size();
	buffer.resize(bufferSize);
	if(bufferSize > 0)
	{
		GetLatestBufferCopy(&buffer[0], bufferSize);
	}
}
