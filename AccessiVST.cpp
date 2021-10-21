/*
AccessiVST
Copyright (C) 2021  Datajake

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "AccessiVST.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#include "Tolk.h"
#endif

AccessiVST::AccessiVST (AudioEffect* effect)
: AEffEditor (effect)
{
	memset(&vstrect, 0, sizeof(vstrect));
	memset(&paramDisplay, 0, sizeof(paramDisplay));
	memset(&paramLabel, 0, sizeof(paramLabel));
	memset(&paramName, 0, sizeof(paramName));
	memset(&progName, 0, sizeof(progName));
	memset(&speakText, 0, sizeof(speakText));
	paramIndex = 0;
	paramValue = 0;
	progIndex = 0;
	speechMuted = false;
	disabled = false;
	if (effect)
	{
		effectData = effect->getAeffect ();
		effect->setEditor (this);
	}
#ifdef _WIN32
	Tolk_TrySAPI(true);
	Tolk_Load();
#endif
}

AccessiVST::~AccessiVST ()
{
#ifdef _WIN32
	Tolk_Unload();
#endif
}

bool AccessiVST::getRect (ERect** rect)
{
	*rect = &vstrect;
	return true;
}

bool AccessiVST::open (void* ptr)
{
	AEffEditor::open (ptr);
	sprintf(speakText, "Editor opened");
	speak();
	return true;
}

void AccessiVST::close ()
{
	sprintf(speakText, "Editor closed");
	speak();
	AEffEditor::close ();
}

void AccessiVST::idle ()
{
}

bool AccessiVST::onKeyDown (VstKeyCode& keyCode)
{
	memset(&paramDisplay, 0, sizeof(paramDisplay));
	memset(&paramLabel, 0, sizeof(paramLabel));
	memset(&paramName, 0, sizeof(paramName));
	memset(&progName, 0, sizeof(progName));
	if (disabled && keyCode.virt != VKEY_INSERT)
	{
		return false;
	}
	switch(keyCode.virt)
	{
	case 0:
		return false;
	case VKEY_RIGHT:
		if (effect)
		{
			paramIndex++;
			if (paramIndex >= effectData->numParams)
			{
				paramIndex = effectData->numParams-1;
			}
			else if (paramIndex < 0)
			{
				paramIndex = 0;
			}
			effect->getParameterName (paramIndex, paramName);
		}
		sprintf(speakText, "%s", paramName);
		break;
	case VKEY_LEFT:
		if (effect)
		{
			paramIndex--;
			if (paramIndex >= effectData->numParams)
			{
				paramIndex = effectData->numParams-1;
			}
			else if (paramIndex < 0)
			{
				paramIndex = 0;
			}
			effect->getParameterName (paramIndex, paramName);
		}
		sprintf(speakText, "%s", paramName);
		break;
	case VKEY_UP:
		if (effect)
		{
			paramValue = effect->getParameter (paramIndex);
			paramValue += 0.001f;
			if (paramValue > 1)
			{
				paramValue = 1;
			}
			else if (paramValue < 0)
			{
				paramValue = 0;
			}
			effect->setParameter (paramIndex, paramValue);
			effect->getParameterDisplay (paramIndex, paramDisplay);
			effect->getParameterLabel (paramIndex, paramLabel);
		}
		sprintf(speakText, "%s %s", paramDisplay, paramLabel);
		break;
	case VKEY_DOWN:
		if (effect)
		{
			paramValue = effect->getParameter (paramIndex);
			paramValue -= 0.001f;
			if (paramValue > 1)
			{
				paramValue = 1;
			}
			else if (paramValue < 0)
			{
				paramValue = 0;
			}
			effect->setParameter (paramIndex, paramValue);
			effect->getParameterDisplay (paramIndex, paramDisplay);
			effect->getParameterLabel (paramIndex, paramLabel);
		}
		sprintf(speakText, "%s %s", paramDisplay, paramLabel);
		break;
	case VKEY_PAGEUP:
		if (effect)
		{
			paramValue = effect->getParameter (paramIndex);
			paramValue += 0.01f;
			if (paramValue > 1)
			{
				paramValue = 1;
			}
			else if (paramValue < 0)
			{
				paramValue = 0;
			}
			effect->setParameter (paramIndex, paramValue);
			effect->getParameterDisplay (paramIndex, paramDisplay);
			effect->getParameterLabel (paramIndex, paramLabel);
		}
		sprintf(speakText, "%s %s", paramDisplay, paramLabel);
		break;
	case VKEY_PAGEDOWN:
		if (effect)
		{
			paramValue = effect->getParameter (paramIndex);
			paramValue -= 0.01f;
			if (paramValue > 1)
			{
				paramValue = 1;
			}
			else if (paramValue < 0)
			{
				paramValue = 0;
			}
			effect->setParameter (paramIndex, paramValue);
			effect->getParameterDisplay (paramIndex, paramDisplay);
			effect->getParameterLabel (paramIndex, paramLabel);
		}
		sprintf(speakText, "%s %s", paramDisplay, paramLabel);
		break;
	case VKEY_HOME:
		if (effect)
		{
			paramValue = 1;
			effect->setParameter (paramIndex, paramValue);
			effect->getParameterDisplay (paramIndex, paramDisplay);
			effect->getParameterLabel (paramIndex, paramLabel);
		}
		sprintf(speakText, "%s %s", paramDisplay, paramLabel);
		break;
	case VKEY_END:
		if (effect)
		{
			paramValue = 0;
			effect->setParameter (paramIndex, paramValue);
			effect->getParameterDisplay (paramIndex, paramDisplay);
			effect->getParameterLabel (paramIndex, paramLabel);
		}
		sprintf(speakText, "%s %s", paramDisplay, paramLabel);
		break;
	case VKEY_SPACE:
		if (effect)
		{
			effect->getParameterDisplay (paramIndex, paramDisplay);
			effect->getParameterLabel (paramIndex, paramLabel);
			effect->getParameterName (paramIndex, paramName);
		}
		sprintf(speakText, "%s: %s %s", paramName, paramDisplay, paramLabel);
		break;
	case VKEY_TAB:
		if (effect)
		{
			progIndex = effect->getProgram ();
			effect->getProgramName (progName);
		}
		sprintf(speakText, "Current program name: %s. Current program index: %d", progName, progIndex);
		break;
	case VKEY_RETURN:
		if (effect)
		{
			progIndex = effect->getProgram ();
			progIndex++;
			if (progIndex >= effectData->numPrograms)
			{
				progIndex = effectData->numPrograms-1;
			}
			else if (progIndex < 0)
			{
				progIndex = 0;
			}
			effect->setProgram (progIndex);
			effect->getProgramName (progName);
		}
		sprintf(speakText, "%s", progName);
		break;
	case VKEY_BACK:
		if (effect)
		{
			progIndex = effect->getProgram ();
			progIndex--;
			if (progIndex >= effectData->numPrograms)
			{
				progIndex = effectData->numPrograms-1;
			}
			else if (progIndex < 0)
			{
				progIndex = 0;
			}
			effect->setProgram (progIndex);
			effect->getProgramName (progName);
		}
		sprintf(speakText, "%s", progName);
		break;
	case VKEY_F1:
		if (effect)
		{
			sprintf(speakText, "Current sample rate: %d", (int)effect->getSampleRate ());
		}
		break;
	case VKEY_F2:
		if (effect)
		{
			sprintf(speakText, "Current block size: %d", effect->getBlockSize ());
		}
		break;
	case VKEY_F3:
		if (effect)
		{
			sprintf(speakText, "Number of inputs: %d", effectData->numInputs);
		}
		break;
	case VKEY_F4:
		if (effect)
		{
			sprintf(speakText, "Number of outputs: %d", effectData->numOutputs);
		}
		break;
	case VKEY_F5:
		if (effect)
		{
			sprintf(speakText, "Number of parameters: %d", effectData->numParams);
		}
		break;
	case VKEY_F6:
		if (effect)
		{
			sprintf(speakText, "Number of programs: %d", effectData->numPrograms);
		}
		break;
	case VKEY_F7:
		if (effect)
		{
			effect->suspend ();
			effect->resume ();
			sprintf(speakText, "Plug-in reset");
		}
		break;
	case VKEY_F8:
		if (effect)
		{
			effect->close ();
			effect->open ();
			sprintf(speakText, "Plug-in reset");
		}
		break;
	case VKEY_ALT:
		break;
	case VKEY_CONTROL:
		interrupt();
		return true;
	case VKEY_DELETE:
		if (!speechMuted)
		{
			speechMuted = true;
			sprintf(speakText, "Speech muted");
		}
		else
		{
			speechMuted = false;
			sprintf(speakText, "Speech unmuted");
		}
		speak();
		return true;
	case VKEY_INSERT:
		if (!disabled)
		{
			disabled = true;
			sprintf(speakText, "AccessiVST disabled");
		}
		else
		{
			disabled = false;
			sprintf(speakText, "AccessiVST enabled");
		}
		speak();
		return true;
	default:
		sprintf(speakText, "Key undefined: key code = %d", keyCode.virt);
		speak();
		return false;
	}
	if (speechMuted)
	{
		return true;
	}
	speak();
	return true;
}

bool AccessiVST::onKeyUp (VstKeyCode& keyCode)
{
	return false;
}

void AccessiVST::speak ()
{
#ifdef _WIN32
	int strsize = MultiByteToWideChar(CP_UTF8, 0, speakText, -1, NULL, 0);
	wchar_t *text2speak = new wchar_t[strsize];
	MultiByteToWideChar(CP_UTF8, 0, speakText, -1, (wchar_t *)text2speak, strsize);
	Tolk_Output((wchar_t *)text2speak, true);
	delete[] text2speak;
#endif
}

void AccessiVST::interrupt ()
{
#ifdef _WIN32
	Tolk_Silence();
#endif
}
