#ifndef SRC_MEESEEKSJOYSTICK_H_
#define SRC_MEESEEKSJOYSTICK_H_

#include <frc/Joystick.h>
#include "JoystickValueButton.h"
#include "JoystickButton.h"
#include "JoystickUpDownButton.h"
#include "JoystickRepeatButton.h"

#include <vector>

class MeeseeksJoystick : public frc::Joystick
{
	public    : MeeseeksJoystick(int port);

							virtual void Initialize();
							virtual void Periodic();

							int GetPOVState();
	protected :
							std::vector<JSButton *> m_buttons;

							int m_oldPOV, m_newPOV, m_POVState;
							bool m_bPOVChanged;
};

#endif
