// これは メイン DLL ファイルです。

#include "stdafx.h"

#include "JoystickInput.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;

namespace JoystickInput {

	public ref class JoystickDevice
	{
	private:
		int g_NumberOfButtons;

		array<bool>^ bButtonStates;
		int lAxisX;
		int lAxisY;
		int lAxisZ;
		int lAxisRz;
		int lHat;

	internal:
		void *hDevice;

		void Update(PRAWINPUT pRawInput, PHIDP_PREPARSED_DATA pPreparsedData, HANDLE hHeap)
		{
			HIDP_CAPS            Caps;
			PHIDP_BUTTON_CAPS    pButtonCaps;
			PHIDP_VALUE_CAPS     pValueCaps;
			USHORT               capsLength;
			USAGE                usage[MAX_BUTTONS];
			ULONG                i, usageLength, value;

			pButtonCaps    = NULL;
			pValueCaps     = NULL;

			//
			// Get the joystick's capabilities
			//

			// Button caps
			CHECK( HidP_GetCaps(pPreparsedData, &Caps) == HIDP_STATUS_SUCCESS )
				CHECK( pButtonCaps = (PHIDP_BUTTON_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_BUTTON_CAPS) * Caps.NumberInputButtonCaps) );

			capsLength = Caps.NumberInputButtonCaps;
			CHECK( HidP_GetButtonCaps(HidP_Input, pButtonCaps, &capsLength, pPreparsedData) == HIDP_STATUS_SUCCESS )
				g_NumberOfButtons = pButtonCaps->Range.UsageMax - pButtonCaps->Range.UsageMin + 1;


			if(this->bButtonStates == nullptr)
			{
				bButtonStates = gcnew array<bool>(g_NumberOfButtons);
			}

			// Value caps
			CHECK( pValueCaps = (PHIDP_VALUE_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_VALUE_CAPS) * Caps.NumberInputValueCaps) );
			capsLength = Caps.NumberInputValueCaps;
			CHECK( HidP_GetValueCaps(HidP_Input, pValueCaps, &capsLength, pPreparsedData) == HIDP_STATUS_SUCCESS )

				//
				// Get the pressed buttons
				//

				usageLength = g_NumberOfButtons;
			CHECK(
				HidP_GetUsages(
				HidP_Input, pButtonCaps->UsagePage, 0, usage, &usageLength, pPreparsedData,
				(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
				) == HIDP_STATUS_SUCCESS );

			Array::Clear(bButtonStates, 0, bButtonStates->Length);

			for(i = 0; i < usageLength; i++)
				bButtonStates[usage[i] - pButtonCaps->Range.UsageMin] = TRUE;

			//
			// Get the state of discrete-valued-controls
			//

			for(i = 0; i < Caps.NumberInputValueCaps; i++)
			{
				CHECK(
					HidP_GetUsageValue(
					HidP_Input, pValueCaps[i].UsagePage, 0, pValueCaps[i].Range.UsageMin, &value, pPreparsedData,
					(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
					) == HIDP_STATUS_SUCCESS );

				switch(pValueCaps[i].Range.UsageMin)
				{
				case 0x30:	// X-axis
					lAxisX = (LONG)value - 128;
					break;

				case 0x31:	// Y-axis
					lAxisY = (LONG)value - 128;
					break;

				case 0x32: // Z-axis
					lAxisZ = (LONG)value - 128;
					break;

				case 0x35: // Rotate-Z
					lAxisRz = (LONG)value - 128;
					break;

				case 0x39:	// Hat Switch
					lHat = value;
					break;
				}
			}

			//
			// Clean up
			//

Error:
			SAFE_FREE(pButtonCaps);
			SAFE_FREE(pValueCaps);
		}

		JoystickDevice(void *hDevice)
		{
			this->hDevice = hDevice;
			this->bButtonStates = nullptr;
		}

	public:
		/*property String^ Name
		{
			String^ get() { 
				UINT bufferSize;
				void* strname;
				HANDLE hHeap = GetProcessHeap();

				CHECK( GetRawInputDeviceInfo(this->hDevice, RIDI_DEVICENAME, NULL, &bufferSize) == 0);
				CHECK( strname = (void*)HeapAlloc(hHeap, 0, bufferSize));
				CHECK( (int)GetRawInputDeviceInfo(this->hDevice, RIDI_DEVICENAME, strname, &bufferSize) >= 0);

				String^ name = Marshal::PtrToStringAuto(IntPtr(strname));
				SAFE_FREE(strname);
				return name;

Error:
				SAFE_FREE(strname);
				return nullptr;
			}
		}*/

		property array<bool>^ ButtonStates
		{
			array<bool>^ get() { return this->bButtonStates; }
		}

		property int AxisX
		{
			int get() { return this->lAxisX; }
		}

		property int AxisY
		{
			int get() { return this->lAxisY; }
		}

		property int AxisZ
		{
			int get() { return this->lAxisZ; }
		}

		property int AxisRz
		{
			int get() { return this->lAxisRz; }
		}

		property int Hat
		{
			int get() { return this->lHat; }
		}
	};

	public ref class Joystick
	{
	private:
		HWND hwnd;		
		List<JoystickDevice^> ^devices;

		void ParseRawInput(PRAWINPUT pRawInput)
		{
			PHIDP_PREPARSED_DATA pPreparsedData;
			UINT                 bufferSize;
			HANDLE               hHeap;

			pPreparsedData = NULL;
			hHeap          = GetProcessHeap();

			//
			// Get the preparsed data block
			//

			CHECK( GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize) == 0 );
			CHECK( pPreparsedData = (PHIDP_PREPARSED_DATA)HeapAlloc(hHeap, 0, bufferSize) );
			CHECK( (int)GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &bufferSize) >= 0 );

			JoystickDevice^ device = nullptr;

			for (int i = 0; i < devices->Count; i++)
			{
				if(devices[i]->hDevice == pRawInput->header.hDevice)
				{
					device = devices[i];
					break;
				}
			}

			if(device == nullptr)
			{
				device = gcnew JoystickDevice(pRawInput->header.hDevice);
				devices->Add(device);
			}

			device->Update(pRawInput, pPreparsedData, hHeap);

Error:
			SAFE_FREE(pPreparsedData);
		}

	public:		
		property IEnumerable<JoystickDevice^>^ Devices
		{
			IEnumerable<JoystickDevice^>^ get() { return this->devices; }
		}

		Joystick(IntPtr handle)
		{
			hwnd = (HWND) handle.ToPointer();
			RAWINPUTDEVICE rid;

			rid.usUsagePage = 1;
			rid.usUsage     = 4;	// Joystick
			rid.dwFlags     = 0;
			rid.hwndTarget  = hwnd;			

			if(!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE)))
				throw gcnew Exception();

			this->devices = gcnew List<JoystickDevice^>();
		}

		void Update(Message m)
		{
			PRAWINPUT pRawInput;
			UINT      bufferSize;
			HANDLE    hHeap;

			HRAWINPUT lparam = (HRAWINPUT)m.LParam.ToPointer();

			GetRawInputData(lparam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));

			hHeap     = GetProcessHeap();
			pRawInput = (PRAWINPUT)HeapAlloc(hHeap, 0, bufferSize);
			if(!pRawInput)
				return;

			GetRawInputData(lparam, RID_INPUT, pRawInput, &bufferSize, sizeof(RAWINPUTHEADER));

			ParseRawInput(pRawInput);

			HeapFree(hHeap, 0, pRawInput);
		}
	};

}