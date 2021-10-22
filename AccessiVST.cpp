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
	helpMode = false;
	if (effect)
	{
		effectData = effect->getAeffect ();
		effect->setEditor (this);
	}
	speechStartup ();
}

AccessiVST::~AccessiVST ()
{
	speechShutdown ();
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
	case VKEY_NUMPAD6:
		if (helpMode)
		{
			sprintf(speakText, "Go to next parameter");
			break;
		}
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
	case VKEY_NUMPAD4:
		if (helpMode)
		{
			sprintf(speakText, "Go to previous parameter");
			break;
		}
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
	case VKEY_NUMPAD8:
		if (helpMode)
		{
			sprintf(speakText, "Increase parameter value");
			break;
		}
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
	case VKEY_NUMPAD2:
		if (helpMode)
		{
			sprintf(speakText, "Decrease parameter value");
			break;
		}
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
	case VKEY_NUMPAD9:
		if (helpMode)
		{
			sprintf(speakText, "Increase parameter value by larger steps");
			break;
		}
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
	case VKEY_NUMPAD3:
		if (helpMode)
		{
			sprintf(speakText, "Decrease parameter value by larger steps");
			break;
		}
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
	case VKEY_NUMPAD7:
		if (helpMode)
		{
			sprintf(speakText, "Set parameter to highest value");
			break;
		}
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
	case VKEY_NUMPAD1:
		if (helpMode)
		{
			sprintf(speakText, "Set parameter to lowest value");
			break;
		}
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
	case VKEY_NUMPAD5:
		if (helpMode)
		{
			sprintf(speakText, "Announce current parameter state");
			break;
		}
		if (effect)
		{
			effect->getParameterDisplay (paramIndex, paramDisplay);
			effect->getParameterLabel (paramIndex, paramLabel);
			effect->getParameterName (paramIndex, paramName);
		}
		sprintf(speakText, "%s: %s %s", paramName, paramDisplay, paramLabel);
		break;
	case VKEY_TAB:
	case VKEY_NUMPAD0:
		if (helpMode)
		{
			sprintf(speakText, "Announce name and index of the current program");
			break;
		}
		if (effect)
		{
			progIndex = effect->getProgram ();
			effect->getProgramName (progName);
		}
		sprintf(speakText, "Current program name: %s. Current program index: %d", progName, progIndex);
		break;
	case VKEY_RETURN:
	case VKEY_MULTIPLY:
		if (helpMode)
		{
			sprintf(speakText, "Go to next program");
			break;
		}
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
	case VKEY_DIVIDE:
		if (helpMode)
		{
			sprintf(speakText, "Go to previous program");
			break;
		}
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
		if (helpMode)
		{
			sprintf(speakText, "Announce current sample rate");
			break;
		}
		if (effect)
		{
			sprintf(speakText, "Current sample rate: %d", (int)effect->getSampleRate ());
		}
		break;
	case VKEY_F2:
		if (helpMode)
		{
			sprintf(speakText, "Announce current block size");
			break;
		}
		if (effect)
		{
			sprintf(speakText, "Current block size: %d", effect->getBlockSize ());
		}
		break;
	case VKEY_F3:
		if (helpMode)
		{
			sprintf(speakText, "Announce number of inputs");
			break;
		}
		if (effect)
		{
			sprintf(speakText, "Number of inputs: %d", effectData->numInputs);
		}
		break;
	case VKEY_F4:
		if (helpMode)
		{
			sprintf(speakText, "Announce number of outputs");
			break;
		}
		if (effect)
		{
			sprintf(speakText, "Number of outputs: %d", effectData->numOutputs);
		}
		break;
	case VKEY_F5:
		if (helpMode)
		{
			sprintf(speakText, "Announce number of parameters");
			break;
		}
		if (effect)
		{
			sprintf(speakText, "Number of parameters: %d", effectData->numParams);
		}
		break;
	case VKEY_F6:
		if (helpMode)
		{
			sprintf(speakText, "Announce number of programs");
			break;
		}
		if (effect)
		{
			sprintf(speakText, "Number of programs: %d", effectData->numPrograms);
		}
		break;
	case VKEY_F7:
	case VKEY_CLEAR:
		if (helpMode)
		{
			sprintf(speakText, "Reset the plug-in");
			break;
		}
		if (effect)
		{
			effect->suspend ();
			effect->resume ();
			sprintf(speakText, "Plug-in reset");
		}
		break;
	case VKEY_F8:
	case VKEY_SELECT:
		if (helpMode)
		{
			sprintf(speakText, "Reset the plug-in (alternate method)");
			break;
		}
		if (effect)
		{
			effect->close ();
			effect->open ();
			sprintf(speakText, "Plug-in reset");
		}
		break;
	case VKEY_F9:
		if (helpMode)
		{
			sprintf(speakText, "Announce unique ID of plug-in as Hex");
			break;
		}
		if (effect)
		{
			sprintf(speakText, "Unique ID of plug-in: %x", effectData->uniqueID);
		}
		break;
	case VKEY_F10:
		if (helpMode)
		{
			sprintf(speakText, "Announce plug-in version");
			break;
		}
		if (effect)
		{
			sprintf(speakText, "Plug-in version: %f", effectData->version/1000.0f);
		}
		break;
	case VKEY_F11:
		if (helpMode)
		{
			sprintf(speakText, "Announce highest supported bit depth");
			break;
		}
		if (effect)
		{
			if (effectData->flags & effFlagsCanDoubleReplacing)
			{
				sprintf(speakText, "Plug-in supports processing both 32 and 64 bit audio");
			}
			else
			{
				sprintf(speakText, "Plug-in can only process 32 bit audio");
			}
		}
		break;
	case VKEY_F12:
		if (helpMode)
		{
			sprintf(speakText, "Announce how the plug-in stores programs");
			break;
		}
		if (effect)
		{
			if (effectData->flags & effFlagsProgramChunks)
			{
				sprintf(speakText, "Programs are stored as chunks");
			}
			else
			{
				sprintf(speakText, "Programs are stored normally");
			}
		}
		break;
	case VKEY_ESCAPE:
		if (helpMode)
		{
			sprintf(speakText, "Announce initial delay");
			break;
		}
		if (effect)
		{
			sprintf(speakText, "Initial delay: %d", effectData->initialDelay);
		}
		break;
	case VKEY_ALT:
	case VKEY_ADD:
		if (helpMode)
		{
			sprintf(speakText, "Repeat last speech string");
			break;
		}
		break;
	case VKEY_SHIFT:
	case VKEY_DECIMAL:
		if (helpMode)
		{
			sprintf(speakText, "Clear speech buffer");
			break;
		}
		sprintf(speakText, "Speech buffer cleared");
		speak();
		memset(&speakText, 0, sizeof(speakText));
		return true;
	case VKEY_CONTROL:
	case VKEY_SUBTRACT:
		if (helpMode)
		{
			sprintf(speakText, "Interrupt speech");
			break;
		}
		interrupt();
		return true;
	case VKEY_DELETE:
	case VKEY_SEPARATOR:
		if (helpMode)
		{
			sprintf(speakText, "Mute/unmute speech");
			break;
		}
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
		if (helpMode)
		{
			sprintf(speakText, "Disable/enable AccessiVST");
			break;
		}
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
	case VKEY_HELP:
	case VKEY_PAUSE:
		if (helpMode)
		{
			sprintf(speakText, "Enable/disable help mode. Returning to normal operation");
			helpMode = false;
		}
		else
		{
			helpMode = true;
			sprintf(speakText, "Help mode enabled");
		}
		break;
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

void AccessiVST::speechStartup ()
{
#ifdef _WIN32
	Tolk_TrySAPI(true);
	Tolk_Load();
#endif
}

void AccessiVST::speechShutdown ()
{
#ifdef _WIN32
	Tolk_Unload();
#endif
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
