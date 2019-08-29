#include "QSPIAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "QSPIAnalyzer.h"
#include "QSPIAnalyzerSettings.h"
#include "QSPIAnalyzerCommands.h"
#include <iostream>
#include <sstream>

QSPIAnalyzerResults::QSPIAnalyzerResults( QSPIAnalyzer* analyzer, QSPIAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

QSPIAnalyzerResults::~QSPIAnalyzerResults()
{
}

void QSPIAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{

	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	switch (frame.mType)
	{
	case FrameTypeCommand:
		{
		char number_str[128];
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, number_str, 128);

		AddResultString(number_str);

		std::stringstream ss;
		ss << "Cmd: " << number_str;
		AddResultString(ss.str().c_str());
		ss.str("");

		ss << "Command: " << number_str << " " << GetQSPICommandAttr(frame.mData1).CommandName;
		AddResultString(ss.str().c_str());
		}
		break;
	case FrameTypeAddress:
	{
		char number_str[128];
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, number_str, 128);

		AddResultString(number_str);

		std::stringstream ss;
		ss << "Addr: " << number_str;
		AddResultString(ss.str().c_str());
		ss.str("");

		ss << "Address: " << number_str;
		AddResultString(ss.str().c_str());
	}
	case FrameTypeDummy:
	{
		std::stringstream ss;
		ss << "Dummy";
		AddResultString(ss.str().c_str());
		ss.str("");

		ss << "Dummy Cycles";
		AddResultString(ss.str().c_str());
	}
	case FrameTypeData:
	{
		char number_str[128];
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, number_str, 128);

		AddResultString(number_str);

		std::stringstream ss;
		ss << "Data: " << number_str;
		AddResultString(ss.str().c_str());
		ss.str("");

		ss << "Data: " << number_str;
		AddResultString(ss.str().c_str());
	}
	break;

	default:
		break;
	}

}

void QSPIAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::stringstream ss;

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();
	U64 num_frames = GetNumFrames();

	void* f = AnalyzerHelpers::StartFile(file);

	ss << "Time [s],Value" << std::endl;

	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

		ss << time_str << "," << number_str << std::endl;

		AnalyzerHelpers::AppendToFile((U8*)ss.str().c_str(), ss.str().length(), f);
		ss.str(std::string());

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			AnalyzerHelpers::EndFile(f);
			return;
		}
	}

	UpdateExportProgressAndCheckForCancel(num_frames, num_frames);
	AnalyzerHelpers::EndFile(f);
}

void QSPIAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
	ClearTabularText();
	Frame frame = GetFrame( frame_index );
	
	switch (frame.mType)
	{
	case FrameTypeCommand:
	{
		char number_str[128];
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, number_str, 128);

		std::stringstream ss;

		ss << "Command: " << number_str << " " << GetQSPICommandAttr(frame.mData1).CommandName;
		AddTabularText(ss.str().c_str());
		break;
	}
	case FrameTypeAddress:
	{
		char number_str[128];
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, number_str, 128);

		std::stringstream ss;

		ss << "Address: " << number_str;
		AddTabularText(ss.str().c_str());
		break;
	}
	case FrameTypeDummy:
	{
		std::stringstream ss;

		ss << "Dummy Cycles";
		AddTabularText(ss.str().c_str());
		break;
	}
	case FrameTypeData:
	{
		char number_str[128];
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, number_str, 128);

		std::stringstream ss;

		ss << "Data: " << number_str;
		AddTabularText(ss.str().c_str());
		break;
	}

	default:
		break;
	}

}

void QSPIAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}

void QSPIAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}