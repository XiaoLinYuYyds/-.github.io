#include "apps.h"

#if 1


__s32 shift_key_to_ir_tx(__gui_msg_t *msg, __hdle hOwi)
{
	static __s32 s_curDownKey = -1;

	if(!hOwi)	return EPDK_FAIL;
	
	if(msg->dwAddData2 == KEY_DOWN_ACTION)
	{
		__u32 keyCode = msg->dwAddData1;
		
		reg_system_para_t *para;
		para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
		if(para)
		{
			if(!(para->m_maskFlags & MASK_SysFlags_IsIrKey))
			{
				keyCode |= MASK_GUI_MSG_KEY_TYPE_IR;
			}
		}
		
		switch(keyCode)
		{
		// IR:
			case GUI_MSG_KEY_POWER:
				keyCode = m_eKeyShiftMapping_power;
				break;
			case GUI_MSG_KEY_SHIFT_SUBTITLE:
				keyCode = m_eKeyShiftMapping_subTitle;
				break;
			case GUI_MSG_KEY_SHIFT_USB_CARD:
				keyCode = m_eKeyShiftMapping_usbCard;
				break;
			case GUI_MSG_KEY_ZOOM:
				keyCode = m_eKeyShiftMapping_zoom;
				break;
			case GUI_MSG_KEY_REPEATE:
				keyCode = m_eKeyShiftMapping_repeat;
				break;
			case GUI_MSG_KEY_SHIFT_AB:
				keyCode = m_eKeyShiftMapping_ab;
				break;

			case GUI_MSG_KEY_SETTING:
				keyCode = m_eKeyShiftMapping_setting;
				break;
			case GUI_MSG_KEY_UP:
				keyCode = m_eKeyShiftMapping_up;
				break;
			case GUI_MSG_KEY_PLAY_PAUSE:
				keyCode = m_eKeyShiftMapping_playPause;
				break;
			case GUI_MSG_KEY_LEFT:
				keyCode = m_eKeyShiftMapping_left;
				break;
			case GUI_MSG_KEY_ENTER:
				keyCode = m_eKeyShiftMapping_enter;
				break;
			case GUI_MSG_KEY_RIGHT:
				keyCode = m_eKeyShiftMapping_right;
				break;
			case GUI_MSG_KEY_LR:
				keyCode = m_eKeyShiftMapping_soundChannel;
				break;
			case GUI_MSG_KEY_DOWN:
				keyCode = m_eKeyShiftMapping_down;
				break;
			case GUI_MSG_KEY_SHIFT_STOP:
				keyCode = m_eKeyShiftMapping_stop;
				break;
			case GUI_MSG_KEY_SUBMENU:
				keyCode = m_eKeyShiftMapping_menu;
				break;
				
			case GUI_MSG_KEY_NUM0:
			case GUI_MSG_KEY_NUM1:
			case GUI_MSG_KEY_NUM2:
			case GUI_MSG_KEY_NUM3:
			case GUI_MSG_KEY_NUM4:
			case GUI_MSG_KEY_NUM5:
			case GUI_MSG_KEY_NUM6:
			case GUI_MSG_KEY_NUM7:
			case GUI_MSG_KEY_NUM8:
			case GUI_MSG_KEY_NUM9:
				keyCode -= GUI_MSG_KEY_NUM0;
				keyCode += m_eKeyShiftMapping_0;
				break;
				
			case GUI_MSG_KEY_SHIFT_SF:
				keyCode = m_eKeyShiftMapping_slowForward;
				break;
			case GUI_MSG_KEY_SHIFT_SELTIME:
				keyCode = m_eKeyShiftMapping_selTime;
				break;
			case GUI_MSG_KEY_RR:
				keyCode = m_eKeyShiftMapping_fastBackward;
				break;
			case GUI_MSG_KEY_FF:
				keyCode = m_eKeyShiftMapping_fastForward;
				break;
			case GUI_MSG_KEY_PREV:
				keyCode = m_eKeyShiftMapping_prev;
				break;
			case GUI_MSG_KEY_NEXT:
				keyCode = m_eKeyShiftMapping_next;
				break;

		// KEY:
			case GUI_MSG_KEY_NUM0 |MASK_GUI_MSG_KEY_TYPE_IR:
			case GUI_MSG_KEY_NUM1 |MASK_GUI_MSG_KEY_TYPE_IR:
			case GUI_MSG_KEY_NUM2 |MASK_GUI_MSG_KEY_TYPE_IR:
			case GUI_MSG_KEY_NUM3 |MASK_GUI_MSG_KEY_TYPE_IR:
			case GUI_MSG_KEY_NUM4 |MASK_GUI_MSG_KEY_TYPE_IR:
			case GUI_MSG_KEY_NUM5 |MASK_GUI_MSG_KEY_TYPE_IR:
			case GUI_MSG_KEY_NUM6 |MASK_GUI_MSG_KEY_TYPE_IR:
			case GUI_MSG_KEY_NUM7 |MASK_GUI_MSG_KEY_TYPE_IR:
			case GUI_MSG_KEY_NUM8 |MASK_GUI_MSG_KEY_TYPE_IR:
			case GUI_MSG_KEY_NUM9 |MASK_GUI_MSG_KEY_TYPE_IR:
				keyCode -= GUI_MSG_KEY_NUM0 |MASK_GUI_MSG_KEY_TYPE_IR;
				keyCode += m_eKeyShiftMapping_panel_0;
				break;
				
			case GUI_MSG_KEY_ENTER |MASK_GUI_MSG_KEY_TYPE_IR:
				keyCode = m_eKeyShiftMapping_panel_enter;
				break;
			case GUI_MSG_KEY_LEFT |MASK_GUI_MSG_KEY_TYPE_IR:
				keyCode = m_eKeyShiftMapping_panel_left;
				break;
			case GUI_MSG_KEY_RIGHT |MASK_GUI_MSG_KEY_TYPE_IR:
				keyCode = m_eKeyShiftMapping_panel_right;
				break;
			case GUI_MSG_KEY_SUBMENU |MASK_GUI_MSG_KEY_TYPE_IR:
				keyCode = m_eKeyShiftMapping_panel_menu;
				break;
				
			default:	return EPDK_FAIL;
		}
		
		keyCode |= 0x00 << 16;
		OWI_MIoctrl(hOwi, m_eOwiCmd_startTx, m_eNecFmt_default, (void *)keyCode);
		__msg_jjl("ir_tx(0x%X):begin.\n", keyCode);

		s_curDownKey = msg->dwAddData1;
		return EPDK_OK;
	}
	else
	{
		__u32 curKey = msg->dwAddData1;
		
		if(curKey >= GUI_MSG_KEY_LONGUP)
		{
			curKey -= GUI_MSG_KEY_LONGUP -GUI_MSG_KEY_UP;
		}
		
		if(curKey == s_curDownKey)
		{
			if(msg->dwAddData2 == KEY_UP_ACTION)
			{
				OWI_MIoctrl(hOwi, m_eOwiCmd_stop, 0, 0);
				s_curDownKey = -1;
				__msg_jjl("ir_tx():end.\n");					
			}

			return EPDK_OK;
		}
	}

	return EPDK_FAIL;
}


#endif

