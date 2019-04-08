#ifndef DELIVERY_SYSTEM_H_INCLUDED
#define DELIVERY_SYSTEM_H_INCLUDED

#include <ctre/Phoenix.h>
#include <frc/wpilib.h>
#include "..\Pneumatics\Pneumatics.h"
#include "..\CanSimulator.h"
#include "..\include\Defines.h"

#define EJECT_TIME 		  25
#define FLIP_TIME 		  50
#define HARD_PULL_IN_TIME 250

#define CARGO_CURRENT_THRESHOLD  5.0
#define CARGO_CURRENT_ITERATIONS 25

#define CAPTURE_TILT_CURRENT_THRESHOLD  3.0
#define CAPTURE_TILT_CURRENT_ITERATIONS 5
#define CAPTURE_TILT_DOWN_POSITION 		2300

typedef enum
{
	eCargoStateInitialize = 0,
    eCargoStateStationIntake,
    eCargoStateHardPullIn,
    eCargoStateSoftPullIn,
    eCargoStateFlipping,
    eCargoStateFlipped,
    eCargoStateEjecting,
    eCargoStateEjected,
    eCargoStateEmpty
} CargoState;

typedef enum
{
    eCargoCaptureStateInitialize = 0,
    eCargoCaptureStateToReady,
    eCargoCaptureStateUp,
    eCargoCaptureStateMovingUp,
    eCargoCaptureStateDown,
    eCargoCaptureStateMovingDown
} CargoCaptureState;

class CargoControl
{
	public    : CargoControl(int leftGrabberID, int rightGrabberID, int tiltID, int intakeID);

				void Initialize(bool bFlip);
				void Periodic();
				
				CargoState 		  GetCargoState();
				CargoCaptureState GetCargoCaptureState();
				Pneumatics   	  m_pneumatics;				
	protected :
				const char* CargoStateToString(CargoState cargoState);

				bool MonitorCaptureMotor(int targetPosition, int maxError, double maxCurrent);

				WPI_TalonSRX 	  m_leftGrabberMotor, m_rightGrabberMotor;
       		 	WPI_TalonSRX 	  m_cargoCaptureTilt, m_cargoCaptureIntake;
				CargoState   	  m_cargoState, m_lastCargoState;
				CargoCaptureState m_cargoCaptureState;

				int m_ejectCounter, m_flippingCounter;

				int m_leftEncoderPosition, m_rightEncoderPosition;
        		int m_currentCounter, m_cargoStateCounter, m_hatchGrabCounter, m_captureCurrentCounter, m_DICounter;
        		int m_hatchGrabInitialPosition, m_cargoCaptureInitialPosition;
				int m_printCounter, m_hardPullInCounter;

        		bool m_bCargoIntakeTestWaiting;
        		bool m_bGlobeTrotterMode;
        		bool m_bFlipped;
				bool m_bChangeHeight;
};

#endif