#ifndef QSPI_SIMULATION_DATA_GENERATOR
#define QSPI_SIMULATION_DATA_GENERATOR

#include <AnalyzerHelpers.h>

class QSPIAnalyzerSettings;

class QSPISimulationDataGenerator
{
public:
	QSPISimulationDataGenerator();
	~QSPISimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, QSPIAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	QSPIAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;
	U64 mValue;

protected:
	ClockGenerator mClockGenerator;

	void CreateQSPITransaction(U64 command, U64 address, U64 data[], int datasize, int modestate);
	void OutputWord(U64 data, int pinmask);

	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptorGroup mQSPISimulationChannels;
	SimulationChannelDescriptor* mDQ0;
	SimulationChannelDescriptor* mDQ1;
	SimulationChannelDescriptor* mDQ2;
	SimulationChannelDescriptor* mDQ3;
	SimulationChannelDescriptor* mClock;
	SimulationChannelDescriptor* mEnable;

};
#endif //QSPI_SIMULATION_DATA_GENERATOR
