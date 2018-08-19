#ifndef __ISN76489_H__
#define __ISN76489_H__
#include "GenericAccess/GenericAccess.pkg"
#include <string>

class ISN76489 :public virtual IGenericAccess
{
public:
	// Enumerations
	enum class ISN76489DataSource;

	// Constants
	static const unsigned int ChannelCount = 4;
	static const unsigned int NoiseChannelNo = 3;
	static const unsigned int ToneRegisterBitCount = 10;
	static const unsigned int VolumeRegisterBitCount = 4;
	static const unsigned int NoiseRegisterBitCount = 3;

public:
	// Interface version functions
	static inline unsigned int ThisISN76489Version() { return 1; }
	virtual unsigned int GetISN76489Version() const = 0;

	// Raw register functions
	inline unsigned int GetVolumeRegisterExternal(unsigned int channelNo) const;
	inline void SetVolumeRegisterExternal(unsigned int channelNo, unsigned int data);
	inline unsigned int GetToneRegisterExternal(unsigned int channelNo) const;
	inline void SetToneRegisterExternal(unsigned int channelNo, unsigned int data);
	inline unsigned int GetNoiseShiftRegisterExternal() const;
	inline void SetNoiseShiftRegisterExternal(unsigned int data);

	// Latched register functions
	inline unsigned int GetLatchedChannelNoExternal() const;
	inline void SetLatchedChannelNoExternal(unsigned int data);
	inline bool GetVolumeRegisterLatchedExternal() const;
	inline void SetVolumeRegisterLatchedExternal(bool data);

	// Device property functions
	inline double GetExternalClockRate() const;
	inline void SetExternalClockRate(double data);
	inline double GetExternalClockDivider() const;
	inline void SetExternalClockDivider(double data);
	inline unsigned int GetShiftRegisterBitCount() const;
	inline void SetShiftRegisterBitCount(unsigned int data);
	inline unsigned int GetShiftRegisterDefaultValue() const;
	inline void SetShiftRegisterDefaultValue(unsigned int data);
	inline unsigned int GetNoiseChannelWhiteNoiseTappedBitMask() const;
	inline void SetNoiseChannelWhiteNoiseTappedBitMask(unsigned int data);
	inline unsigned int GetNoiseChannelPeriodicNoiseTappedBitMask() const;
	inline void SetNoiseChannelPeriodicNoiseTappedBitMask(unsigned int data);

	// Audio logging functions
	inline bool IsAudioLoggingEnabled() const;
	inline void SetAudioLoggingEnabled(bool data);
	inline bool IsChannelAudioLoggingEnabled(unsigned int channelNo) const;
	inline void SetChannelAudioLoggingEnabled(unsigned int channelNo, bool data);
	inline std::wstring GetAudioLoggingOutputPath() const;
	inline void SetAudioLoggingOutputPath(const std::wstring& data);
	inline std::wstring GetChannelAudioLoggingOutputPath(unsigned int channelNo) const;
	inline void SetChannelAudioLoggingOutputPath(unsigned int channelNo, const std::wstring& data);
};

#include "ISN76489.inl"
#endif
