//----------------------------------------------------------------------------------------
//DPI functions
//----------------------------------------------------------------------------------------
template<class T> T DPIScaleWidth(T pixelWidth)
{
	//Calculate a scale value for pixel values based on the current screen DPI settings
	float dpiScaleX;
	float dpiScaleY;
	DPIGetScreenScaleFactors(dpiScaleX, dpiScaleY);

	//Scale the supplied values based on the DPI scale values
	pixelWidth = (T)(((float)pixelWidth * dpiScaleX) + 0.5f);

	//Return the scaled value
	return pixelWidth;
}

//----------------------------------------------------------------------------------------
template<class T> T DPIScaleHeight(T pixelHeight)
{
	//Calculate a scale value for pixel values based on the current screen DPI settings
	float dpiScaleX;
	float dpiScaleY;
	DPIGetScreenScaleFactors(dpiScaleX, dpiScaleY);

	//Scale the supplied values based on the DPI scale values
	pixelHeight = (T)(((float)pixelHeight * dpiScaleY) + 0.5f);

	//Return the scaled value
	return pixelHeight;
}

//----------------------------------------------------------------------------------------
template<class T> T DPIReverseScaleWidth(T pixelWidth)
{
	//Calculate a scale value for pixel values based on the current screen DPI settings
	float dpiScaleX;
	float dpiScaleY;
	DPIGetScreenScaleFactors(dpiScaleX, dpiScaleY);

	//Scale the supplied values based on the DPI scale values
	pixelWidth = (T)(((float)pixelWidth / dpiScaleX) + 0.5f);

	//Return the scaled value
	return pixelWidth;
}

//----------------------------------------------------------------------------------------
template<class T> T DPIReverseScaleHeight(T pixelHeight)
{
	//Calculate a scale value for pixel values based on the current screen DPI settings
	float dpiScaleX;
	float dpiScaleY;
	DPIGetScreenScaleFactors(dpiScaleX, dpiScaleY);

	//Scale the supplied values based on the DPI scale values
	pixelHeight = (T)(((float)pixelHeight / dpiScaleY) + 0.5f);

	//Return the scaled value
	return pixelHeight;
}
