#ifndef QSPI_ANALYZER_H
#define QSPI_ANALYZER_H

#include <Analyzer.h>
#include "QSPIAnalyzerResults.h"
#include "QSPISimulationDataGenerator.h"

class QSPIAnalyzerSettings;
class ANALYZER_EXPORT QSPIAnalyzer : public Analyzer2
{
public:
	QSPIAnalyzer();
	virtual ~QSPIAnalyzer();
	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

#pragma warning( push )
#pragma warning( disable : 4251 ) //warning C4251: 'SerialAnalyzer::<...>' : class <...> needs to have dll-interface to be used by clients of class

protected: //vars
	std::auto_ptr< QSPIAnalyzerSettings > mSettings;
	std::auto_ptr< QSPIAnalyzerResults > mResults;
	bool mSimulationInitilized;
	QSPISimulationDataGenerator mSimulationDataGenerator;

	AnalyzerChannelData* mDQ0;
	AnalyzerChannelData* mDQ1;
	AnalyzerChannelData* mDQ2;
	AnalyzerChannelData* mDQ3;
	AnalyzerChannelData* mClock;
	AnalyzerChannelData* mEnable;

	U64 mCurrentSample;
	AnalyzerResults::MarkerType mArrowMarker;
	std::vector<U64> mArrowLocations;

	struct ParseResult {
		S64 start;
		S64 end;
		U64 data;
	};

#pragma warning( pop )

protected: //functions
	void Setup();
	void AdvanceToActiveEnableEdge();
	bool IsInitialClockPolarityCorrect();
	void AdvanceToActiveEnableEdgeWithCorrectClockPolarity();
	bool WouldAdvancingTheClockToggleEnable();
	bool IsParseResultError(QSPIAnalyzer::ParseResult result);
	void GetFrame();
	ParseResult GetCommand(U64 CommandLineMask);
	void SaveResults(QSPIAnalyzer::ParseResult return_value, QSPIFrameType frame_type);

	ParseResult GetAddress(U64 AddressLineMask);
	ParseResult GetDummy();
	ParseResult GetData(U64 DataLineMask);


};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //QSPI_ANALYZER_H
