#include "QSPISimulationDataGenerator.h"
#include "QSPIAnalyzerSettings.h"
#include "QSPIAnalyzerCommands.h"

#include <AnalyzerHelpers.h>

QSPISimulationDataGenerator::QSPISimulationDataGenerator()
:	mSerialText( "garbage!" ),
	mStringIndex( 0 )
{
}

QSPISimulationDataGenerator::~QSPISimulationDataGenerator()
{
}

void QSPISimulationDataGenerator::Initialize(U32 simulation_sample_rate, QSPIAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mClockGenerator.Init(simulation_sample_rate / 10, simulation_sample_rate);

	mDQ0 = mQSPISimulationChannels.Add(settings->mDQ0Channel, mSimulationSampleRateHz, BIT_LOW);
	mDQ1 = mQSPISimulationChannels.Add(settings->mDQ1Channel, mSimulationSampleRateHz, BIT_LOW);
	mDQ2 = mQSPISimulationChannels.Add(settings->mDQ2Channel, mSimulationSampleRateHz, BIT_LOW);
	mDQ3 = mQSPISimulationChannels.Add(settings->mDQ3Channel, mSimulationSampleRateHz, BIT_LOW);
	mClock = mQSPISimulationChannels.Add(settings->mClockChannel, mSimulationSampleRateHz, mSettings->mClockInactiveState);
	mEnable = mQSPISimulationChannels.Add(settings->mEnableChannel, mSimulationSampleRateHz, BIT_LOW);

	mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10.0)); //insert 10 bit-periods of idle

	mValue = 0;

}

U32 QSPISimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mClock->GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		for (U64 i = 0; i<40; i++) {
			U64 dataarray[] = { 0xDE, 0xAD, 0xBE, 0xEF };
			CreateQSPITransaction(GetQSPICommand(i), 0xBEADED, dataarray, sizeof(dataarray) / sizeof(U64), mSettings->mModeState);
			mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(20.0)); //insert idle
		}
		
		mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(1000.0)); //insert idle
	}

	*simulation_channels = mQSPISimulationChannels.GetArray();
	return mQSPISimulationChannels.GetCount();
}

void QSPISimulationDataGenerator::CreateQSPITransaction(U64 command, U64 address, U64 data[], int datasize, int modestate)
{
	// Begin transaction
	if (mEnable != NULL)
		mEnable->TransitionIfNeeded(BIT_LOW);

	mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(2.0));

	// Convert the address string to three address bytes
	U32 address0 = (address >> 16) & 0xFF;
	U32 address1 = (address >> 8) & 0xFF;
	U32 address2 = (address) & 0xFF;

	// Send the command byte
	switch (modestate) {
	case 1 : OutputWord(command, 0x01); //Extended mode
		break;
	case 2 : OutputWord(command, 0x03); //Dual mode
		break;
	case 3: OutputWord(command, 0x0F); //Quad mode
		break;
	}

	// Send the 3 address bytes
	if (GetQSPICommandAttr(command).AcceptsAddr) {
		switch (modestate) {
		case 1: //Extended Mode
			OutputWord(address0, GetQSPICommandAttr(command).AddressLineMask);
			OutputWord(address1, GetQSPICommandAttr(command).AddressLineMask);
			OutputWord(address2, GetQSPICommandAttr(command).AddressLineMask);
			break;
		case 2: //Dual mode
			OutputWord(address0, 0x03);
			OutputWord(address1, 0x03);
			OutputWord(address2, 0x03);
			break;
		case 3: //Quad mode
			OutputWord(address0, 0x0F);
			OutputWord(address1, 0x0F);
			OutputWord(address2, 0x0F);
			break;
		}
	}

	// Clock the dummy bits
	if (GetQSPICommandAttr(command).UsesDummyCycles) {
		//OutputWord(0x00, 0x01);  // potentially a problem.  this needs to be 8 clock cycles not 8 bits.  some commands require 10 clock cycles.
		for (U32 i = 0; i<mSettings->mDummyCycles; i++)
		{
			mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
			mClock->Transition();  //data valid

			mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
			mClock->Transition();  //data invalid
		}
	}

	// Send / recieve the data
	if (GetQSPICommandAttr(command).HasData) {
		switch (modestate) {
		case 1: //Extended Mode
			if (GetQSPICommandAttr(command).isWrite) {
				for (int i = 0; i<datasize; i++) {
					OutputWord(data[i], GetQSPICommandAttr(command).DataLineMask);
				}
			}
			else {
				OutputWord(0xAA, GetQSPICommandAttr(command).DataLineMask);
			}
			break;
		case 2: //Dual mode
			for (int i = 0; i<datasize; i++) {
				OutputWord(data[i], 0x03);
			}
			break;
		case 3: //Quad mode
			for (int i = 0; i<datasize; i++) {
				OutputWord(data[i], 0x0F);
			}
			break;
		}



	}

	// End Transaction
	if (mEnable != NULL)
		mEnable->TransitionIfNeeded(BIT_HIGH);

}


void QSPISimulationDataGenerator::OutputWord(U64 data, int pinmask)
{
	// this currently produces garbage data (not valid qspi)

	BitExtractor data_bits(data, AnalyzerEnums::MsbFirst, 8); // 8 bits per transfer


  switch (pinmask) { // pin mask defines which pins are used for the operation
    case 0x01: // using DQ0
      for (U32 i = 0; i<8; i++)
			{
				if (mDQ0 != NULL)
					mDQ0->TransitionIfNeeded(data_bits.GetNextBit());

				mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
				mClock->Transition();  //data valid

				mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
				mClock->Transition();  //data invalid
			}
      break;
    case 0x02: // using DQ1
		for (U32 i = 0; i<8; i++)
		{
			if (mDQ1 != NULL)
				mDQ1->TransitionIfNeeded(data_bits.GetNextBit());

			mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
			mClock->Transition();  //data valid

			mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
			mClock->Transition();  //data invalid
		}
      break;

    case 0x03: // using DQ0 and DQ1
      for (U32 i = 0; i<4; i++)
			{
				if (mDQ1 != NULL)
					mDQ1->TransitionIfNeeded(data_bits.GetNextBit());
				if (mDQ0 != NULL)
					mDQ0->TransitionIfNeeded(data_bits.GetNextBit());
				mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
				mClock->Transition();  //data valid
				mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
				mClock->Transition();  //data invalid
			}

      break;
    case 0x0F: // using DQ0 through DQ3
      for (U32 i = 0; i<2; i++)
			{
				if (mDQ3 != NULL)
					mDQ3->TransitionIfNeeded(data_bits.GetNextBit());
				if (mDQ2 != NULL)
					mDQ2->TransitionIfNeeded(data_bits.GetNextBit());
				if (mDQ1 != NULL)
					mDQ1->TransitionIfNeeded(data_bits.GetNextBit());
				if (mDQ0 != NULL)
					mDQ0->TransitionIfNeeded(data_bits.GetNextBit());

				mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
				mClock->Transition();  //data valid
				mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
				mClock->Transition();  //data invalid
			}
  }

  // Set all lines low
	if (mDQ0 != NULL)
		mDQ0->TransitionIfNeeded(BIT_LOW);

	if (mDQ1 != NULL)
		mDQ1->TransitionIfNeeded(BIT_LOW);

	if (mDQ2 != NULL)
		mDQ2->TransitionIfNeeded(BIT_LOW);

	if (mDQ3 != NULL)
		mDQ3->TransitionIfNeeded(BIT_LOW);

	mQSPISimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(1.0));
}
