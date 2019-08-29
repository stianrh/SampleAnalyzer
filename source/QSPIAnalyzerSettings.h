#ifndef QSPI_ANALYZER_SETTINGS
#define QSPI_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class QSPIAnalyzerSettings : public AnalyzerSettings
{
public:
	QSPIAnalyzerSettings();
	virtual ~QSPIAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	void UpdateInterfacesFromSettings();

	Channel mEnableChannel;
	Channel mClockChannel;
	Channel mDQ0Channel;
	Channel mDQ1Channel;
	Channel mDQ2Channel;
	Channel mDQ3Channel;
	BitState mClockInactiveState;
	U32 mModeState;
	U32 mDummyCycles;
	U32 mAddressSize;


protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mEnableChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mClockChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mDQ0ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mDQ1ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mDQ2ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mDQ3ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList >	mClockInactiveStateInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList >	mModeStateInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList >	mDummyCyclesInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList >	mAddressSizeInterface;

};

#endif //QSPI_ANALYZER_SETTINGS
