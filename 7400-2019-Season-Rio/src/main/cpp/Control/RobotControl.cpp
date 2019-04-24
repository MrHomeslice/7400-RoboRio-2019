#include "RobotControl.h"
#include <frc/GenericHID.h>
#include "..\MeeseeksProperties.h"
#include "..\DataTable\TableController.h"

void SetEncoderPosition(WPI_TalonSRX &talon, int position);

extern MeeseeksProperties g_mp;
extern TableController	  g_tc;

RobotControl::RobotControl()
			 			: m_driveJoystick(JOYSTICK_1),
			   			m_cargoControl(15, 14, 13, 12),
			   			m_hatchControl(17, 16),
			   			m_cargoSwitch(0), m_gamePieceSwitch(1), m_calibrationSwitch(3)
{
	m_x 	  = -10.0;
	m_y 	  = -10.0;
	m_z     = -10.0;
	m_lastX = 0.0;
	m_lastY = 0.0;
	m_lastZ = 0.0;

	m_bXYZChanged = false;
<<<<<<< HEAD
=======

	m_printCounter = 0;

	m_bAlligning = false;
	m_bRestartRobotCode = true;
>>>>>>> parent of e7bbbf0... Final Hofstra 1:20pm 3-30-19
}

/**
* Initialize values to be used by subsystems.
*
* @param boolean value to flip the pneumatics in CargoControl Initialize()
*/
void RobotControl::Initialize(bool bFlip)
{
	m_ladder.Initialize();
	m_cargoControl.Initialize(bFlip);
	m_hatchControl.Initialize();

	SetEncoderPosition(*m_ladder.m_pDrives[0], 0);
}

double RobotControl::Deadband(double input, double deadbandHalfWidth)
{
	double yInercept;

	if (input > deadbandHalfWidth)
		yInercept =  deadbandHalfWidth / (deadbandHalfWidth - 1.0);
	else if (input < -deadbandHalfWidth)
		yInercept = deadbandHalfWidth / (1.0 - deadbandHalfWidth);
	else
		return 0.0;

	double slope = 1.0/(1.0 - deadbandHalfWidth);

	return input * slope + yInercept;
}

/**
* Periodic function to run every 20ms.
*
* @param boolean value from Robot.cpp whether the robot is in auton or teleop
*/
bool RobotControl::Periodic(bool bTeleop)
{
	m_driveJoystick.Periodic();

	m_y	= -Deadband(m_driveJoystick.GetY(), g_mp.m_deadbandY);
	m_x =  Deadband(m_driveJoystick.GetX(), g_mp.m_deadbandX);
	//m_z =  Deadband(m_driveJoystick.GetZ(), g_mp.m_deadbandZ);
	m_z = Deadpool(m_driveJoystick.GetZ(), 2.0 + (1.0/3.0));

#ifdef USING_POV
	m_pov =  m_driveJoystick.GetPOV();
#endif
	m_bRobotCentric = m_driveJoystick.CentricityToggle()->Value() == 1 ? true : false;

	ReadButtons();

	m_cargoControl.Periodic();
	m_hatchControl.Periodic();	
	m_ladder.Periodic();

	if ((m_lastX != m_x) || (m_lastY != m_y) || (m_lastZ != m_z)) 
	{
		m_bXYZChanged = true;
		m_lastX = m_x;
		m_lastY = m_y;
		m_lastZ = m_z;
	}
	else
	{
		m_bXYZChanged = false;
	}

	return m_bXYZChanged;
}

bool RobotControl::XYZChanged()
{
	return m_bXYZChanged;
}

/**
* @return Joystick X value
*/
double RobotControl::X()
{
	return m_x;
}

/**
* @return Joystick Y value
*/
double RobotControl::Y()
{
	return m_y;
}

/**
* @return Joystick Z value
*/
double RobotControl::Z()
{
	return m_z / 2;
}

/**
* @return Value that reflects the state of the joystick slider.
*/
bool RobotControl::Cargo()
{
	return m_bCargo;
}

/**
 * Reads the buttons from the joystick
 */
void RobotControl::ReadButtons()
{
	m_bAction = m_driveJoystick.Action()->Changed() && m_driveJoystick.Action()->Pressed();

	m_bAbort = m_driveJoystick.Abort()->Pressed() && m_driveJoystick.Abort()->Changed();

	m_bCargo = m_driveJoystick.GetThrottle() > 0.0;
	
	if (m_bCargo)
	{
		if (m_cargoControl.GetCargoState() == eCargoStateFlipped)
		{
			if (m_driveJoystick.CargoShipHeight()->Changed() && m_driveJoystick.CargoShipHeight()->Pressed())
			{
				m_ladderTargetHeight = eLadderHeightCargoShip;
			}

			if (m_driveJoystick.TopHeight()->Changed() && m_driveJoystick.TopHeight()->Pressed())
			{							
				m_ladderTargetHeight = eLadderHeightCargoTop;
			}
		
			if (m_driveJoystick.MidHeight()->Changed() && m_driveJoystick.MidHeight()->Pressed())
			{			
				m_ladderTargetHeight = eLadderHeightCargoMid;
			}

			if (m_driveJoystick.BottomHeight()->Changed() && m_driveJoystick.BottomHeight()->Pressed())
			{			
				m_ladderTargetHeight = eLadderHeightCargoBottom;
			}
		}
	}
	else
	{
		if (m_driveJoystick.TopHeight()->Changed() && m_driveJoystick.TopHeight()->Pressed() && m_hatchControl.GetHatchGrabState() == eHatchGrabStateAcquried)
		{			
			m_ladderTargetHeight = eLadderHeightHatchTop;
		}
		
		if (m_driveJoystick.MidHeight()->Changed() && m_driveJoystick.MidHeight()->Pressed() && m_hatchControl.GetHatchGrabState() == eHatchGrabStateAcquried)
		{			
			m_ladderTargetHeight = eLadderHeightHatchMid;
		}

		if (m_driveJoystick.BottomHeight()->Changed() && m_driveJoystick.BottomHeight()->Pressed() && m_hatchControl.GetHatchGrabState() == eHatchGrabStateAcquried)
		{			
			m_ladderTargetHeight = eLadderHeightHatchBottom;
		}
	}

	if (m_driveJoystick.CentricityToggle()->Changed() && m_driveJoystick.CentricityToggle()->Pressed())
	{
		printf("Centricity Value: %d\n", m_driveJoystick.CentricityToggle()->Value());
	}
}

/**
* @return true if the robot is in robot centric, false if in field centric.
*/
bool RobotControl::RobotCentric()
{
<<<<<<< HEAD
	return m_driveJoystick.CentricityToggle()->Value();
=======
	return m_driveJoystick.CentricityToggle()->Value() || m_bAlligning;
}

void RobotControl::CargoEjected()
{
	
	m_ladderTargetHeight = eLadderHeightGround;
>>>>>>> parent of e7bbbf0... Final Hofstra 1:20pm 3-30-19
}

/**
* Currently not used.
*
* @return if the ladder is at the desired target height
*/
bool RobotControl::IsLadderAtHeight()
{
	double delta = fabs(GetLadderPosition() - m_ladder.GetTargetLadderPosition());
	return delta < MAX_LADDER_POSITION_ERROR && GetLadderPosition() > (LADDER_HATCH_BOTTOM_HEIGHT - MAX_LADDER_POSITION_ERROR);
}

/**
* @return Cargo ship encoder height constant.
*/
LadderHeight RobotControl::GetCargoShipCargoHeight()
{
	return eLadderHeightCargoShip;
}

/**
* @return Ground encoder height constant.
*/
LadderHeight RobotControl::GetGroundHeight()
{
	return eLadderHeightGround;
}

/**
* @return Current ladder encoder position.
*/
int RobotControl::GetLadderPosition()
{
	return m_ladder.m_pDrives[0]->GetSelectedSensorPosition();
}

/**
* @return Z value raised to the exponent
*/
double RobotControl::Deadpool(double z, double exponent)
{
	if(z > 0)
		return pow(z, exponent);
	else
		return -pow(-z, exponent);
}

/**
* Sets the encoder position of the desired talon.
* If the encoder position was not set after one second, a message is printed to the log to be looked at after the match
* 
* @param &talon Change the encoder position of &talon
* @param position Assigns &talon the int value position
*/
void SetEncoderPosition(WPI_TalonSRX &talon, int position)
{
	ctre::phoenix::ErrorCode error = talon.SetSelectedSensorPosition(position, 0, 1000);

	if(error != ctre::phoenix::ErrorCode::OK)
		printf("*************  %s was not able to go to %d  *************\n", talon.GetName().c_str(), position);
}