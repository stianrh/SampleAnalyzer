#ifndef QSPI_ANALYZER_RESULTS
#define QSPI_ANALYZER_RESULTS

#include <AnalyzerResults.h>

enum QSPIFrameType { FrameTypeCommand, FrameTypeAddress, FrameTypeAlt, FrameTypeDummy, FrameTypeData };

class QSPIAnalyzer;
class QSPIAnalyzerSettings;

class QSPIAnalyzerResults : public AnalyzerResults
{
public:
	QSPIAnalyzerResults( QSPIAnalyzer* analyzer, QSPIAnalyzerSettings* settings );
	virtual ~QSPIAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	QSPIAnalyzerSettings* mSettings;
	QSPIAnalyzer* mAnalyzer;
};

#endif //QSPI_ANALYZER_RESULTS
